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
#ifndef CLITKIMAGEUTILITIES_TXX
#define CLITKIMAGEUTILITIES_TXX
/**
 =================================================
 * @file   clitkImageUtilities.txx
 * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
 * @date   22 Sep 2006 10:39:48
 * 
 * @brief  
 * 
 * 
 =================================================*/

//====================================================================
// Compute the number of different intensities in an image
template<class ImageType>
int ComputeHowManyDifferentIntensity(const typename ImageType::Pointer & image, 
									 std::vector<typename ImageType::PixelType> & l)
{
  //std::set<typename ImageType::PixelType> listOfIntensities;
  std::map<typename ImageType::PixelType, bool> listOfIntensities;
  //  listOfIntensities.resize(0);
  typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  ConstIteratorType pi(image, image->GetLargestPossibleRegion());
  pi.Begin();
  while (!pi.IsAtEnd()) {
	if (!listOfIntensities[pi.Get()]) listOfIntensities[pi.Get()] = true;
	// if (std::find(listOfIntensities.begin(), 
// 				  listOfIntensities.end(), 
// 				  pi.Get()) == listOfIntensities.end()) {
// 	  listOfIntensities.insert(pi.Get());
// 	}
	++pi;
  }
  
  //typename std::set<typename ImageType::PixelType>::const_iterator ppi = listOfIntensities.begin();
  typename std::map<typename ImageType::PixelType, bool>::const_iterator ppi = listOfIntensities.begin();
  while (ppi != listOfIntensities.end()) {
	l.push_back(ppi->first);
	++ppi;
  }

  return listOfIntensities.size();
}
//====================================================================
  
//====================================================================
template<class InputImageType, class MaskImageType>
void ComputeWeightsOfEachClasses(const typename InputImageType::Pointer & input, 
								 const typename MaskImageType::Pointer & mask,
								 const std::vector<typename MaskImageType::PixelType> & listOfIntensities, 
								 std::map<typename MaskImageType::PixelType, 
								 std::map<typename InputImageType::PixelType, double> > & mapOfLabelsAndWeights) {
  // Check size
  if (input->GetLargestPossibleRegion() != mask->GetLargestPossibleRegion()) {
	itkGenericExceptionMacro(<< "Input and mask images have not the same size"
							 << std::endl
							 << "Input = " << input->GetLargestPossibleRegion()
							 << std::endl
							 << "Mask = " << mask->GetLargestPossibleRegion());
  }

  // reset weights list
  mapOfLabelsAndWeights.clear();

  // loop
  typedef itk::ImageRegionConstIterator<InputImageType> ConstInputIteratorType;
  ConstInputIteratorType pi(input, input->GetLargestPossibleRegion());
  typedef itk::ImageRegionConstIterator<MaskImageType> ConstMaskIteratorType;
  ConstMaskIteratorType pm(mask, mask->GetLargestPossibleRegion());
  pi.Begin();
  pm.Begin();
  while (!pi.IsAtEnd()) {
	mapOfLabelsAndWeights[pm.Get()][pi.Get()]++;
	++pi;
	++pm;
  }
}
//====================================================================

// //====================================================================
// template<class ImageType>
// typename ImageType::Pointer NewImage3D(int x, int y, int z, float dx, float dy, float dz) {  
//   typename ImageType::Pointer output = ImageType::New();
//   typename ImageType::RegionType region;
//   typename ImageType::SizeType size;
//   size[0] = x;
//   size[1] = y;
//   size[2] = z;
//   region.SetSize(size);
//   output->SetRegions(region);
//   output->Allocate();
//   typename ImageType::SpacingType spacing;  
//   spacing[0] = dx;
//   spacing[1] = dy;
//   spacing[2] = dz;
//   output->SetSpacing(spacing);
//   return output;
// }
// //====================================================================


#endif /* end #define CLITKIMAGEUTILITIES_TXX */

