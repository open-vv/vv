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
#ifndef CLITKIMAGEUTILITIES_H
#define CLITKIMAGEUTILITIES_H
/**
 ===================================================================
 * @file   clitkImageUtilities.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   22 Sep 2006 10:38:36

 * @brief  

 ===================================================================*/

// clitk
#include "clitkCommon.h"
#include "clitkImageCommon.h"

// std
#include <vector>
#include <map>
#include <set>

// itk
#include "itkImageRegionConstIterator.h"

namespace clitk {
  
  template<class ImageType>
  int ComputeHowManyDifferentIntensity(const typename ImageType::Pointer & image, 
									   std::vector<typename ImageType::PixelType> & listOfIntensities);
  #include "clitkImageUtilities.txx"

} // end namespace

#endif /* end #define CLITKIMAGEUTILITIES_H */

