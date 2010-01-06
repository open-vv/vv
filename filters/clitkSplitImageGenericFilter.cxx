/*=========================================================================

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

/**
 -------------------------------------------------------------------
 * @file   clitkSplitImageGenericFilter.cxx
 * @author Joël Schaerer
 * @date   20 April 2009

 * @brief  
 -------------------------------------------------------------------*/

#include "clitkSplitImageGenericFilter.h"

#include "clitkSplitImageGenericFilter.txx"
//--------------------------------------------------------------------
clitk::SplitImageGenericFilter::SplitImageGenericFilter() {
  mSplitDimension = 0;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::SplitImageGenericFilter::Update () {
  
    // Read the Dimension and PixelType
    int Dimension, Components;
    std::string PixelType;
    ReadImageDimensionAndPixelType(this->mInputFilenames[0], Dimension, PixelType, Components);

    
    // Call UpdateWithDim
    if(Dimension==3) UpdateWithDim<3>(PixelType, Components);
    else if (Dimension==4)UpdateWithDim<4>(PixelType, Components); 
    else 
      {
	std::cout<<"Error, Only for 3 or 4  Dimensions!!!"<<std::endl ;
	return;
      }
}
//--------------------------------------------------------------------
