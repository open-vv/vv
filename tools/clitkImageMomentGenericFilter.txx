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
#ifndef clitkImageMomentGenericFilter_txx
#define clitkImageMomentGenericFilter_txx

/* =================================================
 * @file   clitkImageMomentGenericFilter.txx
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
  ImageMomentGenericFilter::UpdateWithDim(std::string PixelType)
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
  ImageMomentGenericFilter::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
     
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();

    // Filter
    typedef itk::ImageMomentsCalculator<InputImageType> MomentsCalculatorType;
    typename    MomentsCalculatorType::Pointer momentsCalculator=MomentsCalculatorType::New();
    momentsCalculator->SetImage(input);
    momentsCalculator->Compute();
    if (m_ArgsInfo.center_flag)
      {
	if (m_Verbose) std::cout<<"The center of gravity is located at (mm) "<<std::endl;
	typename itk::Vector<double,Dimension> center=momentsCalculator->GetCenterOfGravity();
	  std::cout<<center[0];
	for (unsigned int i=1; i<Dimension;i++)
	  std::cout<<" "<<center[i];
	std::cout<<std::endl;
      }
    if (m_ArgsInfo.second_flag)
      {
	if (m_Verbose) std::cout<<"The second order central moments are (mm) "<<std::endl;
	std::cout<<momentsCalculator->GetCentralMoments()<<std::endl;
      }
    if (m_ArgsInfo.axes_flag)
      {
	if (m_Verbose) std::cout<<"The principal axes are (mm) "<<std::endl;
	std::cout<<momentsCalculator->GetPrincipalAxes()<<std::endl;
      }
  
  }


}//end clitk
 
#endif //#define clitkImageMomentGenericFilter_txx
