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
#ifndef clitkImageGradientMagnitudeGenericFilter_txx
#define clitkImageGradientMagnitudeGenericFilter_txx

/* =================================================
 * @file   clitkImageGradientMagnitudeGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

// itk include
#include "itkGradientMagnitudeImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
ImageGradientMagnitudeGenericFilter<args_info_type>::ImageGradientMagnitudeGenericFilter():
  ImageToImageGenericFilter<Self>("ImageGradientMagnitude")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void ImageGradientMagnitudeGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void ImageGradientMagnitudeGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
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
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
ImageGradientMagnitudeGenericFilter<args_info_type>::UpdateWithInputImageType()
{

  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  // Main filter
  typedef typename InputImageType::PixelType PixelType;
  typedef itk::Image<float, InputImageType::ImageDimension> OutputImageType;

  // Filter
  typedef itk::GradientMagnitudeImageFilter<InputImageType, OutputImageType> GradientMagnitudeImageFilterType;
  typename GradientMagnitudeImageFilterType::Pointer gradientFilter=GradientMagnitudeImageFilterType::New();
  gradientFilter->SetInput(input);
  gradientFilter->Update();

  typename OutputImageType::Pointer outputImage = gradientFilter->GetOutput();
  this->template SetNextOutput<OutputImageType>(outputImage);
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkImageGradientMagnitudeGenericFilter_txx
