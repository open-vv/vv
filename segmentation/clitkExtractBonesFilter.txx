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
#include "clitkSegmentationFunctions.h"
#include "clitkAutoCropFilter.h"

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"

//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
clitk::ExtractBonesFilter<TInputImageType, TOutputImageType>::
ExtractBonesFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<TInputImageType, TOutputImageType>()
{
  // Default global options
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0); // Must be zero
  SetForegroundValue(1);

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
  AutoCropOff();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
void 
clitk::ExtractBonesFilter<TInputImageType, TOutputImageType>::
SetInput(const TInputImageType * image) 
{
  this->SetNthInput(0, const_cast<TInputImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
template<class ArgsInfoType>
void 
clitk::ExtractBonesFilter<TInputImageType, TOutputImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);

  SetMinimalComponentSize_GGO(mArgsInfo);
  SetUpperThreshold1_GGO(mArgsInfo);
  SetLowerThreshold1_GGO(mArgsInfo);
  SetFullConnectivity_GGO(mArgsInfo);

  SetUpperThreshold2_GGO(mArgsInfo);
  SetLowerThreshold2_GGO(mArgsInfo);
  SetRadius2_GGO(mArgsInfo);
  SetSampleRate2_GGO(mArgsInfo);
  SetAutoCrop_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
void 
clitk::ExtractBonesFilter<TInputImageType, TOutputImageType>::
GenerateOutputInformation() { 
  // Get input pointers
  InputImagePointer input   = dynamic_cast<TInputImageType*>(itk::ProcessObject::GetInput(0));
  //  InputImagePointer input = dynamic_cast<TInputImageType*>(itk::ProcessObject::GetInput(0));
  Superclass::GenerateOutputInformation();
  OutputImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(input->GetLargestPossibleRegion());

  // typedefs
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> InputBinarizeFilterType;
  typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> BinarizeFilterType;
  typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
  typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelFilterType;
  typedef clitk::SetBackgroundImageFilter<InternalImageType,InternalImageType, InternalImageType> SetBackgroundFilterType; 
  typedef itk::CastImageFilter<InternalImageType,OutputImageType> CastImageFilterType; 
  typedef itk::ImageFileWriter<OutputImageType> WriterType; 

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Initial Labeling");

  typename InternalImageType::Pointer firstLabelImage;
    
  //---------------------------------
  // Binarize the image
  //---------------------------------
  typename InputBinarizeFilterType::Pointer binarizeFilter=InputBinarizeFilterType::New();
  binarizeFilter->SetInput(input);
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
  neighborhoodConnectedImageFilter->SetInput(input);

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
  StartNewStep("Combine de images");
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
template <class TInputImageType, class TOutputImageType>
void 
clitk::ExtractBonesFilter<TInputImageType, TOutputImageType>::
GenerateData() {

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Final Cast 
  typedef itk::CastImageFilter<InternalImageType, OutputImageType> CastImageFilterType;
  typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
  caster->SetInput(output);
  caster->Update();
  //this->SetNthOutput(0, caster->GetOutput());
  this->GraftOutput(caster->GetOutput());
  return;
}
//--------------------------------------------------------------------

  
#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
