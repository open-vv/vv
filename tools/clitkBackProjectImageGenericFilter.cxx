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
#ifndef CLITKBACKPROJECTIMAGEGENERICFILTER_CXX
#define CLITKBACKPROJECTIMAGEGENERICFILTER_CXX

/**
 * @file   clitkBackProjectImageGenericFilter.cxx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   Wed April 30  13:47:57 2008
 * 
 * @brief  
 * 
 * 
 */

#include "clitkBackProjectImageGenericFilter.h"

namespace clitk
{

  //====================================================================
  // Constructor
  BackProjectImageGenericFilter::BackProjectImageGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }

  //====================================================================
  // Update
  void BackProjectImageGenericFilter::Update()
  {

    //Read the PixelType and dimension of the input image
    int Dimension;
    std::string PixelType;
    clitk::ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);

    //Launch for the right pixelType
    if (Dimension==2)
      {   
	if (m_Verbose) std::cout  << "Input was detected to be "<< Dimension << "D and " << PixelType << "..." << std::endl;
	
	
	if(PixelType == "short"){  
	  if (m_Verbose) std::cout  << "Launching back projection in signed short..." << std::endl;
	  UpdateWithPixelType<signed short>(); 
	}
	
// 	else if(PixelType == "unsigned_short"){  
// 	  if (m_Verbose) std::cout  << "Launching Projection in unsigned_short..." << std::endl;
// 	  UpdateWithPixelType<unsigned short>(); 
// 	}
	
	else if (PixelType == "unsigned_char"){ 
	  if (m_Verbose) std::cout  << "Launching Projection in unsigned_char..." << std::endl;
	  UpdateWithPixelType<unsigned char>();
	}
	
// 	else if (PixelType == "char"){ 
// 	  if (m_Verbose) std::cout  << "Launching Projection in signed_char..." << std::endl;
// 	  UpdateWithPixelType<signed char>();
// 	}
	else {
	  if (m_Verbose) std::cout  << "Launching Projection in float..." << std::endl;
	  UpdateWithPixelType<float>();
	}
      }
    else std::cerr<<"The input image should be 2D!"<<std::endl;

  }


  //====================================================================

}
#endif  //#define CLITKBACKPROJECTIMAGEGENERICFILTER_CXX
