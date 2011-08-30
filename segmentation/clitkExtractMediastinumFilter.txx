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

#ifndef CLITKEXTRACTMEDIASTINUMFILTER_TXX
#define CLITKEXTRACTMEDIASTINUMFILTER_TXX

// clitk
#include "clitkCommon.h"
#include "clitkExtractMediastinumFilter.h"
#include "clitkAddRelativePositionConstraintToLabelImageFilter.h"
#include "clitkSliceBySliceRelativePositionFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkExtractAirwaysTreeInfoFilter.h"
#include "clitkCropLikeImageFilter.h"

// std
#include <deque>

// itk
#include "itkStatisticsLabelMapFilter.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkScalarImageKmeansImageFilter.h"

// itk ENST
#include "RelativePositionPropImageFilter.h"

//--------------------------------------------------------------------
template <class ImageType>
clitk::ExtractMediastinumFilter<ImageType>::
ExtractMediastinumFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<ImageType, MaskImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValuePatient(0);
  SetBackgroundValueLung(0);
  SetBackgroundValueBones(0);
  SetForegroundValueLeftLung(1);
  SetForegroundValueRightLung(2);
  SetBackgroundValue(0);
  SetForegroundValue(1);
  SetIntermediateSpacing(6);
  SetFuzzyThreshold("LR_lungs", 0.3);
  SetFuzzyThreshold("bones", 0.6);
  SetFuzzyThreshold("inf_lungs", 0.05);
  SetDistanceMaxToAnteriorPartOfTheVertebralBody(10);  
  SetOutputMediastinumFilename("mediastinum.mhd");  
  UseBonesOff();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInputPatientLabelImage(const MaskImageType * image, MaskImagePixelType bg) 
{
  this->SetNthInput(0, const_cast<MaskImageType *>(image));
  m_BackgroundValuePatient = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInputLungLabelImage(const MaskImageType * image, MaskImagePixelType bg, 
                       MaskImagePixelType fgLeft, MaskImagePixelType fgRight) 
{
  this->SetNthInput(1, const_cast<MaskImageType *>(image));
  m_BackgroundValueLung = bg;
  m_ForegroundValueLeftLung = fgLeft;
  m_ForegroundValueRightLung = fgRight;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInputBonesLabelImage(const MaskImageType * image, MaskImagePixelType bg) 
{
  this->SetNthInput(2, const_cast<MaskImageType *>(image));
  m_BackgroundValueBones = bg;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInputTracheaLabelImage(const MaskImageType * image, MaskImagePixelType bg) 
{
  this->SetNthInput(3, const_cast<MaskImageType *>(image));
  m_BackgroundValueTrachea = bg;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetFuzzyThreshold(std::string tag, double value)
{
  m_FuzzyThreshold[tag] = value;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
double 
clitk::ExtractMediastinumFilter<ImageType>::
GetFuzzyThreshold(std::string tag)
{
  if (m_FuzzyThreshold.find(tag) == m_FuzzyThreshold.end()) {
    clitkExceptionMacro("Could not find options "+tag+" in the list of FuzzyThresholds.");
    return 0.0;
  }
  
  return m_FuzzyThreshold[tag]; 
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
GenerateInputRequestedRegion() 
{
  // DD("GenerateInputRequestedRegion");
  // Do not call default
  // Superclass::GenerateInputRequestedRegion();  
  // DD("End GenerateInputRequestedRegion");
}

//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
SetInput(const ImageType * image) 
{
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
GenerateOutputInformation() { 
  // Do not call default
  // Superclass::GenerateOutputInformation();

  //--------------------------------------------------------------------
  // Get input pointers
  LoadAFDB();
  ImageConstPointer input = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
  MaskImagePointer patient, lung, bones, trachea;
  patient = GetAFDB()->template GetImage <MaskImageType>("Patient");
  lung = GetAFDB()->template GetImage <MaskImageType>("Lungs");
  if (GetUseBones()) {
    bones = GetAFDB()->template GetImage <MaskImageType>("Bones");  
  }
  trachea = GetAFDB()->template GetImage <MaskImageType>("Trachea");  
  
  //--------------------------------------------------------------------
  // Step : Crop support (patient) to lung extend in RL
  StartNewStep("Crop support like lungs along LR");
  typedef clitk::CropLikeImageFilter<MaskImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(patient);
  cropFilter->SetCropLikeImage(lung, 0);// Indicate that we only crop in X (Left-Right) axe
  cropFilter->Update();
  output = cropFilter->GetOutput();
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step : Crop support (previous) to bones extend in AP
  if (GetUseBones()) {
    StartNewStep("Crop support like bones along AP");
    cropFilter = CropFilterType::New();
    cropFilter->SetInput(output);
    cropFilter->SetCropLikeImage(bones, 1);// Indicate that we only crop in Y (Ant-Post) axe
    cropFilter->Update();
    output = cropFilter->GetOutput();
    this->template StopCurrentStep<MaskImageType>(output);
  }

  //--------------------------------------------------------------------
  // Step : patient minus lungs, minus bones, minus trachea
  StartNewStep("Patient contours minus lungs, trachea [and bones]");
  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BoolFilterType;
  typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
  boolFilter->InPlaceOn();
  boolFilter->SetInput1(output);
  boolFilter->SetInput2(lung);    
  boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update();    
  if (GetUseBones()) {
    boolFilter->SetInput1(boolFilter->GetOutput());
    boolFilter->SetInput2(bones);
    boolFilter->SetOperationType(BoolFilterType::AndNot);
    boolFilter->Update(); 
  }
  boolFilter->SetInput1(boolFilter->GetOutput());
  boolFilter->SetInput2(trachea);
  boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update(); 
  output = boolFilter->GetOutput();

  // Auto crop to gain support area
  output = clitk::AutoCrop<MaskImageType>(output, GetBackgroundValue()); 
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step : LR limits from lung (need separate lung ?)
  // Get separate lung images to get only the right and left lung
  // (because RelativePositionPropImageFilter only consider fg=1);
  // (label must be '1' because right is greater than left).  (WE DO
  // NOT NEED TO SEPARATE ? )
  StartNewStep("Left/Right limits with lungs");
  
  // The following cannot be "inplace" because mask is the same than input ...
  MaskImagePointer right_lung = 
    clitk::SetBackground<MaskImageType, MaskImageType>(lung, lung, 2, 0, false);
  MaskImagePointer left_lung = 
    clitk::SetBackground<MaskImageType, MaskImageType>(lung, lung, 1, 0, false);
  right_lung = clitk::ResizeImageLike<MaskImageType>(right_lung, output, GetBackgroundValue());
  left_lung = clitk::ResizeImageLike<MaskImageType>(left_lung, output, GetBackgroundValue());
  // writeImage<MaskImageType>(right_lung, "right.mhd");
  // writeImage<MaskImageType>(left_lung, "left.mhd");

  typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->WriteStepFlagOff();
  relPosFilter->SetInput(output); 
  relPosFilter->SetInputObject(left_lung); 
  relPosFilter->AddOrientationType(RelPosFilterType::AtRightTo); // warning left lung is at right ;)
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("LR_lungs"));
  relPosFilter->Update();
  output = relPosFilter->GetOutput();

  relPosFilter = RelPosFilterType::New();
  relPosFilter->SetInput(output); 
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->WriteStepFlagOff();
  relPosFilter->SetInput(output); 
  relPosFilter->SetInputObject(right_lung);
  relPosFilter->AddOrientationType(RelPosFilterType::AtLeftTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("LR_lungs"));
  relPosFilter->Update();   
  output = relPosFilter->GetOutput();
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step : superior limits
  StartNewStep("Keep inferior to CricoidCartilag");
  // load Cricoid, get centroid, cut above (or below), lower bound
  MaskImagePointer CricoidCartilag = GetAFDB()->template GetImage <MaskImageType>("CricoidCartilag");
  MaskImagePointType p;
  p[0] = p[1] = p[2] =  0.0; // to avoid warning
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(CricoidCartilag, GetBackgroundValue(), 2, true, p);
  output = clitk::CropImageRemoveGreaterThan<MaskImageType>(output, 2, p[2], true, GetBackgroundValue());
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step : AP limits from bones
  // Separate the bones in the ant-post middle
  MaskImageConstPointer bones_ant;
  MaskImageConstPointer bones_post;
  MaskImagePointType middle_AntPost__position;
  if (GetUseBones()) { 
    StartNewStep("Ant/Post limits with bones");
    middle_AntPost__position[0] = middle_AntPost__position[2] = 0;
    middle_AntPost__position[1] = bones->GetOrigin()[1]+(bones->GetLargestPossibleRegion().GetSize()[1]*bones->GetSpacing()[1])/2.0;
    MaskImageIndexType index_bones_middle;
    bones->TransformPhysicalPointToIndex(middle_AntPost__position, index_bones_middle);
  
    // Split bone image first into two parts (ant and post), and crop
    // lateraly to get vertebral 
    typedef itk::RegionOfInterestImageFilter<MaskImageType, MaskImageType> ROIFilterType;
    //  typedef itk::ExtractImageFilter<MaskImageType, MaskImageType> ROIFilterType;
    typename ROIFilterType::Pointer roiFilter = ROIFilterType::New();
    MaskImageRegionType region = bones->GetLargestPossibleRegion();
    MaskImageSizeType size = region.GetSize();
    MaskImageIndexType index = region.GetIndex();
    // ANT part
    // crop LR to keep 1/4 center part
    index[0] = size[0]/4+size[0]/8;
    size[0] = size[0]/4; 
    // crop AP to keep first (ant) part
    size[1] =  index_bones_middle[1]; //size[1]/2.0;
    region.SetSize(size);
    region.SetIndex(index);
    roiFilter->SetInput(bones);
    roiFilter->SetRegionOfInterest(region);
    roiFilter->ReleaseDataFlagOff();
    roiFilter->Update();
    bones_ant = roiFilter->GetOutput();
    //    writeImage<MaskImageType>(bones_ant, "b_ant.mhd");
    // POST part
    roiFilter = ROIFilterType::New();  
    index[1] = bones->GetLargestPossibleRegion().GetIndex()[1] + size[1]-1;
    size[1] =  bones->GetLargestPossibleRegion().GetSize()[1] - size[1];
    region.SetIndex(index);
    region.SetSize(size);
    roiFilter->SetInput(bones);
    roiFilter->SetRegionOfInterest(region);
    roiFilter->ReleaseDataFlagOff();
    roiFilter->Update();
    bones_post = roiFilter->GetOutput();
    //    writeImage<MaskImageType>(bones_post, "b_post.mhd");

    // Go ! 
    relPosFilter->SetCurrentStepNumber(0);
    relPosFilter->ResetPipeline();// = RelPosFilterType::New();
    relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
    relPosFilter->VerboseStepFlagOff();
    relPosFilter->WriteStepFlagOff();
    relPosFilter->SetInput(output); 
    relPosFilter->SetInputObject(bones_post); 
    relPosFilter->AddOrientationType(RelPosFilterType::AntTo);
    relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
    relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("bones"));
    relPosFilter->Update();
    output = relPosFilter->GetOutput();
    //    writeImage<MaskImageType>(output, "post.mhd");

    relPosFilter->SetInput(relPosFilter->GetOutput()); 
    relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
    relPosFilter->VerboseStepFlagOff();
    relPosFilter->WriteStepFlagOff();
    relPosFilter->SetInput(output); 
    relPosFilter->SetInputObject(bones_ant); 
    relPosFilter->AddOrientationType(RelPosFilterType::PostTo);
    relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
    relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("bones"));
    relPosFilter->Update();   
    output = relPosFilter->GetOutput();
    this->template StopCurrentStep<MaskImageType>(output);
  }

  //--------------------------------------------------------------------
  // Step: Get CCL
  StartNewStep("Keep main connected component");
  output = clitk::Labelize<MaskImageType>(output, GetBackgroundValue(), false, 500);
  // output = RemoveLabels<MaskImageType>(output, BG, param->GetLabelsToRemove());
  output = clitk::KeepLabels<MaskImageType>(output, GetBackgroundValue(), 
                                            GetForegroundValue(), 1, 1, 0);
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step: Remove post part from VertebralBody
  StartNewStep("Remove post part according to VertebralBody");
  RemovePostPartOfVertebralBody();
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step: Remove ant part according to Sternum
  StartNewStep("Remove ant part according to Sternum");
  MaskImagePointer Sternum = GetAFDB()->template GetImage <MaskImageType>("Sternum");
  output = clitk::SliceBySliceRelativePosition<MaskImageType>(output, Sternum, 2, 
                                                              GetFuzzyThreshold("ant_sternum"),
                                                              "PostTo", false, 3, true, false);  
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step: Slice by Slice CCL
  StartNewStep("Slice by Slice keep only one component");
  typedef clitk::ExtractSliceFilter<MaskImageType> ExtractSliceFilterType;
  //  typename ExtractSliceFilterType::Pointer 
  ExtractSliceFilterType::Pointer extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(output);
  extractSliceFilter->SetDirection(2);
  extractSliceFilter->Update();
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  std::vector<typename SliceType::Pointer> mSlices;
  extractSliceFilter->GetOutputSlices(mSlices);
  for(unsigned int i=0; i<mSlices.size(); i++) {
    mSlices[i] = Labelize<SliceType>(mSlices[i], 0, true, 100);
    mSlices[i] = KeepLabels<SliceType>(mSlices[i], 0, 1, 1, 1, true);
  }
  typedef itk::JoinSeriesImageFilter<SliceType, MaskImageType> JoinSeriesFilterType;
  typename JoinSeriesFilterType::Pointer joinFilter = JoinSeriesFilterType::New();
  joinFilter->SetOrigin(output->GetOrigin()[2]);
  joinFilter->SetSpacing(output->GetSpacing()[2]);
  for(unsigned int i=0; i<mSlices.size(); i++) {
    joinFilter->PushBackInput(mSlices[i]);
  }
  joinFilter->Update();
  output = joinFilter->GetOutput();
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step 10 : AutoCrop
  StartNewStep("AutoCrop");
  output = clitk::AutoCrop<MaskImageType>(output, GetBackgroundValue()); 
  this->template StopCurrentStep<MaskImageType>(output);

  // End, set the real size
  this->GetOutput(0)->SetRegions(output->GetLargestPossibleRegion());
  this->GetOutput(0)->SetLargestPossibleRegion(output->GetLargestPossibleRegion());
  this->GetOutput(0)->SetRequestedRegion(output->GetLargestPossibleRegion());
  this->GetOutput(0)->SetBufferedRegion(output->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
GenerateData() 
{
  this->GraftOutput(output);
  // Store image filenames into AFDB 
  GetAFDB()->SetImageFilename("Mediastinum", this->GetOutputMediastinumFilename());  
  WriteAFDB();
}
//--------------------------------------------------------------------

  
//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractMediastinumFilter<ImageType>::
RemovePostPartOfVertebralBody()
{
  
  /*
    Posteriorly, Station 8 abuts the descending aorta and the anterior
    aspect of the vertebral body until an imaginary horizontal line
    running 1 cm posterior to the anterior border of the vertebral
    body (Fig. 3C).
    
    => We use this definition for all the mediastinum

   Find most Ant point in VertebralBody. Consider the horizontal line
   which is 'DistanceMaxToAnteriorPartOfTheVertebralBody' away from
   the most ant point.
  */

  // Get VertebralBody mask image
  MaskImagePointer VertebralBody = 
    GetAFDB()->template GetImage <MaskImageType>("VertebralBody");  

  // Consider vertebral body slice by slice
  std::vector<MaskSlicePointer> vertebralSlices;
  clitk::ExtractSlices<MaskImageType>(VertebralBody, 2, vertebralSlices);

  // For each slice, compute the most anterior point
  std::map<int, MaskSlicePointType> vertebralAntPositionBySlice;
  for(uint i=0; i<vertebralSlices.size(); i++) {
    MaskSlicePointType p;
    bool found = clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(vertebralSlices[i], 
                                                                     GetBackgroundValue(), 
                                                                     1, true, p);
    if (found) {
      vertebralAntPositionBySlice[i] = p;
    }
    else { 
      // It should not happen ! But sometimes, a contour is missing or
      // the VertebralBody is not delineated enough inferiorly ... in
      // those cases, we consider the first found slice.
      //        std::cerr << "No foreground pixels in this VertebralBody slices !?? I try with the previous/next slice" << std::endl;
      // [ Possible alternative -> consider previous limit ]
    }
  }

  // Convert 2D points in slice into 3D points
  std::vector<MaskImagePointType> vertebralAntPositions;
  clitk::PointsUtils<MaskImageType>::Convert2DMapTo3DList(vertebralAntPositionBySlice, 
                                                          VertebralBody, 
                                                          vertebralAntPositions);

  // DEBUG : write list of points
  clitk::WriteListOfLandmarks<MaskImageType>(vertebralAntPositions, 
                                             "VertebralBodyMostAnteriorPoints.txt");

  // Cut support posteriorly 1cm the most anterior point of the
  // VertebralBody. Do nothing below and above the VertebralBody. To
  // do that compute several region, slice by slice and fill. 
  MaskImageRegionType region;
  MaskImageSizeType size;
  MaskImageIndexType start;
  size[2] = 1; // a single slice
  start[0] = output->GetLargestPossibleRegion().GetIndex()[0];
  size[0] = output->GetLargestPossibleRegion().GetSize()[0];
  for(uint i=0; i<vertebralAntPositions.size(); i++) {
    typedef typename itk::ImageRegionIterator<MaskImageType> IteratorType;
    IteratorType iter = 
      IteratorType(output, output->GetLargestPossibleRegion());
    MaskImageIndexType index;
    // Consider some cm posterior to most anterior positions (usually
    // 1 cm).
    vertebralAntPositions[i][1] += GetDistanceMaxToAnteriorPartOfTheVertebralBody();
    // Get index of this point
    output->TransformPhysicalPointToIndex(vertebralAntPositions[i], index);
    // Compute region (a single slice)
    start[2] = index[2];
    start[1] = output->GetLargestPossibleRegion().GetIndex()[1]+index[1];
    size[1] = output->GetLargestPossibleRegion().GetSize()[1]-start[1];
    region.SetSize(size);
    region.SetIndex(start);
    // Fill region
    if (output->GetLargestPossibleRegion().IsInside(start))  {
      itk::ImageRegionIterator<MaskImageType> it(output, region);
      it.GoToBegin();
      while (!it.IsAtEnd()) {
        it.Set(GetBackgroundValue());
        ++it;
      }
    }
  }  
}
//--------------------------------------------------------------------


#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
