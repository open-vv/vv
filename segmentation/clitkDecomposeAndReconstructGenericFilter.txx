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
#ifndef clitkDecomposeAndReconstructGenericFilter_txx
#define clitkDecomposeAndReconstructGenericFilter_txx

namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<unsigned int Dimension>
  void 
  DecomposeAndReconstructGenericFilter::UpdateWithDim(std::string PixelType)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

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
      std::cout  << "WARNING: Working in unsigned pixelType, remember to set the Erosion Padding Value to a suitable value!"<< std::endl;
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


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  DecomposeAndReconstructGenericFilter::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<int, Dimension> InternalImageType;
    typedef itk::Image<PixelType, Dimension> OutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();

    // Structuring element radius
    typename InputImageType::SizeType radius;
    if (m_ArgsInfo.radius_given==Dimension)
      for (unsigned int i=0;i<Dimension;i++)
	{radius[i]=m_ArgsInfo.radius_arg[i];}
    else 
      for (unsigned int i=0;i<Dimension;i++)
	radius[i]=m_ArgsInfo.radius_arg[0];

    // Cast
    typedef itk::CastImageFilter<InputImageType, InternalImageType> InputCastImageFilterType;
    typename InputCastImageFilterType::Pointer inputCaster= InputCastImageFilterType::New();
    inputCaster->SetInput(input);
    inputCaster->Update();

    // Filter
    typedef itk::ImageToImageFilter<InternalImageType, InternalImageType> ImageToImageFilterType;
    typename ImageToImageFilterType::Pointer filter;

    switch (m_ArgsInfo.type_arg)
      {
      case 0:
	{
	  typedef clitk::DecomposeThroughErosionImageFilter<InternalImageType,InternalImageType> FilterType;
	  typename FilterType::Pointer f=FilterType::New();
	  f->SetVerbose(m_Verbose);
	  f->SetFullyConnected(m_ArgsInfo.full_flag);
	  f->SetErosionPaddingValue(m_ArgsInfo.pad_arg);
	  f->SetMinimumNumberOfIterations(m_ArgsInfo.min_arg);
	  f->SetMinimumObjectSize(m_ArgsInfo.minSize_arg);
	  f->SetRadius(radius);
	  f->SetNumberOfNewLabels(m_ArgsInfo.new_arg);
	  if(m_Verbose) std::cout<<"Using the DecomposeThroughErosionImageFilter..."<<std::endl;
	  filter=f;
	  break;
	}
   
      case 1:
	{
	  typedef clitk::ReconstructThroughDilationImageFilter<InternalImageType,InternalImageType> FilterType;
	  typename FilterType::Pointer f=FilterType::New();
	  f->SetVerbose(m_Verbose);
	  f->SetErosionPaddingValue(m_ArgsInfo.pad_arg);
	  f->SetRadius(radius);
	  f->SetMaximumNumberOfLabels(m_ArgsInfo.max_arg);
	  f->SetBackgroundValue(m_ArgsInfo.bg_arg);
	  f->SetForegroundValue(m_ArgsInfo.fg_arg);
	  if(m_Verbose) std::cout<<"Using the ReconstructThroughDilationImageFilter..."<<std::endl;
	  filter=f;
	  break;
	}
      case 2:
	{
	  typedef clitk::DecomposeAndReconstructImageFilter<InternalImageType,InternalImageType> FilterType;
	  typename FilterType::Pointer f=FilterType::New();
	  f->SetVerbose(m_Verbose);
	  f->SetRadius(radius);
	  f->SetMaximumNumberOfLabels(m_ArgsInfo.max_arg);
	  f->SetMinimumNumberOfIterations(m_ArgsInfo.min_arg);
	  f->SetBackgroundValue(m_ArgsInfo.bg_arg);
	  f->SetForegroundValue(m_ArgsInfo.fg_arg);
	  f->SetFullyConnected(m_ArgsInfo.full_flag);
	  f->SetNumberOfNewLabels(m_ArgsInfo.new_arg);
	  f->SetMinimumObjectSize(m_ArgsInfo.minSize_arg);
	  if(m_Verbose) std::cout<<"Using the DecomposeAndReconstructImageFilter..."<<std::endl;
	  filter=f;
	  break;
	}
      }
    
    filter->SetInput(inputCaster->GetOutput());
    filter->Update();
    typename InternalImageType::Pointer output= filter->GetOutput();

    // Cast
    typedef itk::CastImageFilter<InternalImageType, OutputImageType> OutputCastImageFilterType;
    typename OutputCastImageFilterType::Pointer outputCaster= OutputCastImageFilterType::New();
    outputCaster->SetInput(output);
    outputCaster->Update();

    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(outputCaster->GetOutput());
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkDecomposeAndReconstructGenericFilter_txx
