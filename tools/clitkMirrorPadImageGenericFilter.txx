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
#ifndef clitkMirrorPadImageGenericFilter_txx
#define clitkMirrorPadImageGenericFilter_txx

/* =================================================
 * @file   clitkMirrorPadImageGenericFilter.txx
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
  MirrorPadImageGenericFilter::UpdateWithDim(std::string PixelType)
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
  MirrorPadImageGenericFilter::UpdateWithDimAndPixelType()
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

    // Filter
    typedef itk::MirrorPadImageFilter<InputImageType, OutputImageType> MirrorPadImageFilterType;
    typename MirrorPadImageFilterType::Pointer mirrorPadImageFilter=MirrorPadImageFilterType::New();
    mirrorPadImageFilter->SetInput(input);

    //Get the padding size
    typename InputImageType::SizeType padSize;

    // Mirror the entire image
    if (m_ArgsInfo.mirror_given)
      {
	typename InputImageType::SizeType inputSize=input->GetLargestPossibleRegion().GetSize();
	padSize.Fill(0);
	padSize[m_ArgsInfo.mirror_arg]=inputSize[m_ArgsInfo.mirror_arg];
	if (m_Verbose) std::cout<<"Mirroring the entire image along axis "<< m_ArgsInfo.mirror_arg<<"..."<<std::endl;
      }

    // Pad with a given size
    else
      {
	if(m_ArgsInfo.pad_given)
	  for(unsigned int i=0; i<Dimension; i++)
	    padSize[i]=m_ArgsInfo.pad_arg[i];
	if (m_Verbose) std::cout<<"Mirror padding with size "<< padSize<<"..."<<std::endl;
      }

    // Lower or upper?
    if (m_ArgsInfo.lower_given)
      mirrorPadImageFilter->SetPadLowerBound(padSize);
    
    else
      mirrorPadImageFilter->SetPadUpperBound(padSize);
  
    mirrorPadImageFilter->Update();
    typename  OutputImageType::Pointer output = mirrorPadImageFilter->GetOutput();

    // Origin?
    typename OutputImageType::PointType origin;
    origin.Fill(itk::NumericTraits<double>::Zero);
    if (m_ArgsInfo.origin_flag)
      {
	output->Update();
	output->SetOrigin(origin);
	if (m_Verbose) std::cout<<"Setting origin to  "<< output->GetOrigin()<<"..."<<std::endl;
      }

    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();
  }


}//end clitk
 
#endif //#define clitkMirrorPadImageGenericFilter_txx
