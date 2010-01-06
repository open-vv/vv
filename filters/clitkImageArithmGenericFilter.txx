/*-------------------------------------------------------------------------
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  -------------------------------------------------------------------------*/

#ifndef CLITKIMAGEARITHMGENERICFILTER_TXX
#define CLITKIMAGEARITHMGENERICFILTER_TXX

/*-------------------------------------------------
 * @file   clitkImageArithmGenericFilter.txx
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   9 August 2006
 * 
 -------------------------------------------------*/

//--------------------------------------------------------------------
template<unsigned int Dim>
void ImageArithmGenericFilter::Update_WithDim() { 
#define TRY_TYPE(TYPE)							\
  if (IsSameType<TYPE>(mPixelTypeName)) { Update_WithDimAndPixelType<Dim, TYPE>(); return; } 
  TRY_TYPE(char);
  TRY_TYPE(uchar);
  TRY_TYPE(short);
  TRY_TYPE(ushort);
  TRY_TYPE(int);
  TRY_TYPE(unsigned int); 
  TRY_TYPE(float);
  TRY_TYPE(double);
#undef TRY_TYPE

  std::string list = CreateListOfTypes<char, uchar, short, ushort, int, uint, float, double>();
  std::cerr << "Error, I don't know the type '" << mPixelTypeName << "' for the input image '"
	    << mInputFilenames[0] << "'." << std::endl << "Known types are " << list << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim, class PixelType>
void ImageArithmGenericFilter::Update_WithDimAndPixelType() {
  // Read input1
  typedef itk::Image<PixelType,Dim> ImageType;
  typename ImageType::Pointer input1 = clitk::readImage<ImageType>(mInputFilenames[0], mIOVerbose);
  typename ImageType::Pointer outputImage;  

  // Read input2 (float is ok altough it could take too memory ...)
  if (mIsOperationUseASecondImage) {
    typedef itk::Image<float,Dim> ImageType2;
    typename ImageType2::Pointer input2 = clitk::readImage<ImageType2>(mInputFilenames[1], mIOVerbose);
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
ImageArithmGenericFilter::ComputeImage(typename ImageType::Pointer inputImage) {

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
ImageArithmGenericFilter::ComputeImage(typename ImageType1::Pointer inputImage1, 
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

#endif  //#define CLITKIMAGEARITHMGENERICFILTER_TXX
