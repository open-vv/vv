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

#ifndef CLITKEXTRACTLUNGSGENERICFILTER_TXX
#define CLITKEXTRACTLUNGSGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractLungGenericFilter<ArgsInfoType>::ExtractLungGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractLung") 
{
  this->SetFilterBase(NULL);
  // Default values
  cmdline_parser_clitkExtractLung_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractLungGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, short);
  // ADD_IMAGE_TYPE(Dim, int);
  // ADD_IMAGE_TYPE(Dim, float);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractLungGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given)   AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
  if (mArgsInfo.outputTrachea_given)  AddOutputFilename(mArgsInfo.outputTrachea_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void clitk::ExtractLungGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f) 
{
  //f->SetVerboseFlag(mArgsInfo.verbose_flag);
  f->SetVerboseOptionFlag(mArgsInfo.verboseOption_flag);
  f->SetVerboseStepFlag(mArgsInfo.verboseStep_flag);
  f->SetWriteStepFlag(mArgsInfo.writeStep_flag);
  f->SetVerboseWarningFlag(!mArgsInfo.verboseWarningOff_flag);
  f->SetVerboseMemoryFlag(mArgsInfo.verboseMemory_flag);

  if (mArgsInfo.afdb_given)
    f->SetAFDBFilename(mArgsInfo.afdb_arg);
  f->SetOutputLungFilename(mArgsInfo.output_arg);
  f->SetOutputTracheaFilename(mArgsInfo.outputTrachea_arg);
  
  f->SetUpperThreshold(mArgsInfo.upper_arg);
  f->SetLowerThreshold(mArgsInfo.lower_arg);
  
  f->SetNumberOfSlicesToSkipBeforeSearchingSeed(mArgsInfo.skipslices_arg);

  f->SetTracheaVolumeMustBeCheckedFlag(!mArgsInfo.doNotCheckTracheaVolume_flag);
  f->SetVerboseRegionGrowingFlag(mArgsInfo.verboseRG_flag);

  f->SetUpperThresholdForTrachea(mArgsInfo.upperThresholdForTrachea_arg);
  f->SetMultiplierForTrachea(mArgsInfo.multiplierForTrachea_arg);
  f->SetThresholdStepSizeForTrachea(mArgsInfo.thresholdStepSizeForTrachea_arg);

  typename FilterType::InputImageIndexType s;
  if (mArgsInfo.seed_given) {
    ConvertOptionMacro(mArgsInfo.seed, s, 3, false);
  f->AddSeed(s);
  }

  f->SetMinimalComponentSize(mArgsInfo.minSize_arg);
  f->SetNumberOfHistogramBins(mArgsInfo.bins_arg);
  f->SetRadiusForTrachea(mArgsInfo.radius_arg);
  
  f->SetOpenCloseFlag(mArgsInfo.openclose_flag);
  f->SetOpenCloseRadius(mArgsInfo.opencloseRadius_arg);
  
  if (mArgsInfo.doNotFillHoles_given)
    f->SetFillHolesFlag(false);
  else
    f->SetFillHolesFlag(true);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractLungGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Mask & output image type
  typedef itk::Image<uchar, ImageType::ImageDimension> MaskImageType;

  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::ExtractLungFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();

  // Set the filter (needed for example for threaded monitoring)
  this->SetFilterBase(filter);
    
  // Set global Options 
  filter->SetInput(input);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();
  
  // Write/Save results
  typename MaskImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<MaskImageType>(output); 
  this->template SetNextOutput<typename FilterType::MaskImageType>(filter->GetTracheaImage()); 
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTLUNGSGENERICFILTER_TXX
