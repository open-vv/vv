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
#ifndef clitkCalculateDistanceMapGenericFilter_txx
#define clitkCalculateDistanceMapGenericFilter_txx

/* =================================================
 * @file   clitkCalculateDistanceMapGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
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
  CalculateDistanceMapGenericFilter::UpdateWithDim(std::string PixelType)
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
  CalculateDistanceMapGenericFilter::UpdateWithDimAndPixelType()
  {
    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<float, Dimension> OutputImageType; //needs to be floating point
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_ArgsInfo.input_arg);
    typename InputImageType::Pointer input= reader->GetOutput();

    // The fastMarching filter
    typename OutputImageType::Pointer output;
    if (m_ArgsInfo.fastMarching_flag)
      {
// 	typedef itk::FastMarchingImageFilter< OutputImageType, OutputImageType > FastMarchingFilterType;
// 	FastMarchingFilterType::Pointer fastMarchingFilter=FastMarchingFilterType::New();
// 	fastMarchingFilter->
      }

    // The signed Distance map filter
    else 
      {
	typedef itk::SignedDanielssonDistanceMapImageFilter<InputImageType, OutputImageType> DistanceMapImageFilterType;
	typename DistanceMapImageFilterType::Pointer distanceMapImageFilter = DistanceMapImageFilterType::New();
	distanceMapImageFilter->SetInput(input);
	distanceMapImageFilter->SetInsideIsPositive(m_ArgsInfo.invert_flag);
	distanceMapImageFilter->Update();
	output=distanceMapImageFilter->GetOutput();
      }
    
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();
  
  }
  

}//end clitk
 
#endif //#define clitkCalculateDistanceMapGenericFilter_txx
