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
#ifndef clitkDVHGenericFilter_cxx
#define clitkDVHGenericFilter_cxx

#include "clitkDVHGenericFilter.h"

namespace clitk
{

  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  DVHGenericFilter::DVHGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }
  //-----------------------------------------------------------
  
  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void DVHGenericFilter::Update()
  {
    // Read the Dimension and PixelType
    int Dimension, Components;
    std::string PixelType;
    ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType, Components);
    
    if (m_ArgsInfo.channel_arg < -1 || m_ArgsInfo.channel_arg >= Components) {
      std::cout << "Invalid image channel" << std::endl;
      return;
    }
    
    if (m_ArgsInfo.mask_given) {
      int maskDimension, maskComponents;
      std::string maskPixelType;
      ReadImageDimensionAndPixelType(m_ArgsInfo.mask_arg, maskDimension, maskPixelType, maskComponents);
      if (!(maskDimension == Dimension || maskDimension == (Dimension - 1))) {
        std::cout << "Dimension of label mask must be equal to the (d)imension of the input image or d-1." << std::endl;
        return;
      }
    }

    
    // Call UpdateWithDim
    if (Dimension==2) {
      switch (Components) {
        case 1: 
          UpdateWithDim<2,1>(PixelType);
          break;
        case 2: 
          UpdateWithDim<2,2>(PixelType);
          break;
        case 3: 
          UpdateWithDim<2,3>(PixelType);
          break;
        default:
          std::cout << "Unsupported number of channels" << std::endl;
          break;
      }
    }
    else if (Dimension==3) {
      switch (Components) {
        case 1: 
          UpdateWithDim<3,1>(PixelType);
          break;
        case 2: 
          UpdateWithDim<3,2>(PixelType);
          break;
        case 3: 
          UpdateWithDim<3,3>(PixelType);
          break;
        default:
          std::cout << "Unsupported number of channels" << std::endl;
          break;
      }
    }
    else if (Dimension==4) {
      switch (Components) {
        case 1: 
          UpdateWithDim<4,1>(PixelType);
          break;
        case 2: 
          UpdateWithDim<4,2>(PixelType);
          break;
        case 3: 
          UpdateWithDim<4,3>(PixelType);
          break;
        default:
          std::cout << "Unsupported number of channels" << std::endl;
          break;
      }
    }
    else {
      std::cout<<"Error, Only for 2 or 3  Dimensions!!!"<<std::endl ;
      return;
    }
  }


} //end clitk

#endif  //#define clitkDVHGenericFilter_cxx
