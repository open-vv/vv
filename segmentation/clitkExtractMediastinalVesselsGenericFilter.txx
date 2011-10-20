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

#ifndef CLITKEXTRACTLYMPHSTATIONSSGENERICFILTER_TXX
#define CLITKEXTRACTLYMPHSTATIONSSGENERICFILTER_TXX

#include "clitkImageCommon.h"
  
//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::ExtractMediastinalVesselsGenericFilter<ArgsInfoType>::ExtractMediastinalVesselsGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractMediastinalVessels") 
{
  // Default values
  cmdline_parser_clitkExtractMediastinalVessels_init(&mArgsInfo);
  InitializeImageType<3>(); // Only for 3D images
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractMediastinalVesselsGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, short); // Can add float later
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractMediastinalVesselsGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.input_given) AddInputFilename(mArgsInfo.input_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void 
clitk::ExtractMediastinalVesselsGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f)
{
  f->SetVerboseOptionFlag(mArgsInfo.verbose_flag);
  f->SetVerboseStepFlag(mArgsInfo.verboseStep_flag);
  f->SetWriteStepFlag(mArgsInfo.writeStep_flag);
  f->SetVerboseMemoryFlag(mArgsInfo.verboseMemory_flag);
  f->SetAFDBFilename(mArgsInfo.afdb_arg);  

  f->SetThresholdHigh(mArgsInfo.thresholdHigh_arg);
  f->SetThresholdLow(mArgsInfo.thresholdLow_arg);
  f->SetErosionRadius(mArgsInfo.erode_arg);
  f->SetDilatationRadius(mArgsInfo.dilate_arg);
  
  f->SetMaxDistancePostToCarina(mArgsInfo.maxPost_arg);
  f->SetMaxDistanceAntToCarina(mArgsInfo.maxAnt_arg);
  f->SetMaxDistanceLeftToCarina(mArgsInfo.maxLeft_arg);
  f->SetMaxDistanceRightToCarina(mArgsInfo.maxRight_arg);
  
  f->SetSoughtVesselSeedName(mArgsInfo.seed_arg);
  f->SetSoughtVesselName(mArgsInfo.name_arg);
  f->SetMaxNumberOfFoundBifurcation(mArgsInfo.bif_arg);

  f->SetFinalOpeningRadius(mArgsInfo.open_arg);

  f->SetVerboseTrackingFlag(mArgsInfo.verboseTracking_flag);
  
  // Output filename
  this->AddOutputFilename(mArgsInfo.output_arg);
  f->SetOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractMediastinalVesselsGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::ExtractMediastinalVesselsFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetInput(input);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();

  // Write/Save results
  for(uint i=0; i<filter->GetNumberOfOutputs(); i++) {
    typedef uchar MaskImagePixelType;
    typedef itk::Image<MaskImagePixelType, 3> OutputImageType;
    typename OutputImageType::Pointer output = filter->GetOutput(i);
    this->template SetNextOutput<OutputImageType>(output); 
  }
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTLYMPHSTATIONSSGENERICFILTER_TXX
