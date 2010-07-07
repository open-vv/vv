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
// #include "clitkBooleanOperatorLabelImageFilter.h"
// #include "clitkAutoCropFilter.h"
// #include "clitkResampleImageWithOptionsFilter.h"
// #include "clitkBooleanOperatorLabelImageFilter.h"
#include "clitkExtractSliceFilter.h"

// // itk
// #include <deque>
// #include "itkStatisticsLabelMapFilter.h"
// #include "itkLabelImageToStatisticsLabelMapFilter.h"
// #include "itkRegionOfInterestImageFilter.h"
// #include "itkBinaryThresholdImageFilter.h"
// #include "itkBinaryErodeImageFilter.h"
// #include "itkBinaryBallStructuringElement.h"

// // itk [Bloch et al] 
// #include "RelativePositionPropImageFilter.h"

//--------------------------------------------------------------------
template <class ImageType>
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SliceBySliceRelativePositionFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(2);
  SetDirection(2);
  SetObjectBackgroundValue(0);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SetInput(const ImageType * image) {
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SetInputObject(const ImageType * image) {
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(1, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
GenerateOutputInformation() { 
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(input->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
GenerateInputRequestedRegion() {
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
GenerateData() {
  // Get input pointer
  input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  object = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));

  //--------------------------------------------------------------------
  // Resample object to the same spacing than input
  if (!clitk::HaveSameSpacing<ImageType, ImageType>(object, input)) {
    StartNewStep("Resample object to the same spacing than input");
    m_working_object = clitk::ResampleImageSpacing<ImageType>(object, input->GetSpacing());
    StopCurrentStep<ImageType>(m_working_object);
  }
  else {
    DD("no resampling");
    m_working_object = object;
  }
  
  //--------------------------------------------------------------------
  // Pad object to the same size than input
  if (!clitk::HaveSameSizeAndSpacing<ImageType, ImageType>(m_working_object, input)) {
    StartNewStep("Pad object to the same size than input");
    m_working_object = clitk::EnlargeImageLike<ImageType>(m_working_object, 
                                                          input, 
                                                          GetObjectBackgroundValue());
    StopCurrentStep<ImageType>(m_working_object);
  }
  else {
    DD("no pad");
  }
  
  /*


  //--------------------------------------------------------------------
  // Extract input slices
  StartNewStep("Extract input slices");
  typedef clitk::ExtractSliceFilter<ImageType> ExtractSliceFilterType;
  typename ExtractSliceFilterType::Pointer extractSliceFilter = Extractslicefilter::New();
  extractSliceFilter->SetInput(input);
  extractSliceFilter->SetDirection(GetDirection());
  extractSliceFilter->Update();
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  std::vector<typename SliceType::Pointer> & mInputSlices = extractSliceFilter->GetOutput();
  DD(mInputSlices.size());
  StopCurrentStep<SliceType>(mInputSlices[5]);
  
  //--------------------------------------------------------------------
  // Extract object slices

  StartNewStep("Extract object slices");
  extractSliceFilter = Extractslicefilter::New();
  extractSliceFilter->SetInput(input);
  extractSliceFilter->SetDirection(GetDirection());
  extractSliceFilter->Update();
  std::vector<typename SliceType::Pointer> & mObjectSlices = extractSliceFilter->GetOutput();
  DD(mObjectSlices.size());
  StopCurrentStep<SliceType>(mInputSlices[5]);

  
  //--------------------------------------------------------------------
  // Perform slice by slice relative position
  StartNewStep("Perform slice by slice relative position");
  for(int i=0; i<mInputSlices.size(); i++) {
    DD(i);
    typedef clitk::AddRelativePositionConstraintToLabelImageFilter<SliceType> RelPosFilterType;
    typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
    relPosFilter->VerboseStepOff();
    relPosFilter->WriteStepOff();
    relPosFilter->SetInput(mInputSlices[i]); 
    relPosFilter->SetInputObject(mObjectSlices[i]); 
    relPosFilter->SetOrientationType(GetOrientationType());
    relPosFilter->SetIntermediateSpacing(GetIntermediateSpacing());
    relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold());
    relPosFilter->Update();
    mInputSlices[i] = relPosFilter->GetOutput();
  }
  typedef itk::JoinSeriesImageFilter<SliceType, ImageType> JointSeriesFilterType;
  typename JointSeriesFilterType::Pointer jointFilter = JointSeriesFilterType::New();
  for(int i=0; i<mInputSlices.size(); i++) {
    jointFilter->SetInput(i, mInputSlices[i]);
  }
  m_working_input = jointFilter->GetOutput();
  DD(m_working_input->GetSpacing());
  DD(m_working_input->GetOrigin());
  StopCurrentStep<ImageType>(m_working_input);

  */


  //--------------------------------------------------------------------
  //--------------------------------------------------------------------  
  // Final Step -> set output
  this->SetNthOutput(0, m_working_input);
  return;
}
//--------------------------------------------------------------------

