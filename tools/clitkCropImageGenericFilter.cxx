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
#ifndef clitkCropImageGenericFilter_cxx
#define clitkCropImageGenericFilter_cxx

#include "clitkCropImageGenericFilter.h"

//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
clitk::CropImageGenericFilter::CropImageGenericFilter():
  ImageToImageGenericFilter<Self>("CropImage")
{
  cmdline_parser_clitkCropImage_init(&mArgsInfo);
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}

//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::CropImageGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
  ADD_DEFAULT_VEC_IMAGE_TYPES;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::CropImageGenericFilter::SetArgsInfo(const args_info_type& a)
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given)   AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
typename clitk::CropImageGenericFilter::AutoCrop<ImageType>::ImagePointer
clitk::CropImageGenericFilter::AutoCrop<ImageType>::Do(args_info_type &argsInfo, ImagePointer input)
{
  static const unsigned int PixelDimension = itk::PixelTraits<typename ImageType::PixelType>::Dimension;
  return this->Do(argsInfo, input, static_cast< PixelDimType<PixelDimension> *>(NULL) );
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
template<unsigned int Dim>
typename clitk::CropImageGenericFilter::AutoCrop<ImageType>::ImagePointer
clitk::CropImageGenericFilter::AutoCrop<ImageType>::Do(args_info_type &, ImagePointer, PixelDimType<Dim> *)
{
  clitkExceptionMacro("Autocrop is not implemented for vector fields");
  return ITK_NULLPTR;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
typename clitk::CropImageGenericFilter::AutoCrop<ImageType>::ImagePointer
clitk::CropImageGenericFilter::AutoCrop<ImageType>::Do(args_info_type &argsInfo, ImagePointer input, PixelDimType<1> *)
{
  if (argsInfo.boundingBox_given)
    clitkExceptionMacro("Do not use --BG and --boundingBox at the same time");
  if (argsInfo.lower_given)
    clitkExceptionMacro("Do not use --BG and --lower at the same time");
  if (argsInfo.upper_given)
    clitkExceptionMacro("Do not use --BG and --upper at the same time");
  typedef clitk::AutoCropFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input);
  filter->SetBackgroundValue(argsInfo.BG_arg);
  filter->Update();
  return filter->GetOutput();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ImageType>
void clitk::CropImageGenericFilter::UpdateWithInputImageType()
{
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);
  typename ImageType::RegionType input_region = input->GetLargestPossibleRegion();

  // Check options
  if (mArgsInfo.BG_given && mArgsInfo.like_given)
    clitkExceptionMacro("Do not use --BG and --like at the same time");
  if (mArgsInfo.updateOrigin_flag && !mArgsInfo.like_given)
    clitkExceptionMacro("Use --updateOrigin with --like");

  // Prepare output
  typename ImageType::Pointer output;

  // ------------------------------------------------
  if (mArgsInfo.BG_given) { // AutoCrop filter
    AutoCrop<ImageType> autoCrop;
    output = autoCrop.Do(mArgsInfo, input);
  }
  else {
    // ------------------------------------------------
    if (mArgsInfo.like_given) { // CropLike filter
    if (mArgsInfo.boundingBox_given)
      clitkExceptionMacro("Do not use --like and --boundingBox at the same time");
    if (mArgsInfo.lower_given)
      clitkExceptionMacro("Do not use --like and --lower at the same time");
    if (mArgsInfo.upper_given)
      clitkExceptionMacro("Do not use --like and --upper at the same time");
      typedef clitk::CropLikeImageFilter<ImageType> FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      filter->SetInput(input);
      filter->SetCropLikeFilename(mArgsInfo.like_arg);
      filter->SetBackgroundValue(mArgsInfo.BGLike_arg);
      filter->Update();
      output = filter->GetOutput();
    }
    else {
      // ------------------------------------------------
      typename ImageType::SizeType lSize;
      typename ImageType::SizeType uSize;
      if (mArgsInfo.verbose_flag) std::cout << "input region " << input_region << std::endl;
      if (mArgsInfo.boundingBox_given) {
        for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
          lSize[i] = mArgsInfo.boundingBox_arg[2*i];
          uSize[i] = input_region.GetSize()[i]-mArgsInfo.boundingBox_arg[2*i+1]-1;
        }
      }
      else {
        if (mArgsInfo.lower_given) {
          for(unsigned int i=0; i<ImageType::ImageDimension; i++)
            lSize[i]=static_cast<unsigned int >(mArgsInfo.lower_arg[i]);
        }
        else lSize.Fill(0);
        if (mArgsInfo.upper_given) {
          for(unsigned int i=0; i<ImageType::ImageDimension; i++)
            uSize[i]=static_cast<unsigned int >(mArgsInfo.upper_arg[i]);
        }
        else uSize.Fill(0);
      }

      if (mArgsInfo.verbose_flag) {
        std::cout << "lower " << lSize << " upper " << uSize << std::endl;
      }

      typedef  itk::CropImageFilter<ImageType, ImageType> CropImageFilterType;
      typename CropImageFilterType::Pointer filter=CropImageFilterType::New();
      filter->SetInput(input);
      filter->SetLowerBoundaryCropSize(lSize);
      filter->SetUpperBoundaryCropSize(uSize);
      filter->Update();
      output = filter->GetOutput();
    }
  }

  // Force origin if needed
  if (mArgsInfo.origin_flag) {
    typename ImageType::PointType origin;
    origin.Fill(itk::NumericTraits<double>::Zero);
    output->SetOrigin(origin);
  }

  // adjust image origin and force index to zero
  typename ImageType::RegionType region = output->GetLargestPossibleRegion();
  typename ImageType::IndexType index = region.GetIndex();
  typename ImageType::PointType origin = output->GetOrigin();
  typename ImageType::SpacingType spacing = output->GetSpacing();
  if (!mArgsInfo.BG_given && (!mArgsInfo.like_given || mArgsInfo.updateOrigin_flag)) {
    if (mArgsInfo.verbose_flag) std::cout << "origin before crop " << origin << std::endl;
    input->TransformIndexToPhysicalPoint(index,origin);
    if (mArgsInfo.verbose_flag) std::cout << "origin after crop " << origin << std::endl;
    output->SetOrigin(origin);
  }

  index.Fill(itk::NumericTraits<double>::Zero);
  region.SetIndex(index);
  output->SetRegions(region);

  // Write/Save results
  this->template SetNextOutput<ImageType>(output);
}
//--------------------------------------------------------------------

#endif  //#define clitkCropImageGenericFilter_cxx
