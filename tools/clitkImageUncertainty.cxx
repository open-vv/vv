/*=========================================================================

  Program:   clitk
  Module:    $RCSfile: clitkImageUncertainty.cxx,v $
  Language:  C++
  Date:      $Date: 2011/03/03 15:03:30 $
  Version:   $Revision: 1.3 $

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef CLITKIMAGEUNCERTAINTY_CXX
#define CLITKIMAGEUNCERTAINTY_CXX

/**
 =================================================
 * @file   clitkImageUncertainty.cxx
 * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
 * @date   04 Jul 2006 14:03:57
 *
 * @brief
 *
 *
 =================================================*/

// clitk include
#include "clitkImageUncertainty_ggo.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"
#include "clitkPortability.h"

// itk include
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

//====================================================================
int main(int argc, char * argv[]) {

  // init command line
  GGO(clitkImageUncertainty, args_info);

  // Declare main types
  typedef float                          PixelType;
  const unsigned int                              Dimension=3;
  typedef itk::Image< PixelType, Dimension >      ImageType;

  // Read images
  ImageType::Pointer input =
    clitk::readImage<ImageType>(args_info.input_arg, args_info.verbose_flag);
  ImageType::Pointer inputSquared =
    clitk::readImage<ImageType>(args_info.inputSquared_arg, args_info.verbose_flag);

  // Create Output
  ImageType::Pointer output = ImageType::New();
  output->SetRegions(input->GetLargestPossibleRegion());
  output->CopyInformation(input);
  output->Allocate();

  // Loop
  typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  ConstIteratorType pi(input, input->GetLargestPossibleRegion());
  ConstIteratorType pii(inputSquared, inputSquared->GetLargestPossibleRegion());
  pi.GoToBegin();
  pii.GoToBegin();
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType po(output, output->GetLargestPossibleRegion());
  po.GoToBegin();

  int NumberOfEvents = args_info.NumberOfEvents_arg;
  while ( !pi.IsAtEnd() ) {
    double squared = pii.Get();
    double mean = pi.Get();
    double uncert = sqrt((NumberOfEvents*squared - mean*mean) / (NumberOfEvents-1));
    if(!args_info.absolute_flag)
      uncert /= std::abs(mean);
    if (!IsNormal(uncert)) uncert = args_info.default_arg;
    po.Set(uncert);
    ++pi;
    ++pii;
    ++po;
  }

  // Write output image
  // DD(clitk::GetExtension(args_info.output_arg));
  if (clitk::GetExtension(args_info.output_arg) != "txt") {
    clitk::writeImage<ImageType>(output, args_info.output_arg, args_info.verbose_flag);
  }
  else {
    std::ofstream os;
    clitk::openFileForWriting(os, args_info.output_arg);
    typedef itk::ImageRegionConstIterator<ImageType> IteratorType;
    IteratorType pi(output, output->GetLargestPossibleRegion());
    pi.GoToBegin();
    os << "# Image size = " << output->GetLargestPossibleRegion().GetSize() << std::endl;
    os << "# Image spacing = " << output->GetSpacing() << std::endl;
    os << "# Number of events = " << NumberOfEvents << std::endl;
    while (!pi.IsAtEnd()) {
      os << pi.Get() << std::endl;
      ++pi;
    }
  }
}


#endif /* end #define CLITKIMAGEUNCERTAINTY_CXX */
