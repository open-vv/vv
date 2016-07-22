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
#ifndef clitkFooImageGenericFilter_txx
#define clitkFooImageGenericFilter_txx

// itk include
#include "itkBinaryThresholdImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
FooImageGenericFilter<args_info_type>::FooImageGenericFilter():
  ImageToImageGenericFilter<Self>("FooImage")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void FooImageGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void FooImageGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
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
FooImageGenericFilter<args_info_type>::UpdateWithInputImageType()
{

  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  // Main filter
  typedef typename InputImageType::PixelType PixelType;
  typedef itk::Image<char, InputImageType::ImageDimension> OutputImageType;

  // Filter
  typedef itk::BinaryThresholdImageFilter<InputImageType, OutputImageType> BinaryThresholdImageFilterType;
  typename BinaryThresholdImageFilterType::Pointer thresholdFilter=BinaryThresholdImageFilterType::New();
  thresholdFilter->SetInput(input);
  // Set filter members

  // Write/Save results
  typename OutputImageType::Pointer outputImage = thresholdFilter->GetOutput();
  this->template SetNextOutput<OutputImageType>(outputImage);
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkFooImageGenericFilter_txx
