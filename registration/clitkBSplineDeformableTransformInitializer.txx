/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
#ifndef __clitkBSplineDeformableTransformInitializer_txx
#define __clitkBSplineDeformableTransformInitializer_txx
#include "clitkBSplineDeformableTransformInitializer.h"

namespace clitk
{


  template < class TTransform, class TImage >
  BSplineDeformableTransformInitializer<TTransform, TImage >
  ::BSplineDeformableTransformInitializer()
  {
    this->m_NumberOfControlPointsInsideTheImage.Fill( 5 );
    this->m_ControlPointSpacing.Fill(64.);
    this->m_ChosenSpacing.Fill(64.);
    m_NumberOfControlPointsIsGiven=false;
    m_ControlPointSpacingIsGiven=false;
    m_TransformRegionIsGiven=false;
    m_SamplingFactorIsGiven=false;
    m_SplineOrders.Fill(3);
    m_Parameters=NULL;
  }

  template < class TTransform, class TImage >
  void
  BSplineDeformableTransformInitializer<TTransform, TImage >
  ::InitializeTransform()
  {
    // Sanity check:
    // The image is required for the region and spacing
    if( ! this->m_Image )
      {
	itkExceptionMacro( "Reference Image has not been set" );
	return;
      }
    
    // A pointer to the transform is required
    if( ! this->m_Transform )
      {
	itkExceptionMacro( "Transform has not been set" );
	return;
      }

    // If the image come from a filter, then update that filter.
    if( this->m_Image->GetSource() )
      {
	this->m_Image->GetSource()->Update();
      }


    //--------------------------------------
    // Spacing & Size on image
    //--------------------------------------
    SpacingType fixedImageSpacing = m_Image->GetSpacing();
    SizeType fixedImageSize=m_Image->GetLargestPossibleRegion().GetSize();
    typename RegionType::SizeType   gridBorderSize;
    typename RegionType::SizeType   totalGridSize;

    // Only spacing given: adjust if necessary
    if (m_ControlPointSpacingIsGiven  && !m_NumberOfControlPointsIsGiven)
      {
	for(unsigned int r=0; r<InputDimension; r++) 
	  {
	    // JV
	    m_ChosenSpacing[r]= m_ControlPointSpacing[r];
	    m_ControlPointSpacing[r]= ( round(m_ChosenSpacing[r]/fixedImageSpacing[r]) *fixedImageSpacing[r] ) ;
	    m_NumberOfControlPointsInsideTheImage[r] = ceil( (double)fixedImageSize[r]*fixedImageSpacing[r]/ m_ControlPointSpacing[r] );
	    if (  ( ceil( (double)fixedImageSize[r]*fixedImageSpacing[r]/ m_ControlPointSpacing[r] ) )
		 == ( (double)fixedImageSize[r]*fixedImageSpacing[r]/ m_ControlPointSpacing[r] ) )
	      {
		m_NumberOfControlPointsInsideTheImage[r] +=1;
		DD("Adding control point");
	      }
	  }
      }
  
    // Only number of CP given: adjust if necessary
    // JV -1 ?
    else if (!m_ControlPointSpacingIsGiven  && m_NumberOfControlPointsIsGiven)
      {
	for(unsigned int r=0; r<InputDimension; r++) 
	  {
	    m_ChosenSpacing[r]=fixedImageSpacing[r]*( (double)(fixedImageSize[r])  / 
						      (double)(m_NumberOfControlPointsInsideTheImage[r]) );
	    m_ControlPointSpacing[r]= fixedImageSpacing[r]* ceil( (double)(fixedImageSize[r] - 1)  / 
								  (double)(m_NumberOfControlPointsInsideTheImage[r] - 1) );
	  }
      }
  
    // Both or none of Spacing and number of CP given: no adjustment adjust, just warnings
    else 
      {
	for(unsigned int r=0; r<InputDimension; r++) 
	  {
	    m_ChosenSpacing[r]= m_ControlPointSpacing[r];
	    if (m_NumberOfControlPointsInsideTheImage[r]*m_ControlPointSpacing[r]< fixedImageSize[r]*fixedImageSpacing[r]) 
	      {
		std::cout<<"WARNING: Specified control point region ("<<m_NumberOfControlPointsInsideTheImage[r]*m_ControlPointSpacing[r]
			 <<"mm) does not cover the fixed image region ("<< fixedImageSize[r]*fixedImageSpacing[r]
			 <<"mm) for dimension "<<r<<"!" <<std::endl
			 <<"Specify only --spacing or --control for automatic adjustment..."<<std::endl;
	      }
	    if (  fmod(m_ControlPointSpacing[r], fixedImageSpacing[r]) ) 
	      {
		std::cout<<"WARNING: Specified control point spacing for dimension "<<r
			 <<" does not allow exact representation of BLUT FFD!"<<std::endl
			 <<"Spacing ratio is non-integer: "<<m_ControlPointSpacing[r]/ fixedImageSpacing[r]<<std::endl
			 <<"Specify only --spacing or --control for automatic adjustment..."<<std::endl;  
	      }
	  }
      }

    if (m_Verbose) std::cout<<"The chosen control point spacing "<<m_ChosenSpacing<<"..."<<std::endl;
    if (m_Verbose) std::cout<<"The control points spacing was adapted to "<<m_ControlPointSpacing<<"..."<<std::endl; 
    if (m_Verbose) std::cout<<"The number of (internal) control points is "<<m_NumberOfControlPointsInsideTheImage<<"..."<<std::endl; 
    

    //--------------------------------------
    // Border size 
    //--------------------------------------
    for(unsigned int r=0; r<InputDimension; r++) gridBorderSize[r]=m_SplineOrders[r]; // Border for spline order = 3 ( 1 lower, 2 upper )
    totalGridSize = m_NumberOfControlPointsInsideTheImage + gridBorderSize;
    if (m_Verbose) std::cout<<"The total control point grid size was set to "<<totalGridSize<<"..."<<std::endl;
    RegionType gridRegion;
    gridRegion.SetSize(totalGridSize);
 
  
    //--------------------------------------
    // Origin
    //--------------------------------------
    typedef typename TransformType::OriginType OriginType;
    OriginType fixedImageOrigin, gridOrigin;

    // In case of non-zero index
    m_Image->TransformIndexToPhysicalPoint(m_Image->GetLargestPossibleRegion().GetIndex(),fixedImageOrigin);
    typename ImageType::DirectionType gridDirection = m_Image->GetDirection();      
    SizeType orderShift;

    // Shift depends on order
    for(unsigned int r=0; r<InputDimension; r++)
      {
	orderShift[r] = m_SplineOrders[r] / 2;
	gridOrigin[r] = fixedImageOrigin[r]- (double) orderShift[r]* m_ControlPointSpacing[r];
      } 
    if (m_Verbose) std::cout<<"The control point grid origin was set to "<<gridOrigin<<"..."<<std::endl;


    //--------------------------------------
    // LUT sampling factors
    //--------------------------------------
    for (unsigned int i=0; i< InputDimension; i++)
      {
	if (m_Verbose) std::cout<<"For dimension "<<i<<", the ideal sampling factor (if integer) is a multitude of "
				<< m_ControlPointSpacing[i]/ fixedImageSpacing[i]<<"..."<<std::endl;
	if ( !m_SamplingFactorIsGiven) m_SamplingFactors[i]= (int) ( m_ControlPointSpacing[i]/ fixedImageSpacing[i]);
	if (m_Verbose) std::cout<<"Setting sampling factor "<<i<<" to "<<m_SamplingFactors[i]<<"..."<<std::endl;
      }


    //--------------------------------------
    // Set
    //--------------------------------------
    this->m_Transform->SetSplineOrders(m_SplineOrders);
    this->m_Transform->SetGridRegion( gridRegion );
    this->m_Transform->SetGridOrigin( gridOrigin );
    this->m_Transform->SetGridSpacing( m_ControlPointSpacing );
    this->m_Transform->SetGridDirection( gridDirection );
    this->m_Transform->SetLUTSamplingFactors(m_SamplingFactors);

  }

  template < class TTransform, class TImage >
  void
  BSplineDeformableTransformInitializer<TTransform, TImage >
  ::SetInitialParameters( const std::string& s, ParametersType& params) 
  {
    //---------------------------------------
    // Read Initial parameters
    //---------------------------------------
    typedef itk::ImageFileReader<CoefficientImageType> CoefficientReaderType;
    typename CoefficientReaderType::Pointer coeffReader=CoefficientReaderType::New();
    coeffReader->SetFileName(s);
    if(m_Verbose) std::cout<<"Reading initial coefficients from file: "<<s<<"..."<<std::endl;
    coeffReader->Update();
    typename CoefficientImageType::Pointer coefficientImage=coeffReader->GetOutput();
    this->SetInitialParameters(coefficientImage, params);
  }
  
  template < class TTransform, class TImage >
  void
  BSplineDeformableTransformInitializer<TTransform, TImage >
  ::SetInitialParameters(const typename CoefficientImageType::Pointer coefficientImage, ParametersType& params) 
  {
    // Keep a reference 
    m_Parameters=&params;

    // Resample
    typedef ResampleBSplineDeformableTransformImageFilter<CoefficientImageType,CoefficientImageType> ResamplerType;
    typename ResamplerType::Pointer resampler=ResamplerType::New();
    resampler->SetInput(coefficientImage);	
    resampler->SetOutputParametersFromImage(m_Transform->GetCoefficientImage()); 
    if(m_Verbose) std::cout<<"Resampling initial coefficients..."<<std::endl;
    resampler->Update();

    // Copy parameters into the existing array, I know its crappy
    typedef itk::ImageRegionConstIterator<CoefficientImageType> Iterator;
    Iterator it (resampler->GetOutput(), resampler->GetOutput()->GetLargestPossibleRegion() );
    it.GoToBegin();
    unsigned int i=0;
    while(! it.IsAtEnd())
      {
	for (unsigned int j=0; j<InputDimension;j++)
	  params[i+j]=it.Get()[j];

	++it;
	i+=InputDimension;
      }

    // JV pass the reference to the array !!
    this->m_Transform->SetParameters(params);
  }


  
}  // namespace itk

#endif
