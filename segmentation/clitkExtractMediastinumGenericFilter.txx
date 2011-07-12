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

#ifndef CLITKEXTRACTMEDIASTINUMSGENERICFILTER_TXX
#define CLITKEXTRACTMEDIASTINUMSGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::
ExtractMediastinumGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractMediastinum") 
{
  // Default values
  cmdline_parser_clitkExtractMediastinum_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void 
clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::
InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, short);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void 
clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::
SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given) AddInputFilename(mArgsInfo.input_arg);
  if (mArgsInfo.output_given)  AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void 
clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f)
{
  f->SetVerboseOptionFlag(mArgsInfo.verbose_flag);
  f->SetVerboseStepFlag(mArgsInfo.verboseStep_flag);
  f->SetWriteStepFlag(mArgsInfo.writeStep_flag);
  f->SetAFDBFilename(mArgsInfo.afdb_arg);  
  f->SetOutputMediastinumFilename(mArgsInfo.output_arg);
  f->SetVerboseMemoryFlag(mArgsInfo.verboseMemory_flag);

  f->SetDistanceMaxToAnteriorPartOfTheVertebralBody(mArgsInfo.maxAntSpine_arg);
  f->SetUseBones(mArgsInfo.useBones_flag);

  f->SetIntermediateSpacing(mArgsInfo.spacing_arg);
  f->SetFuzzyThreshold("LR_lungs", mArgsInfo.ft_LR_lungs_arg);
  f->SetFuzzyThreshold("bones", mArgsInfo.ft_bones_arg);
  f->SetFuzzyThreshold("inf_lungs", mArgsInfo.ft_inf_lungs_arg);
  f->SetFuzzyThreshold("ant_sternum", mArgsInfo.ft_ant_sternum_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractMediastinumGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::ExtractMediastinumFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetInput(input);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();

  // Write/Save results
  typename FilterType::MaskImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<typename FilterType::MaskImageType>(output); 
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTMEDIASTINUMSGENERICFILTER_TXX
