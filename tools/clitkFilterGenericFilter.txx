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
#ifndef clitkFilterGenericFilter_txx
#define clitkFilterGenericFilter_txx

/* =================================================
 * @file   clitkFilterGenericFilter.txx
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
  template<unsigned int Dimension>
  void 
  FilterGenericFilter::UpdateWithDim(std::string PixelType)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

//     if(PixelType == "short"){  
//       if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
//       UpdateWithDimAndPixelType<Dimension, signed short>(); 
//     }
    //    else if(PixelType == "unsigned_short"){  
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
    //     }
    
//     else if (PixelType == "unsigned_char"){ 
//       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
//       UpdateWithDimAndPixelType<Dimension, unsigned char>();
//     }
    
    //     else if (PixelType == "char"){ 
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, signed char>();
    //     }
 //    else {
      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, float>();
//     }
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  InternalPixelType> 
  void 
  FilterGenericFilter::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<InternalPixelType, Dimension> InputImageType;
    typedef itk::Image<InternalPixelType, Dimension> InternalImageType;
    typedef itk::Image<InternalPixelType, Dimension> OutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();


    // Filter
    typedef itk::ImageToImageFilter< InternalImageType, InternalImageType > ImageToImageFilterType;
    typename  ImageToImageFilterType::Pointer filter;
 
    switch ( m_ArgsInfo.type_arg ){
  
    case 0:{
      typename itk::DerivativeImageFilter< InternalImageType,InternalImageType >::Pointer df = itk::DerivativeImageFilter<InternalImageType,InternalImageType >::New();
   
      //Set parameters
      df->SetDirection(m_ArgsInfo.direction_arg);
      df->SetOrder(m_ArgsInfo.order_arg);
      filter=df;
      if (m_Verbose) std::cout<<"Using the derivative image filter with order "<<m_ArgsInfo.order_arg <<" on direction "<< m_ArgsInfo.direction_arg << "..." << std::endl;
      break;
    }
    case 1:{
      typename itk::GradientMagnitudeImageFilter< InternalImageType,InternalImageType >::Pointer gf = itk::GradientMagnitudeImageFilter<InternalImageType,InternalImageType >::New();
   
      //Set parameters
      gf->SetUseImageSpacingOn();

      filter=gf;
      if (m_Verbose) std::cout<<"Using the gradient magnitude image filter... "<< std::endl;
      break;
    }
    case 2:{

      switch ( m_ArgsInfo.projection_arg ){
      
      case 0:{
	//Sum
	typename itk::SumProjectionImageFilter< InternalImageType,InternalImageType >::Pointer pf = itk::SumProjectionImageFilter<InternalImageType,InternalImageType>::New();
	//Set parameters
	pf->SetProjectionDimension(m_ArgsInfo.direction_arg);
      
	filter=pf;
	if (m_Verbose) std::cout<<"Using the Sum projection image filter on the dimension "<<  m_ArgsInfo.direction_arg << "..."<<std::endl;
	break;
      }
      case 1:{
	//Max
	typename itk::MaximumProjectionImageFilter< InternalImageType,InternalImageType >::Pointer pf = itk::MaximumProjectionImageFilter<InternalImageType,InternalImageType>::New();
	//Set parameters
	pf->SetProjectionDimension(m_ArgsInfo.direction_arg);
      
	filter=pf;
	if (m_Verbose) std::cout<<"Using the maximum intensity projection image filter on the dimension "<<  m_ArgsInfo.direction_arg << "..."<<std::endl;
	break;
      }
      case 2:{
	//Sum
	typename itk::MinimumProjectionImageFilter< InternalImageType,InternalImageType >::Pointer pf = itk::MinimumProjectionImageFilter<InternalImageType,InternalImageType>::New();
	//Set parameters
	pf->SetProjectionDimension(m_ArgsInfo.direction_arg);
      
	filter=pf;
	if (m_Verbose) std::cout<<"Using the minimum intensity projection image filter on the dimension "<<  m_ArgsInfo.direction_arg << "..."<<std::endl;
	break;
      }
      }
      break;
    }//end case projection filter 

    case 3: {
      typename itk::IntensityWindowingImageFilter< InternalImageType,InternalImageType >::Pointer pf 
	= itk::IntensityWindowingImageFilter<InternalImageType,InternalImageType>::New();
      //Set parameters
      pf->SetWindowMinimum((InternalPixelType) m_ArgsInfo.inputMin_arg);
      pf->SetWindowMaximum((InternalPixelType) m_ArgsInfo.inputMax_arg);
      pf->SetOutputMinimum((InternalPixelType) m_ArgsInfo.outputMin_arg);
      pf->SetOutputMaximum((InternalPixelType) m_ArgsInfo.outputMax_arg);

      filter=pf;
      if (m_Verbose) std::cout<<"Using the window intensity image filter with the window min and max being  "
			      <<  m_ArgsInfo.inputMin_arg << " and "<< m_ArgsInfo.inputMax_arg
			      <<" and the output min and max being "<<m_ArgsInfo.outputMin_arg<<" and "
			      << m_ArgsInfo.outputMax_arg << std::endl;
      break;
    }
    case 4: {

      //Downsample using bsplines: four types of resamplers
      typedef itk::BSplineResampleImageFilterBase<InternalImageType, InternalImageType> ResamplerType;
      typedef itk::BSplineCenteredResampleImageFilterBase<InternalImageType, InternalImageType> CenteredResamplerType;
      typedef itk::BSplineL2ResampleImageFilterBase<InternalImageType, InternalImageType> L2ResamplerType;
      typedef itk::BSplineCenteredL2ResampleImageFilterBase<InternalImageType, InternalImageType> CenteredL2ResamplerType;
    
      //ResamplerType
      if(m_ArgsInfo.resamplerType_arg==0)
	{
	  typename itk::BSplineDownsampleImageFilter< InternalImageType,InternalImageType, ResamplerType >::Pointer df 
	    = itk::BSplineDownsampleImageFilter< InternalImageType,InternalImageType, ResamplerType >::New();
	  df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	  filter=df;
	  if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with Standard Resampler"<<std::endl;
	}
      //CenteredResamplerType
      else if(m_ArgsInfo.resamplerType_arg==1)
	{
	  typename itk::BSplineDownsampleImageFilter< InternalImageType,InternalImageType, CenteredResamplerType >::Pointer df 
	    = itk::BSplineDownsampleImageFilter< InternalImageType,InternalImageType, CenteredResamplerType >::New();
	  df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	  filter=df;
	  if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with  Centered Resampler"<<std::endl;
	}
      //L2ResamplerType
      else if(m_ArgsInfo.resamplerType_arg==2)
	{
	  typename itk::BSplineDownsampleImageFilter< InternalImageType,InternalImageType, L2ResamplerType >::Pointer df 
	    = itk::BSplineDownsampleImageFilter< InternalImageType,InternalImageType,L2ResamplerType >::New();
	  df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);	
	  filter=df;
	  if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with L2 Resampler"<<std::endl;
	}
      else if(m_ArgsInfo.resamplerType_arg==3)
	{
	  typename itk::BSplineDownsampleImageFilter< InternalImageType,InternalImageType, CenteredL2ResamplerType >::Pointer df 
	    = itk::BSplineDownsampleImageFilter< InternalImageType,InternalImageType,CenteredL2ResamplerType >::New();
	  df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	  filter=df;
	  if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with L2 Centered Resampler"<<std::endl;
	}
      break;
    }
    case 5: {
      typedef itk::NormalizeImageFilter<InternalImageType, InternalImageType> NormalizeFilterType;
      typename  NormalizeFilterType::Pointer  df = NormalizeFilterType::New(); 
      filter=df;
      if (m_Verbose) std::cout <<  "Normalizing image intensities to a zero mean and 1 SD (float!)..." << std::endl;
      break;
    }
//     case 6: {
//       typedef itk::AnisotropicDiffusionImageFilter<InternalImageType, InternalImageType> FilterType;
//       typename FilterType::Pointer  df = FilterType::New(); 
//       df->SetConductanceParameter(m_ArgsInfo.cond_arg);
//       df->SetNumberOfIterations(m_ArgsInfo.iter_arg);
//       df->SetTimeStep(m_ArgsInfo.time_arg);
//       df->SetUseImageSpacing(true);
      
//       filter=df;
//       if (m_Verbose) std::cout <<  "Using the anisotropic diffusion image filter..." << std::endl;
//       break;
//     }

    case 7: {
      typedef itk::GradientAnisotropicDiffusionImageFilter<InternalImageType, InternalImageType> FilterType;
      typename FilterType::Pointer  df = FilterType::New(); 
      df->SetConductanceParameter(m_ArgsInfo.cond_arg);
      df->SetNumberOfIterations(m_ArgsInfo.iter_arg);
      df->SetTimeStep(m_ArgsInfo.time_arg);
      df->SetUseImageSpacing(true);
      
      filter=df;
      if (m_Verbose) std::cout <<  "Using the gradient anisotropic diffusion image filter..." << std::endl;
      break;
    }

    case 8: {
      typedef itk::CurvatureAnisotropicDiffusionImageFilter<InternalImageType, InternalImageType> FilterType;
      typename  FilterType::Pointer  df = FilterType::New(); 
      df->SetConductanceParameter(m_ArgsInfo.cond_arg);
      df->SetNumberOfIterations(m_ArgsInfo.iter_arg);
      df->SetTimeStep(m_ArgsInfo.time_arg);
      df->SetUseImageSpacing(true);
      
      filter=df;
      if (m_Verbose) std::cout <<  "Using the Curvature anisotropic diffusion image filter..." << std::endl;
      break;
    }

    case 9: {
      typedef itk::CurvatureFlowImageFilter<InternalImageType, InternalImageType> FilterType;
      typename  FilterType::Pointer  df = FilterType::New(); 
      df->SetTimeStep(m_ArgsInfo.time_arg);
      df->SetUseImageSpacing(true);
      df->SetNumberOfIterations(m_ArgsInfo.iter_arg);
      filter=df;
      if (m_Verbose) std::cout <<  "Using the curvature flow image filter......" << std::endl;
      break;
    }

      
    }//end switch filterType
  

    //==================================================================
    //execute the filter
    filter->SetInput(input);
  
  
    if (m_Verbose)std::cout<<"Starting filter..."<<std::endl;

    try {
      filter->Update();
    }
    catch( itk::ExceptionObject & err ) 
      { 
	std::cerr << "ExceptionObject caught! Filter failed!" << std::endl; 
	std::cerr << err << std::endl; 
	return;
      } 
    
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(filter->GetOutput());
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkFilterGenericFilter_txx
