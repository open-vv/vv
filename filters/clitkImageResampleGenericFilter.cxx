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

//--------------------------------------------------------------------
clitk::ImageResampleGenericFilter::ImageResampleGenericFilter() {
  mApplyGaussianFilterBefore = false;
  mDefaultPixelValue = 0.0;
  mInterpolatorName = "NN";
  mBSplineOrder=3;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::Update() {

  // Determine dim, pixel type, number of components
  this->GetInputImageDimensionAndPixelType(mDim,mPixelTypeName,mNbOfComponents);

  // Switch by dimension
  if (mDim == 2) { Update_WithDim<2>(); return; }
  if (mDim == 3) { Update_WithDim<3>(); return; }
  if (mDim == 4) { Update_WithDim<4>(); return; }

  std::cerr << "Error, dimension of input image is " << mDim << ", but I only work with 2,3,4." << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::SetOutputSize(const std::vector<int> & size) {
  mOutputSize.resize(size.size());
  std::copy(size.begin(), size.end(), mOutputSize.begin());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::SetOutputSpacing(const std::vector<double> & spacing) {
  mOutputSpacing.resize(spacing.size());
  std::copy(spacing.begin(), spacing.end(), mOutputSpacing.begin());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::SetInterpolationName(const std::string & inter) {
  mInterpolatorName = inter;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageResampleGenericFilter::SetGaussianSigma(const std::vector<double> & sigma) {
  mApplyGaussianFilterBefore = true;
  mSigma.resize(sigma.size());
  std::copy(sigma.begin(), sigma.end(), mSigma.begin());
}
//--------------------------------------------------------------------

#endif

