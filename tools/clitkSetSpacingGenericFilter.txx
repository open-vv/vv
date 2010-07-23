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
#ifndef clitkSetSpacingGenericFilter_txx
#define clitkSetSpacingGenericFilter_txx

/* =================================================
 * @file   clitkSetSpacingGenericFilter.txx
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
  SetSpacingGenericFilter::UpdateWithDim(std::string PixelType , unsigned int Components)
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
	UpdateWithDimAndPixelType<Dimension, itk::Vector<float, 3> >();
      }

    else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;
  }



  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  SetSpacingGenericFilter::UpdateWithDimAndPixelType()
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
    typename InputImageType::SpacingType spacing;
    spacing.Fill(1.0);

   // Like?
    if (m_ArgsInfo.like_given)
      {
	typedef itk::ImageFileReader<InputImageType> InputReaderType;
	typename InputReaderType::Pointer likeReader = InputReaderType::New();
	likeReader->SetFileName( m_ArgsInfo.like_arg);
	likeReader->Update();
	typename InputImageType::Pointer like= likeReader->GetOutput();
	spacing=like->GetSpacing();
      }
    else
      {
	if (m_ArgsInfo.spacing_given==Dimension)
	  for(unsigned int i=0; i<Dimension;i++)
	    spacing[i]=m_ArgsInfo.spacing_arg[i];
	else
	  for(unsigned int i=0; i<Dimension;i++)
	    spacing[i]=m_ArgsInfo.spacing_arg[0];
      }
    input->SetSpacing(spacing);

    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(input);
    writer->Update();
    
  }


}//end clitk
 
#endif //#define clitkSetSpacingGenericFilter_txx
