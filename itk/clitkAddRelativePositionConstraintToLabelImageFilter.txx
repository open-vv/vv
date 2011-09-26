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

// clitk
#include "clitkCommon.h"
#include "clitkBooleanOperatorLabelImageFilter.h"
#include "clitkAutoCropFilter.h"
#include "clitkResampleImageWithOptionsFilter.h"
#include "clitkBooleanOperatorLabelImageFilter.h"

// itk
#include <deque>
#include <itkStatisticsLabelMapFilter.h>
#include <itkLabelImageToStatisticsLabelMapFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkAddImageFilter.h>
#if ITK_VERSION_MAJOR >= 4
  #include <itkDivideImageFilter.h>
#else
  #include <itkDivideByConstantImageFilter.h>
#endif

// itk [Bloch et al] 
#include "RelativePositionPropImageFilter.h"

//--------------------------------------------------------------------
template <class ImageType>
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
AddRelativePositionConstraintToLabelImageFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(2);
  SetFuzzyThreshold(0.6);
  SetBackgroundValue(0);
  SetObjectBackgroundValue(0);
  ClearOrientationType();
  IntermediateSpacingFlagOn();
  SetIntermediateSpacing(10);
  AutoCropFlagOn();
  InverseOrientationFlagOff();
  RemoveObjectFlagOn();
  CombineWithOrFlagOff();
  VerboseStepFlagOff();
  WriteStepFlagOff();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
SetInput(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
SetInputObject(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(1, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
ClearOrientationType() 
{
  m_OrientationTypeString.clear();
  m_OrientationType.clear();
  m_Angle1.clear();
  m_Angle2.clear();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
int
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
GetNumberOfAngles()
{
  return m_OrientationType.size();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
AddOrientationTypeString(std::string t) 
{
  m_OrientationTypeString.push_back(t);

  if (t == "LeftTo") { AddOrientationType(LeftTo); return; }
  if (t == "RightTo") { AddOrientationType(RightTo); return; }
  if (t == "AntTo") { AddOrientationType(AntTo); return; }
  if (t == "PostTo") { AddOrientationType(PostTo); return; }
  if (t == "SupTo") { AddOrientationType(SupTo); return; }
  if (t == "InfTo") { AddOrientationType(InfTo); return; }

  if (t == "NotLeftTo") { AddOrientationType(LeftTo); InverseOrientationFlagOn(); return; }
  if (t == "NotRightTo") { AddOrientationType(RightTo); InverseOrientationFlagOn(); return; }
  if (t == "NotAntTo") { AddOrientationType(AntTo); InverseOrientationFlagOn(); return; }
  if (t == "NotPostTo") { AddOrientationType(PostTo); InverseOrientationFlagOn(); return; }
  if (t == "NotSupTo") { AddOrientationType(SupTo); InverseOrientationFlagOn(); return; }
  if (t == "NotInfTo") { AddOrientationType(InfTo); InverseOrientationFlagOn(); return; }

  clitkExceptionMacro("Error, you must provide LeftTo,RightTo or AntTo,PostTo or SupTo,InfTo (or NotLeftTo, NotRightTo etc) but you give " << t);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
GenerateOutputInformation() 
{ 
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(outputImage->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
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
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
AddAngles(double a, double b) 
{
  AddOrientationTypeString("Angle");
  m_Angle1.push_back(a);
  m_Angle2.push_back(b);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
AddOrientationType(OrientationTypeEnumeration orientation) 
{
  m_OrientationType.push_back(orientation);
  switch (orientation) {
  case RightTo:   
    m_Angle1.push_back(clitk::deg2rad(0));   
    m_Angle2.push_back(clitk::deg2rad(0));
    break;
  case LeftTo:  
    m_Angle1.push_back(clitk::deg2rad(180)); 
    m_Angle2.push_back(clitk::deg2rad(0));
    break;
  case AntTo:
    m_Angle1.push_back(clitk::deg2rad(90));
    m_Angle2.push_back(clitk::deg2rad(0));
    break;
  case PostTo:
    m_Angle1.push_back(clitk::deg2rad(-90)); 
    m_Angle2.push_back(clitk::deg2rad(0));
    break;
  case InfTo:    
    m_Angle1.push_back(clitk::deg2rad(0));   
    m_Angle2.push_back(clitk::deg2rad(90));
    break;
  case SupTo:    
    m_Angle1.push_back(clitk::deg2rad(0));   
    m_Angle2.push_back(clitk::deg2rad(-90));
    break;
  case Angle:  break;
  }
  /*         A1   A2
             Left      0    0
             Right   180    0
             Ant      90    0
             Post    -90    0
             Inf       0   90
             Sup       0  -90
  */
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
PrintOptions() 
{
  DD((int)this->GetBackgroundValue());
  DD((int)this->GetObjectBackgroundValue());
  DDV(this->GetOrientationTypeString(), (uint)this->GetNumberOfAngles());
  DD(this->GetIntermediateSpacingFlag());
  DD(this->GetIntermediateSpacing());
  DD(this->GetFuzzyThreshold());
  DD(this->GetAutoCropFlag());
  DD(this->GetInverseOrientationFlag());
  DD(this->GetRemoveObjectFlag());
  DD(this->GetCombineWithOrFlag());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
GenerateData() 
{
  if (GetNumberOfAngles() <1) {
    clitkExceptionMacro("Add at least one orientation type");
  }  

  if (GetVerboseOptionFlag()) {
    for(int i=0; i<GetNumberOfAngles(); i++) {
        std::cout << "Orientation    \t:" << GetOrientationTypeString(i) << std::endl;
    }
    std::cout << "Interm Spacing \t:" << GetIntermediateSpacingFlag() << " " << GetIntermediateSpacing() << "mm" << std::endl;
    std::cout << "Fuzzy threshold\t:" << GetFuzzyThreshold() << std::endl;
    std::cout << "AutoCrop       \t:" << GetAutoCropFlag() << std::endl;
    std::cout << "InverseOrient  \t:" << GetInverseOrientationFlag() << std::endl;
    std::cout << "RemoveObject   \t:" << GetRemoveObjectFlag() << std::endl;
    std::cout << "CombineWithOr  \t:" << GetCombineWithOrFlag() << std::endl;
  }

  // Get input pointer
  input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  object = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  static const unsigned int dim = ImageType::ImageDimension;

  // Step 2: object pad to input image -> we want to compute the
  // relative position for each point belonging to the input image
  // domain, so we have to extend (pad) the object image to fit the
  // domain size
  working_image = object;
  if (!clitk::HaveSameSizeAndSpacing<ImageType, ImageType>(input, working_image)) {
    StartNewStep("Pad (resize) object to input size");  

    if (0) { // OLD VERSION (TO REMOVE)
      StartNewStep("Pad object to image size");  
      typename ImageType::Pointer output = ImageType::New();
      SizeType size;
      for(unsigned int i=0; i<dim; i++) {
        size[i] = lrint((input->GetLargestPossibleRegion().GetSize()[i]*
                         input->GetSpacing()[i])/(double)working_image->GetSpacing()[i]);
      }

      // The index of the input is not necessarily zero, so we have to
      // take it into account (not done)
      RegionType region;
      IndexType index = input->GetLargestPossibleRegion().GetIndex();
      region.SetSize(size);
      for(unsigned int i=0; i<dim; i++) {
        if (index[i] != 0) {
          std::cerr << "Index diff from zero : " << index << ". not done yet !" << std::endl;
          exit(0);
        }
      }
      // output->SetLargestPossibleRegion(region);
      output->SetRegions(region);
      output->SetSpacing(working_image->GetSpacing());    
      PointType origin = input->GetOrigin();
      for(unsigned int i=0; i<dim; i++) {
        origin[i] = index[i]*input->GetSpacing()[i] + input->GetOrigin()[i];
      }
      output->SetOrigin(origin);
      //    output->SetOrigin(input->GetOrigin());

      output->Allocate();
      output->FillBuffer(m_BackgroundValue);
      typename PasteFilterType::Pointer padFilter = PasteFilterType::New();
      // typename PasteFilterType::InputImageIndexType index;
      for(unsigned int i=0; i<dim; i++) {
        index[i] = -index[i]*input->GetSpacing()[i]/(double)working_image->GetSpacing()[i]
          + lrint((working_image->GetOrigin()[i] - input->GetOrigin()[i])/working_image->GetSpacing()[i]);
      }
      padFilter->SetSourceImage(working_image);
      padFilter->SetDestinationImage(output);
      padFilter->SetDestinationIndex(index);
      padFilter->SetSourceRegion(working_image->GetLargestPossibleRegion());
      padFilter->Update();
      working_image = padFilter->GetOutput();
    }

    // Resize object like input
    working_image = clitk::ResizeImageLike<ImageType>(working_image, input, GetBackgroundValue());
    StopCurrentStep<ImageType>(working_image);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Step 1 : resample
  if (m_IntermediateSpacingFlag) {
    StartNewStep("Resample object to intermediate spacing");  
    typedef clitk::ResampleImageWithOptionsFilter<ImageType> ResampleFilterType;
    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    resampleFilter->SetInput(working_image);
    resampleFilter->SetDefaultPixelValue(0);
    resampleFilter->SetOutputIsoSpacing(m_IntermediateSpacing);
    resampleFilter->SetGaussianFilteringEnabled(false);
    //    resampleFilter->SetVerboseOptions(true);
    resampleFilter->Update();
    working_image = resampleFilter->GetOutput();
    StopCurrentStep<ImageType>(working_image);
  }

  // Keep object image (with resampline and pad)
  object_resampled = working_image;

  // Step 3: compute rel pos in object
  StartNewStep("Relative Position Map");  
  typedef itk::RelativePositionPropImageFilter<ImageType, FloatImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter;

  typename FloatImageType::Pointer m_FuzzyMap;
  for(int i=0; i<GetNumberOfAngles(); i++) {
    // Compute fuzzy map
    relPosFilter = RelPosFilterType::New();
    relPosFilter->SetInput(working_image);
    relPosFilter->SetAlpha1(m_Angle1[i]); // xy plane
    relPosFilter->SetAlpha2(m_Angle2[i]);
    relPosFilter->SetK1(M_PI/2.0); // Opening parameter, default = pi/2
    relPosFilter->SetFast(true);
    relPosFilter->SetRadius(1); // seems sufficient in this case
    // relPosFilter->SetVerboseProgress(true);
    relPosFilter->Update();
    relPos = relPosFilter->GetOutput();

    if (GetNumberOfAngles() != 1) {
      // Creation of the first m_FuzzyMap
      if (i==0) {
        m_FuzzyMap = clitk::NewImageLike<FloatImageType>(relPos, true);
        m_FuzzyMap->FillBuffer(0.0);
      }
      
      // Add to current fuzzy map
      typedef itk::AddImageFilter<FloatImageType, FloatImageType, FloatImageType> AddImageFilter;
      typename AddImageFilter::Pointer addFilter = AddImageFilter::New();
      addFilter->SetInput1(m_FuzzyMap);
      addFilter->SetInput2(relPos);
      addFilter->Update();
      m_FuzzyMap = addFilter->GetOutput();
    }
    else m_FuzzyMap = relPos;
  }

  // Divide by the number of relpos
  if (GetNumberOfAngles() != 1) {
#if ITK_VERSION_MAJOR >= 4
    typedef itk::DivideImageFilter<FloatImageType, FloatImageType, FloatImageType> DivideFilter;
    typename DivideFilter::Pointer divideFilter = DivideFilter::New();
    divideFilter->SetConstant2(GetNumberOfAngles());
#else
    typedef itk::DivideByConstantImageFilter<FloatImageType, float, FloatImageType> DivideFilter;
    typename DivideFilter::Pointer divideFilter = DivideFilter::New();
    divideFilter->SetConstant(GetNumberOfAngles());
#endif
    divideFilter->SetInput(m_FuzzyMap);
    divideFilter->Update();
    m_FuzzyMap = divideFilter->GetOutput();
  }

  relPos = m_FuzzyMap;
  StopCurrentStep<FloatImageType>(relPos);
               
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Map Threshold");
  // Step 1: threshold
  typedef itk::BinaryThresholdImageFilter<FloatImageType, ImageType> BinaryThresholdImageFilterType;
  typename BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
  thresholdFilter->SetInput(relPos);
  thresholdFilter->SetOutsideValue(m_BackgroundValue);
  thresholdFilter->SetInsideValue(m_BackgroundValue+1);
  thresholdFilter->SetLowerThreshold(m_FuzzyThreshold);
  thresholdFilter->Update();
  working_image = thresholdFilter->GetOutput();
  StopCurrentStep<ImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Post Processing: erosion with initial mask");
  // Step 2 : erosion with initial mask to exclude pixels that were
  // inside the resampled version and outside the original mask
  typedef itk::BinaryBallStructuringElement<unsigned int, ImageDimension> StructuringElementType; 
  StructuringElementType kernel;
  kernel.SetRadius(1);
  kernel.CreateStructuringElement();
  typedef itk::BinaryErodeImageFilter<ImageType, ImageType, StructuringElementType> ErodeFilterType;
  typename ErodeFilterType::Pointer erodeFilter = ErodeFilterType::New();
  erodeFilter->SetInput(working_image);
  erodeFilter->SetKernel(kernel);
  erodeFilter->SetBackgroundValue(m_BackgroundValue);
  erodeFilter->SetErodeValue(m_BackgroundValue+1);
  erodeFilter->Update();
  working_image = erodeFilter->GetOutput();
  StopCurrentStep<ImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Step 5: resample to initial spacing
  if (m_IntermediateSpacingFlag) {
    StartNewStep("Resample to come back to the same sampling than input");
    typedef clitk::ResampleImageWithOptionsFilter<ImageType> ResampleFilterType;
    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    resampleFilter->SetDefaultPixelValue(m_BackgroundValue);
    resampleFilter->SetInput(working_image);
    resampleFilter->SetOutputSpacing(input->GetSpacing());
    resampleFilter->SetGaussianFilteringEnabled(false);
    // resampleFilter->SetVerboseOptions(true);
    resampleFilter->Update();
    working_image = resampleFilter->GetOutput();
    StopCurrentStep<ImageType>(working_image);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Pre Step 6: pad if not the same size : it can occur when downsample and upsample
  //if (!HaveSameSizeAndSpacing(working_image, input)) {
  if (working_image->GetLargestPossibleRegion() != input->GetLargestPossibleRegion()) {
    StartNewStep("Pad to get the same size than input");
    typename ImageType::Pointer temp = ImageType::New();
    temp->CopyInformation(input);
    temp->SetRegions(input->GetLargestPossibleRegion()); // Do not forget !!
    temp->Allocate();
    temp->FillBuffer(m_BackgroundValue); 
    typename PasteFilterType::Pointer padFilter2 = PasteFilterType::New();
    padFilter2->SetSourceImage(working_image);
    padFilter2->SetDestinationImage(temp);
    padFilter2->SetDestinationIndex(input->GetLargestPossibleRegion().GetIndex());
    padFilter2->SetSourceRegion(working_image->GetLargestPossibleRegion());
    padFilter2->Update();
    working_image = padFilter2->GetOutput();
    StopCurrentStep<ImageType>(working_image);
  }
  else {
    //DD("[debug] Rel Pos : no padding after");
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Step 6: combine input+thresholded relpos
  StartNewStep("Combine with initial input (boolean And)");
  typedef clitk::BooleanOperatorLabelImageFilter<ImageType> BoolFilterType;
  typename BoolFilterType::Pointer combineFilter = BoolFilterType::New();
  combineFilter->SetBackgroundValue(m_BackgroundValue);
  combineFilter->SetBackgroundValue1(m_BackgroundValue);
  combineFilter->SetBackgroundValue2(m_BackgroundValue);
  combineFilter->SetForegroundValue(m_BackgroundValue+1);
  combineFilter->SetInput1(input);
  combineFilter->SetInput2(working_image);
  if (GetInverseOrientationFlag())
    combineFilter->SetOperationType(BoolFilterType::AndNot);
  else {
    if (GetCombineWithOrFlag())
      combineFilter->SetOperationType(BoolFilterType::Or);
    else
      combineFilter->SetOperationType(BoolFilterType::And);
  }
  combineFilter->InPlaceOff(); // Do not modify initial input (!)
  combineFilter->Update(); 
  working_image = combineFilter->GetOutput();

  // Remove (if needed the object from the support)
  if (GetRemoveObjectFlag()) {
    combineFilter = BoolFilterType::New();
    combineFilter->SetInput1(working_image);
    combineFilter->SetInput2(object);
    combineFilter->SetOperationType(BoolFilterType::AndNot);
    combineFilter->InPlaceOn();
    combineFilter->Update(); 
    working_image = combineFilter->GetOutput();
  }
  // In the other case, we must *add* the initial object to keep it
  // but not more than the initial support
  else { 
    combineFilter = BoolFilterType::New();
    combineFilter->SetInput1(working_image);
    combineFilter->SetInput2(object);
    combineFilter->SetOperationType(BoolFilterType::Or);
    combineFilter->InPlaceOn();
    combineFilter->Update(); 
    working_image = combineFilter->GetOutput(); // not needed because InPlaceOn ?
    combineFilter = BoolFilterType::New();
    combineFilter->SetInput1(working_image);
    combineFilter->SetInput2(input);
    combineFilter->SetOperationType(BoolFilterType::And);
    combineFilter->InPlaceOn();
    combineFilter->Update(); 
    working_image = combineFilter->GetOutput();
  }

  StopCurrentStep<ImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Step 7: autocrop
  if (GetAutoCropFlag()) {
    StartNewStep("Final AutoCrop");
    typedef clitk::AutoCropFilter<ImageType> CropFilterType;
    typename CropFilterType::Pointer cropFilter = CropFilterType::New();
    cropFilter->SetInput(working_image);
    cropFilter->ReleaseDataFlagOff();
    cropFilter->Update();   
    working_image = cropFilter->GetOutput();
    StopCurrentStep<ImageType>(working_image);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  
  // Final Step -> set output
  this->SetNthOutput(0, working_image);
  //  this->GraftOutput(working_image);
}
//--------------------------------------------------------------------

