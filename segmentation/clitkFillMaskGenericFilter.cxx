#ifndef clitkFillMaskGenericFilter_cxx
#define clitkFillMaskGenericFilter_cxx

/* =================================================
 * @file   clitkFillMaskGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkFillMaskGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  FillMaskGenericFilter::FillMaskGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }


  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void FillMaskGenericFilter::Update()
  {
    // Read the Dimension and PixelType
    int Dimension;
    std::string PixelType;
    ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);

    
    // Call UpdateWithPixelType
     if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;
     if (Dimension != 3) 
       {
	 std::cerr<<"Error: Only for 3D!!"<<std::endl;
	 return;
       }
     
     if(PixelType == "short"){  
       if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
       UpdateWithPixelType<signed short>(); 
     }
     
     //    else if(PixelType == "unsigned_short"){  
     //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
     //       UpdateWithPixelType<unsigned short>(); 
     //     }
     
     else if (PixelType == "unsigned_char"){ 
       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
       UpdateWithPixelType<unsigned char>();
     }
     
     //     else if (PixelType == "char"){ 
     //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
     //       UpdateWithPixelType<signed char>();
     //     }
     
     else {
       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
       UpdateWithPixelType<float>();
     }
  }


} //end clitk

#endif  //#define clitkFillMaskGenericFilter_cxx
