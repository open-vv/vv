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

#ifndef CLITKEXTRACTBONESSGENERICFILTER_TXX
#define CLITKEXTRACTBONESSGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractBonesGenericFilter<ArgsInfoType>::ExtractBonesGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractBones") 
{
  // Default values
  cmdline_parser_clitkExtractBones_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractBonesGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, short);
  // ADD_IMAGE_TYPE(Dim, int);
  ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractBonesGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given)   this->AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given)  this->AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void clitk::ExtractBonesGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f) 
{
  f->SetVerboseOptionFlag(mArgsInfo.verboseOption_flag);
  f->SetVerboseStepFlag(mArgsInfo.verboseStep_flag);
  f->SetWriteStepFlag(mArgsInfo.writeStep_flag);
  f->SetVerboseWarningFlag(!mArgsInfo.verboseWarningOff_flag);
  f->SetVerboseMemoryFlag(mArgsInfo.verboseMemory_flag);

  if (mArgsInfo.afdb_given)
    f->SetAFDBFilename(mArgsInfo.afdb_arg);

  f->SetOutputBonesFilename(mArgsInfo.output_arg);

  f->SetInitialSmoothing(mArgsInfo.smooth_flag);
  f->SetSmoothingConductanceParameter(mArgsInfo.cond_arg);
  f->SetSmoothingNumberOfIterations(mArgsInfo.iter_arg);
  f->SetSmoothingTimeStep(mArgsInfo.time_arg);
  f->SetSmoothingUseImageSpacing(mArgsInfo.spacing_flag);

  f->SetMinimalComponentSize(mArgsInfo.minSize_arg);
  f->SetUpperThreshold1(mArgsInfo.upper1_arg);
  f->SetLowerThreshold1(mArgsInfo.lower1_arg);
  f->SetFullConnectivity(mArgsInfo.full_flag);

  f->SetUpperThreshold2(mArgsInfo.upper2_arg);
  f->SetLowerThreshold2(mArgsInfo.lower2_arg);

  typename FilterType::InputImageSizeType s;
  if (mArgsInfo.radius2_given) {
    ConvertOptionMacro(mArgsInfo.radius2, s, 3, false);
    f->SetRadius2(s);
  }

  f->SetSampleRate2(mArgsInfo.sampleRate2_arg);
  f->SetAutoCrop(!mArgsInfo.noAutoCrop_flag);
  f->SetFillHoles(!mArgsInfo.doNotFillHoles_flag);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractBonesGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Mask & output image type
  typedef itk::Image<uchar, ImageType::ImageDimension> MaskImageType;

  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::ExtractBonesFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetInput(input);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();
  
  // Write/Save results
  typename MaskImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<MaskImageType>(output); 
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTBONESSGENERICFILTER_TXX
