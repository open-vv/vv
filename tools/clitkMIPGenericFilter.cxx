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
#ifndef clitkMIPGenericFilter_cxx
#define clitkMIPGenericFilter_cxx

/* =================================================
 * @file   clitkMIPGenericFilter.cxx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

#include "clitkMIPGenericFilter.h"

// itk include
#include <itkImage.h>
#include <itkMaximumProjectionImageFilter.h>

#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
MIPGenericFilter::MIPGenericFilter():
  ImageToImageGenericFilter<Self>("MIP")
{
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void MIPGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void MIPGenericFilter::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

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
MIPGenericFilter::UpdateWithInputImageType()
{
  typedef typename InputImageType::PixelType PixelType;
  const int Dim = InputImageType::ImageDimension;
  typedef itk::Image<PixelType,Dim> ImageType;
  typedef itk::Image<PixelType,Dim-1> OutputImageType;
  typedef itk::MaximumProjectionImageFilter<ImageType,OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetProjectionDimension(mArgsInfo.dimension_arg);
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
  filter->SetInput(input);
  filter->Update();
  this->template SetNextOutput<OutputImageType>(filter->GetOutput());
}
//--------------------------------------------------------------------


}//end clitk

#endif  //#define clitkMIPGenericFilter_cxx
