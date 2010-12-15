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

#ifndef CLITKEXTRACTBONESSFILTER_TXX
#define CLITKEXTRACTBONESSFILTER_TXX

// clitk
#include "clitkImageCommon.h"
#include "clitkSetBackgroundImageFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkAutoCropFilter.h"
#include "clitkFillMaskFilter.h"

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"

//--------------------------------------------------------------------
template <class TInputImageType>
clitk::ExtractBonesFilter<TInputImageType>::
ExtractBonesFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<TInputImageType, MaskImageType>()
{
  // Default global options
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0); // Must be zero
  SetForegroundValue(1);
  SetInitialSmoothing(false);

  SetSmoothingConductanceParameter(3.0);
  SetSmoothingNumberOfIterations(5);
  SetSmoothingTimeStep(0.0625);
  SetSmoothingUseImageSpacing(false);

  SetMinimalComponentSize(100);
  SetUpperThreshold1(1500);
  SetLowerThreshold1(100);
  SetFullConnectivity(false);

  SetUpperThreshold2(1500);
  SetLowerThreshold2(10);
  InputImageSizeType s;
  s.Fill(1);
  SetRadius2(s);
  SetSampleRate2(0);
  AutoCropOn();
  FillHolesOn();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType>
void 
clitk::ExtractBonesFilter<TInputImageType>::
SetInput(const TInputImageType * image) 
{
  this->SetNthInput(0, const_cast<TInputImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType>
template<class ArgsInfoType>
void 
clitk::ExtractBonesFilter<TInputImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);
  
  SetAFDBFilename_GGO(mArgsInfo); 
  SetOutputBonesFilename_GGO(mArgsInfo);

  SetInitialSmoothing_GGO(mArgsInfo);
  SetSmoothingConductanceParameter_GGO(mArgsInfo);
  SetSmoothingNumberOfIterations_GGO(mArgsInfo);
  SetSmoothingTimeStep_GGO(mArgsInfo);
  SetSmoothingUseImageSpacing_GGO(mArgsInfo);

  SetMinimalComponentSize_GGO(mArgsInfo);
  SetUpperThreshold1_GGO(mArgsInfo);
  SetLowerThreshold1_GGO(mArgsInfo);
  SetFullConnectivity_GGO(mArgsInfo);

  SetUpperThreshold2_GGO(mArgsInfo);
  SetLowerThreshold2_GGO(mArgsInfo);
  SetRadius2_GGO(mArgsInfo);
  SetSampleRate2_GGO(mArgsInfo);
  SetAutoCrop_GGO(mArgsInfo);
  SetFillHoles_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType>
void 
clitk::ExtractBonesFilter<TInputImageType>::
GenerateOutputInformation() { 

  // Get input pointers
  InputImagePointer input   = dynamic_cast<TInputImageType*>(itk::ProcessObject::GetInput(0));
  Superclass::GenerateOutputInformation();
  MaskImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(input->GetLargestPossibleRegion());

  // Read DB
  LoadAFDB();

  // typedefs
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> InputBinarizeFilterType;
  typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> BinarizeFilterType;
  typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
  typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelFilterType;
  typedef clitk::SetBackgroundImageFilter<InternalImageType,InternalImageType, InternalImageType> SetBackgroundFilterType; 
  typedef itk::CastImageFilter<InternalImageType,MaskImageType> CastImageFilterType; 
  typedef itk::ImageFileWriter<MaskImageType> WriterType; 

  //---------------------------------
  // Smoothing [Optional]
  //---------------------------------
  if (GetInitialSmoothing()) {
    StartNewStep("Initial Smoothing");
    typedef itk::CurvatureAnisotropicDiffusionImageFilter<InputImageType, InputImageType> FilterType;
    typename FilterType::Pointer df = FilterType::New(); 
    df->SetConductanceParameter(GetSmoothingConductanceParameter());
    df->SetNumberOfIterations(GetSmoothingNumberOfIterations());
    df->SetTimeStep(GetSmoothingTimeStep());
    df->SetUseImageSpacing(GetSmoothingUseImageSpacing());
    df->SetInput(input);
    df->Update();
    filtered_input = df->GetOutput();
    StopCurrentStep<InputImageType>(filtered_input);
  }
  else {
    filtered_input = input;
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Initial Labeling");
  typename InternalImageType::Pointer firstLabelImage;
    
  //---------------------------------
  // Binarize the image
  //---------------------------------
  typename InputBinarizeFilterType::Pointer binarizeFilter=InputBinarizeFilterType::New();
  binarizeFilter->SetInput(filtered_input);
  binarizeFilter->SetLowerThreshold(GetLowerThreshold1());
  binarizeFilter->SetUpperThreshold(GetUpperThreshold1());
  binarizeFilter->SetInsideValue(this->GetForegroundValue());
  binarizeFilter->SetOutsideValue(this->GetBackgroundValue());

  //---------------------------------
  // Label the connected components
  //---------------------------------
  typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
  connectFilter->SetInput(binarizeFilter->GetOutput());
  connectFilter->SetBackgroundValue(this->GetBackgroundValue());
  connectFilter->SetFullyConnected(GetFullConnectivity());

  //---------------------------------
  // Sort the labels according to size
  //---------------------------------
  typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
  relabelFilter->SetInput(connectFilter->GetOutput());
  relabelFilter->SetMinimumObjectSize(GetMinimalComponentSize());
    
  //---------------------------------
  // Keep the label
  //---------------------------------
  typename BinarizeFilterType::Pointer binarizeFilter2=BinarizeFilterType::New();
  binarizeFilter2->SetInput(relabelFilter->GetOutput());
  binarizeFilter2->SetLowerThreshold(1);
  binarizeFilter2->SetUpperThreshold(1);
  binarizeFilter2->SetInsideValue(this->GetForegroundValue());
  binarizeFilter2->SetOutsideValue(this->GetBackgroundValue());
  binarizeFilter2->Update();

  firstLabelImage = binarizeFilter2->GetOutput();
  StopCurrentStep<InternalImageType>(firstLabelImage);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Neighborhood connected filter");
  typename InternalImageType::Pointer secondLabelImage;
    
  //---------------------------------
  //Neighborhood connected RG 
  //---------------------------------
  typedef itk::NeighborhoodConnectedImageFilter<InputImageType, InternalImageType> 
    NeighborhoodConnectedImageFilterType;
  typename NeighborhoodConnectedImageFilterType::Pointer neighborhoodConnectedImageFilter= 
    NeighborhoodConnectedImageFilterType::New();
  
  // thresholds
  neighborhoodConnectedImageFilter->SetLower(GetLowerThreshold2());
  neighborhoodConnectedImageFilter->SetUpper(GetUpperThreshold2());
  neighborhoodConnectedImageFilter->SetReplaceValue(this->GetForegroundValue());
  neighborhoodConnectedImageFilter->SetRadius(GetRadius2());
  neighborhoodConnectedImageFilter->SetInput(filtered_input);

  // Seeds from label image
  typedef itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;
  IteratorType it(firstLabelImage, firstLabelImage->GetLargestPossibleRegion());
  typename InputImageType::IndexType index;
  unsigned int counter=0;
  while (!it.IsAtEnd())
    {
      if (it.Get()==this->GetForegroundValue())
	{
	  counter++;
	  index=it.GetIndex();
	  neighborhoodConnectedImageFilter->AddSeed(index);
	  ++it;
	  unsigned int i=0;
	  while (!it.IsAtEnd()  &&  i< (unsigned int) GetSampleRate2())
	    {	     
	      ++it;
	      i++;
	    }
	}
      else ++it;
    }

  neighborhoodConnectedImageFilter->Update();
  secondLabelImage = neighborhoodConnectedImageFilter->GetOutput();

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Combine the images");
  typedef clitk::SetBackgroundImageFilter<InternalImageType, InternalImageType, InternalImageType> 
    SetBackgroundImageFilterType;
  typename SetBackgroundImageFilterType::Pointer setBackgroundFilter=SetBackgroundImageFilterType::New();
  setBackgroundFilter->SetInput(firstLabelImage);
  setBackgroundFilter->SetInput2(secondLabelImage);
  setBackgroundFilter->SetMaskValue(this->GetForegroundValue());
  setBackgroundFilter->SetOutsideValue(this->GetForegroundValue());
  setBackgroundFilter->Update();

  output = setBackgroundFilter->GetOutput();

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Fill Bones
  if (GetFillHoles()) {
    StartNewStep("Fill Holes");
    typedef clitk::FillMaskFilter<InternalImageType> FillMaskFilterType;
    typename FillMaskFilterType::Pointer fillMaskFilter = FillMaskFilterType::New();
    fillMaskFilter->SetInput(output);
    fillMaskFilter->AddDirection(2);
    fillMaskFilter->Update();   
    output = fillMaskFilter->GetOutput();
    StopCurrentStep<InternalImageType>(output);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional]
  if (GetAutoCrop()) {
    StartNewStep("AutoCrop");
    typedef clitk::AutoCropFilter<InternalImageType> CropFilterType;
    typename CropFilterType::Pointer cropFilter = CropFilterType::New();
    cropFilter->SetInput(output);
    cropFilter->SetBackgroundValue(GetBackgroundValue());
    cropFilter->Update();   
    output = cropFilter->GetOutput();
    StopCurrentStep<InternalImageType>(output);
    outputImage->SetRegions(output->GetLargestPossibleRegion());
  }

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType>
void 
clitk::ExtractBonesFilter<TInputImageType>::
GenerateData() {

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Final Cast 
  typedef itk::CastImageFilter<InternalImageType, MaskImageType> CastImageFilterType;
  typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
  caster->SetInput(output);
  caster->Update();
  this->GraftOutput(caster->GetOutput());

  // Store image filenames into AFDB 
  GetAFDB()->SetImageFilename("bones", this->GetOutputBonesFilename());  
  WriteAFDB();
  return;
}
//--------------------------------------------------------------------

  
#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
