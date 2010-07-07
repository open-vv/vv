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

#ifndef CLITKEXTRACTMEDIASTINUMFILTER_TXX
#define CLITKEXTRACTMEDIASTINUMFILTER_TXX

// clitk
#include "clitkCommon.h"
#include "clitkExtractMediastinumFilter.h"
#include "clitkAddRelativePositionConstraintToLabelImageFilter.h"
#include "clitkSegmentationUtils.h"

// itk
#include <deque>
#include "itkStatisticsLabelMapFilter.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

// itk ENST
#include "RelativePositionPropImageFilter.h"

//--------------------------------------------------------------------
template <class TImageType>
clitk::ExtractMediastinumFilter<TImageType>::
ExtractMediastinumFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<TImageType, TImageType>()
{
  this->SetNumberOfRequiredInputs(3);

  SetBackgroundValuePatient(0);
  SetBackgroundValueLung(0);
  SetBackgroundValueBones(0);
  SetForegroundValueLeftLung(1);
  SetForegroundValueRightLung(2);
  SetBackgroundValue(0);
  SetForegroundValue(1);

  SetIntermediateSpacing(6);
  SetFuzzyThreshold1(0.6);
  SetFuzzyThreshold2(0.7);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinumFilter<TImageType>::
SetInputPatientLabelImage(const TImageType * image, ImagePixelType bg) {
  this->SetNthInput(0, const_cast<TImageType *>(image));
  m_BackgroundValuePatient = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinumFilter<TImageType>::
SetInputLungLabelImage(const TImageType * image, ImagePixelType bg, 
                       ImagePixelType fgLeft, ImagePixelType fgRight) {
  this->SetNthInput(1, const_cast<TImageType *>(image));
  m_BackgroundValueLung = bg;
  m_ForegroundValueLeftLung = fgLeft;
  m_ForegroundValueRightLung = fgRight;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinumFilter<TImageType>::
SetInputBonesLabelImage(const TImageType * image, ImagePixelType bg) {
  this->SetNthInput(2, const_cast<TImageType *>(image));
  m_BackgroundValueBones = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
template<class ArgsInfoType>
void 
clitk::ExtractMediastinumFilter<TImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);

  SetBackgroundValuePatient_GGO(mArgsInfo);
  SetBackgroundValueLung_GGO(mArgsInfo);
  SetBackgroundValueBones_GGO(mArgsInfo);

  SetForegroundValueLeftLung_GGO(mArgsInfo);
  SetForegroundValueRightLung_GGO(mArgsInfo);

  SetIntermediateSpacing_GGO(mArgsInfo);
  SetFuzzyThreshold1_GGO(mArgsInfo);
  SetFuzzyThreshold2_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinumFilter<TImageType>::
GenerateOutputInformation() { 
  ImagePointer input = dynamic_cast<TImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(outputImage->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinumFilter<TImageType>::
GenerateInputRequestedRegion() {
  // Call default
  Superclass::GenerateInputRequestedRegion();  
  // Get input pointers
  ImagePointer patient = dynamic_cast<TImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer lung    = dynamic_cast<TImageType*>(itk::ProcessObject::GetInput(1));
  ImagePointer bones   = dynamic_cast<TImageType*>(itk::ProcessObject::GetInput(2));
    
  patient->SetRequestedRegion(patient->GetLargestPossibleRegion());
  lung->SetRequestedRegion(lung->GetLargestPossibleRegion());
  bones->SetRequestedRegion(bones->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinumFilter<TImageType>::
GenerateData() {
  // Get input pointers
  ImageConstPointer patient = dynamic_cast<const TImageType*>(itk::ProcessObject::GetInput(0));
  ImageConstPointer lung   = dynamic_cast<const TImageType*>(itk::ProcessObject::GetInput(1));
  ImageConstPointer bones   = dynamic_cast<const TImageType*>(itk::ProcessObject::GetInput(2));
    
  // Get output pointer
  ImagePointer output;

  // Step 1: patient minus lungs, minus bones
  StartNewStep("Patient contours minus lungs and minus bones");
  typedef clitk::BooleanOperatorLabelImageFilter<ImageType> BoolFilterType;
  typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
  boolFilter->InPlaceOn();
  boolFilter->SetInput1(patient);
  boolFilter->SetInput2(lung);    
  boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update();    
  boolFilter->SetInput1(boolFilter->GetOutput());
  boolFilter->SetInput2(bones);
  boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update(); 
  output = boolFilter->GetOutput();

  output = clitk::AutoCrop<ImageType>(output, GetBackgroundValue()); 
  ////autoCropFilter->GetOutput();  typedef clitk::AutoCropFilter<ImageType> CropFilterType;
  //typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  //cropFilter->SetInput(output);
  //cropFilter->Update();   
  //output = cropFilter->GetOutput();

  this->template StopCurrentStep<TImageType>(output);

  // Step 2: LR limits from lung (need separate lung ?)
  StartNewStep("Left/Right limits with lungs");
  typedef clitk::AddRelativePositionConstraintToLabelImageFilter<TImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInput(output); 
  relPosFilter->SetInputObject(lung); 
  relPosFilter->SetOrientationType(RelPosFilterType::LeftTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold1());
  relPosFilter->Update();
  output = relPosFilter->GetOutput();

  relPosFilter->SetInput(output); 
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInputObject(lung); 
  relPosFilter->SetOrientationType(RelPosFilterType::RightTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold1());
  relPosFilter->Update();   
  output = relPosFilter->GetOutput();
  this->template StopCurrentStep<TImageType>(output);

  // Step 3: AP limits from bones
  StartNewStep("Ant/Post limits with bones");
  relPosFilter->SetCurrentStepNumber(0);
  relPosFilter->ResetPipeline();// = RelPosFilterType::New();
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInput(output); 
  relPosFilter->SetInputObject(bones); 
  relPosFilter->SetOrientationType(RelPosFilterType::AntTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold2());
  relPosFilter->Update();

  relPosFilter->SetInput(relPosFilter->GetOutput()); 
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInputObject(bones); 
  relPosFilter->SetOrientationType(RelPosFilterType::PostTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold2());
  relPosFilter->Update();   
  output = relPosFilter->GetOutput();
  this->template StopCurrentStep<TImageType>(output);

  // Get CCL
  output = clitk::Labelize<TImageType>(output, GetBackgroundValue(), true, 100);
  // output = RemoveLabels<TImageType>(output, BG, param->GetLabelsToRemove());
  output = clitk::KeepLabels<TImageType>(output, GetBackgroundValue(), 
                                  GetForegroundValue(), 1, 1, 0);

  output = clitk::AutoCrop<ImageType>(output, GetBackgroundValue()); 
  //  cropFilter = CropFilterType::New();
  //cropFilter->SetInput(output);
  //cropFilter->Update();   
  //output = cropFilter->GetOutput();

  // Final Step -> set output
  this->SetNthOutput(0, output);
}
//--------------------------------------------------------------------
  

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
