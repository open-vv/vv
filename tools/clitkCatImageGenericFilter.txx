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
#ifndef clitkCatImageGenericFilter_txx
#define clitkCatImageGenericFilter_txx

#include <itkConstantPadImageFilter.h>
#include <itkPasteImageFilter.h>
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
CatImageGenericFilter<args_info_type>::CatImageGenericFilter():
  ImageToImageGenericFilter<Self>("CatImage")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void CatImageGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
  ADD_DEFAULT_VEC_IMAGE_TYPES;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void CatImageGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

  if (mArgsInfo.input1_given)
    this->AddInputFilename(mArgsInfo.input1_arg);
  if (mArgsInfo.input2_given)
    this->AddInputFilename(mArgsInfo.input2_arg);
  if (mArgsInfo.output_given)
    this->SetOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
CatImageGenericFilter<args_info_type>::UpdateWithInputImageType()
{

  // Reading input 1
  typename InputImageType::Pointer input1 = this->template GetInput<InputImageType>(0);
  typename InputImageType::Pointer input2 = this->template GetInput<InputImageType>(1);

  // Main filter
  typedef typename InputImageType::PixelType PixelType;
  typedef itk::Image<char, InputImageType::ImageDimension> OutputImageType;

  typename InputImageType::RegionType region;
  region = input2->GetLargestPossibleRegion();

  // Filter
  typename InputImageType::SizeType upperBound;
  upperBound.Fill(0);
  upperBound[mArgsInfo.dimension_arg] = region.GetSize()[mArgsInfo.dimension_arg];
  typedef itk::ConstantPadImageFilter<InputImageType, InputImageType> PadImageFilterType;
  typename PadImageFilterType::Pointer pad= PadImageFilterType::New();
  pad->SetInput(input1);
  pad->SetPadUpperBound(upperBound);

  typename InputImageType::IndexType index;
  index.Fill(0);
  index[mArgsInfo.dimension_arg] = input1->GetLargestPossibleRegion().GetSize(mArgsInfo.dimension_arg);

  typedef itk::PasteImageFilter<InputImageType, InputImageType, InputImageType> PasteImageFilterType;
  typename PasteImageFilterType::Pointer pasteFilter=PasteImageFilterType::New();
  pasteFilter->SetSourceImage(input2);
  pasteFilter->SetDestinationImage(pad->GetOutput());
  pasteFilter->SetDestinationIndex(index);
  pasteFilter->SetSourceRegion(region);
  pasteFilter->InPlaceOn();
  pasteFilter->Update();

  typename InputImageType::Pointer output = pasteFilter->GetOutput();

  this->template SetNextOutput<InputImageType>(output);
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkCatImageGenericFilter_txx
