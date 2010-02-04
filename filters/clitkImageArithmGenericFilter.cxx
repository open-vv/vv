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
clitk::ImageArithmGenericFilter::ImageArithmGenericFilter()
  :ImageToImageGenericFilter<Self>("ImageArithmGenericFilter"),mTypeOfOperation(0) {
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();  
  mIsOperationUseASecondImage = false;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::ImageArithmGenericFilter::InitializeImageType() {      
  ADD_IMAGE_TYPE(Dim, char);
  ADD_IMAGE_TYPE(Dim, short);
  ADD_IMAGE_TYPE(Dim, float);
  /*  ADD_IMAGE_TYPE(Dim, short);
  ADD_IMAGE_TYPE(Dim, ushort;
  ADD_IMAGE_TYPE(Dim, int);
  ADD_IMAGE_TYPE(Dim, unsigned int);
  ADD_IMAGE_TYPE(Dim, double);
  */
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void clitk::ImageArithmGenericFilter::UpdateWithInputImageType() {

  // Check Input

  //DS TO BE CHANGED IN GetNumberOfInput() !!!!!!!!

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

  // Read input1
  //  typename ImageType::Pointer input1 = clitk::readImage<ImageType>(mInputFilenames[0], mIOVerbose);
  typename ImageType::Pointer input1 = this->template GetInput<ImageType>(0);
  typename ImageType::Pointer outputImage;  

  // Read input2 (float is ok altough it could take too memory ...)
  if (mIsOperationUseASecondImage) {
    typedef itk::Image<float,ImageType::ImageDimension> ImageType2;
    //    typename ImageType2::Pointer input2 = clitk::readImage<ImageType2>(mInputFilenames[1], mIOVerbose);
    typename ImageType2::Pointer input2 = this->template GetInput<ImageType2>(1);
    outputImage = ComputeImage<ImageType, ImageType2>(input1, input2);
  }
  else {
    outputImage = ComputeImage<ImageType>(input1);
  }

  // Write results
  this->SetNextOutput<ImageType>(outputImage);
  //clitk::writeImage<ImageType>(outputImage, mOutputFilename, mIOVerbose);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer 
clitk::ImageArithmGenericFilter::ComputeImage(typename ImageType::Pointer inputImage) {

  typedef typename ImageType::PixelType PixelType;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType it(inputImage, inputImage->GetLargestPossibleRegion());
  it.GoToBegin();
  
  switch (mTypeOfOperation) {
  case 0: // Addition
    while (!it.IsAtEnd()) {
      it.Set(PixelTypeDownCast<double, PixelType>((double)it.Get() + mScalar) ); 
      ++it;
    }
    break;
  case 1: // Multiply
    while (!it.IsAtEnd()) {
      it.Set(PixelTypeDownCast<double, PixelType>((double)it.Get() * mScalar) ); 
      ++it;
    }
    break;
  case 2: // Inverse
    while (!it.IsAtEnd()) {
      if (it.Get() != 0)
	it.Set(PixelTypeDownCast<double, PixelType>(mScalar / (double)it.Get())); 
      else it.Set(mDefaultPixelValue);
      ++it;
    }
    break;
  case 3: // Max 
    while (!it.IsAtEnd()) {
      if (it.Get() < mScalar) it.Set(PixelTypeDownCast<double, PixelType>(mScalar)); 
      ++it;
    }
    break;
  case 4: // Min
    while (!it.IsAtEnd()) {
      if (it.Get() > mScalar) it.Set(PixelTypeDownCast<double, PixelType>(mScalar)); 
      ++it;
    }
    break;
  case 5: // Absolute value 
    while (!it.IsAtEnd()) {
      if (it.Get() <= 0) it.Set(PixelTypeDownCast<double, PixelType>(-it.Get())); 
      // <= zero to avoid warning for unsigned types
      ++it;
    }
    break;
  case 6: // Squared value
    while (!it.IsAtEnd()) {
      it.Set(PixelTypeDownCast<double, PixelType>((double)it.Get()*(double)it.Get())); 
      ++it;
    }
    break;
  case 7: // Log
    while (!it.IsAtEnd()) {
      if (it.Get() > 0) 
	it.Set(PixelTypeDownCast<double, PixelType>(log((double)it.Get()))); 
      else it.Set(mDefaultPixelValue);
      ++it;
    }
    break;
  case 8: // exp
    while (!it.IsAtEnd()) {
      it.Set(PixelTypeDownCast<double, PixelType>(exp((double)it.Get()))); 
      ++it;
    }
    break;
  case 9: // sqrt
    while (!it.IsAtEnd()) {
      if (it.Get() > 0) 
	it.Set(PixelTypeDownCast<double, PixelType>(sqrt((double)it.Get()))); 
      else {
	if (it.Get() ==0) it.Set(0);
	else it.Set(mDefaultPixelValue);
      }
      ++it;
    }
    break;
  default: // error ?
    std::cerr << "ERROR : the operation number (" << mTypeOfOperation << ") is not known." << std::endl;
    exit(-1);
  }

  return inputImage;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType1, class ImageType2>
typename ImageType1::Pointer
clitk::ImageArithmGenericFilter::ComputeImage(typename ImageType1::Pointer inputImage1, 
				       typename ImageType2::Pointer inputImage2) {

  typedef typename ImageType1::PixelType PixelType;
  typedef itk::ImageRegionIterator<ImageType1> IteratorType;
  IteratorType it1(inputImage1, inputImage1->GetLargestPossibleRegion());
  it1.GoToBegin();
  
  typedef itk::ImageRegionConstIterator<ImageType2> ConstIteratorType;
  ConstIteratorType it2(inputImage2, inputImage2->GetLargestPossibleRegion());
  it2.GoToBegin();
  
  switch (mTypeOfOperation) {
  case 0: // Addition
    while (!it1.IsAtEnd()) {
      it1.Set(PixelTypeDownCast<double, PixelType>((double)it1.Get() + (double)it2.Get()) ); 
      ++it1; ++it2;
    }
    break;
  case 1: // Multiply
    while (!it1.IsAtEnd()) {
      it1.Set(PixelTypeDownCast<double, PixelType>((double)it1.Get() * (double)it2.Get()) ); 
      ++it1; ++it2;
    }
    break;
  case 2: // Divide
    while (!it1.IsAtEnd()) {
      if (it1.Get() != 0)
	it1.Set(PixelTypeDownCast<double, PixelType>((double)it1.Get() / (double)it2.Get())); 
      else it1.Set(mDefaultPixelValue);
      ++it1; ++it2;
    }
    break;
  case 3: // Max 
    while (!it1.IsAtEnd()) {
      if (it1.Get() < it2.Get()) it1.Set(PixelTypeDownCast<double, PixelType>(it2.Get())); 
      ++it1; ++it2;
    }
    break;
  case 4: // Min
    while (!it1.IsAtEnd()) {
      if (it1.Get() > it2.Get()) it1.Set(PixelTypeDownCast<double, PixelType>(it2.Get())); 
      ++it1; ++it2;
    }
    break;
  case 5: // Absolute difference
    while (!it1.IsAtEnd()) {
      it1.Set(PixelTypeDownCast<double, PixelType>(fabs(it2.Get()-it1.Get()))); 
      ++it1; ++it2;
    }
    break;
  case 6: // Squared differences
    while (!it1.IsAtEnd()) {
      it1.Set(PixelTypeDownCast<double, PixelType>(pow((double)it1.Get()-(double)it2.Get(),2))); 
      ++it1; ++it2;
    }
    break;
  case 7: // Difference
    while (!it1.IsAtEnd()) {
      it1.Set(PixelTypeDownCast<double, PixelType>((double)it1.Get()-(double)it2.Get())); 
      ++it1; ++it2;
    }
    break;
  case 8: // Relative Difference
    while (!it1.IsAtEnd()) {
      if (it1.Get() != 0) it1.Set(PixelTypeDownCast<double, PixelType>(((double)it1.Get()-(double)it2.Get()))/(double)it1.Get()); 
      else it1.Set(0.0);
      ++it1; ++it2;
    }
    break;
  default: // error ?
    std::cerr << "ERROR : the operation number (" << mTypeOfOperation << ") is not known." << std::endl;
    exit(-1);
  }

  return inputImage1;
}
//--------------------------------------------------------------------

#endif //define CLITKIMAGEARITHMGENERICFILTER_CXX
