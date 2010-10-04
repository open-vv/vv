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
#include "clitkExtractSliceFilter.h"

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
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
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
  
  SetAFDBFilename_GGO(mArgsInfo);
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
    clitkExceptionMacro("the 'input' and 'patient' masks must have the same size & spacing.");
  }
  
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Set background to initial image");
  working_input = SetBackground<ImageType, MaskImageType>
    (input, patient, GetPatientMaskBackgroundValue(), -1000);
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

  // Try to extract bifurcation in the trachea (bronchi)
  if (m_Seeds.size() != 0) { // if ==0 ->no trachea found

    if (GetFindBronchialBifurcations()) {
      StartNewStep("Find bronchial bifurcations");
      // Step 1 : extract skeleton
      typedef itk::BinaryThinningImageFilter3D<MaskImageType, MaskImageType> ThinningFilterType;
      typename ThinningFilterType::Pointer thinningFilter = ThinningFilterType::New();
      thinningFilter->SetInput(trachea);
      thinningFilter->Update();
      typename MaskImageType::Pointer skeleton = thinningFilter->GetOutput();

      // Step 2.1 : find first point for tracking
      typedef itk::ImageRegionConstIteratorWithIndex<MaskImageType> IteratorType;
      IteratorType it(skeleton, skeleton->GetLargestPossibleRegion());
      it.GoToReverseBegin();
      while ((!it.IsAtEnd()) && (it.Get() == GetBackgroundValue())) { 
        --it;
      }
      if (it.IsAtEnd()) {
        clitkExceptionMacro("first point in the trachea skeleton not found.");
      }
      typename MaskImageType::IndexType index = it.GetIndex();
    
      // Step 2.2 : initialize neighborhooditerator
      typedef itk::NeighborhoodIterator<MaskImageType> NeighborhoodIteratorType;
      typename NeighborhoodIteratorType::SizeType radius;
      radius.Fill(1);
      NeighborhoodIteratorType nit(radius, skeleton, skeleton->GetLargestPossibleRegion());
    
      // Find first label number (must be different from BG and FG)
      typename MaskImageType::PixelType label = GetForegroundValue()+1;
      while ((label == GetBackgroundValue()) || (label == GetForegroundValue())) { label++; }

      // Track from the first point
      std::vector<BifurcationType> listOfBifurcations;
      m_SkeletonTree.set_head(index);
      TrackFromThisIndex(listOfBifurcations, skeleton, index, label, m_SkeletonTree.begin());
      DD("end track");
      DD(listOfBifurcations.size());
      DD(m_SkeletonTree.size());
      
      for(unsigned int i=0; i<listOfBifurcations.size(); i++) {
        skeleton->TransformIndexToPhysicalPoint(listOfBifurcations[i].index, 
                                                listOfBifurcations[i].point);
      }

      // Search for the first slice that separate the bronchus (carena)
      typedef clitk::ExtractSliceFilter<MaskImageType> ExtractSliceFilterType;
      typename ExtractSliceFilterType::Pointer extractSliceFilter = ExtractSliceFilterType::New();
      extractSliceFilter->SetInput(trachea);
      extractSliceFilter->SetDirection(2);
      extractSliceFilter->Update();
      typedef typename ExtractSliceFilterType::SliceType SliceType;
      std::vector<typename SliceType::Pointer> mInputSlices;
      extractSliceFilter->GetOutputSlices(mInputSlices);
      
      bool stop = false;
      DD(listOfBifurcations[0].index);
      DD(listOfBifurcations[1].index);
      int slice_index = listOfBifurcations[0].index[2]; // first slice from carena in skeleton
      int i=0;
      TreeIterator firstIter = m_SkeletonTree.child(listOfBifurcations[1].treeIter, 0);
      TreeIterator secondIter = m_SkeletonTree.child(listOfBifurcations[1].treeIter, 1);
      typename SliceType::IndexType in1;
      typename SliceType::IndexType in2;
      while (!stop) {
        DD(slice_index);

	//  Labelize the current slice
        typename SliceType::Pointer temp = Labelize<SliceType>(mInputSlices[slice_index],
                                                               GetBackgroundValue(), 
                                                               true, 
                                                               GetMinimalComponentSize());
	// Check the value of the two skeleton points;
        in1[0] = (*firstIter)[0];
        in1[1] = (*firstIter)[1];
	typename SliceType::PixelType v1 = temp->GetPixel(in1);
	DD(in1);
	DD((int)v1);
        in2[0] = (*secondIter)[0];
        in2[1] = (*secondIter)[1];
	typename SliceType::PixelType v2 = temp->GetPixel(in2);
	DD(in2);
	DD((int)v2);

	// TODO IF NOT FOUND ????

        if (v1 != v2) {
	  stop = true;
	}
	else {
	  i++;
	  --slice_index;
	  ++firstIter;
	  ++secondIter;
	}
      }
      MaskImageIndexType carena_index;
      carena_index[0] = lrint(in2[0] + in1[0])/2.0;
      carena_index[1] = lrint(in2[1] + in1[1])/2.0;
      carena_index[2] = slice_index;
      MaskImagePointType carena_position;
      DD(carena_index);
      skeleton->TransformIndexToPhysicalPoint(carena_index,
					      carena_position);
      DD(carena_position);

      // Set and save Carina position      
      StartNewStep("Save carina position");
      // Try to load the DB
      try {
        LoadAFDB();
      } catch (clitk::ExceptionObject e) {
        // Do nothing if not found, it will be used anyway to write the result
      }
      GetAFDB()->SetPoint3D("Carena", carena_position);
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
GenerateData() 
{
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
                   MaskImagePixelType label, 
		   TreeIterator currentNode) 
{
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
    nit.SetCenterPixel(label);
    listOfTrackedPoint.clear();
    for(unsigned int i=0; i<nit.Size(); i++) {
      if (i != nit.GetCenterNeighborhoodIndex ()) { // Do not observe the current point
        if (nit.GetPixel(i) == GetForegroundValue()) { // if this is foreground, we continue the tracking
          listOfTrackedPoint.push_back(nit.GetIndex(i));
        }
      }
    }
    if (listOfTrackedPoint.size() == 1) {
      // Add this point to the current path
      currentNode = m_SkeletonTree.append_child(currentNode, listOfTrackedPoint[0]);
      index = listOfTrackedPoint[0];
    }
    else {
      if (listOfTrackedPoint.size() == 2) {
        // m_SkeletonTree->Add(listOfTrackedPoint[0], index); // the parent is 'index'
        // m_SkeletonTree->Add(listOfTrackedPoint[1], index); // the parent is 'index'
        BifurcationType bif(index, label, label+1, label+2);
	bif.treeIter = currentNode;
        listOfBifurcations.push_back(bif);
	TreeIterator firstNode = m_SkeletonTree.append_child(currentNode, listOfTrackedPoint[0]);
        TreeIterator secondNode = m_SkeletonTree.append_child(currentNode, listOfTrackedPoint[1]);
        TrackFromThisIndex(listOfBifurcations, skeleton, listOfTrackedPoint[0], label+1, firstNode);
        TrackFromThisIndex(listOfBifurcations, skeleton, listOfTrackedPoint[1], label+2, secondNode);
      }
      else {
        if (listOfTrackedPoint.size() > 2) {
          clitkExceptionMacro("error while tracking trachea bifurcation. Too much bifurcation points ... ?");
        }
        // Else this it the end of the tracking
      }
      stop = true;
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType, class MaskImageType>
bool 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
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
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
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
    // DD(m_Seeds[i]);
  }  
  f->Update();

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
template <class ImageType, class MaskImageType>
double 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
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
template <class ImageType, class MaskImageType>
void 
clitk::ExtractLungFilter<ImageType, MaskImageType>::
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
