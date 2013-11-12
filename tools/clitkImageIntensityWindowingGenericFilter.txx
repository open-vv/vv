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
#ifndef clitkImageIntensityWindowingGenericFilter_txx
#define clitkImageIntensityWindowingGenericFilter_txx

/* =================================================
 * @file   clitkImageIntensityWindowingGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

// itk include
#include "itkIntensityWindowingImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
ImageIntensityWindowingGenericFilter<args_info_type>::ImageIntensityWindowingGenericFilter():
  ImageToImageGenericFilter<Self>("ImageIntensityWindowing")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void ImageIntensityWindowingGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void ImageIntensityWindowingGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

  if (mArgsInfo.input_given) {
    this->SetInputFilename(mArgsInfo.input_arg);
  }
  if (mArgsInfo.output_given) {
    this->SetOutputFilename(mArgsInfo.output_arg);
  }
  if (mArgsInfo.mask_given) {
    this->AddInputFilename(mArgsInfo.mask_arg);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
ImageIntensityWindowingGenericFilter<args_info_type>::UpdateWithInputImageType()
{

  // Main filter
  typedef typename InputImageType::PixelType InputPixelType;
  typedef itk::Image<float, InputImageType::ImageDimension> OutputImageType;
  typedef itk::Image<unsigned char, InputImageType::ImageDimension> MaskImageType;

  // Reading input
  typename OutputImageType::Pointer input = this->template GetInput<OutputImageType>(0);

  typename MaskImageType::Pointer mask = NULL;
  if(mArgsInfo.mask_given) {
   mask = this->template GetInput<MaskImageType>(1);
  }
  else {
   mask = MaskImageType::New();
   mask->SetRegions(input->GetLargestPossibleRegion());
   mask->SetOrigin(input->GetOrigin());
   mask->SetSpacing(input->GetSpacing());
   mask->Allocate();
   mask->FillBuffer(1);
  }
  // Set mask iterator
  typedef itk::ImageRegionIterator<MaskImageType> IteratorMaskType;
  IteratorMaskType itm(mask, mask->GetLargestPossibleRegion());
  //
  typedef itk::LabelStatisticsImageFilter< OutputImageType, MaskImageType > LabelStatisticsImageFilterType;
  typename LabelStatisticsImageFilterType::Pointer labelStatisticsImageFilter = LabelStatisticsImageFilterType::New();
  labelStatisticsImageFilter->SetLabelInput( mask );
  labelStatisticsImageFilter->SetInput(input);
  labelStatisticsImageFilter->Update();
  float maxImg = labelStatisticsImageFilter->GetMaximum(1);
  //std::cout << "maxImg: " << maxImg << std::endl;
  // Filter
  typedef itk::IntensityWindowingImageFilter <OutputImageType, OutputImageType> IntensityWindowingImageFilterType;
  typename IntensityWindowingImageFilterType::Pointer filter = IntensityWindowingImageFilterType::New();
  filter->SetInput(input);
  filter->SetWindowMinimum(0.0);
  filter->SetWindowMaximum(maxImg);
  filter->SetOutputMinimum(0.0);
  filter->SetOutputMaximum(1.0);
  filter->Update();

  // Set iterator
  typedef itk::ImageRegionIterator<OutputImageType> IteratorType;
  IteratorType it(filter->GetOutput(), filter->GetOutput()->GetLargestPossibleRegion());
  //A little bit strange to do that but I think I need to do it.
  // Create output image
  typename OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage->SetRegions(filter->GetOutput()->GetLargestPossibleRegion());
  outputImage->SetOrigin(filter->GetOutput()->GetOrigin());
  outputImage->SetSpacing(filter->GetOutput()->GetSpacing());
  outputImage->Allocate();
  outputImage->FillBuffer(-100.0);
  // Set output iterator
  typedef itk::ImageRegionIterator<OutputImageType> IteratorOutputType;
  IteratorOutputType ito = IteratorOutputType(outputImage, outputImage->GetLargestPossibleRegion());

  it.GoToBegin();
  ito.GoToBegin();
  itm.GoToBegin();

  while (!ito.IsAtEnd()) {
      if(itm.Get() > 0) {
          ito.Set(it.Get());
      }
      ++it;
      ++ito;
      ++itm;
  }

  // Write/Save results
  this->template SetNextOutput<OutputImageType>(outputImage);
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkImageIntensityWindowingGenericFilter_txx
