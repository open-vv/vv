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

#ifndef CLITKEXTRACTLYMPHSTATIONSFILTER_TXX
#define CLITKEXTRACTLYMPHSTATIONSFILTER_TXX

// clitk
#include "clitkCommon.h"
#include "clitkExtractLymphStationsFilter.h"
#include "clitkAddRelativePositionConstraintToLabelImageFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkAutoCropFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkSliceBySliceRelativePositionFilter.h"

// itk
#include <itkStatisticsLabelMapFilter.h>
#include <itkLabelImageToStatisticsLabelMapFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkImageSliceConstIteratorWithIndex.h>
#include <itkBinaryThinningImageFilter.h>

// itk ENST
#include "RelativePositionPropImageFilter.h"

//--------------------------------------------------------------------
template <class TImageType>
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractLymphStationsFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<TImageType, TImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValueMediastinum(0);
  SetBackgroundValueTrachea(0);
  SetBackgroundValue(0);
  SetForegroundValue(1);

  SetIntermediateSpacing(6);
  SetFuzzyThreshold1(0.6);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
SetInputMediastinumLabelImage(const TImageType * image, ImagePixelType bg) {
  this->SetNthInput(0, const_cast<TImageType *>(image));
  m_BackgroundValueMediastinum = bg;
  SetCarenaZPositionInMM(image->GetOrigin()[2]+image->GetLargestPossibleRegion().GetSize()[2]*image->GetSpacing()[2]);
  SetMiddleLobeBronchusZPositionInMM(image->GetOrigin()[2]);
  // DD(m_CarenaZPositionInMM);
//   DD(m_MiddleLobeBronchusZPositionInMM);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
SetInputTracheaLabelImage(const TImageType * image, ImagePixelType bg) {
  this->SetNthInput(1, const_cast<TImageType *>(image));
  m_BackgroundValueTrachea = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
template<class ArgsInfoType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);
  SetCarenaZPositionInMM_GGO(mArgsInfo);
  SetMiddleLobeBronchusZPositionInMM_GGO(mArgsInfo);
  SetIntermediateSpacing_GGO(mArgsInfo);
  SetFuzzyThreshold1_GGO(mArgsInfo);
  //SetBackgroundValueMediastinum_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateOutputInformation() { 
  //  Superclass::GenerateOutputInformation();
  
  // Get input
  m_mediastinum = dynamic_cast<const TImageType*>(itk::ProcessObject::GetInput(0));
  m_trachea = dynamic_cast<const TImageType*>(itk::ProcessObject::GetInput(1));
    
  // ----------------------------------------------------------------
  // ----------------------------------------------------------------
  // Superior limit = carena
  // Inferior limit = origine middle lobe bronchus
  StartNewStep("Inf/Sup mediastinum limits with carena/bronchus");
  ImageRegionType region = m_mediastinum->GetLargestPossibleRegion(); DD(region);
  ImageSizeType size = region.GetSize();
  ImageIndexType index = region.GetIndex();
  DD(m_CarenaZPositionInMM);
  DD(m_MiddleLobeBronchusZPositionInMM);
  index[2] = floor((m_MiddleLobeBronchusZPositionInMM - m_mediastinum->GetOrigin()[2]) / m_mediastinum->GetSpacing()[2]);
  size[2] = ceil((m_CarenaZPositionInMM-m_MiddleLobeBronchusZPositionInMM) / m_mediastinum->GetSpacing()[2]);
  region.SetSize(size);
  region.SetIndex(index);
  DD(region);
  typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(m_mediastinum);
  cropFilter->SetRegionOfInterest(region);
  cropFilter->Update();
  m_working_image = cropFilter->GetOutput();
  // Auto Crop (because following rel pos is faster)
  m_working_image = clitk::AutoCrop<ImageType>(m_working_image, 0); 
  StopCurrentStep<ImageType>(m_working_image);
  m_output = m_working_image;

  // ----------------------------------------------------------------
  // ----------------------------------------------------------------
  // Separate trachea in two CCL
  StartNewStep("Separate trachea under carena");
  // DD(region);
  ImageRegionType trachea_region = m_trachea->GetLargestPossibleRegion();
  for(int i=0; i<3; i++) {
    index[i] = floor(((index[i]*m_mediastinum->GetSpacing()[i])+m_mediastinum->GetOrigin()[i]
                      -m_trachea->GetOrigin()[i])/m_trachea->GetSpacing()[i]);
    // DD(index[i]);
    size[i] = ceil((size[i]*m_mediastinum->GetSpacing()[i])/m_trachea->GetSpacing()[i]);
    //  DD(size[i]);
    if (index[i] < 0) { 
      size[i] += index[i];
      index[i] = 0;       
    }
    if (size[i]+index[i] > (trachea_region.GetSize()[i] + trachea_region.GetIndex()[i])) {
      size[i] = trachea_region.GetSize()[i] + trachea_region.GetIndex()[i] - index[i];
    }
  }
  // DD(index);
  //   DD(size);
  region.SetIndex(index);
  region.SetSize(size);  
  //  typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> CropFilterType;
  //  typename CropFilterType::Pointer 
  cropFilter = CropFilterType::New();
 //  m_trachea.Print(std::cout);
  cropFilter->SetInput(m_trachea);
  cropFilter->SetRegionOfInterest(region);
  cropFilter->Update();
  m_working_trachea = cropFilter->GetOutput();

  // Labelize and consider two main labels
  m_working_trachea = Labelize<ImageType>(m_working_trachea, 0, true, 1);

  // Detect wich label is at Left
  typedef itk::ImageSliceConstIteratorWithIndex<ImageType> SliceIteratorType;
  SliceIteratorType iter(m_working_trachea, m_working_trachea->GetLargestPossibleRegion());
  iter.SetFirstDirection(0);
  iter.SetSecondDirection(1);
  iter.GoToBegin();
  bool stop = false;
  ImagePixelType leftLabel;
  ImagePixelType rightLabel;
  while (!stop) {
    if (iter.Get() != m_BackgroundValueTrachea) {
      //     DD(iter.GetIndex());
      //       DD((int)iter.Get());
      leftLabel = iter.Get();
      stop = true;
    }
    ++iter;
  }
  if (leftLabel == 1) rightLabel = 2;
  else rightLabel = 1;
  DD((int)leftLabel);
  DD((int)rightLabel);  

  // End step
  StopCurrentStep<ImageType>(m_working_trachea);
  
  //-----------------------------------------------------
  /*  DD("TEST Skeleton");
  typedef itk::BinaryThinningImageFilter<ImageType, ImageType> SkeletonFilterType;
  typename SkeletonFilterType::Pointer skeletonFilter = SkeletonFilterType::New();
  skeletonFilter->SetInput(m_working_trachea);
  skeletonFilter->Update();
  writeImage<ImageType>(skeletonFilter->GetOutput(), "skel.mhd");
  writeImage<ImageType>(skeletonFilter->GetThinning(), "skel2.mhd");  
  */

  //-----------------------------------------------------
  StartNewStep("Left limits with bronchus (slice by slice)");  
  // Select LeftLabel (set right label to 0)
  ImagePointer temp = SetBackground<ImageType, ImageType>(m_working_trachea, m_working_trachea, rightLabel, 0);
  writeImage<ImageType>(temp, "temp1.mhd");

  typedef clitk::SliceBySliceRelativePositionFilter<ImageType> SliceRelPosFilterType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  sliceRelPosFilter->VerboseStepOn();
  sliceRelPosFilter->WriteStepOn();
  sliceRelPosFilter->SetInput(m_working_image);
  sliceRelPosFilter->SetInputObject(temp);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(0.5);
  sliceRelPosFilter->SetOrientationType(SliceRelPosFilterType::RelPosFilterType::RightTo);
  sliceRelPosFilter->Update();
  m_working_image = sliceRelPosFilter->GetOutput();
  writeImage<ImageType>(m_working_image, "afterslicebyslice.mhd");


  //-----------------------------------------------------
  StartNewStep("Right limits with bronchus (slice by slice)");
  // Select LeftLabel (set right label to 0)
  temp = SetBackground<ImageType, ImageType>(m_working_trachea, m_working_trachea, leftLabel, 0);
  writeImage<ImageType>(temp, "temp2.mhd");

  sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  sliceRelPosFilter->VerboseStepOn();
  sliceRelPosFilter->WriteStepOn();
  sliceRelPosFilter->SetInput(m_working_image);
  sliceRelPosFilter->SetInputObject(temp);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(0.5);
  sliceRelPosFilter->SetOrientationType(SliceRelPosFilterType::RelPosFilterType::LeftTo);
  sliceRelPosFilter->Update();
  m_working_image = sliceRelPosFilter->GetOutput();
  writeImage<ImageType>(m_working_image, "afterslicebyslice.mhd");


  DD("end");
  m_output = m_working_image;
  StopCurrentStep<ImageType>(m_output);

  // Set output image information (required)
  ImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(m_working_image->GetLargestPossibleRegion());
  outputImage->SetOrigin(m_working_image->GetOrigin());
  outputImage->SetRequestedRegion(m_working_image->GetLargestPossibleRegion());
  DD("end2");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateInputRequestedRegion() {
  DD("GenerateInputRequestedRegion");
  // Call default
  Superclass::GenerateInputRequestedRegion();
  // Following needed because output region can be greater than input (trachea)
  ImagePointer mediastinum = dynamic_cast<TImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer trachea = dynamic_cast<TImageType*>(itk::ProcessObject::GetInput(1));
  mediastinum->SetRequestedRegion(mediastinum->GetLargestPossibleRegion());
  trachea->SetRequestedRegion(trachea->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateData() {
  DD("GenerateData");
  // Final Step -> graft output (if SetNthOutput => redo)
  this->GraftOutput(m_output);
}
//--------------------------------------------------------------------
  

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
