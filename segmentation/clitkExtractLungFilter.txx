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

#ifndef CLITKEXTRACTLUNGSFILTER_TXX
#define CLITKEXTRACTLUNGSFILTER_TXX

// clitk
#include "clitkImageCommon.h"
#include "clitkSetBackgroundImageFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkAutoCropFilter.h"

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"

//--------------------------------------------------------------------
template <class TInputImageType, class TMaskImageType>
clitk::ExtractLungFilter<TInputImageType, TMaskImageType>::
ExtractLungFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<TInputImageType, TMaskImageType>()
{
  SetNumberOfSteps(10);
  // Default global options
  this->SetNumberOfRequiredInputs(2);
  SetPatientMaskBackgroundValue(0);
  SetBackgroundValue(0); // Must be zero
  SetForegroundValue(1);

  // Step 1 default values
  SetUpperThreshold(-300);
  SetLowerThreshold(-1000);
  UseLowerThresholdOff();
  LabelParamType * p1 = new LabelParamType;
  p1->SetFirstKeep(1);
  p1->UseLastKeepOff();
  p1->AddLabelToRemove(2);
  SetLabelizeParameters1(p1);

  // Step 2 default values
  SetUpperThresholdForTrachea(-900);
  SetMultiplierForTrachea(5);
  SetThresholdStepSizeForTrachea(64);
  
  // Step 3 default values
  SetNumberOfHistogramBins(500);
  LabelParamType * p2 = new LabelParamType;
  p2->SetFirstKeep(1);
  p2->UseLastKeepOff();
  // p->AddLabelToRemove(2); // No label to remove by default
  SetLabelizeParameters2(p2);

  // Step 4 default values
  SetRadiusForTrachea(1);
  LabelParamType * p3 = new LabelParamType;
  p3->SetFirstKeep(1);
  p3->SetLastKeep(2);
  p3->UseLastKeepOff();
  SetLabelizeParameters3(p3);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TMaskImageType>
void 
clitk::ExtractLungFilter<TInputImageType, TMaskImageType>::
SetInput(const TInputImageType * image) 
{
  this->SetNthInput(0, const_cast<TInputImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TMaskImageType>
void 
clitk::ExtractLungFilter<TInputImageType, TMaskImageType>::
SetInputPatientMask(TMaskImageType * image, MaskImagePixelType bg ) 
{
  this->SetNthInput(1, const_cast<TMaskImageType *>(image));
  SetPatientMaskBackgroundValue(bg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TMaskImageType>
void 
clitk::ExtractLungFilter<TInputImageType, TMaskImageType>::
AddSeed(InternalIndexType s) 
{ 
  m_Seeds.push_back(s);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TMaskImageType>
template<class ArgsInfoType>
void 
clitk::ExtractLungFilter<TInputImageType, TMaskImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);

  SetUpperThreshold_GGO(mArgsInfo);
  SetLowerThreshold_GGO(mArgsInfo);
  SetLabelizeParameters1_GGO(mArgsInfo);
  if (!mArgsInfo.remove1_given) {
    GetLabelizeParameters1()->AddLabelToRemove(2); 
    if (GetVerboseOption()) VerboseOption("remove1", 2);
  }

  SetUpperThresholdForTrachea_GGO(mArgsInfo);
  SetMultiplierForTrachea_GGO(mArgsInfo);
  SetThresholdStepSizeForTrachea_GGO(mArgsInfo);
  AddSeed_GGO(mArgsInfo);

  SetNumberOfHistogramBins_GGO(mArgsInfo);
  SetLabelizeParameters2_GGO(mArgsInfo);

  SetRadiusForTrachea_GGO(mArgsInfo);
  SetLabelizeParameters3_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TMaskImageType>
void 
clitk::ExtractLungFilter<TInputImageType, TMaskImageType>::
GenerateOutputInformation() 
{ 
  input   = dynamic_cast<const TInputImageType*>(itk::ProcessObject::GetInput(0));
  Superclass::GenerateOutputInformation();

  // Get input pointers
  input   = dynamic_cast<const TInputImageType*>(itk::ProcessObject::GetInput(0));
  patient = dynamic_cast<const TMaskImageType*>(itk::ProcessObject::GetInput(1));

  // Check image
  if (!HaveSameSizeAndSpacing<TInputImageType, TMaskImageType>(input, patient)) {
    this->SetLastError("* ERROR * the images (input and patient mask) must have the same size & spacing");
    return;
  }
  
  // Set Number of steps
  SetNumberOfSteps(9);
  
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Set background to initial image");
  working_input = SetBackground<TInputImageType, TMaskImageType>
    (input, patient, GetPatientMaskBackgroundValue(), -1000);
  StopCurrentStep<InputImageType>(working_input);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Remove Air");
  // Threshold to get air
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> InputBinarizeFilterType; 
  typename InputBinarizeFilterType::Pointer binarizeFilter=InputBinarizeFilterType::New();
  binarizeFilter->SetInput(working_input);
  if (m_UseLowerThreshold) binarizeFilter->SetLowerThreshold(m_LowerThreshold);
  binarizeFilter->SetUpperThreshold(m_UpperThreshold);
  binarizeFilter ->SetInsideValue(this->GetForegroundValue());
  binarizeFilter ->SetOutsideValue(this->GetBackgroundValue());
  binarizeFilter->Update();
  working_image = binarizeFilter->GetOutput();
  
  // Labelize and keep right labels
  working_image = Labelize<InternalImageType>(working_image, GetBackgroundValue(), true, GetMinimalComponentSize());
  working_image = RemoveLabels<InternalImageType>
    (working_image, GetBackgroundValue(), GetLabelizeParameters1()->GetLabelsToRemove());
  typename InternalImageType::Pointer air = KeepLabels<InternalImageType>
    (working_image, 
     GetBackgroundValue(), 
     GetForegroundValue(), 
     GetLabelizeParameters1()->GetFirstKeep(), 
     GetLabelizeParameters1()->GetLastKeep(), 
     GetLabelizeParameters1()->GetUseLastKeep());
 
  // Set Air to BG
  working_input = SetBackground<TInputImageType, InternalImageType>
    (working_input, air, this->GetForegroundValue(), this->GetBackgroundValue());

  // End
  StopCurrentStep<InputImageType>(working_input);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Find the trachea");
  //DD(m_Seeds.size());
  if (m_Seeds.size() == 0) { // try to find seed
    // Search seed (parameters = UpperThresholdForTrachea)
    static const unsigned int Dim = InputImageType::ImageDimension;
    typename InternalImageType::RegionType sliceRegion = working_input->GetLargestPossibleRegion();
    typename InternalImageType::SizeType sliceRegionSize = sliceRegion.GetSize();
    typename InternalImageType::IndexType sliceRegionIndex = sliceRegion.GetIndex();
    sliceRegionIndex[Dim-1]=sliceRegionSize[Dim-1]-5;
    sliceRegionSize[Dim-1]=5;
    sliceRegion.SetSize(sliceRegionSize);
    sliceRegion.SetIndex(sliceRegionIndex);
    //DD(GetUpperThresholdForTrachea());
    //DD(sliceRegion);
    typedef  itk::ImageRegionConstIterator<InputImageType> IteratorType;
    IteratorType it(working_input, sliceRegion);
    it.GoToBegin();
    while (!it.IsAtEnd()) {
      if(it.Get() < GetUpperThresholdForTrachea() ) {
        AddSeed(it.GetIndex());
	//	DD(it.GetIndex());
      }
      ++it;
    }
  }
  
  //DD(m_Seeds.size());
  if (m_Seeds.size() != 0) {
    // Explosion controlled region growing
    typedef clitk::ExplosionControlledThresholdConnectedImageFilter<InputImageType, InternalImageType> ImageFilterType;
    typename ImageFilterType::Pointer f= ImageFilterType::New();
    f->SetInput(working_input);
    f->SetVerbose(false);
    f->SetLower(-2000);
    f->SetUpper(GetUpperThresholdForTrachea());
    f->SetMinimumLowerThreshold(-2000);
    f->SetMaximumUpperThreshold(0);
    f->SetAdaptLowerBorder(false);
    f->SetAdaptUpperBorder(true);
    f->SetMinimumSize(5000); 
    f->SetReplaceValue(1);
    f->SetMultiplier(GetMultiplierForTrachea());
    f->SetThresholdStepSize(GetThresholdStepSizeForTrachea());
    f->SetMinimumThresholdStepSize(1);
    for(unsigned int i=0; i<m_Seeds.size();i++) {
      //    std::cout<<"Adding seed " <<m_Seeds[i]<<"..."<<std::endl;
      f->AddSeed(m_Seeds[i]);
    }  
    f->Update();
    trachea_tmp = f->GetOutput();
    // Set output
    StopCurrentStep<InternalImageType>(trachea_tmp);

  }
  else { // Trachea not found
    this->SetWarning("* WARNING * No seed found for trachea.");
    // Set output
    StopCurrentStep();
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Extract the lung with Otsu filter");
  // Automated Otsu thresholding and relabeling
  typedef itk::OtsuThresholdImageFilter<InputImageType,InternalImageType> OtsuThresholdImageFilterType;
  typename OtsuThresholdImageFilterType::Pointer otsuFilter=OtsuThresholdImageFilterType::New();
  otsuFilter->SetInput(working_input);
  otsuFilter->SetNumberOfHistogramBins(GetNumberOfHistogramBins());
  otsuFilter->SetInsideValue(this->GetForegroundValue());
  otsuFilter->SetOutsideValue(this->GetBackgroundValue());
  otsuFilter->Update();
  working_image =  otsuFilter->GetOutput();

  // Set output
  StopCurrentStep<InternalImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Select labels");
  // Keep right labels
  working_image = LabelizeAndSelectLabels<InternalImageType>
    (working_image, 
     GetBackgroundValue(), 
     GetForegroundValue(), 
     false, 
     GetMinimalComponentSize(), 
     GetLabelizeParameters2());

  // Set output
  StopCurrentStep<InternalImageType>(working_image);
  
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  if (m_Seeds.size() != 0) { // if ==0 ->no trachea found
    StartNewStep("Remove the trachea");
    // Set the trachea
    working_image = SetBackground<InternalImageType, InternalImageType>
      (working_image, trachea_tmp, 1, -1);
  
   // Dilate the trachea 
    static const unsigned int Dim = InputImageType::ImageDimension;
    typedef itk::BinaryBallStructuringElement<InternalPixelType, Dim> KernelType;
    KernelType structuringElement;
    structuringElement.SetRadius(GetRadiusForTrachea());
    structuringElement.CreateStructuringElement();
    typedef clitk::ConditionalBinaryDilateImageFilter<InternalImageType, InternalImageType, KernelType> ConditionalBinaryDilateImageFilterType;
    typename ConditionalBinaryDilateImageFilterType::Pointer dilateFilter = ConditionalBinaryDilateImageFilterType::New();
    dilateFilter->SetBoundaryToForeground(false);
    dilateFilter->SetKernel(structuringElement);
    dilateFilter->SetBackgroundValue (1);
    dilateFilter->SetForegroundValue (-1);
    dilateFilter->SetInput (working_image);
    dilateFilter->Update();
    working_image = dilateFilter->GetOutput();  
    
    // Set trachea with dilatation
    trachea_tmp = SetBackground<InternalImageType, InternalImageType>
      (trachea_tmp, working_image, -1, this->GetForegroundValue()); 

    // Remove the trachea
    working_image = SetBackground<InternalImageType, InternalImageType>
      (working_image, working_image, -1, this->GetBackgroundValue());  
    
    // Label
    working_image = LabelizeAndSelectLabels<InternalImageType>
      (working_image, 
       GetBackgroundValue(), 
       GetForegroundValue(), 
       false, 
       GetMinimalComponentSize(), 
       GetLabelizeParameters3());
    
    // Set output
    StopCurrentStep<InternalImageType>(working_image);
  }
  

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  typedef clitk::AutoCropFilter<InternalImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  if (m_Seeds.size() != 0) { // if ==0 ->no trachea found
    StartNewStep("Croping trachea");
    cropFilter->SetInput(trachea_tmp);
    cropFilter->Update(); // Needed
    typedef itk::CastImageFilter<InternalImageType, MaskImageType> CastImageFilterType;
    typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
    caster->SetInput(cropFilter->GetOutput());
    caster->Update();   
    trachea = caster->GetOutput();
    StopCurrentStep<MaskImageType>(trachea);  
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Croping lung");
  typename CropFilterType::Pointer cropFilter2 = CropFilterType::New(); // Needed to reset pipeline
  cropFilter2->SetInput(working_image);
  cropFilter2->Update();   
  working_image = cropFilter2->GetOutput();
  StopCurrentStep<InternalImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Separate Left/Right lungs");
  // Initial label
  working_image = Labelize<InternalImageType>(working_image, 
                                              GetBackgroundValue(), 
                                              false, 
                                              GetMinimalComponentSize());

  // Count the labels
  typedef itk::StatisticsImageFilter<InternalImageType> StatisticsImageFilterType;
  typename StatisticsImageFilterType::Pointer statisticsImageFilter=StatisticsImageFilterType::New();
  statisticsImageFilter->SetInput(working_image);
  statisticsImageFilter->Update();
  unsigned int initialNumberOfLabels = statisticsImageFilter->GetMaximum();
  working_image = statisticsImageFilter->GetOutput();	
 
  // Decompose the first label
  static const unsigned int Dim = InputImageType::ImageDimension;
  if (initialNumberOfLabels<2) {
    // Structuring element radius
    typename InputImageType::SizeType radius;
    for (unsigned int i=0;i<Dim;i++) radius[i]=1;
    typedef clitk::DecomposeAndReconstructImageFilter<InternalImageType,InternalImageType> DecomposeAndReconstructFilterType;
    typename DecomposeAndReconstructFilterType::Pointer decomposeAndReconstructFilter=DecomposeAndReconstructFilterType::New();
    decomposeAndReconstructFilter->SetInput(working_image);
    decomposeAndReconstructFilter->SetVerbose(false);
    decomposeAndReconstructFilter->SetRadius(radius);
    decomposeAndReconstructFilter->SetMaximumNumberOfLabels(2);
    decomposeAndReconstructFilter->SetMinimumObjectSize(this->GetMinimalComponentSize());
    decomposeAndReconstructFilter->SetMinimumNumberOfIterations(1);
    decomposeAndReconstructFilter->SetBackgroundValue(this->GetBackgroundValue());
    decomposeAndReconstructFilter->SetForegroundValue(this->GetForegroundValue());
    decomposeAndReconstructFilter->SetFullyConnected(true);
    decomposeAndReconstructFilter->SetNumberOfNewLabels(1);
    decomposeAndReconstructFilter->Update();
    working_image = decomposeAndReconstructFilter->GetOutput();      
  }

  // Retain labels (lungs)
  typedef itk::ThresholdImageFilter<InternalImageType> ThresholdImageFilterType;
  typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
  thresholdFilter->SetInput(working_image);
  thresholdFilter->ThresholdAbove(2);
  thresholdFilter->SetOutsideValue(this->GetBackgroundValue());
  thresholdFilter->Update();
  working_image = thresholdFilter->GetOutput();
  StopCurrentStep<InternalImageType> (working_image);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TMaskImageType>
void 
clitk::ExtractLungFilter<TInputImageType, TMaskImageType>::
GenerateData() {
  // Final Cast 
  typedef itk::CastImageFilter<InternalImageType, MaskImageType> CastImageFilterType;
  typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
  caster->SetInput(working_image);
  caster->Update();
  // Set output
  //this->SetNthOutput(0, caster->GetOutput()); // -> no because redo filter otherwise
  this->GraftOutput(caster->GetOutput());
  return;
}
//--------------------------------------------------------------------

  
#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
