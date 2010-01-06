/*=========================================================================

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

#ifndef CLITKIMAGEARITHMGENERICFILTER_CXX
#define CLITKIMAGEARITHMGENERICFILTER_CXX

/**
 -------------------------------------------------------------------
 * @file   clitkImageArithmGenericFilter.cxx
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008

 * @brief  
 -------------------------------------------------------------------*/

#include "clitkImageArithmGenericFilter.h"

//--------------------------------------------------------------------
clitk::ImageArithmGenericFilter::ImageArithmGenericFilter():mTypeOfOperation(0) {
  mIsOperationUseASecondImage = false;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageArithmGenericFilter::Update () {
  
  // Load image header
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(mInputFilenames[0]);
  
  // Determine dim, pixel type, number of components
  mDim = header->GetNumberOfDimensions();
  mPixelTypeName = header->GetComponentTypeAsString(header->GetComponentType());  
  mNbOfComponents = header->GetNumberOfComponents();
  
  if (mInputFilenames.size() == 0) {
    std::cerr << "ERROR : please provide at least a input filename." << std::endl;
  }
  if (mInputFilenames.size() == 1) {
    mIsOperationUseASecondImage = false;
  }
  if (mInputFilenames.size() == 2) {
    mIsOperationUseASecondImage = true;
  }
  if (mInputFilenames.size() > 2) {
    std::cerr << "ERROR : please provide only 1 or 2 input filenames." << std::endl;
  }

  // Switch by dimension
  if (mDim == 2) { Update_WithDim<2>(); return; }
  if (mDim == 3) { Update_WithDim<3>(); return; }
  if (mDim == 4) { Update_WithDim<4>(); return; }

  std::cerr << "Error, dimension of input image is " << mDim << ", but I only work with 2,3." << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

#endif //define CLITKIMAGEARITHMGENERICFILTER_CXX
