/*=========================================================================

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

#ifndef CLITKIMAGEFILLREGIONGENERICFILTER_CXX
#define CLITKIMAGEFILLREGIONGENERICFILTER_CXX

/**
 -------------------------------------------------------------------
 * @file   clitkImageFillRegionGenericFilter.cxx
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008

 * @brief  
 -------------------------------------------------------------------*/

#include "clitkImageFillRegionGenericFilter.h"

//--------------------------------------------------------------------
clitk::ImageFillRegionGenericFilter::ImageFillRegionGenericFilter() {
  mPixelValue = 0;
  m_IsCentered=false;
  mSphericRegion=false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageFillRegionGenericFilter::SetSphericRegion(std::vector<double> &  radius, 
  							   std::vector<double> & center) 
{
  mRadius.clear(); 
  mRadius.resize(radius.size());
  std::copy(radius.begin(), radius.end(), mRadius.begin());
  mCenter.clear();
  mCenter.resize(center.size());
  std::copy(center.begin(), center.end(), mCenter.begin());
  mSphericRegion = true;
  m_IsCentered=false;
}

void clitk::ImageFillRegionGenericFilter::SetSphericRegion(std::vector<double> & radius) {
  mRadius.clear(); 
  mRadius.resize(radius.size());
  std::copy(radius.begin(), radius.end(), mRadius.begin());
  m_IsCentered=true;
  mSphericRegion = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageFillRegionGenericFilter::Update () {
  
  // Load image header
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(mInputFilenames[0]);
  
  // Determine dim, pixel type, number of components
  mDim = header->GetNumberOfDimensions();
  mPixelTypeName = header->GetComponentTypeAsString(header->GetComponentType());  
  mNbOfComponents = header->GetNumberOfComponents();
  
  // Switch by dimension
  if (mDim == 2) { Update_WithDim<2>(); return; }
  if (mDim == 3) { Update_WithDim<3>(); return; }
  // if (mDim == 4) { Update_WithDim<4>(); return; }

  std::cerr << "Error, dimension of input image is " << mDim << ", but I only work with 2,3,4." << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

#endif //define CLITKIMAGEFILLREGIONGENERICFILTER_CXX
