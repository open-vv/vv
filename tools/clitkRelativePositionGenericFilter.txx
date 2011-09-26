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

#include "clitkImageCommon.h"

//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::RelativePositionGenericFilter<ArgsInfoType>::
RelativePositionGenericFilter():
  ImageToImageGenericFilter<Self>("RelativePosition")
{
  // Default values
  cmdline_parser_clitkRelativePosition_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::RelativePositionGenericFilter<ArgsInfoType>::
InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::RelativePositionGenericFilter<ArgsInfoType>::
SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given) AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.object_given) AddInputFilename(mArgsInfo.object_arg);
  if (mArgsInfo.output_given) AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void clitk::RelativePositionGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f) 
{
  f->SetVerboseOptionFlag(mArgsInfo.verboseOptions_flag);
  f->SetVerboseStepFlag(mArgsInfo.verboseStep_flag);
  f->SetWriteStepFlag(mArgsInfo.writeStep_flag);

  for(uint i=0; i<mArgsInfo.orientation_given; i++) {
    f->AddOrientationTypeString(mArgsInfo.orientation_arg[i]);
  }
  
  if (mArgsInfo.spacing_given) {
    f->IntermediateSpacingFlagOn();
    f->SetIntermediateSpacing(mArgsInfo.spacing_arg);
  }
  else {
    f->IntermediateSpacingFlagOff();
  }

  f->SetFuzzyThreshold(mArgsInfo.threshold_arg);
  f->SetRemoveObjectFlag(!mArgsInfo.doNotRemoveObject_flag);
  f->SetAutoCropFlag(!mArgsInfo.noAutoCrop_flag);
  f->SetCombineWithOrFlag(mArgsInfo.combineWithOr_flag);
  f->SetInverseOrientationFlag(mArgsInfo.inverse_flag);
  
}

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::RelativePositionGenericFilter<ArgsInfoType>::
UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);
  typename ImageType::Pointer object = this->template GetInput<ImageType>(1);

  if (mArgsInfo.sliceBySlice_flag) {
    // Create filter
    typedef clitk::SliceBySliceRelativePositionFilter<ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    
    // Set the filter (needed for example for threaded monitoring)
    this->SetFilterBase(filter);
    
    // Set global Options 
    filter->SetInput(input);
    filter->SetInputObject(object);
    SetOptionsFromArgsInfoToFilter<FilterType>(filter);

    // Set options only for SliceBySliceRelativePositionFilter
    filter->SetDirection(mArgsInfo.direction_arg);
    filter->SetUniqueConnectedComponentBySliceFlag(mArgsInfo.uniqueCCL_flag);
    if (mArgsInfo.uniqueObjectCCL_flag) {
      filter->UseTheLargestObjectCCLFlagOn();
    }
    else {
      filter->UseTheLargestObjectCCLFlagOff();
    }
    
    // Go !
    filter->Update();
    
    // Write/Save results
    typename ImageType::Pointer output = filter->GetOutput();
    this->template SetNextOutput<ImageType>(output); 
  }
  else {
    // Create filter
    typedef clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    
    // Set the filter (needed for example for threaded monitoring)
    this->SetFilterBase(filter);
    
    // Set global Options 
    filter->SetInput(input);
    filter->SetInputObject(object);
    if (mArgsInfo.angle1_given && mArgsInfo.angle2_given)
      filter->AddAngles(mArgsInfo.angle1_arg, mArgsInfo.angle2_arg);
    SetOptionsFromArgsInfoToFilter<FilterType>(filter);
   
    // Go !
    filter->Update();
    
    // Write/Save results
    typename ImageType::Pointer output = filter->GetOutput();
    this->template SetNextOutput<ImageType>(output); 
  }
}
//--------------------------------------------------------------------


