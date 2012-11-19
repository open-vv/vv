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
#include "itkConstantPadImageFilter.h"
#include <itkBinaryBallStructuringElement.h>
#include "itkStatisticsLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkOrientImageFilter.h"
#include "itkSpatialOrientationAdapter.h"
#include "itkImageDuplicator.h"
#include "itkRelabelComponentImageFilter.h"

#include <fcntl.h>

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
  RemoveSmallLabelBeforeSeparationFlagOn();

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
  SetTracheaSeedAlgorithm(0);
  SetUpperThresholdForTrachea(-700);
  SetMultiplierForTrachea(5);
  SetThresholdStepSizeForTrachea(64);
  SetNumberOfSlicesToSkipBeforeSearchingSeed(0);
  TracheaVolumeMustBeCheckedFlagOn();
  SetNumSlices(50);
  SetMaxElongation(0.5);
  SetSeedPreProcessingThreshold(-400);
 
  
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
  AutoCropOn();
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

  // Pad images with air to prevent patient touching the image border
  static const unsigned int Dim = ImageType::ImageDimension;
  typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadFilterType;
  typename PadFilterType::Pointer padFilter = PadFilterType::New();
  padFilter->SetInput(working_input);
  padFilter->SetConstant(-1000);
  typename ImageType::SizeType bounds;
  for (unsigned i = 0; i < Dim - 1; ++i)
    bounds[i] = 1;
  bounds[Dim - 1] = 0;
  padFilter->SetPadLowerBound(bounds);
  padFilter->SetPadUpperBound(bounds);
  padFilter->Update();
  working_input = padFilter->GetOutput();

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
  if (m_Seeds.empty()) {
    clitkExceptionMacro("No seeds for trachea... Aborting.");
  }

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
    if (GetAutoCrop())
      trachea = clitk::AutoCrop<MaskImageType>(trachea, GetBackgroundValue());
    else
    {
      // Remove Padding region
      typedef itk::CropImageFilter<MaskImageType, MaskImageType> CropFilterType;
      typename CropFilterType::Pointer cropFilter = CropFilterType::New();
      cropFilter->SetInput(trachea);
      cropFilter->SetLowerBoundaryCropSize(bounds);
      cropFilter->SetUpperBoundaryCropSize(bounds);
      cropFilter->Update();
      trachea = cropFilter->GetOutput();
    }
    StopCurrentStep<MaskImageType>(trachea);  
    PrintMemory(GetVerboseMemoryFlag(), "after delete trachea");
  }
  PrintMemory(GetVerboseMemoryFlag(), "after delete trachea");

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Cropping lung");
  PrintMemory(GetVerboseMemoryFlag(), "Before Autocropfilter");
  if (GetAutoCrop())
    working_mask = clitk::AutoCrop<MaskImageType>(working_mask, GetBackgroundValue());
  else
  {
    // Remove Padding region
    typedef itk::CropImageFilter<MaskImageType, MaskImageType> CropFilterType;
    typename CropFilterType::Pointer cropFilter = CropFilterType::New();
    cropFilter->SetInput(working_mask);
    cropFilter->SetLowerBoundaryCropSize(bounds);
    cropFilter->SetUpperBoundaryCropSize(bounds);
    cropFilter->Update();
    working_mask = cropFilter->GetOutput();
  }
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

  if (GetSeparateLungsFlag()) {
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

    // If already 2 labels, but a too big differences, remove the
    // smalest one (sometimes appends with the stomach
    if (initialNumberOfLabels >1) {
      if (GetRemoveSmallLabelBeforeSeparationFlag()) {
        DD(GetRemoveSmallLabelBeforeSeparationFlag());
        typedef itk::RelabelComponentImageFilter<MaskImageType, MaskImageType> RelabelFilterType;
        typename RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
        relabelFilter->SetInput(working_mask);
        relabelFilter->SetMinimumObjectSize(10);
        relabelFilter->Update();
        const std::vector<float> & a = relabelFilter->GetSizeOfObjectsInPhysicalUnits();
        std::vector<MaskImagePixelType> remove_label;
        for(unsigned int i=1; i<a.size(); i++) {
          if (a[i] < 0.5*a[0]) { // more than 0.5 difference
            remove_label.push_back(i+1); // label zero is BG
          }
        }
        working_mask = 
          clitk::RemoveLabels<MaskImageType>(working_mask, GetBackgroundValue(), remove_label);
        statisticsImageFilter->SetInput(working_mask);
        statisticsImageFilter->Update();
        initialNumberOfLabels = statisticsImageFilter->GetMaximum();
      }
    }
  
    // Decompose the first label
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
  }

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


bool is_orientation_superior(itk::SpatialOrientation::ValidCoordinateOrientationFlags orientation)
{
  itk::SpatialOrientation::CoordinateTerms sup = itk::SpatialOrientation::ITK_COORDINATE_Superior;
  bool primary = (orientation & 0x0000ff) == sup;
  bool secondary = ((orientation & 0x00ff00) >> 8) == sup;
  bool tertiary = ((orientation & 0xff0000) >> 16) == sup;
  return primary || secondary || tertiary;
}

//--------------------------------------------------------------------
template <class ImageType>
bool 
clitk::ExtractLungFilter<ImageType>::
SearchForTracheaSeed2(int numberOfSlices)
{
  if (m_Seeds.size() == 0) { // try to find seed (if not zero, it is given by user)    
    if (GetVerboseRegionGrowingFlag())
      std::cout << "SearchForTracheaSeed2(" << numberOfSlices << ", " << GetMaxElongation() << ")" << std::endl;
    
    typedef unsigned char MaskPixelType;
    typedef itk::Image<MaskPixelType, ImageType::ImageDimension> MaskImageType;
    typedef itk::Image<typename MaskImageType::PixelType, 2> MaskImageType2D;
    typedef itk::BinaryThresholdImageFilter<ImageType, MaskImageType> ThresholdFilterType;
    typedef itk::BinaryBallStructuringElement<MaskPixelType, MaskImageType::ImageDimension> KernelType;
    typedef itk::BinaryMorphologicalClosingImageFilter<MaskImageType, MaskImageType, KernelType> ClosingFilterType;
    typedef itk::BinaryMorphologicalOpeningImageFilter<MaskImageType, MaskImageType, KernelType> OpeningFilterType;
    typedef itk::ExtractImageFilter<MaskImageType, MaskImageType2D> SlicerFilterType;
    typedef itk::ConnectedComponentImageFilter<MaskImageType2D, MaskImageType2D> LabelFilterType;
    typedef itk::ShapeLabelObject<MaskPixelType, MaskImageType2D::ImageDimension> ShapeLabelType;
    typedef itk::LabelMap<ShapeLabelType> LabelImageType;
    typedef itk::LabelImageToShapeLabelMapFilter<MaskImageType2D, LabelImageType> ImageToLabelMapFilterType;
    typedef itk::LabelMapToLabelImageFilter<LabelImageType, MaskImageType2D> LabelMapToImageFilterType;
    typedef itk::ImageFileWriter<MaskImageType2D> FileWriterType;
    
    // threshold to isolate airawys and lungs
    typename ThresholdFilterType::Pointer threshold = ThresholdFilterType::New();
    threshold->SetLowerThreshold(-2000);
    threshold->SetUpperThreshold(GetSeedPreProcessingThreshold());
    threshold->SetInput(working_input);
    threshold->Update();
    
    KernelType kernel_closing, kernel_opening;
    
    // remove small noise
    typename OpeningFilterType::Pointer opening = OpeningFilterType::New();
    kernel_opening.SetRadius(1);
    opening->SetKernel(kernel_opening);
    opening->SetInput(threshold->GetOutput());
    opening->Update();
    
    typename SlicerFilterType::Pointer slicer = SlicerFilterType::New();
#if ITK_VERSION_MAJOR >= 4
    slicer->SetDirectionCollapseToIdentity();
#endif
    slicer->SetInput(opening->GetOutput());
    
    // label result
    typename LabelFilterType::Pointer label_filter = LabelFilterType::New();
    label_filter->SetInput(slicer->GetOutput());
    
    // extract shape information from labels
    typename ImageToLabelMapFilterType::Pointer label_to_map_filter = ImageToLabelMapFilterType::New();
    label_to_map_filter->SetInput(label_filter->GetOutput());
    
    typename LabelMapToImageFilterType::Pointer map_to_label_filter = LabelMapToImageFilterType::New();
    typename FileWriterType::Pointer writer = FileWriterType::New();
    
    typename ImageType::IndexType index;
    typename ImageType::RegionType region = working_input->GetLargestPossibleRegion();
    typename ImageType::SizeType size = region.GetSize();
    typename ImageType::SpacingType spacing = working_input->GetSpacing();
    typename ImageType::PointType origin = working_input->GetOrigin();

    int nslices = min(numberOfSlices, size[2]);
    int start = 0, increment = 1;
    itk::SpatialOrientationAdapter orientation;
    typename ImageType::DirectionType dir = working_input->GetDirection();
    if (!is_orientation_superior(orientation.FromDirectionCosines(dir))) {
      start = size[2]-1;
      increment = -1;
    }
    
    typename MaskImageType::PointType image_centre;
    image_centre[0] = size[0]/2;
    image_centre[1] = size[1]/2;
    image_centre[2] = 0;
  
    typedef InternalIndexType SeedType;
    SeedType trachea_centre, shape_centre, max_e_centre, prev_e_centre;
    typedef std::list<SeedType> PointListType;
    typedef std::list<PointListType> SequenceListType;
    PointListType* current_sequence = NULL;
    SequenceListType sequence_list;

    prev_e_centre.Fill(0);
    std::ostringstream file_name;
    index[0] = index[1] = 0;
    size[0] = size[1] = 512;
    size[2] = 0;
    while (nslices--) {
      index[2] = start;
      start += increment;
      
      region.SetIndex(index);
      region.SetSize(size);
      slicer->SetExtractionRegion(region);
      slicer->Update();
      label_filter->SetInput(slicer->GetOutput());
      label_filter->Update();

      label_to_map_filter->SetInput(label_filter->GetOutput());
      label_to_map_filter->Update();
      typename LabelImageType::Pointer label_map = label_to_map_filter->GetOutput();

      if (GetWriteStepFlag()) {
        map_to_label_filter->SetInput(label_map);
        writer->SetInput(map_to_label_filter->GetOutput());
        file_name.str("");
        file_name << "labels_";
        file_name.width(3);
        file_name.fill('0');
        file_name << index[2] << ".mhd";
        writer->SetFileName(file_name.str().c_str());
        writer->Update();
      }

      typename ShapeLabelType::Pointer shape, max_e_shape;
      double max_elongation = GetMaxElongation();
      double max_size = size[0]*size[1]/128;
      double max_e = 0;
      int nshapes = 0;
      unsigned int nlables = label_map->GetNumberOfLabelObjects();
      for (unsigned int j = 0; j < nlables; j++) {
        shape = label_map->GetNthLabelObject(j);
        if (shape->Size() > 150 && shape->Size() <= max_size) {
#if ITK_VERSION_MAJOR < 4
          double e = 1 - 1/shape->GetBinaryElongation();
#else
          double e = 1 - 1/shape->GetElongation();
#endif
          //double area = 1 - r->Area() ;
          if (e < max_elongation) {
            nshapes++;
            shape_centre[0] = (shape->GetCentroid()[0] - origin[0])/spacing[0];
            shape_centre[1] = (shape->GetCentroid()[1] - origin[1])/spacing[1];
            shape_centre[2] = index[2];
            //double d = 1 - (shape_centre - image_centre).Magnitude()/max_dist;
            double dx = shape_centre[0] - image_centre[0];
            double d = 1 - dx*2/size[0];
            e = e + d;
            if (e > max_e)
            {
              max_e = e;
              max_e_shape = shape;
              max_e_centre = shape_centre;
            }
          }
        }
      }
      
      if (nshapes > 0)
      {
        itk::Point<typename SeedType::IndexValueType, ImageType::ImageDimension> p1, p2;
        p1[0] = max_e_centre[0];
        p1[1] = max_e_centre[1];
        p1[2] = max_e_centre[2];
        
        p2[0] = prev_e_centre[0];
        p2[1] = prev_e_centre[1];
        p2[2] = prev_e_centre[2];
        
        double mag = (p2 - p1).GetNorm();
        if (GetVerboseRegionGrowingFlag()) {
          cout.precision(3);
          cout << index[2] << ": ";
          cout << "region(" << max_e_centre[0] << ", " << max_e_centre[1] << ", " << max_e_centre[2] << "); ";
          cout << "prev_region(" << prev_e_centre[0] << ", " << prev_e_centre[1] << ", " << prev_e_centre[2] << "); ";
          cout << "mag(" << mag << "); " << endl;
        }
        
        if (mag > 5)
        {
          PointListType point_list;
          point_list.push_back(max_e_centre);
          sequence_list.push_back(point_list);
          current_sequence = &sequence_list.back();
        }
        else if (current_sequence)
          current_sequence->push_back(max_e_centre);
        
        prev_e_centre= max_e_centre;
      }
      else {
        if (GetVerboseRegionGrowingFlag()) {
          cout << "No shapes found at slice " << index[2] << std::endl;
        }
      }
    }
    
    size_t longest = 0;
    for (typename SequenceListType::iterator s = sequence_list.begin(); s != sequence_list.end(); s++)
    {
      if (s->size() > longest)
      {
        longest = s->size();
        trachea_centre = s->front();
      }
    }
    
    if (longest > 0) {
      if (GetVerboseRegionGrowingFlag()) 
        std::cout << "seed at: " << trachea_centre << std::endl;
      m_Seeds.push_back(trachea_centre);
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
  f->SetMaximumUpperThreshold(-300); // MAYBE TO CHANGE ???
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

  bool has_seed;
  bool stop = false;
  double volume = 0.0;
  int skip = GetNumberOfSlicesToSkipBeforeSearchingSeed();
  while (!stop) {
    stop = true;
    if (GetTracheaSeedAlgorithm() == 0)
      has_seed = SearchForTracheaSeed(skip);
    else
      has_seed = SearchForTracheaSeed2(GetNumSlices());
    
    if (has_seed) {
      TracheaRegionGrowing();
      volume = ComputeTracheaVolume()/1000; // assume mm3, so divide by 1000 to get cc
      if (GetWriteStepFlag()) {
        writeImage<MaskImageType>(trachea, "step-trachea-"+toString(skip)+".mhd");
      }
      if (GetTracheaVolumeMustBeCheckedFlag()) {
        if ((volume > 10) && (volume < 65 )) { // depend on image size ...
          // Typical volume 22.59 cm 3 (± 7.69 cm 3 ) [ Leader 2004 ]
          if (GetVerboseStepFlag())
          {
            std::cout << "\t Found trachea with volume " << volume << " cc." << std::endl;
          }
        }
        else 
          if (GetTracheaSeedAlgorithm() == 0) {
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
        if (skip > 0.5 * working_input->GetLargestPossibleRegion().GetSize()[2]) {
          // we want to skip more than a half of the image, it is probably a bug
          std::cerr << "2 : Number of slices to skip to find trachea too high = " << skip << std::endl;
          stop = true;
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
