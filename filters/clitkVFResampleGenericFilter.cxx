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
clitk::VFResampleGenericFilter::VFResampleGenericFilter() {
  mApplyGaussianFilterBefore = false;
  mDefaultPixelValue = 0.0;
  mInterpolatorName = "NN";
  mBSplineOrder=3;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::VFResampleGenericFilter::Update() {
  // Load image header
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(mInputFilenames[0]);
  
  // Determine dim, pixel type, number of components
  mDim = header->GetNumberOfDimensions();
  mPixelTypeName = header->GetComponentTypeAsString(header->GetComponentType());  
  mNbOfComponents = header->GetNumberOfComponents();

  // Switch by dimension
  if (mDim == 2) { Update_WithDim<2>(); return; }
  if (mDim == 3) { Update_WithDim<3>(); return; }
  if (mDim == 4) { Update_WithDim<4>(); return; }

  std::cerr << "Error, dimension of input image is " << mDim << ", but I only work with 2,3,4." << std::endl;
  exit(0);
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

