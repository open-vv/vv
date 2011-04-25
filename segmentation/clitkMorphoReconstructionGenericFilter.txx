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
#ifndef clitkMorphoReconstructionGenericFilter_txx
#define clitkMorphoReconstructionGenericFilter_txx

/* =================================================
 * @file   clitkMorphoReconstructionGenericFilter.txx
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
  MorphoReconstructionGenericFilter::UpdateWithDim(std::string PixelType)
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
  MorphoReconstructionGenericFilter::UpdateWithDimAndPixelType()
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

    // Read the mask
    typename InputImageType::Pointer mask;
    if (m_ArgsInfo.mask_given)
      {
	typename InputReaderType::Pointer maskReader=InputReaderType::New();
	maskReader->SetFileName(m_ArgsInfo.mask_arg);
	maskReader->Update();
	mask=maskReader->GetOutput();
      }


    //---------------------------------
    // Find the type of action
    //---------------------------------
    typedef itk::ImageToImageFilter<InputImageType, InputImageType> ImageFilterType;
    typename ImageFilterType::Pointer filter; 

    typedef itk::BinaryBallStructuringElement<PixelType, Dimension > KernelType;
    KernelType structuringElement;
    structuringElement.SetRadius(m_ArgsInfo.radius_arg);
    structuringElement.CreateStructuringElement();


    switch(m_ArgsInfo.type_arg)
      {

      case 0:
	{
	  typedef itk::ReconstructionByErosionImageFilter<InputImageType, OutputImageType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetMarkerImage(input);
	  m->SetMaskImage(mask);

	  filter=m;
	  if(m_Verbose) std::cout<<"Using the reconstruction by erosion filter..."<<std::endl;
	  break;
	}

      case 1:
	{
	  typedef itk::ReconstructionByDilationImageFilter<InputImageType, OutputImageType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetMarkerImage(input);
	  m->SetMaskImage(mask);

	  filter=m;
	  if(m_Verbose) std::cout<<"Using the reconstruction by dilation filter..."<<std::endl;
	  break;
	  break;
	}

      case 2:
	{
	  typedef itk::ClosingByReconstructionImageFilter<InputImageType, OutputImageType, KernelType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetInput(input);
	  m->SetKernel(structuringElement);
	  m->SetFullyConnected(m_ArgsInfo.full_flag);
	  m->SetPreserveIntensities(m_ArgsInfo.int_flag);

	  filter=m;
	  if(m_Verbose) std::cout<<"Using the closing by reconstruction filter..."<<std::endl;
	  break;
	 
	}

      case 3:
	{
	  typedef itk::OpeningByReconstructionImageFilter<InputImageType, OutputImageType, KernelType> FilterType;
	  typename FilterType::Pointer m = FilterType::New();
	  m->SetInput(input);
	  m->SetKernel(structuringElement);
	  m->SetFullyConnected(m_ArgsInfo.full_flag);
	  m->SetPreserveIntensities(m_ArgsInfo.int_flag);

	  filter=m;
	  if(m_Verbose) std::cout<<"Using the opening by reconstruction filter..."<<std::endl;
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
    typename OutputImageType::Pointer output=filter->GetOutput();
 

    
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkMorphoReconstructionGenericFilter_txx
