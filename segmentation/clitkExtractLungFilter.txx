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
  OpenCloseOff();
  SetOpenCloseRadius(1);
  
  // Step 6
  FillHolesOn();
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
template<class ArgsInfoType>
void 
clitk::ExtractLungFilter<ImageType>::
SetArgsInfo(ArgsInfoType mArgsInfo)
{
  SetVerboseOption_GGO(mArgsInfo);
  SetVerboseStep_GGO(mArgsInfo);
  SetWriteStep_GGO(mArgsInfo);
  SetVerboseWarningOff_GGO(mArgsInfo);

  SetAFDBFilename_GGO(mArgsInfo);
  SetOutputLungFilename_GGO(mArgsInfo);
  SetOutputTracheaFilename_GGO(mArgsInfo);

  SetUpperThreshold_GGO(mArgsInfo);
  SetLowerThreshold_GGO(mArgsInfo);
  SetNumberOfSlicesToSkipBeforeSearchingSeed_GGO(mArgsInfo);
  SetLabelizeParameters1_GGO(mArgsInfo);
  if (!mArgsInfo.remove1_given) {
    GetLabelizeParameters1()->AddLabelToRemove(2); 
    if (GetVerboseOption()) VerboseOption("remove1", 2);
  }

  SetUpperThresholdForTrachea_GGO(mArgsInfo);
  SetMultiplierForTrachea_GGO(mArgsInfo);
  SetThresholdStepSizeForTrachea_GGO(mArgsInfo);
  AddSeed_GGO(mArgsInfo);

  SetMinimalComponentSize_GGO(mArgsInfo);
  
  SetNumberOfHistogramBins_GGO(mArgsInfo);
  SetLabelizeParameters2_GGO(mArgsInfo);

  SetRadiusForTrachea_GGO(mArgsInfo);
  SetLabelizeParameters3_GGO(mArgsInfo);
  
  SetOpenCloseRadius_GGO(mArgsInfo);
  SetOpenClose_GGO(mArgsInfo);
  
  SetFillHoles_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLungFilter<ImageType>::
GenerateOutputInformation() 
{ 
  Superclass::GenerateOutputInformation();
  
  // Read DB
  LoadAFDB();

  // Get input pointers
  input   = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
  patient = GetAFDB()->template GetImage <MaskImageType>("patient");  

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Crop input like patient image (must have the same spacing)
  StartNewStep("Crop input image to 'patient' extends");
  typedef clitk::CropLikeImageFilter<ImageType> CropImageFilter;
  typename CropImageFilter::Pointer cropFilter = CropImageFilter::New();
  cropFilter->SetInput(input);
  cropFilter->SetCropLikeImage(patient);
  cropFilter->Update();
  working_input = cropFilter->GetOutput();
  DD(working_input->GetLargestPossibleRegion());
  StopCurrentStep<ImageType>(working_input);
 
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Set background to initial image");
  working_input = SetBackground<ImageType, MaskImageType>
    (working_input, patient, GetPatientMaskBackgroundValue(), -1000);
  StopCurrentStep<ImageType>(working_input);

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
  typedef itk::BinaryThresholdImageFilter<ImageType, InternalImageType> InputBinarizeFilterType; 
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
  working_input = SetBackground<ImageType, InternalImageType>
    (working_input, air, this->GetForegroundValue(), this->GetBackgroundValue());

  // End
  StopCurrentStep<ImageType>(working_input);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Search for the trachea");
  SearchForTrachea();

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Extract the lung with Otsu filter");
  // Automated Otsu thresholding and relabeling
  typedef itk::OtsuThresholdImageFilter<ImageType,InternalImageType> OtsuThresholdImageFilterType;
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
    static const unsigned int Dim = ImageType::ImageDimension;
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
  typedef clitk::AutoCropFilter<InternalImageType> AutoCropFilterType;
  typename AutoCropFilterType::Pointer autocropFilter = AutoCropFilterType::New();
  if (m_Seeds.size() != 0) { // if ==0 ->no trachea found
    StartNewStep("Cropping trachea");
    autocropFilter->SetInput(trachea_tmp);
    autocropFilter->Update(); // Needed
    typedef itk::CastImageFilter<InternalImageType, MaskImageType> CastImageFilterType;
    typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
    caster->SetInput(autocropFilter->GetOutput());
    caster->Update();   
    trachea = caster->GetOutput();
    StopCurrentStep<MaskImageType>(trachea);  
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Cropping lung");
  typename AutoCropFilterType::Pointer autocropFilter2 = AutoCropFilterType::New(); // Needed to reset pipeline
  autocropFilter2->SetInput(working_image);
  autocropFilter2->Update();   
  working_image = autocropFilter2->GetOutput();
  DD(working_image->GetLargestPossibleRegion());
  StopCurrentStep<InternalImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Final OpenClose
  if (GetOpenClose()) {
    StartNewStep("Open/Close"); 

    // Structuring element
    typedef itk::BinaryBallStructuringElement<InternalPixelType, ImageDimension> KernelType;
    KernelType structuringElement;
    structuringElement.SetRadius(GetOpenCloseRadius());
    structuringElement.CreateStructuringElement();
	
    // Open
    typedef itk::BinaryMorphologicalOpeningImageFilter<InternalImageType, InternalImageType, KernelType> OpenFilterType;
    typename OpenFilterType::Pointer openFilter = OpenFilterType::New();
    openFilter->SetInput(working_image);
    openFilter->SetBackgroundValue(GetBackgroundValue());
    openFilter->SetForegroundValue(GetForegroundValue());
    openFilter->SetKernel(structuringElement);
	
    // Close
    typedef itk::BinaryMorphologicalClosingImageFilter<InternalImageType, InternalImageType, KernelType> CloseFilterType;
    typename CloseFilterType::Pointer closeFilter = CloseFilterType::New();
    closeFilter->SetInput(openFilter->GetOutput());
    closeFilter->SetSafeBorder(true);
    closeFilter->SetForegroundValue(GetForegroundValue());
    closeFilter->SetKernel(structuringElement);
    closeFilter->Update();
    working_image = closeFilter->GetOutput();
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Fill Lungs
  if (GetFillHoles()) {
    StartNewStep("Fill Holes");
    typedef clitk::FillMaskFilter<InternalImageType> FillMaskFilterType;
    typename FillMaskFilterType::Pointer fillMaskFilter = FillMaskFilterType::New();
    fillMaskFilter->SetInput(working_image);
    fillMaskFilter->AddDirection(2);
    //fillMaskFilter->AddDirection(1);
    fillMaskFilter->Update();   
    working_image = fillMaskFilter->GetOutput();
    StopCurrentStep<InternalImageType>(working_image);
  }

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
  static const unsigned int Dim = ImageType::ImageDimension;
  if (initialNumberOfLabels<2) {
    DD(initialNumberOfLabels);
    // Structuring element radius
    typename ImageType::SizeType radius;
    for (unsigned int i=0;i<Dim;i++) radius[i]=1;
    typedef clitk::DecomposeAndReconstructImageFilter<InternalImageType,InternalImageType> DecomposeAndReconstructFilterType;
    typename DecomposeAndReconstructFilterType::Pointer decomposeAndReconstructFilter=DecomposeAndReconstructFilterType::New();
    decomposeAndReconstructFilter->SetInput(working_image);
    decomposeAndReconstructFilter->SetVerbose(true);
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

  // Retain labels ('1' is largset lung, so right. '2' is left)
  typedef itk::ThresholdImageFilter<InternalImageType> ThresholdImageFilterType;
  typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
  thresholdFilter->SetInput(working_image);
  thresholdFilter->ThresholdAbove(2);
  thresholdFilter->SetOutsideValue(this->GetBackgroundValue());
  thresholdFilter->Update();
  working_image = thresholdFilter->GetOutput();
  StopCurrentStep<InternalImageType> (working_image);
  
  // Final Cast 
  StartNewStep("Cast the lung mask"); 
  typedef itk::CastImageFilter<InternalImageType, MaskImageType> CastImageFilterType;
  typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
  caster->SetInput(working_image);
  caster->Update();
  output = caster->GetOutput();

  // Update output info
  this->GetOutput(0)->SetRegions(output->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLungFilter<ImageType>::
GenerateData() 
{
  // Set the output
  this->GraftOutput(output); // not SetNthOutput
  // Store image filenames into AFDB 
  GetAFDB()->SetImageFilename("lungs", this->GetOutputLungFilename());  
  GetAFDB()->SetImageFilename("trachea", this->GetOutputTracheaFilename());  
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
	if (GetVerboseStep()) {
	  std::cout << "\t No seed found this time. I skip some slices and restart." << std::endl;
	}
        if (skip > 0.5 * working_input->GetLargestPossibleRegion().GetSize()[2]) {
          // we want to skip more than a half of the image, it is probably a bug
          std::cerr << "Number of slices to skip to find trachea to high = " << skip << std::endl;
          stop = true;
        }
        skip += 5;
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
  typedef clitk::ExplosionControlledThresholdConnectedImageFilter<ImageType, InternalImageType> ImageFilterType;
  typename ImageFilterType::Pointer f= ImageFilterType::New();
  f->SetInput(working_input);
  f->SetVerbose(false);
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
  f->VerboseOn();
  for(unsigned int i=0; i<m_Seeds.size();i++) {
    f->AddSeed(m_Seeds[i]);
    // DD(m_Seeds[i]);
  }  
  f->Update();

  writeImage<InternalImageType>(f->GetOutput(), "trg.mhd");

  // take first (main) connected component
  trachea_tmp = Labelize<InternalImageType>(f->GetOutput(), 
					    GetBackgroundValue(), 
					    true, 
					    GetMinimalComponentSize());
  trachea_tmp = KeepLabels<InternalImageType>(trachea_tmp, 
					      GetBackgroundValue(), 
					      GetForegroundValue(), 
					      1, 1, false);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
double 
clitk::ExtractLungFilter<ImageType>::
ComputeTracheaVolume()
{
  typedef itk::ImageRegionConstIterator<InternalImageType> IteratorType;
  IteratorType iter(trachea_tmp, trachea_tmp->GetLargestPossibleRegion());
  iter.GoToBegin();
  double volume = 0.0;
  while (!iter.IsAtEnd()) {
    if (iter.Get() == this->GetForegroundValue()) volume++;
    ++iter;
  }
  
  double voxelsize = trachea_tmp->GetSpacing()[0]*trachea_tmp->GetSpacing()[1]*trachea_tmp->GetSpacing()[2];
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
      if ((volume > 10) && (volume < 55 )) { // it is ok
        // Typical volume 22.59 cm 3 (± 7.69 cm 3 ) [ Leader 2004 ]
	if (GetVerboseStep()) {
	  std::cout << "\t Found trachea with volume " << volume << " cc." << std::endl;
	}
        stop = true; 
      }
      else {
	if (GetVerboseStep()) {
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
  }

  if (volume != 0.0) {
    // Set output
    StopCurrentStep<InternalImageType>(trachea_tmp);
  }
  else { // Trachea not found
    this->SetWarning("* WARNING * No seed found for trachea.");
    StopCurrentStep();
  }
}
//--------------------------------------------------------------------

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
