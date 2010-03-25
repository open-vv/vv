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

