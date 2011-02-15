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
  SetFuzzyThreshold1(0.5);
  SetFuzzyThreshold2(0.6);
  SetFuzzyThreshold3(0.05);
  
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
  clitk::PrintMemory(GetVerboseMemoryFlag(), "Initial memory"); // OK
  LoadAFDB();
  ImageConstPointer input = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
  MaskImagePointer patient = GetAFDB()->template GetImage <MaskImageType>("Patient");  
  MaskImagePointer lung = GetAFDB()->template GetImage <MaskImageType>("Lungs");
  MaskImagePointer bones;
  if (GetUseBones()) {
    bones = GetAFDB()->template GetImage <MaskImageType>("Bones");  
  }
  MaskImagePointer trachea = GetAFDB()->template GetImage <MaskImageType>("Trachea");  
    
  clitk::PrintMemory(GetVerboseMemoryFlag(), "After read patient, lung"); 
  
  //--------------------------------------------------------------------
  // Step 1: Crop support (patient) to lung extend in RL
  StartNewStep("Crop support like lungs along LR");
  typedef clitk::CropLikeImageFilter<MaskImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(patient);
  cropFilter->SetCropLikeImage(lung, 0);// Indicate that we only crop in X (Left-Right) axe
  cropFilter->Update();
  output = cropFilter->GetOutput();
  this->template StopCurrentStep<MaskImageType>(output);
 
  //--------------------------------------------------------------------
  // Step 2: Crop support (previous) to bones extend in AP
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
  // Step 3: patient minus lungs, minus bones, minus trachea
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
  // Step 4: LR limits from lung (need separate lung ?)
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
  //  relPosFilter->SetInputObject(lung); 
  relPosFilter->AddOrientationType(RelPosFilterType::LeftTo); // warning left lung is at right ;)
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold1());
  relPosFilter->Update();
  output = relPosFilter->GetOutput();
  //writeImage<MaskImageType>(right_lung, "step4-left.mhd");

  relPosFilter = RelPosFilterType::New();
  relPosFilter->SetInput(output); 
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->WriteStepFlagOff();
  relPosFilter->SetInput(output); 
  relPosFilter->SetInputObject(right_lung);
  //relPosFilter->SetInputObject(lung); 
  relPosFilter->AddOrientationType(RelPosFilterType::RightTo);
  relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold1());
  relPosFilter->Update();   
  output = relPosFilter->GetOutput();
  this->template StopCurrentStep<MaskImageType>(output);

  //--------------------------------------------------------------------
  // Step 5: AP limits from bones
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
    relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold2());
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
    relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold2());
    relPosFilter->Update();   
    output = relPosFilter->GetOutput();
    this->template StopCurrentStep<MaskImageType>(output);
  }

  //--------------------------------------------------------------------
  // Step 6: Get CCL
  StartNewStep("Keep main connected component");
  output = clitk::Labelize<MaskImageType>(output, GetBackgroundValue(), false, 500);
  // output = RemoveLabels<MaskImageType>(output, BG, param->GetLabelsToRemove());
  output = clitk::KeepLabels<MaskImageType>(output, GetBackgroundValue(), 
                                            GetForegroundValue(), 1, 1, 0);
  this->template StopCurrentStep<MaskImageType>(output);


  //--------------------------------------------------------------------
  // Step 8: Trial segmentation KMeans
  if (0) {
    StartNewStep("K means");
    // Take input, crop like current mask
    typedef CropLikeImageFilter<ImageType> CropLikeFilterType;
    typename CropLikeFilterType::Pointer cropLikeFilter = CropLikeFilterType::New();
    cropLikeFilter->SetInput(input);
    cropLikeFilter->SetCropLikeImage(output);
    cropLikeFilter->Update();
    ImagePointer working_input = cropLikeFilter->GetOutput();
    writeImage<ImageType>(working_input, "crop-input.mhd");
    // Set bG at -1000
    working_input = clitk::SetBackground<ImageType, MaskImageType>(working_input, output, GetBackgroundValue(), -1000, true);
    writeImage<ImageType>(working_input, "crop-input2.mhd");
    // Kmeans
    typedef itk::ScalarImageKmeansImageFilter<ImageType> KMeansFilterType;
    typename KMeansFilterType::Pointer kmeansFilter = KMeansFilterType::New();
    kmeansFilter->SetInput(working_input);
    //  const unsigned int numberOfInitialClasses = 3;
    // const unsigned int useNonContiguousLabels = 0;
    kmeansFilter->AddClassWithInitialMean(-1000);
    kmeansFilter->AddClassWithInitialMean(30);
    kmeansFilter->AddClassWithInitialMean(-40);  // ==> I want this one
    DD("Go!");
    kmeansFilter->Update();
    DD("End");
    typename KMeansFilterType::ParametersType estimatedMeans = kmeansFilter->GetFinalMeans();
    const unsigned int numberOfClasses = estimatedMeans.Size();
    for ( unsigned int i = 0 ; i < numberOfClasses ; ++i ) {
      std::cout << "cluster[" << i << "] ";
      std::cout << "    estimated mean : " << estimatedMeans[i] << std::endl;
    }
    MaskImageType::Pointer kmeans = kmeansFilter->GetOutput();
    kmeans = clitk::SetBackground<MaskImageType, MaskImageType>(kmeans, kmeans, 
                                                                1, GetBackgroundValue(), true);
    writeImage<MaskImageType>(kmeans, "kmeans.mhd");
    // Get final results, and remove from current mask
    boolFilter = BoolFilterType::New(); 
    boolFilter->InPlaceOn();
    boolFilter->SetInput1(output);
    boolFilter->SetInput2(kmeans);    
    boolFilter->SetOperationType(BoolFilterType::And);
    boolFilter->Update();    
    output = boolFilter->GetOutput();
    writeImage<MaskImageType>(output, "out-kmean.mhd");
    this->template StopCurrentStep<MaskImageType>(output);

    // TODO -> FillMASK ?
    // comment speed ? mask ? 2 class ?


    //TODO 
    // Confidence connected ?

  }

  //--------------------------------------------------------------------
  // Step 8: Lower limits from lung (need separate lung ?)
  if (1) {
    // StartNewStep("Trial : minus segmented struct");
    // MaskImagePointer heart = GetAFDB()->template GetImage <MaskImageType>("heart");  
    // boolFilter = BoolFilterType::New(); 
    // boolFilter->InPlaceOn();
    // boolFilter->SetInput1(output);
    // boolFilter->SetInput2(heart);
    // boolFilter->SetOperationType(BoolFilterType::AndNot);
    // boolFilter->Update();  
    //  output = boolFilter->GetOutput(); // not needed because InPlace
    
    // Not below the heart
    // relPosFilter = RelPosFilterType::New();
    // relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
    // relPosFilter->VerboseStepFlagOff();
    // relPosFilter->WriteStepFlagOff();
    // relPosFilter->SetInput(output); 
    // relPosFilter->SetInputObject(heart);
    // relPosFilter->SetOrientationType(RelPosFilterType::SupTo);
    // relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
    // relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold3());
    // relPosFilter->Update();
    // output = relPosFilter->GetOutput();
  }

  //--------------------------------------------------------------------
  // Step 8: Lower limits from lung (need separate lung ?)
  if (0) {
    StartNewStep("Lower limits with lungs");
    // TODO BOFFF ????
    relPosFilter = RelPosFilterType::New();
    relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
    relPosFilter->VerboseStepFlagOff();
    relPosFilter->WriteStepFlagOff();
    relPosFilter->SetInput(output); 
    //  relPosFilter->SetInputObject(left_lung); 
    relPosFilter->SetInputObject(lung); 
    relPosFilter->AddOrientationType(RelPosFilterType::SupTo);
    relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
    relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold3());
    relPosFilter->Update();
    output = relPosFilter->GetOutput();
    this->template StopCurrentStep<MaskImageType>(output);
  }

  //--------------------------------------------------------------------
  // Step 10: Slice by Slice CCL
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
  // Step 9: Binarize to remove too high HU
  // --> warning CCL slice by slice must be done before
  if (0) {
    StartNewStep("Remove hypersignal (bones and injected part");
    // Crop initial ct like current support
    typedef CropLikeImageFilter<ImageType> CropLikeFilterType;
    typename CropLikeFilterType::Pointer cropLikeFilter = CropLikeFilterType::New();
    cropLikeFilter->SetInput(input);
    cropLikeFilter->SetCropLikeImage(output);
    cropLikeFilter->Update();
    ImagePointer working_input = cropLikeFilter->GetOutput();
    //  writeImage<ImageType>(working_input, "crop-ct.mhd");
    // Binarize
    typedef itk::BinaryThresholdImageFilter<ImageType, MaskImageType> InputBinarizeFilterType; 
    typename InputBinarizeFilterType::Pointer binarizeFilter=InputBinarizeFilterType::New();
    binarizeFilter->SetInput(working_input);
    binarizeFilter->SetLowerThreshold(GetLowerThreshold());
    binarizeFilter->SetUpperThreshold(GetUpperThreshold());
    binarizeFilter->SetInsideValue(this->GetBackgroundValue());  // opposite
    binarizeFilter->SetOutsideValue(this->GetForegroundValue()); // opposite
    binarizeFilter->Update();
    MaskImagePointer working_bin = binarizeFilter->GetOutput();
    // writeImage<MaskImageType>(working_bin, "bin.mhd");
    // Remove from support
    boolFilter = BoolFilterType::New(); 
    boolFilter->InPlaceOn();
    boolFilter->SetInput1(output);
    boolFilter->SetInput2(working_bin);    
    boolFilter->SetOperationType(BoolFilterType::AndNot);
    boolFilter->Update();
    output = boolFilter->GetOutput();
    StopCurrentStep<MaskImageType>(output);
  }

  //--------------------------------------------------------------------
  // Step 10 : AutoCrop
  StartNewStep("AutoCrop");
  output = clitk::AutoCrop<MaskImageType>(output, GetBackgroundValue()); 
  this->template StopCurrentStep<MaskImageType>(output);

  // Bones ? pb with RAM ? FillHoles ?

  // how to do with post part ? spine /lung ?
  // POST the spine (should be separated from the rest) 
  /// DO THAT ---->>
  // histo Y on the whole bones_post (3D) -> result is the Y center on the spine (?)
  // by slice on bones_post
  //       find the most ant point in the center
  //       from this point go to post until out of bones.
  //       


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
  

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
