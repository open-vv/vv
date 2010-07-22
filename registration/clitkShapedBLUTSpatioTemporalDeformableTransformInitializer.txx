/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
#ifndef __clitkShapedBLUTSpatioTemporalDeformableTransformInitializer_txx
#define __clitkShapedBLUTSpatioTemporalDeformableTransformInitializer_txx
#include "clitkShapedBLUTSpatioTemporalDeformableTransformInitializer.h"

namespace clitk
{


  template < class TTransform, class TImage >
  ShapedBLUTSpatioTemporalDeformableTransformInitializer<TTransform, TImage >
  ::ShapedBLUTSpatioTemporalDeformableTransformInitializer()
  {
    this->m_NumberOfControlPointsInsideTheImage.Fill( 5 );
    this->m_ControlPointSpacing.Fill(64.);
    m_ControlPointSpacing[InputDimension-1]=2;
    this->m_ChosenSpacing.Fill(64.);
    m_ChosenSpacing[InputDimension-1]=2;
    m_ControlPointSpacing[InputDimension-1]=2;
    m_NumberOfControlPointsIsGiven=false;
    m_ControlPointSpacingIsGiven=false;
    m_TransformRegionIsGiven=false;
    m_SamplingFactorIsGiven=false;
    m_SplineOrders.Fill(3);
    m_BC1=true;
    m_BC2=true;
    m_TrajectoryShape=0;
  }

  template < class TTransform, class TImage >
  void
  ShapedBLUTSpatioTemporalDeformableTransformInitializer<TTransform, TImage >
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
	    if ( (  ( ceil( (double)fixedImageSize[r]*fixedImageSpacing[r]/ m_ControlPointSpacing[r] ) )
		 == ( (double)fixedImageSize[r]*fixedImageSpacing[r]/ m_ControlPointSpacing[r] ) )
		 &&  (r!= InputDimension-1)  )
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

    //--------------------------------------
    // LUT sampling factors
    //--------------------------------------
    for (unsigned int i=0; i< InputDimension; i++)
      {
	if (m_Verbose) std::cout<<"For dimension "<<i<<", the ideal sampling factor (if integer) is a multitude of "
				<< m_ControlPointSpacing[i]/ fixedImageSpacing[i]<<"..."<<std::endl;
	if ( !m_SamplingFactorIsGiven) m_SamplingFactors[i]= (int) ( m_ControlPointSpacing[i]/ fixedImageSpacing[i]);
      }

    //--------------------------------------
    // Set the transform properties 
    //--------------------------------------
    RegionType gridRegion;
    OriginType gridOrigin;
    DirectionType gridDirection;

    //--------------------------------------
    // Border size 
    //--------------------------------------
    for(unsigned int r=0; r<InputDimension; r++) gridBorderSize[r]=m_SplineOrders[r]; // Border for spline order = 3 ( 1 lower, 2 upper )
    totalGridSize = m_NumberOfControlPointsInsideTheImage + gridBorderSize;

    //--------------------------------------
    // Origin
    //--------------------------------------
    OriginType fixedImageOrigin;

    // In case of non-zero index
    m_Image->TransformIndexToPhysicalPoint(m_Image->GetLargestPossibleRegion().GetIndex(),fixedImageOrigin);
    gridDirection = m_Image->GetDirection();      
    itk::FixedArray<int, InputDimension> orderShift;
    
    // Spacing is 2.5 : manually modify the props
    if( !(m_Transform->GetTransformShape()%2) )
      {
	m_ControlPointSpacing[InputDimension-1]=2.5;
	m_SamplingFactors[InputDimension-1]=5;
	totalGridSize[InputDimension-1]-=1;
	gridRegion.SetSize(totalGridSize);
      }


    switch(m_Transform->GetTransformShape()){
    
      // The egg shape
    case 0:
    case 1:  
      {
	if (m_Verbose) std::cout<<"Using the egg shape..."<<std::endl;

	//--------------------------------------
	// Border size 
	//--------------------------------------
	// Boundary condition1: cyclic
	if (m_BC1)
	  totalGridSize[InputDimension-1]-=3;
	
	// Boundary condition1: reference is on trajectory
	if (m_BC2)
	  totalGridSize[InputDimension-1]-=1;
	
	gridRegion.SetSize(totalGridSize);
		
	//--------------------------------------
	// Origin
	//--------------------------------------
	// Shift depends on order
	for(unsigned int r=0; r<InputDimension; r++)
	  {
	    orderShift[r] = m_SplineOrders[r] / 2;
	    if (r==InputDimension-1)
	      {
		// Boundary condition1: cyclic
		if (m_BC1)
		  orderShift[r]-=1;
		
		// Boundary condition1: reference is on trajectory
		if (m_BC2)
		  orderShift[r]-=1;
	      }
	    
	    gridOrigin[r] = fixedImageOrigin[r]- (double) orderShift[r]* m_ControlPointSpacing[r];
	  } 
	break;
      }

      // The rabit: a diamond with fixed extreme values
    case 2:
    case 3:
      {

	if (m_Verbose) std::cout<<"Using the rabbit shape..."<<std::endl;
	
	//--------------------------------------
	// Border size 
	//--------------------------------------
	// Boundary condition1: cyclic
	if (m_BC1)
	  totalGridSize[InputDimension-1]-=1;
	
	// Boundary condition1: reference is on trajectory
	if (m_BC2)
	  totalGridSize[InputDimension-1]-=1;

	// Extreme values are constrained
	totalGridSize[InputDimension-1]-=2;
	
	gridRegion.SetSize(totalGridSize);
	
	//--------------------------------------
	// Origin
	//--------------------------------------
	// Shift depends on order
	for(unsigned int r=0; r<InputDimension; r++)
	  {
	    orderShift[r] = m_SplineOrders[r] / 2;
	    if (r==InputDimension-1)
	      {
		// Boundary condition1: cyclic
		orderShift[r]=0;
	      }
	    gridOrigin[r] = fixedImageOrigin[r]- (double) orderShift[r]* m_ControlPointSpacing[r];
	  } 

	break;
      }

      // The sputnik: a diamond with one indepent extreme value and 4DOF
    case 4:
    case 5:
      {
	if (m_Verbose) std::cout<<"Using the sputnik shape..."<<std::endl;

	//--------------------------------------
	// Border size 
	//--------------------------------------
	// Boundary condition1: cyclic
	if (m_BC1)
	  totalGridSize[InputDimension-1]-=1;
	
	// Boundary condition1: reference is on trajectory
	if (m_BC2)
	  totalGridSize[InputDimension-1]-=1;

	// One extreme value is fixed
	totalGridSize[InputDimension-1]-=1;
	gridRegion.SetSize(totalGridSize);
	
	//--------------------------------------
	// Origin
	//--------------------------------------
	// Shift depends on order
	for(unsigned int r=0; r<InputDimension; r++)
	  {
	    orderShift[r] = m_SplineOrders[r] / 2;
	    if (r==InputDimension-1)
	      {
		// Boundary condition1: cyclic
		orderShift[r]=0;
	      }
	    gridOrigin[r] = fixedImageOrigin[r]- (double) orderShift[r]* m_ControlPointSpacing[r];
	  } 
	break;
      }

      // The diamond
    case 6:  
    case 7:
      {
	if (m_Verbose) std::cout<<"Using the diamond shape..."<<std::endl;
	
	//--------------------------------------
	// Border size 
	//--------------------------------------
	// Boundary condition1: cyclic
	if (m_BC1)
	  totalGridSize[InputDimension-1]-=1;
	
	// Boundary condition1: reference is on trajectory
	if (m_BC2)
	  totalGridSize[InputDimension-1]-=1;
	
	gridRegion.SetSize(totalGridSize);
	
	//--------------------------------------
	// Origin
	//--------------------------------------
	// Shift depends on order
	for(unsigned int r=0; r<InputDimension; r++)
	  {
	    orderShift[r] = m_SplineOrders[r] / 2;
	    gridOrigin[r] = fixedImageOrigin[r]- (double) orderShift[r]* m_ControlPointSpacing[r];
	  } 

	break;

      } //end case 7

   // The sputnik: a diamond with one indepent extreme value and 4DOF
    case 8:
    case 9:
      {
	if (m_Verbose) std::cout<<"Using the sputnik shape..."<<std::endl;

	//--------------------------------------
	// Border size 
	//--------------------------------------
	// Boundary condition1: cyclic
	if (m_BC1)
	  totalGridSize[InputDimension-1]-=1;
	
	// Boundary condition1: reference is on trajectory
	if (m_BC2)
	  totalGridSize[InputDimension-1]-=1;

	// One extreme value is fixed
	totalGridSize[InputDimension-1]-=1;
	gridRegion.SetSize(totalGridSize);
	
	//--------------------------------------
	// Origin
	//--------------------------------------
	// Shift depends on order
	for(unsigned int r=0; r<InputDimension; r++)
	  {
	    orderShift[r] = m_SplineOrders[r] / 2;
	    if (r==InputDimension-1)
	      {
		// Boundary condition1: cyclic
		orderShift[r]=0;
	      }
	    gridOrigin[r] = fixedImageOrigin[r]- (double) orderShift[r]* m_ControlPointSpacing[r];
	  } 
	break;
      }
  
    default:
      {
	DD("Shape not available");
      }

    }



    if (m_Verbose) std::cout<<"The chosen control point spacing "<<m_ChosenSpacing<<"..."<<std::endl;
    if (m_Verbose) std::cout<<"The control points spacing was adapted to "<<m_ControlPointSpacing<<"..."<<std::endl; 
    if (m_Verbose) std::cout<<"The number of (internal) control points is "<<m_NumberOfControlPointsInsideTheImage<<"..."<<std::endl;
    if (m_Verbose) std::cout<<"Setting sampling factors to "<<m_SamplingFactors<<"..."<<std::endl; 
    if (m_Verbose) std::cout<<"The total control point grid size was set to "<<totalGridSize<<"..."<<std::endl;
    if (m_Verbose) std::cout<<"The control point grid origin was set to "<<gridOrigin<<"..."<<std::endl;
    
    //--------------------------------------
    // Set
    //--------------------------------------
    this->m_Transform->SetSplineOrders(m_SplineOrders);
    this->m_Transform->SetGridSpacing( m_ControlPointSpacing );
    this->m_Transform->SetGridOrigin( gridOrigin );
    this->m_Transform->SetGridDirection( gridDirection );
    this->m_Transform->SetGridRegion( gridRegion );
    this->m_Transform->SetLUTSamplingFactors(m_SamplingFactors);

  }

  template < class TTransform, class TImage >
  void
  ShapedBLUTSpatioTemporalDeformableTransformInitializer<TTransform, TImage >
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
  ShapedBLUTSpatioTemporalDeformableTransformInitializer<TTransform, TImage >
  ::SetInitialParameters(const typename CoefficientImageType::Pointer coefficientImage, ParametersType& params) 
  {
    // Keep a reference 
    m_Parameters=&params;

    // Resample
    typedef ResampleBSplineDeformableTransformImageFilter<CoefficientImageType,CoefficientImageType> ResamplerType;
    typename ResamplerType::Pointer resampler=ResamplerType::New();
    resampler->SetVerbose(m_Verbose);
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
	for (unsigned int j=0; j<InputDimension-1;j++)
	  params[i+j]=it.Get()[j];

	++it;
	i+=InputDimension-1;
      }

    // JV pass the reference to the array !!
    // JV Update the padded version !!
    this->m_Transform->SetParameters(params);
  }


  //   template < class TTransform, class TImage >
  //   void
  //   ShapedBLUTSpatioTemporalDeformableTransformInitializer<TTransform, TImage >
  //   ::SetInitialPaddedParameters( const std::string& s, ParametersType& params) 
  //   {
  //     //---------------------------------------
  //     // Read Initial parameters
  //     //---------------------------------------
  //     typedef itk::ImageFileReader<CoefficientImageType> CoefficientReaderType;
  //     typename CoefficientReaderType::Pointer coeffReader=CoefficientReaderType::New();
  //     coeffReader->SetFileName(s);
  //     if(m_Verbose) std::cout<<"Reading initial padded coefficients from file: "<<s<<"..."<<std::endl;
  //     coeffReader->Update();
  //     typename CoefficientImageType::Pointer coefficientImage=coeffReader->GetOutput();
  //     this->SetInitialPaddedParameters(coefficientImage, params);
  //   }
  
  //   template < class TTransform, class TImage >
  //   void
  //   ShapedBLUTSpatioTemporalDeformableTransformInitializer<TTransform, TImage >
  //   ::SetInitialPaddedParameters(const typename CoefficientImageType::Pointer coefficientImage, ParametersType& params) 
  //   {
  //     // Resample
  //     typedef ResampleBSplineSpatioTemporalDeformableTransformImageFilter<CoefficientImageType,CoefficientImageType> ResamplerType;
  //     typename ResamplerType::Pointer resampler=ResamplerType::New();
  //     resampler->SetInput(coefficientImage);	
  //     resampler->SetOutputParametersFromImage(m_Transform->GetCoefficientImage()); 
  //     if(m_Verbose) std::cout<<"Resampling initial padded coefficients..."<<std::endl;
  //     resampler->Update();
    
  //     // Copy parameters into the existing array, I know its crappy
  //     typedef itk::ImageRegionConstIterator<CoefficientImageType> Iterator;
  //     Iterator it (resampler->GetOutput(), resampler->GetOutput()->GetLargestPossibleRegion() );
  //     it.GoToBegin();
  //     unsigned int i=0;
  //     while(! it.IsAtEnd())
  //       {
  // 	for (unsigned int j=0; j<InputDimension;j++)
  // 	  params[i+j]=it.Get()[j];

  // 	++it;
  // 	i+=InputDimension;
  //       }
  //   }

  
}  // namespace itk

#endif
