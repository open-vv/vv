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
#ifndef clitkExtractSliceGenericFilter_txx
#define clitkExtractSliceGenericFilter_txx

/* =================================================
 * @file   clitkExtractSliceGenericFilter.txx
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
  ExtractSliceGenericFilter::UpdateWithDim(std::string PixelType)
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
  ExtractSliceGenericFilter::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<PixelType, Dimension-1> OutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();

    // Filter
    typedef clitk::ExtractImageFilter<InputImageType, OutputImageType> ExtractImageFilterType;
    typename ExtractImageFilterType::Pointer extractFilter=ExtractImageFilterType::New();
    extractFilter->SetInput(input);
    
    // Verify input
    if( (m_ArgsInfo.axe_arg<0) || (static_cast<unsigned int >(m_ArgsInfo.axe_arg) >= Dimension) ) 
      {
	std::cerr<<"The axe index does not name a dimension!!"<<std::endl;
	return;
      }
    typename InputImageType::RegionType region= input->GetLargestPossibleRegion();
    typename InputImageType::RegionType::SizeType size= region.GetSize();
    
    if( (m_ArgsInfo.slice_arg<0) || (static_cast<unsigned int>(m_ArgsInfo.slice_arg) >= size[static_cast<unsigned int >(m_ArgsInfo.axe_arg)]) ) 
      {
	std::cerr<<"The slice index does not name a slice!!"<<std::endl;
	return;
      }

    // Collapse dimension
    size[m_ArgsInfo.axe_arg]=0;
    typename InputImageType::IndexType start; 
    start.Fill(0);
    start[m_ArgsInfo.axe_arg]=m_ArgsInfo.slice_arg;
    region.SetSize( size );
    region.SetIndex(start);
    extractFilter->SetExtractionRegion(region);
    extractFilter->Update();
    typename OutputImageType::Pointer output =extractFilter->GetOutput();
   
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkExtractSliceGenericFilter_txx
