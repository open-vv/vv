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
#include "clitkCropLikeImageFilter.h"
#include "clitkFillMaskFilter.h"
#include "clitkMemoryUsage.h"

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkBinaryThinningImageFilter3D.h"
#include "itkImageIteratorWithIndex.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"

//--------------------------------------------------------------------
template <class ImageType>
clitk::ExtractLungFilter<ImageType>::
ExtractLungFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<ImageType, MaskImageType>()
{
  SetNumberOfSteps(10);
  m_MaxSeedNumber = 500;

  // Default global options
  this->SetNumberOfRequiredInputs(1);
  SetPatientMaskBackgroundValue(0);
  SetBackgroundValue(0); // Must be zero
  SetForegroundValue(1);
  SetMinimalComponentSize(100);
  VerboseRegionGrowingFlagOff();

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
  SetNumberOfSlicesToSkipBeforeSearchingSeed(0);
  TracheaVolumeMustBeCheckedFlagOn();
  
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
  
  // Step 5
  OpenCloseFlagOff();
  SetOpenCloseRadius(1);
  
  // Step 6
  FillHolesFlagOn();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLungFilter<ImageType>::
SetInput(const ImageType * image) 
{
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLungFilter<ImageType>::
AddSeed(InternalIndexType s) 
{ 
  m_Seeds.push_back(s);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLungFilter<ImageType>::
GenerateOutputInformation() 
{ 
  clitk::PrintMemory(GetVerboseMemoryFlag(), "Initial memory"); // OK
  Superclass::GenerateOutputInformation();
  
  // Read DB
  LoadAFDB();

  // Get input pointers
  input   = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
  patient = GetAFDB()->template GetImage <MaskImageType>("Patient");  
  PrintMemory(GetVerboseMemoryFlag(), "After reading patient"); // OK

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Crop input like patient image (must have the same spacing)
  // It copy the input if the same are the same
  StartNewStep("Copy and crop input image to 'patient' extends");
  typedef clitk::CropLikeImageFilter<ImageType> CropImageFilter;
  typename CropImageFilter::Pointer cropFilter = CropImageFilter::New();
  // cropFilter->ReleaseDataFlagOn(); // NO=seg fault !!
  cropFilter->SetInput(input);
  cropFilter->SetCropLikeImage(patient);
  cropFilter->Update();
  working_input = cropFilter->GetOutput();
  //  cropFilter->Delete(); // NO !!!! if yes, sg fault, Cropfilter is buggy !??
  StopCurrentStep<ImageType>(working_input);
  PrintMemory(GetVerboseMemoryFlag(), "After crop"); // OK, slightly more than a copy of input

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Set background to initial image");
  working_input = SetBackground<ImageType, MaskImageType>
    (working_input, patient, GetPatientMaskBackgroundValue(), -1000, true);
  StopCurrentStep<ImageType>(working_input);
  PrintMemory(GetVerboseMemoryFlag(), "After set bg"); // OK, additional mem = 0

  /*
  // We do not need patient mask anymore, release memory 
  GetAFDB()->template ReleaseImage<MaskImageType>("Patient");
  DD(patient->GetReferenceCount());
  PrintMemory(GetVerboseMemoryFlag(), "After delete patient"); // OK, additional mem = 0
  patient->Delete();
  PrintMemory(GetVerboseMemoryFlag(), "After delete patient"); // OK, additional mem = 0
  */

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Remove Air");
  // Check threshold
  if (m_UseLowerThreshold) {
    if (m_LowerThreshold > m_UpperThreshold) {
      clitkExceptionMacro("lower threshold cannot be greater than upper threshold.");
    }
  }
  // Threshold to get air
  typedef itk::BinaryThresholdImageFilter<ImageType, MaskImageType> InputBinarizeFilterType; 
  typename InputBinarizeFilterType::Pointer binarizeFilter = InputBinarizeFilterType::New();
  binarizeFilter->SetInput(working_input);
  if (m_UseLowerThreshold) binarizeFilter->SetLowerThreshold(m_LowerThreshold);
  // binarizeFilter->CanRunInPlace() is false
  binarizeFilter->SetUpperThreshold(m_UpperThreshold);
  binarizeFilter->SetInsideValue(this->GetForegroundValue());
  binarizeFilter->SetOutsideValue(this->GetBackgroundValue());
  binarizeFilter->Update();
  working_mask = binarizeFilter->GetOutput();
  PrintMemory(GetVerboseMemoryFlag(), "After Binarizefilter"); // OK, additional mem is one mask image

  // Labelize and keep right labels
  working_mask = 
    Labelize<MaskImageType>
    (working_mask, GetBackgroundValue(), true, GetMinimalComponentSize());
  PrintMemory(GetVerboseMemoryFlag(), "After Labelize"); // BUG ? additional mem around 1 time the input ? 
  
  working_mask = RemoveLabels<MaskImageType>
    (working_mask, GetBackgroundValue(), GetLabelizeParameters1()->GetLabelsToRemove());
  PrintMemory(GetVerboseMemoryFlag(), "After RemoveLabels"); // OK additional mem = 0

  working_mask = KeepLabels<MaskImageType>
    (working_mask, 
     GetBackgroundValue(), 
     GetForegroundValue(), 
     GetLabelizeParameters1()->GetFirstKeep(), 
     GetLabelizeParameters1()->GetLastKeep(), 
     GetLabelizeParameters1()->GetUseLastKeep());
  PrintMemory(GetVerboseMemoryFlag(), "After KeepLabels to create the 'air'");
 
  // Set Air to BG
  working_input = SetBackground<ImageType, MaskImageType>
    (working_input, working_mask, this->GetForegroundValue(), this->GetBackgroundValue(), true);
  PrintMemory(GetVerboseMemoryFlag(), "After SetBackground");

  // End
  StopCurrentStep<ImageType>(working_input);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Search for the trachea");
  SearchForTrachea();
  PrintMemory(GetVerboseMemoryFlag(), "After SearchForTrachea");

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Extract the lung with Otsu filter");
  // Automated Otsu thresholding and relabeling
  typedef itk::OtsuThresholdImageFilter<ImageType,MaskImageType> OtsuThresholdImageFilterType;
  typename OtsuThresholdImageFilterType::Pointer otsuFilter=OtsuThresholdImageFilterType::New();
  otsuFilter->SetInput(working_input);
  otsuFilter->SetNumberOfHistogramBins(GetNumberOfHistogramBins());
  otsuFilter->SetInsideValue(this->GetForegroundValue());
  otsuFilter->SetOutsideValue(this->GetBackgroundValue());
  otsuFilter->Update();
  working_mask =  otsuFilter->GetOutput();

  // Set output
  StopCurrentStep<MaskImageType>(working_mask);
  PrintMemory(GetVerboseMemoryFlag(), "After Otsufilter");

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Select labels");
  // Keep right labels
  working_mask = LabelizeAndSelectLabels<MaskImageType>
    (working_mask, 
     GetBackgroundValue(), 
     GetForegroundValue(), 
     false, 
     GetMinimalComponentSize(), 
     GetLabelizeParameters2());

  // Set output
  StopCurrentStep<MaskImageType>(working_mask);
  PrintMemory(GetVerboseMemoryFlag(), "After LabelizeAndSelectLabels");
  
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  if (m_Seeds.size() != 0) { // if ==0 ->no trachea found
    StartNewStep("Remove the trachea");
    // Set the trachea
    working_mask = SetBackground<MaskImageType, MaskImageType>
      (working_mask, trachea, 1, -1, true);
    PrintMemory(GetVerboseMemoryFlag(), "After SetBackground");
  
    // Dilate the trachea 
    static const unsigned int Dim = ImageType::ImageDimension;
    typedef itk::BinaryBallStructuringElement<InternalPixelType, Dim> KernelType;
    KernelType structuringElement;
    structuringElement.SetRadius(GetRadiusForTrachea());
    structuringElement.CreateStructuringElement();
    typedef clitk::ConditionalBinaryDilateImageFilter<MaskImageType, MaskImageType, KernelType> ConditionalBinaryDilateImageFilterType;
    typename ConditionalBinaryDilateImageFilterType::Pointer dilateFilter = ConditionalBinaryDilateImageFilterType::New();
    dilateFilter->SetBoundaryToForeground(false);
    dilateFilter->SetKernel(structuringElement);
    dilateFilter->SetBackgroundValue (1);
    dilateFilter->SetForegroundValue (-1);
    dilateFilter->SetInput (working_mask);
    dilateFilter->Update();
    working_mask = dilateFilter->GetOutput();  
    PrintMemory(GetVerboseMemoryFlag(), "After dilate");
    
    // Set trachea with dilatation
    trachea = SetBackground<MaskImageType, MaskImageType>
      (trachea, working_mask, -1, this->GetForegroundValue(), true); 

    // Remove the trachea
    working_mask = SetBackground<MaskImageType, MaskImageType>
      (working_mask, working_mask, -1, this->GetBackgroundValue(), true);  
    
    // Label
    working_mask = LabelizeAndSelectLabels<MaskImageType>
      (working_mask, 
       GetBackgroundValue(), 
       GetForegroundValue(), 
       false, 
       GetMinimalComponentSize(), 
       GetLabelizeParameters3());
    
    // Set output
    StopCurrentStep<MaskImageType>(working_mask);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  PrintMemory(GetVerboseMemoryFlag(), "before autocropfilter");
  if (m_Seeds.size() != 0) { // if ==0 ->no trachea found
    trachea = clitk::AutoCrop<MaskImageType>(trachea, GetBackgroundValue());
    StopCurrentStep<MaskImageType>(trachea);  
    PrintMemory(GetVerboseMemoryFlag(), "after delete trachea");
  }
  PrintMemory(GetVerboseMemoryFlag(), "after delete trachea");

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Cropping lung");
  PrintMemory(GetVerboseMemoryFlag(), "Before Autocropfilter");
  working_mask = clitk::AutoCrop<MaskImageType>(working_mask, GetBackgroundValue());
  StopCurrentStep<MaskImageType>(working_mask);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Final OpenClose
  if (GetOpenCloseFlag()) {
    StartNewStep("Open/Close"); 
    PrintMemory(GetVerboseMemoryFlag(), "Before OpenClose");
  
    // Structuring element
    typedef itk::BinaryBallStructuringElement<InternalPixelType, ImageDimension> KernelType;
    KernelType structuringElement;
    structuringElement.SetRadius(GetOpenCloseRadius());
    structuringElement.CreateStructuringElement();
	
    // Open
    typedef itk::BinaryMorphologicalOpeningImageFilter<MaskImageType, InternalImageType, KernelType> OpenFilterType;
    typename OpenFilterType::Pointer openFilter = OpenFilterType::New();
    openFilter->SetInput(working_mask);
    openFilter->SetBackgroundValue(GetBackgroundValue());
    openFilter->SetForegroundValue(GetForegroundValue());
    openFilter->SetKernel(structuringElement);
	
    // Close
    typedef itk::BinaryMorphologicalClosingImageFilter<MaskImageType, MaskImageType, KernelType> CloseFilterType;
    typename CloseFilterType::Pointer closeFilter = CloseFilterType::New();
    closeFilter->SetInput(openFilter->GetOutput());
    closeFilter->SetSafeBorder(true);
    closeFilter->SetForegroundValue(GetForegroundValue());
    closeFilter->SetKernel(structuringElement);
    closeFilter->Update();
    working_mask = closeFilter->GetOutput();
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Fill Lungs
  if (GetFillHolesFlag()) {
    StartNewStep("Fill Holes");
    PrintMemory(GetVerboseMemoryFlag(), "Before Fill Holes");
    typedef clitk::FillMaskFilter<MaskImageType> FillMaskFilterType;
    typename FillMaskFilterType::Pointer fillMaskFilter = FillMaskFilterType::New();
    fillMaskFilter->SetInput(working_mask);
    fillMaskFilter->AddDirection(2);
    //fillMaskFilter->AddDirection(1);
    fillMaskFilter->Update();   
    working_mask = fillMaskFilter->GetOutput();
    StopCurrentStep<MaskImageType>(working_mask);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Separate Left/Right lungs");
    PrintMemory(GetVerboseMemoryFlag(), "Before Separate");
  // Initial label
  working_mask = Labelize<MaskImageType>(working_mask, 
                                         GetBackgroundValue(), 
                                         false, 
                                         GetMinimalComponentSize());

  PrintMemory(GetVerboseMemoryFlag(), "After Labelize");

  // Count the labels
  typedef itk::StatisticsImageFilter<MaskImageType> StatisticsImageFilterType;
  typename StatisticsImageFilterType::Pointer statisticsImageFilter=StatisticsImageFilterType::New();
  statisticsImageFilter->SetInput(working_mask);
  statisticsImageFilter->Update();
  unsigned int initialNumberOfLabels = statisticsImageFilter->GetMaximum();
  working_mask = statisticsImageFilter->GetOutput();	
  
  PrintMemory(GetVerboseMemoryFlag(), "After count label");
 
  // Decompose the first label
  static const unsigned int Dim = ImageType::ImageDimension;
  if (initialNumberOfLabels<2) {
    // Structuring element radius
    typename ImageType::SizeType radius;
    for (unsigned int i=0;i<Dim;i++) radius[i]=1;
    typedef clitk::DecomposeAndReconstructImageFilter<MaskImageType,MaskImageType> DecomposeAndReconstructFilterType;
    typename DecomposeAndReconstructFilterType::Pointer decomposeAndReconstructFilter=DecomposeAndReconstructFilterType::New();
    decomposeAndReconstructFilter->SetInput(working_mask);
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
    working_mask = decomposeAndReconstructFilter->GetOutput();      
  }
  PrintMemory(GetVerboseMemoryFlag(), "After decomposeAndReconstructFilter");

  // Retain labels ('1' is largset lung, so right. '2' is left)
  typedef itk::ThresholdImageFilter<MaskImageType> ThresholdImageFilterType;
  typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
  thresholdFilter->SetInput(working_mask);
  thresholdFilter->ThresholdAbove(2);
  thresholdFilter->SetOutsideValue(this->GetBackgroundValue());
  thresholdFilter->Update();
  working_mask = thresholdFilter->GetOutput();
  StopCurrentStep<MaskImageType> (working_mask);
  PrintMemory(GetVerboseMemoryFlag(), "After Thresholdfilter");

  // Update output info
  //  output = working_mask;
  //this->GetOutput(0)->SetRegions(output->GetLargestPossibleRegion());

  //  this->GetOutput(0)->SetRegions(working_mask->GetLargestPossibleRegion());

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLungFilter<TImageType>::
GenerateInputRequestedRegion() {
  //  DD("GenerateInputRequestedRegion (nothing?)");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLungFilter<ImageType>::
GenerateData() 
{
  // Set the output
  //  this->GraftOutput(output); // not SetNthOutput
  this->GraftOutput(working_mask); // not SetNthOutput
  // Store image filenames into AFDB 
  GetAFDB()->SetImageFilename("Lungs", this->GetOutputLungFilename());  
  GetAFDB()->SetImageFilename("Trachea", this->GetOutputTracheaFilename());  
  WriteAFDB();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
bool 
clitk::ExtractLungFilter<ImageType>::
SearchForTracheaSeed(int skip)
{
  if (m_Seeds.size() == 0) { // try to find seed (if not zero, it is given by user)    
    // Restart the search until a seed is found, skipping more and more slices
    bool stop = false;
    while (!stop) {
      // Search seed (parameters = UpperThresholdForTrachea)
      static const unsigned int Dim = ImageType::ImageDimension;
      typename InternalImageType::RegionType sliceRegion = working_input->GetLargestPossibleRegion();
      typename InternalImageType::SizeType sliceRegionSize = sliceRegion.GetSize();
      typename InternalImageType::IndexType sliceRegionIndex = sliceRegion.GetIndex();
      sliceRegionIndex[Dim-1]=sliceRegionSize[Dim-1]-skip-5;
      sliceRegionSize[Dim-1]=5;
      sliceRegion.SetSize(sliceRegionSize);
      sliceRegion.SetIndex(sliceRegionIndex);
      typedef  itk::ImageRegionConstIterator<ImageType> IteratorType;
      IteratorType it(working_input, sliceRegion);
      it.GoToBegin();
      while (!it.IsAtEnd()) {
        if(it.Get() < GetUpperThresholdForTrachea() ) {
          AddSeed(it.GetIndex());
          // DD(it.GetIndex());
        }
        ++it;
      }
      
      // if we do not found : restart
      stop = (m_Seeds.size() != 0);
      if (!stop) {
	if (GetVerboseStepFlag()) {
	  std::cout << "\t No seed found this time. I skip some slices and restart." << std::endl;
	}
        if (skip > 0.5 * working_input->GetLargestPossibleRegion().GetSize()[2]) {
          // we want to skip more than a half of the image, it is probably a bug
          std::cerr << "Number of slices to skip to find trachea to high = " << skip << std::endl;
          stop = true;
        }
        skip += 5;
      }
      else {
        // DD(m_Seeds[0]);
        // DD(m_Seeds.size());
      }
    }
  }
  return (m_Seeds.size() != 0);
}
//--------------------------------------------------------------------

  
//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLungFilter<ImageType>::
TracheaRegionGrowing()
{
  // Explosion controlled region growing
  PrintMemory(GetVerboseMemoryFlag(), "Before ExplosionControlledThresholdConnectedImageFilter");
  typedef clitk::ExplosionControlledThresholdConnectedImageFilter<ImageType, MaskImageType> ImageFilterType;
  typename ImageFilterType::Pointer f= ImageFilterType::New();
  f->SetInput(working_input);
  f->SetLower(-2000);
  f->SetUpper(GetUpperThresholdForTrachea());
  f->SetMinimumLowerThreshold(-2000);
  //  f->SetMaximumUpperThreshold(0); // MAYBE TO CHANGE ???
  f->SetMaximumUpperThreshold(-800); // MAYBE TO CHANGE ???
  f->SetAdaptLowerBorder(false);
  f->SetAdaptUpperBorder(true);
  f->SetMinimumSize(5000); 
  f->SetReplaceValue(1);
  f->SetMultiplier(GetMultiplierForTrachea());
  f->SetThresholdStepSize(GetThresholdStepSizeForTrachea());
  f->SetMinimumThresholdStepSize(1);
  f->SetVerbose(GetVerboseRegionGrowingFlag());
  for(unsigned int i=0; i<m_Seeds.size();i++) {
    f->AddSeed(m_Seeds[i]);
    // DD(m_Seeds[i]);
  }  
  f->Update();
  PrintMemory(GetVerboseMemoryFlag(), "After RG update");
  
  // take first (main) connected component
  trachea = Labelize<MaskImageType>(f->GetOutput(), 
                                    GetBackgroundValue(), 
                                    true, 
                                    1000);//GetMinimalComponentSize());
  PrintMemory(GetVerboseMemoryFlag(), "After Labelize");
  trachea = KeepLabels<MaskImageType>(trachea, 
					      GetBackgroundValue(), 
					      GetForegroundValue(), 
					      1, 1, false);
  PrintMemory(GetVerboseMemoryFlag(), "After KeepLabels");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
double 
clitk::ExtractLungFilter<ImageType>::
ComputeTracheaVolume()
{
  typedef itk::ImageRegionConstIterator<InternalImageType> IteratorType;
  IteratorType iter(trachea, trachea->GetLargestPossibleRegion());
  iter.GoToBegin();
  double volume = 0.0;
  while (!iter.IsAtEnd()) {
    if (iter.Get() == this->GetForegroundValue()) volume++;
    ++iter;
  }
  
  double voxelsize = trachea->GetSpacing()[0]*trachea->GetSpacing()[1]*trachea->GetSpacing()[2];
  return volume*voxelsize;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLungFilter<ImageType>::
SearchForTrachea()
{
  // Search for seed among n slices, skip some slices before starting
  // if not found -> skip more and restart 
  
  // when seed found : perform region growing
  // compute trachea volume
  // if volume not plausible  -> skip more slices and restart 

  bool stop = false;
  double volume = 0.0;
  int skip = GetNumberOfSlicesToSkipBeforeSearchingSeed();
  while (!stop) {
    stop = SearchForTracheaSeed(skip);
    if (stop) {
      TracheaRegionGrowing();
      volume = ComputeTracheaVolume()/1000; // assume mm3, so divide by 1000 to get cc
      if (GetWriteStepFlag()) {
        writeImage<MaskImageType>(trachea, "step-trachea-"+toString(skip)+".mhd");
      }
      if (GetTracheaVolumeMustBeCheckedFlag()) {
        if ((volume > 10) && (volume < 65 )) { // depend on image size ...
          // Typical volume 22.59 cm 3 (± 7.69 cm 3 ) [ Leader 2004 ]
          if (GetVerboseStepFlag()) {
            std::cout << "\t Found trachea with volume " << volume << " cc." << std::endl;
          }
          stop = true; 
        }
        else {
          if (GetVerboseStepFlag()) {
            std::cout << "\t The volume of the trachea (" << volume 
                      << " cc) seems not correct. I skip some slices (" << skip << ") and restart to find seeds." 
                      << std::endl;
          }
          skip += 5;
          stop = false;
          // empty the list of seed
          m_Seeds.clear();
        }
      }
      else {
        stop = true;
      }
    }
  }

  if (volume != 0.0) {
    // Set output
    StopCurrentStep<MaskImageType>(trachea);
  }
  else { // Trachea not found
    this->SetWarning("* WARNING * No seed found for trachea.");
    StopCurrentStep();
  }
}
//--------------------------------------------------------------------

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
