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

// clitk
#include "clitkMinMaxMask_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"
#include "itkConstSliceIterator.h"
#include "itkImageRegionIteratorWithIndex.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkMinMaxMask, args_info);
  CLITK_INIT;

  typedef short PixelType;
  static const int Dim=3;
  typedef itk::Image<PixelType, Dim> ImageType;

  ImageType::Pointer input1;
  ImageType::Pointer input2;
  input1 = clitk::readImage<ImageType>(args_info.input1_arg, false);
  input2 = clitk::readImage<ImageType>(args_info.input2_arg, false);
  
  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;
  IteratorType it = IteratorType(input1, input1->GetLargestPossibleRegion());
  it.GoToBegin();
  ImageType::IndexType first1=it.GetIndex();
  ImageType::IndexType last1=it.GetIndex();
  bool firstFound = false;
  while (!it.IsAtEnd()) {
    if (!firstFound) {
      if (it.Get() == 1) {
        firstFound = true;
        first1 = it.GetIndex();
      }
    }
    else {
      if (it.Get() == 1) {
        last1 = it.GetIndex();
      }
    }
    ++it;
  }
  
  // typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;
  it = IteratorType(input2, input2->GetLargestPossibleRegion());
  it.GoToBegin();
  ImageType::IndexType first2=it.GetIndex();
  ImageType::IndexType last2=it.GetIndex();
  firstFound = false;
  while (!it.IsAtEnd()) {
    if (!firstFound) {
      if (it.Get() == 1) {
        firstFound = true;
        first2 = it.GetIndex();
      }
    }
    else {
      if (it.Get() == 1) {
        last2 = it.GetIndex();
      }
    }
    ++it;
  }
  
  std::cout << " --i1 " << first1[2] << " --f1 " << last1[2]
            << " --i2 " << first2[2] << " --f2 " << last2[2] << std::endl;

  return EXIT_SUCCESS;
} // This is the end, my friend
//--------------------------------------------------------------------
