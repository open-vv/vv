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
#include "clitkCommon.h"
#include "clitkBooleanOperatorLabelImageFilter.h"
#include "clitkAutoCropFilter.h"
#include "clitkResampleImageWithOptionsFilter.h"
#include "clitkBooleanOperatorLabelImageFilter.h"

// itk
#include <deque>
#include "itkStatisticsLabelMapFilter.h"
#include "itkLabelImageToStatisticsLabelMapFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

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
  SetOrientationType(LeftTo);
  ResampleBeforeRelativePositionFilterOn();
  SetIntermediateSpacing(10);
  AutoCropOn();
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
SetAngle1(double a) 
{
  SetOrientationType(Angle);
  m_Angle1 = a;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
SetAngle2(double a) 
{
  SetOrientationType(Angle);
  m_Angle2 = a;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>::
SetOrientationType(OrientationTypeEnumeration orientation) 
{
  m_OrientationType = orientation;
  switch (m_OrientationType) {
  case LeftTo:   m_Angle1 = clitk::deg2rad(0);   m_Angle2 = clitk::deg2rad(0);   break;
  case RightTo:  m_Angle1 = clitk::deg2rad(180); m_Angle2 = clitk::deg2rad(0);   break;
  case AntTo:    m_Angle1 = clitk::deg2rad(90);  m_Angle2 = clitk::deg2rad(0);   break;
  case PostTo:   m_Angle1 = clitk::deg2rad(-90); m_Angle2 = clitk::deg2rad(0);   break;
  case InfTo:    m_Angle1 = clitk::deg2rad(0);   m_Angle2 = clitk::deg2rad(90);  break;
  case SupTo:    m_Angle1 = clitk::deg2rad(0);   m_Angle2 = clitk::deg2rad(-90); break;
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
GenerateData() 
{
  // Get input pointer
  input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  object = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  static const unsigned int dim = ImageType::ImageDimension;
  StartNewStep("Initial resample");  
  // Step 1 : resample
  if (m_ResampleBeforeRelativePositionFilter) {
    typedef clitk::ResampleImageWithOptionsFilter<ImageType> ResampleFilterType;
    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    resampleFilter->SetInput(object);
    resampleFilter->SetOutputIsoSpacing(m_IntermediateSpacing);
    resampleFilter->SetGaussianFilteringEnabled(false);
    //    resampleFilter->SetVerboseOptions(true);
    resampleFilter->Update();
    working_image = resampleFilter->GetOutput();
  }
  else {
    working_image = object;
  }
  StopCurrentStep<ImageType>(working_image);

  // Step 2: object pad to input image -> we want to compute the
  // relative position for each point belonging to the input image
  // domain, so we have to extend (pad) the object image to fit the
  // domain size
  if (!clitk::HaveSameSizeAndSpacing<ImageType, ImageType>(input, working_image)) {
    StartNewStep("Pad object to image size");  
    typename ImageType::Pointer output = ImageType::New();
    SizeType size;
    for(unsigned int i=0; i<dim; i++) {
      size[i] = lrint((input->GetLargestPossibleRegion().GetSize()[i]*input->GetSpacing()[i])/(double)working_image->GetSpacing()[i]);
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
    typename PadFilterType::Pointer padFilter = PadFilterType::New();
    // typename PadFilterType::InputImageIndexType index;
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
    StopCurrentStep<ImageType>(working_image);
  }
  else {
    // DD("[debug] RelPos : same size and spacing : no padding");
  }
  // Keep object image (with resampline and pad)
  object_resampled = working_image;
 //  StopCurrentStep<ImageType>(working_image);

  // Step 3: compute rel pos in object
  StartNewStep("Relative Position Map");  
  typedef itk::RelativePositionPropImageFilter<ImageType, FloatImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->SetInput(working_image);
  relPosFilter->SetAlpha1(m_Angle1); // xy plane
  relPosFilter->SetAlpha2(m_Angle2);
  relPosFilter->SetK1(M_PI/2.0); // Opening parameter, default = pi/2
  relPosFilter->SetFast(true);
  relPosFilter->SetRadius(1); // seems sufficient in this cas
  // relPosFilter->SetVerboseProgress(true);
  relPosFilter->Update();
  relPos = relPosFilter->GetOutput();
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
  if (m_ResampleBeforeRelativePositionFilter) {
    StartNewStep("Resample to get the same sampling than input");
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
  if (working_image->GetLargestPossibleRegion() != input->GetLargestPossibleRegion()) {
    StartNewStep("Pad to get the same size than input");
    typename ImageType::Pointer temp = ImageType::New();
    temp->CopyInformation(input);
    temp->SetRegions(input->GetLargestPossibleRegion()); // Do not forget !!
    temp->Allocate();
    temp->FillBuffer(m_BackgroundValue);
    typename PadFilterType::Pointer padFilter2 = PadFilterType::New(); // if yes : redo relpos
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
  writeImage<ImageType>(input, "i.mhd");
  writeImage<ImageType>(working_image, "w.mhd");
  combineFilter->SetBackgroundValue(m_BackgroundValue);
  combineFilter->SetBackgroundValue1(m_BackgroundValue);
  combineFilter->SetBackgroundValue2(m_BackgroundValue);
  combineFilter->SetForegroundValue(m_BackgroundValue+1);
  combineFilter->SetInput1(input);
  combineFilter->SetInput2(working_image);
  combineFilter->SetOperationType(BoolFilterType::And);
  combineFilter->InPlaceOn();
  combineFilter->Update(); 
  working_image = combineFilter->GetOutput();
  // writeImage<ImageType>(working_image, "res.mhd");
 
  combineFilter = BoolFilterType::New();
  combineFilter->SetInput1(working_image);
  combineFilter->SetInput2(object);
  combineFilter->SetOperationType(BoolFilterType::AndNot);
  combineFilter->InPlaceOn();
  combineFilter->Update(); 

  working_image = combineFilter->GetOutput();
  StopCurrentStep<ImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Step 7: autocrop
  if (GetAutoCrop()) {
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
  return;
}
//--------------------------------------------------------------------

