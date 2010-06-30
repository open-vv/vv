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
#ifndef CLITKIMAGERESAMPLEGENERICFILTER2_CXX
#define CLITKIMAGERESAMPLEGENERICFILTER2_CXX
/**
 -------------------------------------------------------------------
 * @file   clitkImageResampleGenericFilter.cxx
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008 08:37:53

 * @brief

 -------------------------------------------------------------------*/

#include "clitkImageResampleGenericFilter.h"

// itk include
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunctionWithLUT.h"
#include "itkCommand.h"

//--------------------------------------------------------------------
clitk::ImageResampleGenericFilter::ImageResampleGenericFilter():
  ImageToImageGenericFilter<Self>("ImageResample")
{
  mApplyGaussianFilterBefore = false;
  mDefaultPixelValue = 0.0;
  mInterpolatorName = "NN";
  mBSplineOrder=3;
  InitializeImageTypeWithDim<2>();
  InitializeImageTypeWithDim<3>();
  InitializeImageTypeWithDim<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::ImageResampleGenericFilter::InitializeImageTypeWithDim()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class InputImageType>
void clitk::ImageResampleGenericFilter::UpdateWithInputImageType()
{

  // Some typedefs
  typedef typename InputImageType::SizeType    SizeType;
  typedef typename InputImageType::SpacingType SpacingType;
  typedef typename InputImageType::PointType   PointType;
  typedef typename InputImageType::PixelType   PixelType;
  static unsigned int dim = InputImageType::ImageDimension;

  // Reading input
  typename InputImageType::Pointer input = this->GetInput<InputImageType>(0);

  // Warning
  if (!std::numeric_limits<PixelType>::is_signed) {
    if ((mInterpolatorName == "bspline") || (mInterpolatorName == "blut")) {
      std::cerr << "Warning : input pixel type is not signed, use bspline interpolation at your own risk ..." << std::endl;
    }
  }

  // Check options
  if (mOutputSize.size() != dim) {
    std::cerr << "Please set size with " << dim << " dimensions." << std::endl;
    return;
  }
  if (mOutputSpacing.size() != dim) {
    std::cerr << "Please set spacing with " << dim << " dimensions." << std::endl;
    return;
  }
  mOutputOrigin.resize(dim);

  if (mApplyGaussianFilterBefore && mSigma.size() != dim) {
    std::cerr << "Please set sigma with " << dim << " dimensions." << std::endl;
    return;
  }

  // Create Image Filter
  typedef itk::ResampleImageFilter<InputImageType,InputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();

  // Instance of the transform object to be passed to the resample
  // filter. By default, identity transform is applied
  typedef itk::AffineTransform<double, InputImageType::ImageDimension> TransformType;
  typename TransformType::Pointer transform =  TransformType::New();
  filter->SetTransform(transform);

  // Set filter's parameters
  SizeType outputSize;
  SpacingType outputSpacing;
  PointType outputOrigin;
  for(unsigned int i=0; i<InputImageType::ImageDimension; i++) {
    outputSize[i] = mOutputSize[i];
    outputSpacing[i] = mOutputSpacing[i];
    outputOrigin[i] = input->GetOrigin()[i];
  }

  filter->SetSize(outputSize);
  filter->SetOutputSpacing(outputSpacing);
  filter->SetOutputOrigin(outputOrigin);
  filter->SetDefaultPixelValue(static_cast<PixelType>(mDefaultPixelValue));//DS TODO//JV comme ça?

  // Select interpolator
  if (mInterpolatorName == "nn") {
    typedef itk::NearestNeighborInterpolateImageFunction<InputImageType, double> InterpolatorType;
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    filter->SetInterpolator(interpolator);
  } else {
    if (mInterpolatorName == "linear") {
      typedef itk::LinearInterpolateImageFunction<InputImageType, double> InterpolatorType;
      typename InterpolatorType::Pointer interpolator =  InterpolatorType::New();
      filter->SetInterpolator(interpolator);
    } else {
      if (mInterpolatorName == "windowed sinc") {
        typedef itk::WindowedSincInterpolateImageFunction<InputImageType, 4> InterpolatorType;
        typename InterpolatorType::Pointer interpolator =  InterpolatorType::New();
        filter->SetInterpolator(interpolator);
      } else {
        if (mInterpolatorName == "bspline") {
          typedef itk::BSplineInterpolateImageFunction<InputImageType, double> InterpolatorType;
          typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
          interpolator->SetSplineOrder(mBSplineOrder);
          filter->SetInterpolator(interpolator); 
        } else {
          if (mInterpolatorName == "blut") {
            typedef itk::BSplineInterpolateImageFunctionWithLUT<InputImageType, double> InterpolatorType;
            typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
            interpolator->SetSplineOrder(mBSplineOrder);
            interpolator->SetLUTSamplingFactor(mSamplingFactors[0]);
            filter->SetInterpolator(interpolator);
          } else {
            std::cerr << "Sorry, I do not know the interpolator '" << mInterpolatorName
              << "'. Known interpolators are :  nn, linear, bspline, blut" << std::endl;
            exit(0);
          }
        }
      }
    }
  }

  // Build initial Gaussian bluring (if needed)
  typedef itk::RecursiveGaussianImageFilter<InputImageType, InputImageType> GaussianFilterType;
  std::vector<typename GaussianFilterType::Pointer> gaussianFilters;
  if (mApplyGaussianFilterBefore) {
    for(unsigned int i=0; i<InputImageType::ImageDimension; i++) {
      // Create filter
      gaussianFilters.push_back(GaussianFilterType::New());
      // Set options
      gaussianFilters[i]->SetDirection(i);
      gaussianFilters[i]->SetOrder(GaussianFilterType::ZeroOrder);
      gaussianFilters[i]->SetNormalizeAcrossScale(false);
      gaussianFilters[i]->SetSigma(mSigma[i]); // in millimeter !
      // Set input
      if (i==0) gaussianFilters[i]->SetInput(input);
      else gaussianFilters[i]->SetInput(gaussianFilters[i-1]->GetOutput());
    }
    filter->SetInput(gaussianFilters[InputImageType::ImageDimension-1]->GetOutput());
  } else {
    filter->SetInput(input);
  }

  // Go !
  try {
    filter->Update();
  } catch( itk::ExceptionObject & err ) {
    std::cerr << "Error while filtering " << m_InputFilenames[0].c_str()
              << " " << err << std::endl;
    exit(0);
  }

  // Get result
  typename InputImageType::Pointer outputImage = filter->GetOutput();

  // Write/save results
  this->SetNextOutput<InputImageType>(outputImage);

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::SetOutputSize(const std::vector<int> & size)
{
  mOutputSize.resize(size.size());
  std::copy(size.begin(), size.end(), mOutputSize.begin());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::SetOutputSpacing(const std::vector<double> & spacing)
{
  mOutputSpacing.resize(spacing.size());
  std::copy(spacing.begin(), spacing.end(), mOutputSpacing.begin());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::SetInterpolationName(const std::string & inter)
{
  mInterpolatorName = inter;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::SetGaussianSigma(const std::vector<double> & sigma)
{
  mApplyGaussianFilterBefore = true;
  mSigma.resize(sigma.size());
  std::copy(sigma.begin(), sigma.end(), mSigma.begin());
}
//--------------------------------------------------------------------

#endif

