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
#ifndef clitkResampleBSplineDeformableTransformImageFilter_txx
#define clitkResampleBSplineDeformableTransformImageFilter_txx

/* =================================================
 * @file   clitkResampleBSplineDeformableTransformImageFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<class InputImageType, class OutputImageType>
  ResampleBSplineDeformableTransformImageFilter<InputImageType, OutputImageType>::ResampleBSplineDeformableTransformImageFilter()
  {
   m_Verbose=false;
   m_SplineOrders.Fill(3);
   m_Size.Fill(0);
   m_OutputSpacing.Fill(1.0);
   m_OutputOrigin.Fill(-1);
   m_OutputStartIndex.Fill(0);
   m_OutputDirection.SetIdentity();
   
   // Components
   m_Resampler = ResamplerType::New();
   m_Function = FunctionType::New();
   m_Identity = IdentityTransformType::New();
   m_Decomposition = DecompositionType::New();
   
   // Connect
   m_Resampler->SetInterpolator( m_Function );
   m_Resampler->SetTransform( m_Identity );
   m_Decomposition->SetInput( m_Resampler->GetOutput() );

  }

  //-------------------------------------------------------------------
  // Output Info
  //-------------------------------------------------------------------
  template <class InputImageType, class  OutputImageType> 
  void 
  ResampleBSplineDeformableTransformImageFilter<InputImageType, OutputImageType>::SetOutputSpacing( const double* spacing )
  {
    OutputSpacingType s(spacing);
    this->SetOutputSpacing( s );
  }
  
  //-------------------------------------------------------------------
  // Output Info
  //-------------------------------------------------------------------
  template <class InputImageType, class  OutputImageType> 
  void 
  ResampleBSplineDeformableTransformImageFilter<InputImageType, OutputImageType>::SetOutputOrigin( const double* origin )
  {
    OutputPointType p(origin);
    this->SetOutputOrigin( p );
  }



  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <class InputImageType, class  OutputImageType> 
  void 
  ResampleBSplineDeformableTransformImageFilter<InputImageType, OutputImageType>::GenerateData()
  {

    // Resample?
    bool sizeIsCorrect=true;
    
    if (this->GetInput()->GetSpacing()  !=m_OutputSpacing)
      sizeIsCorrect=false;
    else if (this->GetInput()->GetLargestPossibleRegion().GetSize()!=m_Size)
      sizeIsCorrect=false;
    else if (this->GetInput()->GetOrigin()!=m_OutputOrigin)
      sizeIsCorrect=false;
    
    // No resampling resquired
    if (sizeIsCorrect)
      {
	if(m_Verbose)std::cout<<"Output properties are up to date, no resampling required!"<<std::endl;
	typedef itk::CastImageFilter<InputImageType, OutputImageType>  CastImageFilterType;
	typename CastImageFilterType::Pointer caster = CastImageFilterType::New();
	caster->SetInput(this->GetInput());
	caster->Update();
	this->GraftOutput(caster->GetOutput());
      }

    // Resample
    else
      {
	if(m_Verbose)std::cout<<"Resampling transform..."<<std::endl;

	// Resample the coeeficients to values of correct size, spacing,...
	m_Resampler->SetInput( this->GetInput() );
	m_Resampler->SetSize( m_Size);
	m_Resampler->SetOutputSpacing( m_OutputSpacing );
	m_Resampler->SetOutputOrigin( m_OutputOrigin );
	m_Resampler->SetOutputDirection( m_OutputDirection );
	m_Resampler->SetOutputStartIndex( m_OutputStartIndex );
	
	// Transform the values back to Bspline coefficients
	m_Decomposition->SetSplineOrders( m_SplineOrders );	
	m_Decomposition->Update();
	this->GraftOutput( m_Decomposition->GetOutput() );
      }

  }


}//end clitk
 
#endif //#define clitkResampleBSplineDeformableTransformImageFilter_txx
