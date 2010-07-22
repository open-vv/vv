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
#ifndef clitkMorphoMathGenericFilter_txx
#define clitkMorphoMathGenericFilter_txx
/**
   =================================================
   * @file   clitkMorphoMathGenericFilter.txx
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   5 May 2009
   * 
   * @brief 
   * 
   =================================================*/


namespace clitk
{


  //==============================================================================
  // Update with the number of dimensions
  //==============================================================================
  template<unsigned int Dimension>
  void 
  MorphoMathGenericFilter::UpdateWithDim(std::string PixelType)
  {
    if(PixelType == "short"){  
      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, signed short>(); 
    }
    //    else if(PixelType == "unsigned_short"){  
    //      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    //      UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
    //    }
    
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


  //==============================================================================
  // Update with the pixel type
  //==============================================================================
  template <unsigned int Dimension, class  InputPixelType> 
  void 
  MorphoMathGenericFilter::UpdateWithDimAndPixelType()
  {
   
    //---------------------------------
    // Define the images
    //---------------------------------
    typedef float InternalPixelType;
    typedef itk::Image<InputPixelType, Dimension> InputImageType;
    typedef itk::Image<InternalPixelType, Dimension> InternalImageType;
    typedef itk::Image<InputPixelType, Dimension> OutputImageType;
    typedef itk::ImageFileReader<InputImageType> FileReaderType;
    typename FileReaderType::Pointer fileReader=FileReaderType::New();
    fileReader->SetFileName(m_InputFileName);
    typedef itk::CastImageFilter<InputImageType, InternalImageType> InputCastImageFilterType;
    typename InputCastImageFilterType::Pointer caster = InputCastImageFilterType::New();
    caster->SetInput(fileReader->GetOutput());
    caster->Update();
    typename InternalImageType::Pointer input =caster->GetOutput();


    //---------------------------------
    // Find the type of action
    //---------------------------------
    typedef itk::ImageToImageFilter<InternalImageType, InternalImageType> ImageFilterType;
    typename ImageFilterType::Pointer filter; 

    typedef itk::BinaryBallStructuringElement<InputPixelType,Dimension > KernelType;
    KernelType structuringElement;
    typename InternalImageType::SizeType radius;
    if (m_ArgsInfo.radius_given==Dimension)
      for (unsigned int i=0;i<Dimension;i++)
	{radius[i]=m_ArgsInfo.radius_arg[i];}
    else 
      for (unsigned int i=0;i<Dimension;i++)
	radius[i]=m_ArgsInfo.radius_arg[0];

    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();

    switch(m_ArgsInfo.type_arg)
      {

      case 0:
	{
	  typedef itk::BinaryErodeImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetBackgroundValue(m_ArgsInfo.bg_arg);
	  m->SetForegroundValue(m_ArgsInfo.fg_arg);
	  m->SetBoundaryToForeground(m_ArgsInfo.bound_flag);
          m->SetKernel(structuringElement);
          
	  filter=m;
	  if(m_Verbose) std::cout<<"Using the erode filter..."<<std::endl;
	  break;
	}

      case 1:
	{
	  typedef itk::BinaryDilateImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetBackgroundValue(m_ArgsInfo.bg_arg);
	  m->SetForegroundValue(m_ArgsInfo.fg_arg);
	  m->SetBoundaryToForeground(m_ArgsInfo.bound_flag);
          m->SetKernel(structuringElement);

	  filter=m;
	  if(m_Verbose) std::cout<<"Using the dilate filter..."<<std::endl;
	  break;
	}

      case 2:
	{
	  typedef itk::BinaryMorphologicalClosingImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetForegroundValue(m_ArgsInfo.fg_arg);
	  m->SetSafeBorder(m_ArgsInfo.bound_flag);
	  m->SetKernel(structuringElement);

	  filter=m;
	  if(m_Verbose) std::cout<<"Using the closing filter..."<<std::endl;
	  break;
	}

      case 3:
	{
	  typedef itk::BinaryMorphologicalOpeningImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetBackgroundValue(m_ArgsInfo.bg_arg);
	  m->SetForegroundValue(m_ArgsInfo.fg_arg);
	  m->SetKernel(structuringElement);

	  filter=m;
	  if(m_Verbose) std::cout<<"Using the opening filter..."<<std::endl;
	  break;
	}

      case 4:
	{
	  typedef clitk::ConditionalBinaryErodeImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetBackgroundValue(m_ArgsInfo.bg_arg);
	  m->SetForegroundValue(m_ArgsInfo.fg_arg);
	  m->SetBoundaryToForeground(m_ArgsInfo.bound_flag);
          m->SetKernel(structuringElement);
          
	  filter=m;
	  if(m_Verbose) std::cout<<"Using the conditional erode filter..."<<std::endl;
	  break;
	}

      case 5:
	{
	  typedef clitk::ConditionalBinaryDilateImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetBackgroundValue(m_ArgsInfo.bg_arg);
	  m->SetForegroundValue(m_ArgsInfo.fg_arg);
	  m->SetBoundaryToForeground(m_ArgsInfo.bound_flag);
          m->SetKernel(structuringElement);
          
	  filter=m;
	  if(m_Verbose) std::cout<<"Using the conditional dilate filter..."<<std::endl;
	  break;
	}

      }
  

    //---------------------------------
    // Execute the filter
    //---------------------------------
    filter->SetInput(input);
    
    try 
      {
	filter->Update();
      }
    catch( itk::ExceptionObject & err ) 
      { 
	std::cerr << "ExceptionObject caught executing the filter!" << std::endl; 
	std::cerr << err << std::endl; 
	return;
      } 
    

    //---------------------------------
    // Write the output
    //---------------------------------
    typedef itk::CastImageFilter< InternalImageType, OutputImageType > OutputCastImageFilterType;
    typename OutputCastImageFilterType::Pointer oCaster = OutputCastImageFilterType::New();
    oCaster->SetInput(filter->GetOutput());
    typedef itk::ImageFileWriter<OutputImageType> FileWriterType;
    typename FileWriterType::Pointer writer=FileWriterType::New();
    writer->SetInput(oCaster->GetOutput());
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->Update();
   
  }

}//end namespace
 
#endif //#define clitkMorphoMathGenericFilter_txx
