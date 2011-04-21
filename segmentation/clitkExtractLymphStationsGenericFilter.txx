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
clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::ExtractLymphStationsGenericFilter():
  ImageToImageGenericFilter<Self>("ExtractLymphStations") 
{
  // Default values
  cmdline_parser_clitkExtractLymphStations_init(&mArgsInfo);
  InitializeImageType<3>(); // Only for 3D images
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, short); // Can add float later
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::SetArgsInfo(const ArgsInfoType & a) 
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
void 
clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f)
{
  f->SetVerboseOptionFlag(mArgsInfo.verbose_flag);
  f->SetVerboseStepFlag(mArgsInfo.verboseStep_flag);
  f->SetWriteStepFlag(mArgsInfo.writeStep_flag);
  f->SetVerboseMemoryFlag(mArgsInfo.verboseMemory_flag);
  f->SetAFDBFilename(mArgsInfo.afdb_arg);  

  // Station 8
  f->SetDistanceMaxToAnteriorPartOfTheSpine(mArgsInfo.maxAntSpine_arg);
  f->SetFuzzyThreshold("8", "Esophagus", mArgsInfo.tS8_Esophagus_arg);
  f->SetInjectedThresholdForS8(mArgsInfo.injectedThresholdForS8_arg);

  // Check multiple options for radius dilatation
  /*
    typename FilterType::MaskImagePointType p;
    SetMultipleOptionMacro(mArgsInfo, esophagusDilatation, 3, p);
    default ? = set before
    exception if fail
   */
  typename FilterType::MaskImagePointType p;
  p[0] = 7; p[1] = 5; p[2] = 0; // default value
  if (mArgsInfo.esophagusDilatationForAnt_given == 3) {
    for(uint i=0; i<3; i++)
      p[i] = mArgsInfo.esophagusDilatationForAnt_arg[i];
  }
  else {
    if (mArgsInfo.esophagusDilatationForAnt_given == 1) {
      for(uint i=0; i<3; i++)
        p[i] = mArgsInfo.esophagusDilatationForAnt_arg[0];
    }
  }
  f->SetEsophagusDiltationForAnt(p);
  
  p[0] = 5; p[1] = 10; p[2] = 1; // default value
  if (mArgsInfo.esophagusDilatationForRight_given == 3) {
    for(uint i=0; i<3; i++)
      p[i] = mArgsInfo.esophagusDilatationForRight_arg[i];
  }
  else {
    if (mArgsInfo.esophagusDilatationForRight_given == 1) {
      for(uint i=0; i<3; i++)
        p[i] = mArgsInfo.esophagusDilatationForRight_arg[0];
    }
  }
  f->SetEsophagusDiltationForRight(p);  
  
  for(uint i=0; i<mArgsInfo.station_given; i++)
    f->AddComputeStation(mArgsInfo.station_arg[i]);

  // Station 7
  f->SetFuzzyThreshold("7", "Bronchi", mArgsInfo.tS7_Bronchi_arg);
  f->SetFuzzyThreshold("7", "LeftSuperiorPulmonaryVein", mArgsInfo.tS7_LeftSuperiorPulmonaryVein_arg);
  f->SetFuzzyThreshold("7", "RightSuperiorPulmonaryVein", mArgsInfo.tS7_RightSuperiorPulmonaryVein_arg);
  f->SetFuzzyThreshold("7", "RightPulmonaryArtery", mArgsInfo.tS7_RightPulmonaryArtery_arg);
  f->SetFuzzyThreshold("7", "LeftPulmonaryArtery", mArgsInfo.tS7_LeftPulmonaryArtery_arg);
  f->SetFuzzyThreshold("7", "SVC", mArgsInfo.tS7_SVC_arg);

  // Station 3A
  f->SetFuzzyThreshold("3A", "Sternum", mArgsInfo.tS3A_Sternum_arg);
  f->SetFuzzyThreshold("3A", "SubclavianArtery", mArgsInfo.tS3A_SubclavianArtery_arg);
  
  // Station 2RL
  f->SetFuzzyThreshold("2RL", "CommonCarotidArtery", mArgsInfo.tS2RL_CommonCarotidArtery_arg);
  f->SetFuzzyThreshold("2RL", "BrachioCephalicTrunk", mArgsInfo.tS2RL_BrachioCephalicTrunk_arg);
  f->SetFuzzyThreshold("2RL", "BrachioCephalicVein", mArgsInfo.tS2RL_BrachioCephalicVein_arg);
  f->SetFuzzyThreshold("2RL", "Aorta", mArgsInfo.tS2RL_Aorta_arg);
  f->SetFuzzyThreshold("2RL", "SubclavianArteryLeft", mArgsInfo.tS2RL_SubclavianArteryLeft_arg);
  f->SetFuzzyThreshold("2RL", "SubclavianArteryRight", mArgsInfo.tS2RL_SubclavianArteryRight_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::ExtractLymphStationsGenericFilter<ArgsInfoType>::UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  // Create filter
  typedef clitk::ExtractLymphStationsFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
    
  // Set global Options 
  filter->SetInput(input);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();

  // Write/Save results
  typedef uchar MaskImagePixelType;
  typedef itk::Image<MaskImagePixelType, 3> OutputImageType;
  typename OutputImageType::Pointer output = filter->GetOutput();
  this->template SetNextOutput<OutputImageType>(output); 
}
//--------------------------------------------------------------------

#endif //#define CLITKEXTRACTLYMPHSTATIONSSGENERICFILTER_TXX
