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
  SetFuzzyThreshold(0.6);
  SetOrientationType(RelPosFilterType::LeftTo);
  SetIntermediateSpacing(10);
  ResampleBeforeRelativePositionFilterOff();
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
GenerateOutputInformation() 
{ 
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(input->GetLargestPossibleRegion());
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
GenerateData() 
{
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
    - extract vector of slices in input, in object
    - slice by slice rel position
    - joint result
    - post process
  */


  //--------------------------------------------------------------------
  // Extract input slices
  StartNewStep("Extract input slices");
  writeImage<ImageType>(input, "beforex.mhd");
  typedef clitk::ExtractSliceFilter<ImageType> ExtractSliceFilterType;
  typename ExtractSliceFilterType::Pointer extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(input);
  extractSliceFilter->SetDirection(GetDirection());
  extractSliceFilter->Update();
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  std::vector<typename SliceType::Pointer> mInputSlices;
  extractSliceFilter->GetOutputSlices(mInputSlices);
  DD(mInputSlices.size());
  StopCurrentStep<SliceType>(mInputSlices[0]);
  
  //--------------------------------------------------------------------
  // Extract object slices
  StartNewStep("Extract object slices");
  extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(object);
  extractSliceFilter->SetDirection(GetDirection());
  extractSliceFilter->Update();
  std::vector<typename SliceType::Pointer> mObjectSlices;
  extractSliceFilter->GetOutputSlices(mObjectSlices);
  DD(mObjectSlices.size());
  StopCurrentStep<SliceType>(mObjectSlices[0]);

  //--------------------------------------------------------------------
  // Perform slice by slice relative position
  StartNewStep("Perform slice by slice relative position");
  for(unsigned int i=0; i<mInputSlices.size(); i++) {
    // DD(i);
    //     DD(mInputSlices[i]->GetOrigin());
    //     writeImage<SliceType>(mInputSlices[i], "inp"+clitk::toString(i)+".mhd");

    // Select main CC in each object slice : this should be the main bronchus
    mObjectSlices[i] = Labelize<SliceType>(mObjectSlices[i], 0, true, 1);
    mObjectSlices[i] = KeepLabels<SliceType>(mObjectSlices[i], 0, 1, 1, 1, true);

    // Relative position
    typedef clitk::AddRelativePositionConstraintToLabelImageFilter<SliceType> RelPosFilterType;
    typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
    relPosFilter->VerboseStepOff();
    relPosFilter->WriteStepOff();
    relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
    relPosFilter->SetInput(mInputSlices[i]); 
    relPosFilter->SetInputObject(mObjectSlices[i]); 
    relPosFilter->SetOrientationType(this->GetOrientationType());
    relPosFilter->SetIntermediateSpacing(this->GetIntermediateSpacing());
    relPosFilter->SetResampleBeforeRelativePositionFilter(this->GetResampleBeforeRelativePositionFilter());
    relPosFilter->SetFuzzyThreshold(this->GetFuzzyThreshold());
    relPosFilter->AutoCropOff(); // important ! because we join the slices after this loop
    relPosFilter->Update();
    // writeImage<SliceType>(relPosFilter->GetOutput(), "inp-after"+clitk::toString(i)+".mhd");
    mInputSlices[i] = relPosFilter->GetOutput();
  }
  DD(this->GetIntermediateSpacing());
  DD(this->GetResampleBeforeRelativePositionFilter());
  DD("End slice");

  typedef itk::JoinSeriesImageFilter<SliceType, ImageType> JoinSeriesFilterType;
  typename JoinSeriesFilterType::Pointer joinFilter = JoinSeriesFilterType::New();
  joinFilter->SetOrigin(input->GetOrigin()[GetDirection()]);
  joinFilter->SetSpacing(input->GetSpacing()[GetDirection()]);
  for(unsigned int i=0; i<mInputSlices.size(); i++) {
  // DD(mInputSlices[i]->GetLargestPossibleRegion().GetIndex());
//   DD(mInputSlices[i]->GetLargestPossibleRegion().GetSize());
//   DD(mInputSlices[i]->GetRequestedRegion().GetIndex());
//   DD(mInputSlices[i]->GetRequestedRegion().GetSize());
    joinFilter->PushBackInput(mInputSlices[i]);
    //SetInput(i, mInputSlices[i]);
  }
  DD("before update");
  joinFilter->Update();
  DD("after update");
  m_working_input = joinFilter->GetOutput();
  
  // Update the origin
  DD(input->GetSpacing());
  DD(input->GetOrigin());
  DD(mInputSlices[0]->GetSpacing());
  DD(mInputSlices[0]->GetOrigin());
  DD(m_working_input->GetSpacing());
  DD(m_working_input->GetOrigin());
  // typename ImageType::PointType origin = m_working_input->GetOrigin();
//   origin[GetDirection()] = input->GetOrigin()[GetDirection()];
//   m_working_input->SetOrigin(origin);
//   DD(m_working_input->GetOrigin());
  StopCurrentStep<ImageType>(m_working_input);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------  
  // Final Step -> set output
  this->SetNthOutput(0, m_working_input);
  return;
}
//--------------------------------------------------------------------

