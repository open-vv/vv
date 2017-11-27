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
#ifndef clitkBinarizeImageGenericFilter_cxx
#define clitkBinarizeImageGenericFilter_cxx

/* =================================================
 * @file   clitkBinarizeImageGenericFilter.cxx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

#include "clitkBinarizeImageGenericFilter.h"

// itk include
#include "itkBinaryThresholdImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"

#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
BinarizeImageGenericFilter::BinarizeImageGenericFilter():
  ImageToImageGenericFilter<Self>("BinarizeImage")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void BinarizeImageGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void BinarizeImageGenericFilter::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  if (mArgsInfo.verbose_given)
    SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_given && mArgsInfo.imagetypes_flag)
    this->PrintAvailableImageTypes();

  if (mArgsInfo.input_given) {
    SetInputFilename(mArgsInfo.input_arg);
  }
  if (mArgsInfo.output_given) {
    SetOutputFilename(mArgsInfo.output_arg);
  }
  if (mArgsInfo.percentage_given) {
    SetPercentage(mArgsInfo.percentage_arg);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class InputImageType>
void
BinarizeImageGenericFilter::UpdateWithInputImageType()
{
  if (mArgsInfo.percentage_given)
    MaskOfIntegratedIntensity<InputImageType>();
  else {
    // Reading input
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

    // Main filter
    typedef typename InputImageType::PixelType PixelType;
    typedef itk::Image<uchar, InputImageType::ImageDimension> OutputImageType;

    // Filter
    typedef itk::BinaryThresholdImageFilter<InputImageType, OutputImageType> BinaryThresholdImageFilterType;
    typename BinaryThresholdImageFilterType::Pointer thresholdFilter=BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(input);
    thresholdFilter->SetInsideValue(mArgsInfo.fg_arg);

    if (mArgsInfo.lower_given) thresholdFilter->SetLowerThreshold(static_cast<PixelType>(mArgsInfo.lower_arg));
    if (mArgsInfo.upper_given) thresholdFilter->SetUpperThreshold(static_cast<PixelType>(mArgsInfo.upper_arg));

    /* Three modes :
       - FG -> only use FG value for pixel in the Foreground (or Inside), keep input values for outside
       - BG -> only use BG value for pixel in the Background (or Outside), keep input values for inside
       - both -> use FG and BG (real binary image)
    */
    if (mArgsInfo.mode_arg == std::string("both")) {
      thresholdFilter->SetOutsideValue(mArgsInfo.bg_arg);
      thresholdFilter->Update();
      typename OutputImageType::Pointer outputImage = thresholdFilter->GetOutput();
      this->template SetNextOutput<OutputImageType>(outputImage);
    } else {
      typename InputImageType::Pointer outputImage;
      thresholdFilter->SetOutsideValue(0);
      if (mArgsInfo.mode_arg == std::string("BG")) {
        typedef itk::MaskImageFilter<InputImageType,OutputImageType> maskFilterType;
        typename maskFilterType::Pointer maskFilter = maskFilterType::New();
        maskFilter->SetInput1(input);
        maskFilter->SetInput2(thresholdFilter->GetOutput());
        maskFilter->SetOutsideValue(mArgsInfo.bg_arg);
        maskFilter->Update();
        outputImage = maskFilter->GetOutput();
      } else {
        typedef itk::MaskNegatedImageFilter<InputImageType,OutputImageType> maskFilterType;
        typename maskFilterType::Pointer maskFilter = maskFilterType::New();
        maskFilter->SetInput1(input);
        maskFilter->SetInput2(thresholdFilter->GetOutput());
        maskFilter->SetOutsideValue(mArgsInfo.fg_arg);
        maskFilter->Update();
        outputImage = maskFilter->GetOutput();
      }
      // Write/Save results
      this->template SetNextOutput<InputImageType>(outputImage);
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
//  https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
template <typename T>
std::vector<size_t> sort_indexes(const std::vector<T> &v) {

  // initialize original index locations
  std::vector<size_t> idx(v.size());
  std::vector<std::pair<T, size_t> > compVector(v.size());
  for (size_t i = 0; i < v.size(); ++i) {
    compVector[i].first = v[i];
    compVector[i].second = i;
  }

  // sort indexes based on comparing values in v
  std::sort(compVector.begin(), compVector.end(), comparator<T>);
  for (size_t i = 0; i < v.size(); ++i) {
    idx[i] = compVector[i].second;
  }

  return idx;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class InputImageType>
void
BinarizeImageGenericFilter::MaskOfIntegratedIntensity()
{
  // Main filter
  typedef typename InputImageType::PixelType InputPixelType;
  typedef itk::Image<unsigned char, InputImageType::ImageDimension> MaskImageType;

  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  typename MaskImageType::Pointer mask;
  mask = MaskImageType::New();
  mask->SetRegions(input->GetLargestPossibleRegion());
  mask->SetOrigin(input->GetOrigin());
  mask->SetSpacing(input->GetSpacing());
  mask->Allocate();
  mask->FillBuffer(0);

  // Get a vector of all values (will be easier to sort)
  // And compute total sum of values
  std::vector<double> values;
  typedef itk::ImageRegionIterator<InputImageType> IteratorInputType;
  IteratorInputType iter(input, input->GetLargestPossibleRegion());
  iter.GoToBegin();
  double total = 0.0;
  while (!iter.IsAtEnd()) {
    values.push_back(iter.Get());
    total += iter.Get();
    ++iter;
  }

  // Sort (reverse)
  std::vector<size_t> indices = sort_indexes(values);

  // Get max index of pixel to reach xx percent
  double current = 0.0;
  double max = GetPercentage()/100.0*total;
  int i=0;
  int n = input->GetLargestPossibleRegion().GetNumberOfPixels();
  std::vector<int> should_keep(values.size());;
  std::fill(should_keep.begin(), should_keep.end(), 0);
  while (current<max and i<n) { // loop by decreasing pixel values
    current += values[indices[i]];
    should_keep[indices[i]] = 1.0;
    ++i;
  }
  int nb = i;

  // Set mask values
  typedef itk::ImageRegionIterator<MaskImageType> IteratorMaskType;
  IteratorMaskType itm(mask, mask->GetLargestPossibleRegion());
  iter.GoToBegin();
  itm.GoToBegin();
  i = 0;
  while (!iter.IsAtEnd()) {
    if (should_keep[i]) itm.Set(1);
    ++iter;
    ++itm;
    ++i;
  }

  // Verbose option
  if (this->m_IOVerbose)
    std::cout << "Sum of pixel values : " << total << std::endl
              << "Percentage          : " << GetPercentage() << "%" << std::endl
              << "Number of pixels    : " << nb << "/" << n << std::endl
              << "Number of pixels    : " << nb/n*100.0 << "%" << std::endl;

  // Write/Save results
  this->template SetNextOutput<MaskImageType>(mask);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <typename T>
bool comparator ( const std::pair<T, size_t>& l, const std::pair<T, size_t>& r)
 { return l.first > r.first; }
//--------------------------------------------------------------------

}//end clitk

#endif  //#define clitkBinarizeImageGenericFilter_cxx
