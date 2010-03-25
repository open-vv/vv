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
#ifndef CLITKVFRESAMPLEGENERICFILTER_CXX
#define CLITKVFRESAMPLEGENERICFILTER_CXX
/**
 -------------------------------------------------------------------
 * @file   clitkVFResampleGenericFilter.cxx
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008 08:37:53

 * @brief  

 -------------------------------------------------------------------*/

#include "clitkVFResampleGenericFilter.h"

//--------------------------------------------------------------------
clitk::VFResampleGenericFilter::VFResampleGenericFilter():
  clitk::ImageToImageGenericFilter<Self>("VFResample") {
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
  mApplyGaussianFilterBefore = false;
  mDefaultPixelValue = 0.0;
  mInterpolatorName = "NN";
  mBSplineOrder=3;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::VFResampleGenericFilter::InitializeImageType() {
  ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void clitk::VFResampleGenericFilter::UpdateWithInputImageType() {

  if (mNbOfComponents == 1) { 
    std::cerr << "Error, only one components ? Use clitkImageResample instead." << std::endl;
    exit(0);
  }
  typedef typename ImageType::PixelType PixelType;
  if (mNbOfComponents == 2) Update_WithDimAndPixelTypeAndComponent<ImageType::ImageDimension,PixelType,2>();
  if (mNbOfComponents == 3) Update_WithDimAndPixelTypeAndComponent<ImageType::ImageDimension,PixelType,3>();  
  if (mNbOfComponents == 4) Update_WithDimAndPixelTypeAndComponent<ImageType::ImageDimension,PixelType,4>();  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim, class PixelType, unsigned int DimCompo>
void clitk::VFResampleGenericFilter::Update_WithDimAndPixelTypeAndComponent() {
  // Reading input
  typedef itk::Vector<PixelType, DimCompo> DisplacementType;
  typedef itk::Image< DisplacementType, Dim > ImageType;

  typename ImageType::Pointer input = clitk::readImage<ImageType>(mInputFilenames, mIOVerbose);

  // Main filter
  typename ImageType::Pointer outputImage = ComputeImage<ImageType>(input);

  // Write results
  SetNextOutput<ImageType>(outputImage);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer 
clitk::VFResampleGenericFilter::ComputeImage(typename ImageType::Pointer inputImage) {

  // Check options
  static unsigned int dim = ImageType::ImageDimension;
  if (mOutputSize.size() != dim) {
    std::cerr << "Please set size with " << dim << " dimensions." << std::endl;
    return NULL;
  }
  if (mOutputSpacing.size() != dim) {
    std::cerr << "Please set spacing with " << dim << " dimensions." << std::endl;
    return NULL;
  }
  mOutputOrigin.resize(dim);

  if (mApplyGaussianFilterBefore && mSigma.size() != dim) {
    std::cerr << "Please set sigma with " << dim << " dimensions." << std::endl;
    return NULL;
  }

  // Some typedefs
  typedef typename ImageType::SizeType SizeType;
  typedef typename ImageType::SpacingType SpacingType;
  typedef typename ImageType::PointType PointType;

  // Create Image Filter
  typedef itk::VectorResampleImageFilter<ImageType,ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Instance of the transform object to be passed to the resample
  // filter. By default, identity transform is applied
  typedef itk::AffineTransform<double, ImageType::ImageDimension> TransformType;
  typename TransformType::Pointer transform =  TransformType::New();
  filter->SetTransform(transform);

  // Set filter's parameters
  SizeType outputSize;
  SpacingType outputSpacing;
  PointType outputOrigin;
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    outputSize[i] = mOutputSize[i];
    outputSpacing[i] = mOutputSpacing[i];
    outputOrigin[i] = inputImage->GetOrigin()[i];
  }

  filter->SetSize(outputSize);
  filter->SetOutputSpacing(outputSpacing);
  filter->SetOutputOrigin(outputOrigin);
  filter->SetDefaultPixelValue(static_cast<typename ImageType::PixelType>(mDefaultPixelValue));

  // Select interpolator
  if (mInterpolatorName == "nn") {
    typedef itk::VectorNearestNeighborInterpolateImageFunction<ImageType, double> InterpolatorType;     
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    filter->SetInterpolator(interpolator);
  }
  else { 
    if (mInterpolatorName == "linear") {
      typedef itk::VectorLinearInterpolateImageFunction<ImageType, double> InterpolatorType;     
      typename InterpolatorType::Pointer interpolator =  InterpolatorType::New();
      filter->SetInterpolator(interpolator);
    }
    else {
      std::cerr << "Sorry, I do not know the interpolator (for vector field) '" << mInterpolatorName 
                << "'. Known interpolators are :  nn, linear" << std::endl;
      exit(0);
    }
  }

  // Build initial Gaussian bluring (if needed)
  typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType> GaussianFilterType;
  std::vector<typename GaussianFilterType::Pointer> gaussianFilters;
  if (mApplyGaussianFilterBefore) {
    for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
      // Create filter
      gaussianFilters.push_back(GaussianFilterType::New());
      // Set options
      gaussianFilters[i]->SetDirection(i);
      gaussianFilters[i]->SetOrder(GaussianFilterType::ZeroOrder);
      gaussianFilters[i]->SetNormalizeAcrossScale(false);
      gaussianFilters[i]->SetSigma(mSigma[i]); // in millimeter !
      // Set input
      if (i==0) gaussianFilters[i]->SetInput(inputImage);
      else gaussianFilters[i]->SetInput(gaussianFilters[i-1]->GetOutput());
    }
    filter->SetInput(gaussianFilters[ImageType::ImageDimension-1]->GetOutput());
  }
  else {
    filter->SetInput(inputImage);
  }

  // Go !
  try { 
    filter->Update();
  }
  catch( itk::ExceptionObject & err ) {
    std::cerr << "Error while filtering " << mInputFilenames[0].c_str() 
	      << " " << err << std::endl;
    exit(0);
  }

  // Return result
  return filter->GetOutput();
  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::VFResampleGenericFilter::SetOutputSize(const std::vector<int> & size) {
  mOutputSize.resize(size.size());
  std::copy(size.begin(), size.end(), mOutputSize.begin());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::VFResampleGenericFilter::SetOutputSpacing(const std::vector<double> & spacing) {
  mOutputSpacing.resize(spacing.size());
  std::copy(spacing.begin(), spacing.end(), mOutputSpacing.begin());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::VFResampleGenericFilter::SetInterpolationName(const std::string & inter) {
  mInterpolatorName = inter;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::VFResampleGenericFilter::SetGaussianSigma(const std::vector<double> & sigma) {
  mApplyGaussianFilterBefore = true;
  mSigma.resize(sigma.size());
  std::copy(sigma.begin(), sigma.end(), mSigma.begin());
}
//--------------------------------------------------------------------

#endif

