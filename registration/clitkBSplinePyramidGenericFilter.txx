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
#ifndef clitkBSplinePyramidGenericFilter_txx
#define clitkBSplinePyramidGenericFilter_txx

/* =================================================
 * @file   clitkBSplinePyramidGenericFilter.txx
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
  BSplinePyramidGenericFilter::UpdateWithDim(std::string PixelType, unsigned int Components)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<<Components<<" component(s) of "<<  PixelType<<"..."<<std::endl;
    
    if (Components==1)
      {
	if(PixelType == "short"){  
	  if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, signed short>(); 
	}
	//    else if(PixelType == "unsigned_short"){  
	//       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
	//       UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
	//     }
	
	else if (PixelType == "unsigned_char"){ 
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, unsigned char>();
	}
	
	//     else if (PixelType == "char"){ 
	//       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
	//       UpdateWithDimAndPixelType<Dimension, signed char>();
	//     }
	else {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, float>();
	}
      }
    else if (Components==3)
      {
    	if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and 3D float (DVF)" << std::endl;
    	UpdateWithDimAndVectorType<Dimension, itk::Vector<float, 3> >();
      }
    else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;
    
  }



  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  BSplinePyramidGenericFilter::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<PixelType, Dimension> OutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();

    // Resampler Types
    typedef itk::BSplineResampleImageFilterBase<InputImageType, InputImageType> ResamplerType;
    typedef itk::BSplineCenteredResampleImageFilterBase<InputImageType, InputImageType> CenteredResamplerType;
    typedef itk::BSplineL2ResampleImageFilterBase<InputImageType, InputImageType> L2ResamplerType;
    typedef itk::BSplineCenteredL2ResampleImageFilterBase<InputImageType, InputImageType> CenteredL2ResamplerType;
    
    // Filter
    typedef itk::ImageToImageFilter< InputImageType, InputImageType > ImageToImageFilterType;
    typename  ImageToImageFilterType::Pointer filter;

    if (!m_ArgsInfo.up_flag)
      {
	switch (m_ArgsInfo.resamplerType_arg)
	  {
	    
	    // Resampler
	  case 0: {
	    typename itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, ResamplerType >::Pointer df 
	      = itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, ResamplerType >::New();
	    df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	    filter=df;
	    if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with standard Resampler"<<std::endl;
	    break;
	  }
	    
	    //CenteredResamplerType
	  case 1: {
	    typename itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, CenteredResamplerType >::Pointer df 
	      = itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, CenteredResamplerType >::New();
	    df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	    filter=df;
	    if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with centered Resampler"<<std::endl;
	    break;
	  }
	  
	    //L2ResamplerType
	  case 2:
	    {
	      typename itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, L2ResamplerType >::Pointer df 
		= itk::BSplineDownsampleImageFilter< InputImageType,InputImageType,L2ResamplerType >::New();
	      df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);	
	      filter=df;
	      if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with L2 Resampler"<<std::endl;
	      break;
	    }
	  
	    //Centered L2ResamplerType
	  case 3:
	    {
	      typename itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, CenteredL2ResamplerType >::Pointer df 
		= itk::BSplineDownsampleImageFilter< InputImageType,InputImageType,CenteredL2ResamplerType >::New();
	      df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	      filter=df;
	      if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with L2 centered Resampler"<<std::endl;
	      break;
	    }

	  }
      }
    else
      {
	switch (m_ArgsInfo.resamplerType_arg)
	  {
	    
	    // Resampler
	  case 0: {
	    typename itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, ResamplerType >::Pointer df 
	      = itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, ResamplerType >::New();
	    df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	    filter=df;
	    if (m_Verbose) std::cout<<"Using the BSpline upsample image filter with standard Resampler"<<std::endl;
	    break;
	  }
	    
	    //CenteredResamplerType
	  case 1: {
	    typename itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, CenteredResamplerType >::Pointer df 
	      = itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, CenteredResamplerType >::New();
	    df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	    filter=df;
	    if (m_Verbose) std::cout<<"Using the BSpline upsample image filter with centered Resampler"<<std::endl;
	    break;
	  }
	  
	    //L2ResamplerType
	  case 2:
	    {
	      typename itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, L2ResamplerType >::Pointer df 
		= itk::BSplineUpsampleImageFilter< InputImageType,InputImageType,L2ResamplerType >::New();
	      df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);	
	      filter=df;
	      if (m_Verbose) std::cout<<"Using the BSpline upsample image filter with L2 Resampler"<<std::endl;
	      break;
	    }
	  
	    //Centered L2ResamplerType
	  case 3:
	    {
	      typename itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, CenteredL2ResamplerType >::Pointer df 
		= itk::BSplineUpsampleImageFilter< InputImageType,InputImageType,CenteredL2ResamplerType >::New();
	      df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
	      filter=df;
	      if (m_Verbose) std::cout<<"Using the BSpline upsample image filter with L2 centered Resampler"<<std::endl;
	      break;
	    }

	  }
      }


    // Update
    filter->SetInput(input);
    filter->Update();
    typename OutputImageType::Pointer output=filter->GetOutput();
	  
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }
  

  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  VectorType> 
  void 
  BSplinePyramidGenericFilter::UpdateWithDimAndVectorType()
  {

    // ImageTypes
    typedef typename VectorType::ValueType PixelType;
    typedef itk::Image<VectorType, Dimension> VectorInputImageType;
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<PixelType, Dimension> OutputImageType;
    typedef itk::Image<VectorType, Dimension> VectorOutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<VectorInputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename VectorInputImageType::Pointer vectorInput= reader->GetOutput();

    // Process components separetely and gather afterwards
    std::vector<typename OutputImageType::Pointer> components;

    for (unsigned int i=0; i<3;i++)
      {
	// Extract component
	typedef clitk::VectorImageToImageFilter<VectorInputImageType, OutputImageType> FilterType;
	typename FilterType::Pointer dfilter=FilterType::New();
	dfilter->SetInput(vectorInput);
	dfilter->SetComponentIndex(i);
	dfilter->Update();
	typename InputImageType::Pointer input=dfilter->GetOutput();

	//============================================================================================
	// Resampler Types
	typedef itk::BSplineResampleImageFilterBase<InputImageType, InputImageType> ResamplerType;
	typedef itk::BSplineCenteredResampleImageFilterBase<InputImageType, InputImageType> CenteredResamplerType;
	typedef itk::BSplineL2ResampleImageFilterBase<InputImageType, InputImageType> L2ResamplerType;
	typedef itk::BSplineCenteredL2ResampleImageFilterBase<InputImageType, InputImageType> CenteredL2ResamplerType;
	
	// Filter
	typedef itk::ImageToImageFilter< InputImageType, InputImageType > ImageToImageFilterType;
	typename  ImageToImageFilterType::Pointer filter;
	
	if (!m_ArgsInfo.up_flag)
	  {
	    switch (m_ArgsInfo.resamplerType_arg)
	      {
		
		// Resampler
	      case 0: {
		typename itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, ResamplerType >::Pointer df 
		  = itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, ResamplerType >::New();
		df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
		filter=df;
		if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with standard Resampler"<<std::endl;
		break;
	      }
	    
		//CenteredResamplerType
	      case 1: {
		typename itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, CenteredResamplerType >::Pointer df 
		  = itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, CenteredResamplerType >::New();
		df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
		filter=df;
		if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with centered Resampler"<<std::endl;
		break;
	      }
	  
		//L2ResamplerType
	      case 2:
		{
		  typename itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, L2ResamplerType >::Pointer df 
		    = itk::BSplineDownsampleImageFilter< InputImageType,InputImageType,L2ResamplerType >::New();
		  df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);	
		  filter=df;
		  if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with L2 Resampler"<<std::endl;
		  break;
		}
	  
		//Centered L2ResamplerType
	      case 3:
		{
		  typename itk::BSplineDownsampleImageFilter< InputImageType,InputImageType, CenteredL2ResamplerType >::Pointer df 
		    = itk::BSplineDownsampleImageFilter< InputImageType,InputImageType,CenteredL2ResamplerType >::New();
		  df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
		  filter=df;
		  if (m_Verbose) std::cout<<"Using the BSpline downsample image filter with L2 centered Resampler"<<std::endl;
		  break;
		}

	      }
	  }
	else
	  {
	    switch (m_ArgsInfo.resamplerType_arg)
	      {
	    
		// Resampler
	      case 0: {
		typename itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, ResamplerType >::Pointer df 
		  = itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, ResamplerType >::New();
		df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
		filter=df;
		if (m_Verbose) std::cout<<"Using the BSpline upsample image filter with standard Resampler"<<std::endl;
		break;
	      }
	    
		//CenteredResamplerType
	      case 1: {
		typename itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, CenteredResamplerType >::Pointer df 
		  = itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, CenteredResamplerType >::New();
		df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
		filter=df;
		if (m_Verbose) std::cout<<"Using the BSpline upsample image filter with centered Resampler"<<std::endl;
		break;
	      }
	  
		//L2ResamplerType
	      case 2:
		{
		  typename itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, L2ResamplerType >::Pointer df 
		    = itk::BSplineUpsampleImageFilter< InputImageType,InputImageType,L2ResamplerType >::New();
		  df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);	
		  filter=df;
		  if (m_Verbose) std::cout<<"Using the BSpline upsample image filter with L2 Resampler"<<std::endl;
		  break;
		}
	  
		//Centered L2ResamplerType
	      case 3:
		{
		  typename itk::BSplineUpsampleImageFilter< InputImageType,InputImageType, CenteredL2ResamplerType >::Pointer df 
		    = itk::BSplineUpsampleImageFilter< InputImageType,InputImageType,CenteredL2ResamplerType >::New();
		  df->SetSplineOrder(m_ArgsInfo.splineOrder_arg);
		  filter=df;
		  if (m_Verbose) std::cout<<"Using the BSpline upsample image filter with L2 centered Resampler"<<std::endl;
		  break;
		}

	      }
	  }

	// Update
	filter->SetInput(input);
	filter->Update();
	typename OutputImageType::Pointer output=filter->GetOutput();

	//===============================================================================
	// keep component
	components.push_back(output);
      }
    
    // Join
    typedef itk::Compose3DVectorImageFilter<InputImageType,VectorOutputImageType> ComposeFilterType;
    typename ComposeFilterType::Pointer composeFilter=ComposeFilterType::New();
    for (unsigned int i=0; i<3;i++)
      composeFilter->SetInput(i,components[i]);
    composeFilter->Update();
    typename VectorOutputImageType::Pointer vectorOutput = composeFilter->GetOutput();

	  
    // Output
    typedef itk::ImageFileWriter<VectorOutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(vectorOutput);
    writer->Update();

  }
  
}//end clitk
 
#endif //#define clitkBSplinePyramidGenericFilter_txx
