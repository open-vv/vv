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
#ifndef clitkSetBackgroundGenericFilter_cxx
#define clitkSetBackgroundGenericFilter_cxx

/* =================================================
 * @file   clitkSetBackgroundGenericFilter.cxx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

#include "clitkSetBackgroundGenericFilter.h"


namespace clitk
{


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
SetBackgroundGenericFilter::SetBackgroundGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
void SetBackgroundGenericFilter::Update()
{
  // Read the Dimension and PixelType
  int Dimension, Components;
  std::string PixelType;
  //ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);
  ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType, Components);

  if (Dimension > 4) {
    std::cout<<"Error, Only for 2 , 3 or 4 Dimensions!!!"<<std::endl ;
    return;
  }
  
  if (Components > 3) {
    std::cout<<"Error, Only 1, 2, or 3-component images are supported!!!"<<std::endl ;
    return;
  }

  if (Components > 1 && PixelType != "float") {
    std::cout<<"Error, Only float multi-component images are supported!!!"<<std::endl ;
    return;
  }

  switch (Components) {
    case 1:
      // Call UpdateWithDim
      if(Dimension==2) UpdateWithDim<2>(PixelType);
      else if(Dimension==3) UpdateWithDim<3>(PixelType);
      else if (Dimension==4)UpdateWithDim<4>(PixelType);
      break;
    case 2:
    {
      typedef itk::Vector<float, 2> TPixelType;
      if(Dimension==2) UpdateWithDimAndPixelType<2, TPixelType>();
      else if(Dimension==3) UpdateWithDimAndPixelType<3, TPixelType>();
      else if (Dimension==4)UpdateWithDimAndPixelType<4, TPixelType>();
      break;
    }
    case 3:
    {
      typedef itk::Vector<float, 3> TPixelType;
      if(Dimension==2) UpdateWithDimAndPixelType<2, TPixelType>();
      else if(Dimension==3) UpdateWithDimAndPixelType<3, TPixelType>();
      else if (Dimension==4)UpdateWithDimAndPixelType<4, TPixelType>();
      break;
    }
  }
}


} //end clitk

#endif  //#define clitkSetBackgroundGenericFilter_cxx
