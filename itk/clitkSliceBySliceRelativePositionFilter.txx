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

// clitk
#include "clitkSegmentationUtils.h"
#include "clitkExtractSliceFilter.h"
#include "clitkResampleImageWithOptionsFilter.h"

// itk
#include <itkJoinSeriesImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SliceBySliceRelativePositionFilter():
  clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>()
{
  SetDirection(2);
  UniqueConnectedComponentBySliceOff();
  SetIgnoreEmptySliceObjectFlag(false);
  UseASingleObjectConnectedComponentBySliceFlagOn();
  this->VerboseStepFlagOff();
  this->WriteStepFlagOff();
  this->SetCombineWithOrFlag(false);
  CCLSelectionFlagOn();
  SetCCLSelectionDimension(0);
  SetCCLSelectionDirection(1);
  CCLSelectionIgnoreSingleCCLFlagOff();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SetInput(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SetInputObject(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(1, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
PrintOptions() 
{
  DD(this->GetDirection());
  DD((int)this->GetObjectBackgroundValue());
  DDV(this->GetOrientationTypeString(), (uint)this->GetNumberOfAngles());
  DD(this->GetIntermediateSpacingFlag());
  DD(this->GetIntermediateSpacing());
  DD(this->GetFuzzyThreshold());
  DD(this->GetUniqueConnectedComponentBySlice());
  DD(this->GetAutoCropFlag());
  DD(this->GetInverseOrientationFlag());
  DD(this->GetRemoveObjectFlag());
  DD(this->GetCombineWithOrFlag());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
GenerateInputRequestedRegion() 
{
  // Call default
  itk::ImageToImageFilter<ImageType, ImageType>::GenerateInputRequestedRegion();
  // Get input pointers and set requested region to common region
  ImagePointer input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer input2 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  input1->SetRequestedRegion(input1->GetLargestPossibleRegion());
  input2->SetRequestedRegion(input2->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
GenerateOutputInformation() 
{
  if (this->GetVerboseOptionFlag()) {
    PrintOptions();
  }

  // Get input pointer
  input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  object = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));

  //--------------------------------------------------------------------
  // Resample object to the same spacing than input
  if (!clitk::HaveSameSpacing<ImageType, ImageType>(object, input)) {
    this->StartNewStep("Resample object to the same spacing than input");
    m_working_object = clitk::ResampleImageSpacing<ImageType>(object, input->GetSpacing());
    this->template StopCurrentStep<ImageType>(m_working_object);
  }
  else {
    m_working_object = object;
  }
  
  //--------------------------------------------------------------------
  // Pad object to the same size than input
  if (!clitk::HaveSameSizeAndSpacing<ImageType, ImageType>(m_working_object, input)) {
    this->StartNewStep("Pad object to the same size than input");
    m_working_object = clitk::ResizeImageLike<ImageType>(m_working_object, 
                                                         input, 
                                                         this->GetObjectBackgroundValue());
    this->template StopCurrentStep<ImageType>(m_working_object);
  }
  else {
  }

  /*
    - extract vector of slices in input, in object
    - slice by slice rel position
    - joint result
    - post process
  */


  //--------------------------------------------------------------------
  // Extract input slices
  this->StartNewStep("Extract input slices");
  typedef clitk::ExtractSliceFilter<ImageType> ExtractSliceFilterType;
  typename ExtractSliceFilterType::Pointer extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(input);
  extractSliceFilter->SetDirection(GetDirection());
  extractSliceFilter->Update();
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  std::vector<typename SliceType::Pointer> mInputSlices;
  extractSliceFilter->GetOutputSlices(mInputSlices);
  this->template StopCurrentStep<SliceType>(mInputSlices[0]);
  
  //--------------------------------------------------------------------
  // Extract object slices
  this->StartNewStep("Extract object slices");
  extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(m_working_object);//object);
  extractSliceFilter->SetDirection(GetDirection());
  extractSliceFilter->Update();
  std::vector<typename SliceType::Pointer> mObjectSlices;
  extractSliceFilter->GetOutputSlices(mObjectSlices);
  this->template StopCurrentStep<SliceType>(mObjectSlices[0]);

  //--------------------------------------------------------------------
  // Perform slice by slice relative position
  this->StartNewStep("Perform slice by slice relative position");
  for(unsigned int i=0; i<mInputSlices.size(); i++) {
    
    // Count the number of CCL (allow to ignore empty slice)
    int nb=0;
    mObjectSlices[i] = LabelizeAndCountNumberOfObjects<SliceType>(mObjectSlices[i], 0, true, 1, nb);
    if ((!GetIgnoreEmptySliceObjectFlag()) || (nb!=0)) {

      // Select or not a single CCL ?
      if (GetUseASingleObjectConnectedComponentBySliceFlag()) {
        mObjectSlices[i] = KeepLabels<SliceType>(mObjectSlices[i], 0, 1, 1, 1, true);
      }

      // Select a single according to a position if more than one CCL
      if (GetCCLSelectionFlag()) {
        // if several CCL, choose the most extrema according a direction, 
        // if not -> should we consider this slice ? 
        if (nb<2) {
          if (GetCCLSelectionIgnoreSingleCCLFlag()) {
            mObjectSlices[i] = SetBackground<SliceType, SliceType>(mObjectSlices[i], mObjectSlices[i], 
                                                                   1, this->GetBackgroundValue(), 
                                                                   true);
          }
        }
        int dim = GetCCLSelectionDimension();
        int direction = GetCCLSelectionDirection();
        std::vector<typename SliceType::PointType> centroids;
        ComputeCentroids<SliceType>(mObjectSlices[i], this->GetBackgroundValue(), centroids);
        uint index=1;
        for(uint j=1; j<centroids.size(); j++) {
          if (direction == 1) {
            if (centroids[j][dim] > centroids[index][dim]) index = j;
          }
          else {
            if (centroids[j][dim] < centroids[index][dim]) index = j;
          }
        }
        for(uint v=1; v<centroids.size(); v++) {
          if (v != index) {
            mObjectSlices[i] = SetBackground<SliceType, SliceType>(mObjectSlices[i], mObjectSlices[i], 
                                                                   (char)v, this->GetBackgroundValue(), 
                                                                   true);
          }
        }
      }

      // Relative position
      typedef clitk::AddRelativePositionConstraintToLabelImageFilter<SliceType> RelPosFilterType;
      typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();

      relPosFilter->VerboseStepFlagOff();
      relPosFilter->WriteStepFlagOff();
      relPosFilter->SetBackgroundValue(this->GetBackgroundValue());
      relPosFilter->SetInput(mInputSlices[i]); 
      relPosFilter->SetInputObject(mObjectSlices[i]); 
      relPosFilter->SetRemoveObjectFlag(this->GetRemoveObjectFlag());
      for(int j=0; j<this->GetNumberOfAngles(); j++) {
        relPosFilter->AddOrientationTypeString(this->GetOrientationTypeString(j));
      }
      relPosFilter->SetInverseOrientationFlag(this->GetInverseOrientationFlag());
      //relPosFilter->SetOrientationType(this->GetOrientationType());
      relPosFilter->SetIntermediateSpacing(this->GetIntermediateSpacing());
      relPosFilter->SetIntermediateSpacingFlag(this->GetIntermediateSpacingFlag());
      relPosFilter->SetFuzzyThreshold(this->GetFuzzyThreshold());
      relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
      relPosFilter->SetCombineWithOrFlag(this->GetCombineWithOrFlag()); 
      relPosFilter->Update();
      mInputSlices[i] = relPosFilter->GetOutput();

      // Select main CC if needed
      if (GetUniqueConnectedComponentBySlice()) {
        mInputSlices[i] = Labelize<SliceType>(mInputSlices[i], 0, true, 1);
        mInputSlices[i] = KeepLabels<SliceType>(mInputSlices[i], 0, 1, 1, 1, true);
      }

      /*
      // Select unique CC according to the most in a given direction
      if (GetUniqueConnectedComponentBySliceAccordingToADirection()) {
        int nb;
        mInputSlices[i] = LabelizeAndCountNumberOfObjects<SliceType>(mInputSlices[i], 0, true, 1, nb);
        std::vector<typename ImageType::PointType> & centroids;
        ComputeCentroids
        }
      */
    }
  }

  // Join the slices
  m_working_input = clitk::JoinSlices<ImageType>(mInputSlices, input, GetDirection());
  this->template StopCurrentStep<ImageType>(m_working_input);

  //--------------------------------------------------------------------
  // Step 7: autocrop
  if (this->GetAutoCropFlag()) {
    this->StartNewStep("Final AutoCrop");
    typedef clitk::AutoCropFilter<ImageType> CropFilterType;
    typename CropFilterType::Pointer cropFilter = CropFilterType::New();
    cropFilter->SetInput(m_working_input);
    cropFilter->ReleaseDataFlagOff();
    cropFilter->Update();   
    m_working_input = cropFilter->GetOutput();
    this->template StopCurrentStep<ImageType>(m_working_input);    
  }

  // Update output info
  this->GetOutput(0)->SetRegions(m_working_input->GetLargestPossibleRegion());  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
GenerateData() 
{
  // Get input pointer
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------  
  // Final Step -> set output
  //this->SetNthOutput(0, m_working_input);
  this->GraftOutput(m_working_input);
  return;
}
//--------------------------------------------------------------------

