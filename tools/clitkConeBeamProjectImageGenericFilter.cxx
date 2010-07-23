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
#ifndef CLITKCONEBEAMPROJECTIMAGEGENERICFILTER_CXX
#define CLITKCONEBEAMPROJECTIMAGEGENERICFILTER_CXX

/**
 * @file   clitkConeBeamProjectImageGenericFilter.cxx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   Wed April 30  13:47:57 2008
 * 
 * @brief  
 * 
 * 
 */

#include "clitkConeBeamProjectImageGenericFilter.h"

namespace clitk
{

  //====================================================================
  // Constructor
  ConeBeamProjectImageGenericFilter::ConeBeamProjectImageGenericFilter()
  {

    m_Verbose=false;
    m_InputFileName="";

//     mIsoCenter.resize(3);
//     for (unsigned int j=0; j < 3; j++) mIsoCenter[j]=128.;
//     mSourceToScreen=1536.;
//     mSourceToAxis=1000.;
//     mProjectionAngle=0.;

//     // set the rigid transform matrix to the identity
//     for (unsigned int i=0; i <4; i++)
//       for (unsigned int j=0; j <4; j++)
// 	if (i==j)mRigidTransformMatrix[i][j]=1.;
// 	else mRigidTransformMatrix[i][j]=0.;

//     //Padding value
//     mEdgePaddingValue=0.0;
  }

  //====================================================================
  // Update
  void ConeBeamProjectImageGenericFilter::Update()
  {
    //Read the PixelType and dimension of the input image
    int Dimension;
    std::string PixelType;
    clitk::ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);
    if (Dimension == 3)
      {
	if (m_Verbose) std::cout  << "Input was detected to be "<< Dimension << "D and " << PixelType << "..." << std::endl;
  
	if(PixelType == "short"){  
	  if (m_Verbose) std::cout  << "Launching Projection in signed short..." << std::endl;
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
    
    else {itkExceptionMacro(<< "Input Image dimension is " << Dimension 
			    << " but I can only work on 3D images.");
    }
  }


  //====================================================================

}
#endif  //#define CLITKCONEBEAMPROJECTIMAGEGENERICFILTER_CXX
