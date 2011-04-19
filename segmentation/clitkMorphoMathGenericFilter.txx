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

#include "clitkSegmentationUtils.h"

//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::MorphoMathGenericFilter<ArgsInfoType>::
MorphoMathGenericFilter():
  ImageToImageGenericFilter<MorphoMathGenericFilter<ArgsInfoType> >("MorphMath")
{
  // Default values
  cmdline_parser_clitkMorphoMath_init(&mArgsInfo);
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::MorphoMathGenericFilter<ArgsInfoType>::
InitializeImageType() 
{  
  ADD_DEFAULT_IMAGE_TYPES(Dim);
  //ADD_IMAGE_TYPE(Dim, short); // 
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::MorphoMathGenericFilter<ArgsInfoType>::
SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given) AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given) AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void clitk::MorphoMathGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f) 
{
  f->SetVerboseFlag(mArgsInfo.verbose_flag);
  f->SetExtendSupportFlag(mArgsInfo.extend_flag);
  f->SetOperationType(mArgsInfo.type_arg);
  f->SetBackgroundValue(mArgsInfo.bg_arg);
  f->SetForegroundValue(mArgsInfo.fg_arg);
  f->SetBoundaryToForegroundFlag(mArgsInfo.bound_flag);
  if (mArgsInfo.radius_given && mArgsInfo.radiusInMM_given) {
    clitkExceptionMacro("Please give --radius OR --radiusInMM, not both.");
  }

  if (mArgsInfo.radiusInMM_given) {
    typename FilterType::PointType p;
    if (mArgsInfo.radiusInMM_given) {
      ConvertOptionMacro(mArgsInfo.radiusInMM, p, FilterType::ImageDimension, false);
      f->SetRadiusInMM(p);
    }
  }
  else {
    typename FilterType::SizeType r;
    if (mArgsInfo.radius_given) {
      ConvertOptionMacro(mArgsInfo.radius, r, FilterType::ImageDimension, false);
      f->SetRadius(r);
    }
  }
}

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::MorphoMathGenericFilter<ArgsInfoType>::
UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::MorphoMathFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set the filter (needed for example for threaded monitoring)
  this->SetFilterBase(filter);
    
  // Set global Options 
  filter->SetInput(input);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();
  
  // Write/Save results
  typename ImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<ImageType>(output); 
}
//--------------------------------------------------------------------


