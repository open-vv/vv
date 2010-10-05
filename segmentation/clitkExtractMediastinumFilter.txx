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
#include "clitkExtractAirwaysTreeInfoFilter.h"

// std
#include <deque>

// itk
#include "itkStatisticsLabelMapFilter.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

// itk ENST
#include "RelativePositionPropImageFilter.h"

//--------------------------------------------------------------------
template <class ImageType>
clitk::ExtractMediastinumFilter<ImageType>::
ExtractMediastinumFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(4);

  SetBackgroundValuePatient(0);
  SetBackgroundValueLung(0);
  SetBackgroundValueBones(0);
  SetForegroundValueLeftLung(1);
  SetForegroundValueRightLung(2);
  SetBackgroundValue(0);
  SetForegroundValue(1);

  SetIntermediateSpacing(6);
  SetFuzzyThreshold1(0.4);
  SetFuzzyThreshold2(0.6);
  SetFuzzyThreshold3(0.2);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInputPatientLabelImage(const ImageType * image, ImagePixelType bg) 
{
  this->SetNthInput(0, const_cast<ImageType *>(image));
  m_BackgroundValuePatient = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInputLungLabelImage(const ImageType * image, ImagePixelType bg, 
                       ImagePixelType fgLeft, ImagePixelType fgRight) 
{
  this->SetNthInput(1, const_cast<ImageType *>(image));
  m_BackgroundValueLung = bg;
  m_ForegroundValueLeftLung = fgLeft;
  m_ForegroundValueRightLung = fgRight;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInputBonesLabelImage(const ImageType * image, ImagePixelType bg) 
{
  this->SetNthInput(2, const_cast<ImageType *>(image));
  m_BackgroundValueBones = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInputTracheaLabelImage(const ImageType * image, ImagePixelType bg) 
{
  this->SetNthInput(3, const_cast<ImageType *>(image));
  m_BackgroundValueTrachea = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
template<class ArgsInfoType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);

  SetBackgroundValuePatient_GGO(mArgsInfo);
  SetBackgroundValueLung_GGO(mArgsInfo);
  SetBackgroundValueTrachea_GGO(mArgsInfo);

  SetForegroundValueLeftLung_GGO(mArgsInfo);
  SetForegroundValueRightLung_GGO(mArgsInfo);

  SetIntermediateSpacing_GGO(mArgsInfo);
  SetFuzzyThreshold1_GGO(mArgsInfo);
  SetFuzzyThreshold2_GGO(mArgsInfo);
  SetFuzzyThreshold3_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
GenerateOutputInformation() { 
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(outputImage->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
GenerateInputRequestedRegion() 
{
  // Call default
  Superclass::GenerateInputRequestedRegion();  
  // Get input pointers
  ImagePointer patient = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer lung    = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  ImagePointer bones   = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(2));
  ImagePointer trachea = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(3));
    
  patient->SetRequestedRegion(patient->GetLargestPossibleRegion());
  lung->SetRequestedRegion(lung->GetLargestPossibleRegion());
  bones->SetRequestedRegion(bones->GetLargestPossibleRegion());
  trachea->SetRequestedRegion(trachea->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
GenerateData() 
{
  // Get input pointers
  ImageConstPointer patient = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
  ImageConstPointer lung    = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(1));
  ImageConstPointer bones   = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(2));
  ImageConstPointer trachea = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(3));
    
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

  // Auto crop to gain support area
  output = clitk::AutoCrop<ImageType>(output, GetBackgroundValue()); 
  this->template StopCurrentStep<ImageType>(output);

  // Step 2: LR limits from lung (need separate lung ?)
  StartNewStep("Left/Right limits with lungs");

  /* // WE DO NOT NEED THE FOLLOWING
     // Get separate lung images to get only the right and left lung
     // (label must be '1' because right is greater than left).
     ImagePointer right_lung = clitk::SetBackground<ImageType, ImageType>(lung, lung, 2, 0);
     ImagePointer left_lung = clitk::SetBackground<ImageType, ImageType>(lung, lung, 1, 0);
     writeImage<ImageType>(right_lung, "right.mhd");
     writeImage<ImageType>(left_lung, "left.mhd");
  */

  typedef clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInput(output); 
  DD(output->GetLargestPossibleRegion().GetIndex());
  //  relPosFilter->SetInputObject(left_lung); 
  relPosFilter->SetInputObject(lung); 
  relPosFilter->SetOrientationType(RelPosFilterType::LeftTo); // warning left lung is at right ;)
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold1());
  relPosFilter->Update();
  output = relPosFilter->GetOutput();
  DD(output->GetLargestPossibleRegion());

  relPosFilter->SetInput(output); 
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  //  relPosFilter->SetInputObject(right_lung);
  relPosFilter->SetInputObject(lung); 
  relPosFilter->SetOrientationType(RelPosFilterType::RightTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold1());
  relPosFilter->Update();   
  output = relPosFilter->GetOutput();
  DD(output->GetLargestPossibleRegion());
  this->template StopCurrentStep<ImageType>(output);

  // Step 3: AP limits from bones
  StartNewStep("Ant/Post limits with bones");
  ImageConstPointer bones_ant;
  ImageConstPointer bones_post;

  // Find ant-post coordinate of trachea (assume the carena position is a
  // good estimation of the ant-post position of the trachea)
  ImagePointType carina;
  LoadAFDB();
  GetAFDB()->GetPoint3D("Carina", carina);
  DD(carina);
  ImageIndexType index_trachea;
  bones->TransformPhysicalPointToIndex(carina, index_trachea);
  DD(index_trachea);
  
  // Split bone image first into two parts (ant and post)
  typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> CropFilterType;
  //  typedef itk::ExtractImageFilter<ImageType, ImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  ImageRegionType region = bones->GetLargestPossibleRegion();
  ImageSizeType size = region.GetSize();
  DD(size);
  size[1] =  index_trachea[1]; //size[1]/2.0;
  DD(size);
  region.SetSize(size);
  cropFilter->SetInput(bones);
  //  cropFilter->SetExtractionRegion(region);
  cropFilter->SetRegionOfInterest(region);
  cropFilter->ReleaseDataFlagOff();
  cropFilter->Update();
  bones_ant = cropFilter->GetOutput();
  writeImage<ImageType>(bones_ant, "b_ant.mhd");
  
  //  cropFilter->ResetPipeline();// = CropFilterType::New();  
  cropFilter = CropFilterType::New();  
  ImageIndexType index = region.GetIndex();
  index[1] = bones->GetLargestPossibleRegion().GetIndex()[1] + size[1]-1;
  size[1] =  bones->GetLargestPossibleRegion().GetSize()[1] - size[1];
  DD(size);
  region.SetIndex(index);
  region.SetSize(size);
  cropFilter->SetInput(bones);
  //  cropFilter->SetExtractionRegion(region);
  cropFilter->SetRegionOfInterest(region);
  cropFilter->ReleaseDataFlagOff();
  cropFilter->Update();
  bones_post = cropFilter->GetOutput();
  writeImage<ImageType>(bones_post, "b_post.mhd");

  // Go ! 
  relPosFilter->SetCurrentStepNumber(0);
  relPosFilter->ResetPipeline();// = RelPosFilterType::New();
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInput(output); 
  relPosFilter->SetInputObject(bones_post); 
  relPosFilter->SetOrientationType(RelPosFilterType::AntTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold2());
  relPosFilter->Update();
  output = relPosFilter->GetOutput();
  writeImage<ImageType>(output, "post.mhd");

  relPosFilter->SetInput(relPosFilter->GetOutput()); 
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInput(output); 
  relPosFilter->SetInputObject(bones_ant); 
  relPosFilter->SetOrientationType(RelPosFilterType::PostTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold2());
  relPosFilter->Update();   
  output = relPosFilter->GetOutput();
  this->template StopCurrentStep<ImageType>(output);
  // Get CCL
  output = clitk::Labelize<ImageType>(output, GetBackgroundValue(), true, 100);
  // output = RemoveLabels<ImageType>(output, BG, param->GetLabelsToRemove());
  output = clitk::KeepLabels<ImageType>(output, GetBackgroundValue(), 
                                        GetForegroundValue(), 1, 1, 0);


  // Step : Lower limits from lung (need separate lung ?)
  StartNewStep("Lower limits with lungs");
  relPosFilter = RelPosFilterType::New();
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInput(output); 
  DD(output->GetLargestPossibleRegion().GetIndex());
  //  relPosFilter->SetInputObject(left_lung); 
  relPosFilter->SetInputObject(lung); 
  relPosFilter->SetOrientationType(RelPosFilterType::SupTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold3());
  relPosFilter->Update();
  output = relPosFilter->GetOutput();
  DD(output->GetLargestPossibleRegion());

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
