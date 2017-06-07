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

#ifndef clitkMaskOfIntegratedIntensityGenericFilter_txx
#define clitkMaskOfIntegratedIntensityGenericFilter_txx

// itk include
#include "itkIntensityWindowingImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include <clitkCommon.h>
#include <numeric>

namespace clitk
{

  //--------------------------------------------------------------------
  template<class args_info_type>
  MaskOfIntegratedIntensityGenericFilter<args_info_type>::MaskOfIntegratedIntensityGenericFilter():
    ImageToImageGenericFilter<Self>("MaskOfIntegratedIntensity")
  {
    InitializeImageType<2>();
    InitializeImageType<3>();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class args_info_type>
  template<unsigned int Dim>
  void MaskOfIntegratedIntensityGenericFilter<args_info_type>::InitializeImageType()
  {
    ADD_DEFAULT_IMAGE_TYPES(Dim);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class args_info_type>
  void MaskOfIntegratedIntensityGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
  {
    mArgsInfo=a;
    this->SetIOVerbose(mArgsInfo.verbose_flag);
    if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

    this->SetInputFilename(mArgsInfo.input_arg);
    this->SetOutputFilename(mArgsInfo.output_arg);
    this->SetPercentage(mArgsInfo.percentage_arg);

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
  template<class args_info_type>
  template<class InputImageType>
  void
  MaskOfIntegratedIntensityGenericFilter<args_info_type>::UpdateWithInputImageType()
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

#endif //#define clitkMaskOfIntegratedIntensityGenericFilter_txx
