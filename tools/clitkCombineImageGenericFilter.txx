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
#ifndef clitkCombineImageGenericFilter_txx
#define clitkCombineImageGenericFilter_txx

/* =================================================
 * @file   clitkCombineImageGenericFilter.txx
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
  CombineImageGenericFilter::UpdateWithDim(std::string PixelType, int Components)
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
	UpdateWithDimAndPixelType<Dimension, itk::Vector<float, Dimension> >();
      }
    else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  CombineImageGenericFilter::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<int, Dimension> MaskImageType;
    typedef itk::Image<PixelType, Dimension> OutputImageType;
    
    // Read the inputs
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input1= reader->GetOutput();
    typename InputReaderType::Pointer reader2 = InputReaderType::New();
    reader2->SetFileName( m_ArgsInfo.input2_arg);
    reader2->Update();
    typename InputImageType::Pointer input2= reader2->GetOutput();

    // Read the mask
    typedef itk::ImageFileReader<MaskImageType> MaskReaderType;
    typename MaskReaderType::Pointer mReader = MaskReaderType::New();
    mReader->SetFileName( m_ArgsInfo.mask_arg);
    mReader->Update();
    typename MaskImageType::Pointer mask= mReader->GetOutput();

    // Filter
    typedef CombineImageFilter<InputImageType> CombineImageFilterType;
    typename CombineImageFilterType::Pointer filter=CombineImageFilterType::New();
    filter->SetInput( input1 );
    filter->SetInput(1, input2 );
    filter->SetMask( mask );
    filter->Update();
    typename OutputImageType::Pointer output=filter->GetOutput();
   
    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkCombineImageGenericFilter_txx
