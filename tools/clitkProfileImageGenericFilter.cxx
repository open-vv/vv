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
#ifndef clitkProfileImageGenericFilter_cxx
#define clitkProfileImageGenericFilter_cxx

/* =================================================
 * @file   clitkProfileImageGenericFilter.cxx
 * @author Thomas Baudier <thomas.baudier@creatis.insa-lyon.fr>
 * @date   22 dec 2015
 *
 * @brief
 *
 ===================================================*/

#include "clitkProfileImageGenericFilter.h"

// itk include
#include "itkBinaryThresholdImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"

#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
ProfileImageGenericFilter::ProfileImageGenericFilter():
  ImageToImageGenericFilter<Self>("ProfileImage")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void ProfileImageGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void ProfileImageGenericFilter::SetArgsInfo(const args_info_type & a)
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
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class InputImageType>
void
ProfileImageGenericFilter::UpdateWithInputImageType()
{

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
//--------------------------------------------------------------------


}//end clitk

#endif  //#define clitkProfileImageGenericFilter_cxx
