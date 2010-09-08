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

// clitk
#include "clitkCommon.h"

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
template <class InputImageType, class OutputImageType>
clitk::ResampleImageWithOptionsFilter<InputImageType, OutputImageType>::
ResampleImageWithOptionsFilter():itk::ImageToImageFilter<InputImageType, OutputImageType>() 
{
  static const unsigned int dim = InputImageType::ImageDimension;
  this->SetNumberOfRequiredInputs(1);
  m_OutputIsoSpacing = -1;
  m_InterpolationType = NearestNeighbor;
  m_GaussianFilteringEnabled = true;
  m_BSplineOrder = 3;
  m_BLUTSamplingFactor = 20;
  m_LastDimensionIsTime = false;
  m_Transform =  TransformType::New();
  if (dim == 4) m_LastDimensionIsTime = true; // by default 4D is 3D+t
  for(unsigned int i=0; i<dim; i++) {
    m_OutputSize[i] = 0;
    m_OutputSpacing[i] = -1;
    m_GaussianSigma[i] = -1;
  }
  m_VerboseOptions = false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class InputImageType, class OutputImageType>
void
clitk::ResampleImageWithOptionsFilter<InputImageType, OutputImageType>::
SetInput(const InputImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<InputImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class InputImageType, class OutputImageType>
void
clitk::ResampleImageWithOptionsFilter<InputImageType, OutputImageType>::
GenerateInputRequestedRegion() 
{
  // call the superclass's implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input and output
  InputImagePointer  inputPtr  =
    const_cast< InputImageType *>( this->GetInput() );

  // Request the entire input image
  InputImageRegionType inputRegion;
  inputRegion = inputPtr->GetLargestPossibleRegion();
  inputPtr->SetRequestedRegion(inputRegion);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class InputImageType, class OutputImageType>
void
clitk::ResampleImageWithOptionsFilter<InputImageType, OutputImageType>::
GenerateOutputInformation() 
{
  static const unsigned int dim = InputImageType::ImageDimension;

  // Warning
  if (!std::numeric_limits<InputImagePixelType>::is_signed) {
    if ((m_InterpolationType == BSpline) ||
        (m_InterpolationType == B_LUT)) {
      std::cerr << "Warning : input pixel type is not signed, use bspline interpolation at your own risk ..." << std::endl;
    }
  }

  // Get input pointer
  InputImagePointer input = dynamic_cast<InputImageType*>(itk::ProcessObject::GetInput(0));

  // Perform default implementation
  Superclass::GenerateOutputInformation();

  // Compute sizes
  InputImageSpacingType inputSpacing = input->GetSpacing();
  InputImageSizeType inputSize = input->GetLargestPossibleRegion().GetSize();

  if (m_OutputIsoSpacing != -1) { // apply isoSpacing
    for(unsigned int i=0; i<dim; i++) {
      m_OutputSpacing[i] = m_OutputIsoSpacing;
      m_OutputSize[i] = (int)lrint(inputSize[i]*inputSpacing[i]/m_OutputSpacing[i]);
    }
  } else {
    if (m_OutputSpacing[0] != -1) { // apply spacing, compute size
      for(unsigned int i=0; i<dim; i++) {
        m_OutputSize[i] = (int)lrint(inputSize[i]*inputSpacing[i]/m_OutputSpacing[i]);
      }
    } else {
      if (m_OutputSize[0] != 0) { // apply size, compute spacing
        for(unsigned int i=0; i<dim; i++) {
          m_OutputSpacing[i] = (double)inputSize[i]*inputSpacing[i]/(double)m_OutputSize[i];
        }
      } else { // copy input size/spacing ... (no resampling)
        m_OutputSize = inputSize;
        m_OutputSpacing = inputSpacing;
      }
    }
  }

  // Special case for temporal image 2D+t or 3D+t
  if (m_LastDimensionIsTime) {
    int l = dim-1;
    m_OutputSize[l] = inputSize[l];
    m_OutputSpacing[l] = inputSpacing[l];
  }
    
  // Set Size/Spacing
  OutputImagePointer outputImage = this->GetOutput(0);
  // OutputImageRegionType region;
  m_OutputRegion.SetSize(m_OutputSize);
  m_OutputRegion.SetIndex(input->GetLargestPossibleRegion().GetIndex());
  outputImage->CopyInformation(input);
  outputImage->SetLargestPossibleRegion(m_OutputRegion);
  outputImage->SetSpacing(m_OutputSpacing);

  // Init Gaussian sigma
  if (m_GaussianSigma[0] != -1) { // Gaussian filter set by user
    m_GaussianFilteringEnabled = true;
  }
  else {
    if (m_GaussianFilteringEnabled) { // Automated sigma when downsample
      for(unsigned int i=0; i<dim; i++) {
        if (m_OutputSpacing[i] > inputSpacing[i]) { // downsample
          m_GaussianSigma[i] = 0.5*m_OutputSpacing[i];// / inputSpacing[i]);
        }
        else m_GaussianSigma[i] = 0; // will be ignore after
      }
    }
  }
  if (m_GaussianFilteringEnabled && m_LastDimensionIsTime) {
    m_GaussianSigma[dim-1] = 0;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class InputImageType, class OutputImageType>
void 
clitk::ResampleImageWithOptionsFilter<InputImageType, OutputImageType>::
GenerateData() 
{
   
  // Get input pointer
  InputImagePointer input = dynamic_cast<InputImageType*>(itk::ProcessObject::GetInput(0));
  static const unsigned int dim = InputImageType::ImageDimension;

  // Set regions and allocate
  //DD(this->GetOutput()->GetLargestPossibleRegion());
  //this->GetOutput()->SetRegions(m_OutputRegion);
  //this->GetOutput()->Allocate();
  // this->GetOutput()->FillBuffer(m_DefaultPixelValue);

  // Create main Resample Image Filter
  typedef itk::ResampleImageFilter<InputImageType,OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->GraftOutput(this->GetOutput());
  //     this->GetOutput()->Print(std::cout);
  this->GetOutput()->SetBufferedRegion(this->GetOutput()->GetLargestPossibleRegion());
  //     this->GetOutput()->Print(std::cout);

  // Print options if needed
  if (m_VerboseOptions) {
    std::cout << "Output Spacing = " << m_OutputSpacing << std::endl
              << "Output Size    = " << m_OutputSize << std::endl
              << "Gaussian       = " << m_GaussianFilteringEnabled << std::endl;
    if (m_GaussianFilteringEnabled)
      std::cout << "Sigma          = " << m_GaussianSigma << std::endl;
    std::cout << "Interpol       = ";
    switch (m_InterpolationType) {
    case NearestNeighbor: std::cout << "NearestNeighbor" << std::endl; break;
    case Linear: std::cout << "Linear" << std::endl; break;
    case BSpline: std::cout << "BSpline " << m_BSplineOrder << std::endl; break;
    case B_LUT: std::cout << "B-LUT " << m_BSplineOrder << " " << m_BLUTSamplingFactor << std::endl; break;
    case WSINC: std::cout << "Windowed Sinc" << std::endl; break;
    }
    std::cout << "Threads        = " << this->GetNumberOfThreads() << std::endl;
    std::cout << "LastDimIsTime  = " << m_LastDimensionIsTime << std::endl;
  }

  // Instance of the transform object to be passed to the resample
  // filter. By default, identity transform is applied
  filter->SetTransform(m_Transform);
  filter->SetSize(m_OutputSize);
  filter->SetOutputSpacing(m_OutputSpacing);
  filter->SetOutputOrigin(input->GetOrigin());
  filter->SetDefaultPixelValue(m_DefaultPixelValue);
  filter->SetNumberOfThreads(this->GetNumberOfThreads()); 
  filter->SetOutputDirection(input->GetDirection()); // <-- NEEDED if we want to keep orientation (in case of PermutAxes for example)

  // Select interpolator
  switch (m_InterpolationType) {
  case NearestNeighbor: {
    typedef itk::NearestNeighborInterpolateImageFunction<InputImageType, double> InterpolatorType;
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    filter->SetInterpolator(interpolator);
    break;
  }
  case Linear: {
    typedef itk::LinearInterpolateImageFunction<InputImageType, double> InterpolatorType;
    typename InterpolatorType::Pointer interpolator =  InterpolatorType::New();
    filter->SetInterpolator(interpolator);
    break;
  }
  case BSpline: {
    typedef itk::BSplineInterpolateImageFunction<InputImageType, double> InterpolatorType;
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetSplineOrder(m_BSplineOrder);
    filter->SetInterpolator(interpolator);
    break;
  }
  case B_LUT: {
    typedef itk::BSplineInterpolateImageFunctionWithLUT<InputImageType, double> InterpolatorType;
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetSplineOrder(m_BSplineOrder);
    interpolator->SetLUTSamplingFactor(m_BLUTSamplingFactor);
    filter->SetInterpolator(interpolator);
    break;
  }
  case WSINC: {
    typedef itk::WindowedSincInterpolateImageFunction<InputImageType, 4> InterpolatorType;
    typename InterpolatorType::Pointer interpolator =  InterpolatorType::New();
    filter->SetInterpolator(interpolator);
    break;
  }
  }

  // Initial Gaussian blurring if needed
  // TODO : replace by itk::DiscreteGaussianImageFilter for small sigma
  typedef itk::RecursiveGaussianImageFilter<InputImageType, InputImageType> GaussianFilterType;
  std::vector<typename GaussianFilterType::Pointer> gaussianFilters;
  if (m_GaussianFilteringEnabled) {
    for(unsigned int i=0; i<dim; i++) {
      if (m_GaussianSigma[i] != 0) {
        gaussianFilters.push_back(GaussianFilterType::New());
        gaussianFilters[i]->SetDirection(i);
        gaussianFilters[i]->SetOrder(GaussianFilterType::ZeroOrder);
        gaussianFilters[i]->SetNormalizeAcrossScale(false);
        gaussianFilters[i]->SetSigma(m_GaussianSigma[i]); // in millimeter !
        if (gaussianFilters.size() == 1) { // first
          gaussianFilters[0]->SetInput(input);
        } else {
          gaussianFilters[i]->SetInput(gaussianFilters[i-1]->GetOutput());
        }
      }
    }
    if (gaussianFilters.size() > 0) {
      filter->SetInput(gaussianFilters[gaussianFilters.size()-1]->GetOutput());
    } else filter->SetInput(input);
  } else filter->SetInput(input);

  // Go !
  filter->Update();

  // Set output
  // DD("before Graft");

  //this->GraftOutput(filter->GetOutput());
  this->SetNthOutput(0, filter->GetOutput());

  // DD("after Graft");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class InputImageType>
typename InputImageType::Pointer 
clitk::ResampleImageSpacing(typename InputImageType::Pointer input, 
                            typename InputImageType::SpacingType spacing, 
                            int interpolationType)
{
  typedef clitk::ResampleImageWithOptionsFilter<InputImageType> ResampleFilterType;
  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  resampler->SetInput(input);
  resampler->SetOutputSpacing(spacing);
  typename ResampleFilterType::InterpolationTypeEnumeration inter=ResampleFilterType::NearestNeighbor;
  switch(interpolationType) {
  case 0: inter = ResampleFilterType::NearestNeighbor; break;
  case 1: inter = ResampleFilterType::Linear; break;
  case 2: inter = ResampleFilterType::BSpline; break;
  case 3: inter = ResampleFilterType::B_LUT; break;
  case 4: inter = ResampleFilterType::WSINC; break;
  }
  resampler->SetInterpolationType(inter);
  resampler->SetGaussianFilteringEnabled(true);
  resampler->Update();
  return resampler->GetOutput();
}
//--------------------------------------------------------------------
