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
#include "itkBinaryThinningImageFilter3D.h"
#include "itkImageIteratorWithIndex.h"


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
clitk::ExtractLungFilter<ImageType, MaskImageType>::
ExtractLungFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<ImageType, MaskImageType>()
{
  SetNumberOfSteps(10);
  m_MaxSeedNumber = 500;

  // Default global options
  this->SetNumberOfRequiredInputs(2);
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
  
  // Step 5 : find bronchial bifurcations
  FindBronchialBifurcationsOn();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
SetInput(const ImageType * image) 
{
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
SetInputPatientMask(MaskImageType * image, MaskImagePixelType bg ) 
{
  this->SetNthInput(1, const_cast<MaskImageType *>(image));
  SetPatientMaskBackgroundValue(bg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
AddSeed(InternalIndexType s) 
{ 
  m_Seeds.push_back(s);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
template<class ArgsInfoType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
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

  SetMinimalComponentSize_GGO(mArgsInfo);
  
  SetNumberOfHistogramBins_GGO(mArgsInfo);
  SetLabelizeParameters2_GGO(mArgsInfo);

  SetRadiusForTrachea_GGO(mArgsInfo);
  SetLabelizeParameters3_GGO(mArgsInfo);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
GenerateOutputInformation() 
{ 
  Superclass::GenerateOutputInformation();
  //this->GetOutput(0)->SetRequestedRegion(this->GetOutput(0)->GetLargestPossibleRegion());

  // Get input pointers
  patient = dynamic_cast<const MaskImageType*>(itk::ProcessObject::GetInput(1));
  input   = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));

  // Check image
  if (!HaveSameSizeAndSpacing<ImageType, MaskImageType>(input, patient)) {
    this->SetLastError("* ERROR * the images (input and patient mask) must have the same size & spacing");
    return;
  }
  
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStepOrStop("Set background to initial image");
  working_input = SetBackground<ImageType, MaskImageType>
    (input, patient, GetPatientMaskBackgroundValue(), -1000);
  StopCurrentStep<ImageType>(working_input);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStepOrStop("Remove Air");
  // Check threshold
  if (m_UseLowerThreshold) {
    if (m_LowerThreshold > m_UpperThreshold) {
      this->SetLastError("ERROR: lower threshold cannot be greater than upper threshold.");
      return;
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
  StartNewStepOrStop("Find the trachea");
  if (m_Seeds.size() == 0) { // try to find seed
    // Search seed (parameters = UpperThresholdForTrachea)
    static const unsigned int Dim = ImageType::ImageDimension;
    typename InternalImageType::RegionType sliceRegion = working_input->GetLargestPossibleRegion();
    typename InternalImageType::SizeType sliceRegionSize = sliceRegion.GetSize();
    typename InternalImageType::IndexType sliceRegionIndex = sliceRegion.GetIndex();
    sliceRegionIndex[Dim-1]=sliceRegionSize[Dim-1]-5;
    sliceRegionSize[Dim-1]=5;
    sliceRegion.SetSize(sliceRegionSize);
    sliceRegion.SetIndex(sliceRegionIndex);
    typedef  itk::ImageRegionConstIterator<ImageType> IteratorType;
    IteratorType it(working_input, sliceRegion);
    it.GoToBegin();
    while (!it.IsAtEnd()) {
      if(it.Get() < GetUpperThresholdForTrachea() ) {
        AddSeed(it.GetIndex());
      }
      ++it;
    }
  }
  
  if (m_Seeds.size() != 0) {
    // Explosion controlled region growing
    typedef clitk::ExplosionControlledThresholdConnectedImageFilter<ImageType, InternalImageType> ImageFilterType;
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
      f->AddSeed(m_Seeds[i]);
    }  
    f->Update();
    trachea_tmp = f->GetOutput();
    // Set output
    StopCurrentStep<InternalImageType>(trachea_tmp);
  }
  else { // Trachea not found
    this->SetWarning("* WARNING * No seed found for trachea.");
    StopCurrentStep();
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStepOrStop("Extract the lung with Otsu filter");
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
  StartNewStepOrStop("Select labels");
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
    StartNewStepOrStop("Remove the trachea");
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
  typedef clitk::AutoCropFilter<InternalImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  if (m_Seeds.size() != 0) { // if ==0 ->no trachea found
    StartNewStepOrStop("Croping trachea");
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
  StartNewStepOrStop("Croping lung");
  typename CropFilterType::Pointer cropFilter2 = CropFilterType::New(); // Needed to reset pipeline
  cropFilter2->SetInput(working_image);
  cropFilter2->Update();   
  working_image = cropFilter2->GetOutput();
  StopCurrentStep<InternalImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStepOrStop("Separate Left/Right lungs");
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
    // Structuring element radius
    typename ImageType::SizeType radius;
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
  
  // Final Cast 
  StartNewStepOrStop("Final cast"); 
  typedef itk::CastImageFilter<InternalImageType, MaskImageType> CastImageFilterType;
  typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
  caster->SetInput(working_image);
  caster->Update();
  output = caster->GetOutput();

  // Update output info
  this->GetOutput(0)->SetRegions(output->GetLargestPossibleRegion());

  // Try to extract bifurcation in the trachea (bronchi)
  // STILL EXPERIMENTAL
  if (GetFindBronchialBifurcations()) {
    StartNewStepOrStop("Find bronchial bifurcations");
    // Step 1 : extract skeleton
    // Define the thinning filter
    typedef itk::BinaryThinningImageFilter3D<MaskImageType, MaskImageType> ThinningFilterType;
    typename ThinningFilterType::Pointer thinningFilter = ThinningFilterType::New();
    thinningFilter->SetInput(trachea);
    thinningFilter->Update();
    typename MaskImageType::Pointer skeleton = thinningFilter->GetOutput();
    writeImage<MaskImageType>(skeleton, "skeleton.mhd");

    // Step 2 : tracking
    DD("tracking");
    
    // Step 2.1 : find first point for tracking
    typedef itk::ImageRegionConstIteratorWithIndex<MaskImageType> IteratorType;
    IteratorType it(skeleton, skeleton->GetLargestPossibleRegion());
    it.GoToReverseBegin();
    while ((!it.IsAtEnd()) && (it.Get() == GetBackgroundValue())) { 
      --it;
    }
    if (it.IsAtEnd()) {
      this->SetLastError("ERROR: first point in the skeleton not found ! Abort");
      return;
    }
    DD(skeleton->GetLargestPossibleRegion().GetIndex());
    typename MaskImageType::IndexType index = it.GetIndex();
    DD(index);
    
    // Step 2.2 : initialize neighborhooditerator
    typedef itk::NeighborhoodIterator<MaskImageType> NeighborhoodIteratorType;
    typename NeighborhoodIteratorType::SizeType radius;
    radius.Fill(1);
    NeighborhoodIteratorType nit(radius, skeleton, skeleton->GetLargestPossibleRegion());
    DD(nit.GetSize());
    DD(nit.Size());
    
    // Find first label number (must be different from BG and FG)
    typename MaskImageType::PixelType label = GetForegroundValue()+1;
    while ((label == GetBackgroundValue()) || (label == GetForegroundValue())) { label++; }
    DD(label);

    // Track from the first point
    std::vector<BifurcationType> listOfBifurcations;
    TrackFromThisIndex(listOfBifurcations, skeleton, index, label);
    DD("end track");
    DD(listOfBifurcations.size());
    writeImage<MaskImageType>(skeleton, "skeleton2.mhd");

    for(unsigned int i=0; i<listOfBifurcations.size(); i++) {
      typename MaskImageType::PointType p;
      skeleton->TransformIndexToPhysicalPoint(listOfBifurcations[i].index, p);
      DD(p);
    }

  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
GenerateData() {
  // Do not put some "startnewstep" here, because the object if
  // modified and the filter's pipeline it do two times. But it is
  // required to quit if MustStop was set before.
  if (GetMustStop()) return;
  
  // If everything goes well, set the output
  this->GraftOutput(output); // not SetNthOutput
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
TrackFromThisIndex(std::vector<BifurcationType> & listOfBifurcations, 
                   MaskImagePointer skeleton, 
                   MaskImageIndexType index,
                   MaskImagePixelType label) {
  DD("TrackFromThisIndex");
  DD(index);
  DD((int)label);
  // Create NeighborhoodIterator 
  typedef itk::NeighborhoodIterator<MaskImageType> NeighborhoodIteratorType;
  typename NeighborhoodIteratorType::SizeType radius;
  radius.Fill(1);
  NeighborhoodIteratorType nit(radius, skeleton, skeleton->GetLargestPossibleRegion());
      
  // Track
  std::vector<typename NeighborhoodIteratorType::IndexType> listOfTrackedPoint;
  bool stop = false;
  while (!stop) {
    nit.SetLocation(index);
    // DD((int)nit.GetCenterPixel());
    nit.SetCenterPixel(label);
    listOfTrackedPoint.clear();
    for(unsigned int i=0; i<nit.Size(); i++) {
      if (i != nit.GetCenterNeighborhoodIndex ()) { // Do not observe the current point
        //          DD(nit.GetIndex(i));
        if (nit.GetPixel(i) == GetForegroundValue()) { // if this is foreground, we continue the tracking
          // DD(nit.GetIndex(i));
          listOfTrackedPoint.push_back(nit.GetIndex(i));
        }
      }
    }
    // DD(listOfTrackedPoint.size());
    if (listOfTrackedPoint.size() == 1) {
      index = listOfTrackedPoint[0];
    }
    else {
      if (listOfTrackedPoint.size() == 2) {
        BifurcationType bif(index, label, label+1, label+2);
        listOfBifurcations.push_back(bif);
        TrackFromThisIndex(listOfBifurcations, skeleton, listOfTrackedPoint[0], label+1);
        TrackFromThisIndex(listOfBifurcations, skeleton, listOfTrackedPoint[1], label+2);
      }
      else {
        if (listOfTrackedPoint.size() > 2) {
          std::cerr << "too much bifurcation points ... ?" << std::endl;
          exit(0);
        }
        // Else this it the end of the tracking
      }
      stop = true;
    }
  }
}
//--------------------------------------------------------------------

  
#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
