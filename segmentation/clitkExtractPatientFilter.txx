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

#ifndef CLITKEXTRACTPATIENTFILTER_TXX
#define CLITKEXTRACTPATIENTFILTER_TXX

// clitk
#include "clitkImageCommon.h"
#include "clitkSetBackgroundImageFilter.h"
#include "clitkDecomposeAndReconstructImageFilter.h"
#include "clitkAutoCropFilter.h"
#include "clitkMemoryUsage.h"
#include "clitkSegmentationUtils.h"

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkCastImageFilter.h"
#include "itkConstantPadImageFilter.h"

//--------------------------------------------------------------------
template <class TInputImageType>
clitk::ExtractPatientFilter<TInputImageType>::
ExtractPatientFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<TInputImageType, MaskImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0); // Must be zero
  SetForegroundValue(1);
  SetPrimaryOpeningRadius(0);

  // Step 1: Threshold + CC + sort (Find low density areas)
  SetUpperThreshold(-300);
  SetLowerThreshold(-1000);
  UseLowerThresholdOff();

  // Step 2: DecomposeAndReconstructImageFilter (optional)
  DecomposeAndReconstructDuringFirstStepOff();
  InternalImageSizeType r;
  r.Fill(1);
  SetRadius1(r);
  SetMaximumNumberOfLabels1(2);
  SetNumberOfNewLabels1(1);

  // Step 3: Remove the air (largest area).

  // Step 4: 2nd DecomposeAndReconstructImageFilter
  DecomposeAndReconstructDuringSecondStepOff();
  SetRadius2(r);
  SetMaximumNumberOfLabels2(2);
  SetNumberOfNewLabels2(1);

  // Step 5: Only keep label corresponding (Keep patient's labels)
  SetFirstKeep(1);
  SetLastKeep(1);

  // Step 4: OpenClose (option)
  FinalOpenCloseOff();
  AutoCropOn();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType>
void
clitk::ExtractPatientFilter<TInputImageType>::
SetInput(const TInputImageType * image)
{
  this->SetNthInput(0, const_cast<TInputImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType>
void
clitk::ExtractPatientFilter<TInputImageType>::
GenerateOutputInformation() {

  clitk::PrintMemory(GetVerboseMemoryFlag(), "Initial memory"); // OK

  Superclass::GenerateOutputInformation();
  input = dynamic_cast<const TInputImageType*>(itk::ProcessObject::GetInput(0));

  // MaskImagePointer outputImage = this->GetOutput(0);
//   outputImage->SetRegions(input->GetLargestPossibleRegion());

  // Get input pointers
  static const unsigned int Dim = InputImageType::ImageDimension;
  //input = dynamic_cast<const TInputImageType*>(itk::ProcessObject::GetInput(0));

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Step 1:
  StartNewStep("Find low densities areas");

  // Pad images with air to prevent patient touching the image border
  typedef itk::ConstantPadImageFilter<InputImageType, InputImageType> PadFilterType;
  typename PadFilterType::Pointer padFilter = PadFilterType::New();
  padFilter->SetInput(input);
  padFilter->SetConstant(GetUpperThreshold() - 1);
  typename InputImageType::SizeType bounds;
  for (unsigned i = 0; i < Dim - 1; ++i)
    bounds[i] = 1;
  bounds[Dim - 1] = 0;
  padFilter->SetPadLowerBound(bounds);
  padFilter->SetPadUpperBound(bounds);
  padFilter->Update();
  
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> BinarizeFilterType;
  typename BinarizeFilterType::Pointer binarizeFilter=BinarizeFilterType::New();
  binarizeFilter->SetInput(padFilter->GetOutput());
  if (m_UseLowerThreshold) binarizeFilter->SetLowerThreshold(GetLowerThreshold());
  binarizeFilter->SetUpperThreshold(GetUpperThreshold());
  binarizeFilter ->SetInsideValue(this->GetForegroundValue());
  binarizeFilter ->SetOutsideValue(this->GetBackgroundValue());
  padFilter->GetOutput()->ReleaseData();
  working_image = binarizeFilter->GetOutput();

  typedef itk::BinaryBallStructuringElement<InternalPixelType,Dim> KernelType;
  unsigned int radius = this->GetPrimaryOpeningRadius();
  if (radius > 0)
  {
    if (this->GetVerboseOptionFlag()) std::cout << ("Opening after threshold; R = ") << radius << std::endl;
    KernelType kernel;
    kernel.SetRadius(radius);
    
    typedef itk::BinaryMorphologicalOpeningImageFilter<InternalImageType, InternalImageType , KernelType> OpenFilterType2;
    typename OpenFilterType2::Pointer openFilter2 = OpenFilterType2::New();
    openFilter2->SetInput(working_image);
    openFilter2->SetBackgroundValue(0);
    openFilter2->SetForegroundValue(1);
    openFilter2->SetKernel(kernel);
    openFilter2->Update();
    working_image->ReleaseData();
    working_image = openFilter2->GetOutput();
  }

  if (this->GetVerboseOptionFlag()) std::cout << ("Labelling") << std::endl;
  // Connected component labeling
  typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
  typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
  connectFilter->SetInput(working_image);
  connectFilter->SetBackgroundValue(this->GetBackgroundValue());
  connectFilter->SetFullyConnected(false);
  connectFilter->Update();
  working_image->ReleaseData();
  working_image = connectFilter->GetOutput();

  if (this->GetVerboseOptionFlag()) std::cout << ("RelabelComponentImageFilter") << std::endl;
  // Sort labels according to size
  typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelFilterType;
  typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
  relabelFilter->InPlaceOn();
  relabelFilter->SetInput(connectFilter->GetOutput());
  relabelFilter->Update();
  working_image->ReleaseData();
  working_image = relabelFilter->GetOutput();

  // End
  StopCurrentStep<InternalImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional]
  if (GetDecomposeAndReconstructDuringFirstStep()) {
    StartNewStep("First Decompose & Reconstruct step");
    typedef clitk::DecomposeAndReconstructImageFilter<InternalImageType,InternalImageType> FilterType;
    typename FilterType::Pointer f = FilterType::New();
    f->SetInput(working_image);
    // f->SetVerbose(m_Verbose);
    f->SetRadius(GetRadius1());
    f->SetMaximumNumberOfLabels(GetMaximumNumberOfLabels1());
    f->SetBackgroundValue(this->GetBackgroundValue());
    f->SetForegroundValue(this->GetForegroundValue());
    f->SetFullyConnected(true);
    f->SetNumberOfNewLabels(GetNumberOfNewLabels1());
    f->Update();
    working_image->ReleaseData();
    working_image = f->GetOutput();
    StopCurrentStep<InternalImageType>(working_image);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  if (this->GetVerboseOptionFlag()) std::cout << ("Remove the air (largest area)") << std::endl;
  StartNewStep("Remove the air (largest area)");
  typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> iBinarizeFilterType;
  typename iBinarizeFilterType::Pointer binarizeFilter2 = iBinarizeFilterType::New();
  binarizeFilter2->SetInput(working_image);
  binarizeFilter2->SetLowerThreshold(GetFirstKeep());
  binarizeFilter2->SetUpperThreshold(GetLastKeep());
  binarizeFilter2 ->SetInsideValue(0);
  binarizeFilter2 ->SetOutsideValue(1);
  binarizeFilter2 ->Update();
  working_image->ReleaseData();
  working_image = binarizeFilter2->GetOutput();

  typename ConnectFilterType::Pointer connectFilter2 = ConnectFilterType::New();
  connectFilter2->SetInput(working_image);
  connectFilter2->SetBackgroundValue(this->GetBackgroundValue());
  connectFilter2->SetFullyConnected(false);
  connectFilter2->Update();
  working_image->ReleaseData();
  working_image = connectFilter2->GetOutput();

  typename RelabelFilterType::Pointer relabelFilter2 = RelabelFilterType::New();
  relabelFilter2->SetInput(working_image);
  relabelFilter2->Update();
  working_image->ReleaseData();
  working_image = relabelFilter2->GetOutput();

  // Keep main label
  working_image = KeepLabels<InternalImageType>
    (working_image, GetBackgroundValue(), GetForegroundValue(), 1, 1, true);
  StopCurrentStep<InternalImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional]
  if (GetDecomposeAndReconstructDuringSecondStep()) {
    StartNewStep("Second Decompose & Reconstruct step");
    typedef clitk::DecomposeAndReconstructImageFilter<InternalImageType,InternalImageType> FilterType;
    typename FilterType::Pointer f = FilterType::New();
    f->SetInput(working_image);
    // f->SetVerbose(m_Verbose);
    f->SetRadius(GetRadius2());
    f->SetMaximumNumberOfLabels(GetMaximumNumberOfLabels2());
    f->SetBackgroundValue(this->GetBackgroundValue());
    f->SetForegroundValue(this->GetForegroundValue());
    f->SetFullyConnected(true);
    f->SetNumberOfNewLabels(GetNumberOfNewLabels2());
    f->Update();
    working_image->ReleaseData();
    working_image = f->GetOutput();
    StopCurrentStep<InternalImageType>(working_image);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional]
  if (GetFinalOpenClose()) {
    StartNewStep("Final OpenClose");
    // Open
    KernelType structuringElement;
    structuringElement.SetRadius(1);
    structuringElement.CreateStructuringElement();
    typedef itk::BinaryMorphologicalOpeningImageFilter<InternalImageType, InternalImageType , KernelType> OpenFilterType;
    typename OpenFilterType::Pointer openFilter = OpenFilterType::New();
    openFilter->SetInput(working_image);
    openFilter->SetBackgroundValue(this->GetBackgroundValue());
    openFilter->SetForegroundValue(this->GetForegroundValue());
    openFilter->SetKernel(structuringElement);
    // Close
    typedef itk::BinaryMorphologicalClosingImageFilter<InternalImageType, InternalImageType , KernelType> CloseFilterType;
    typename CloseFilterType::Pointer closeFilter = CloseFilterType::New();
    closeFilter->SetInput(openFilter->GetOutput());
    closeFilter->SetSafeBorder(true);
    closeFilter->SetForegroundValue(this->GetForegroundValue());
    //  closeFilter->SetBackgroundValue(SetBackgroundValue());
    closeFilter->SetKernel(structuringElement);
    closeFilter->Update();
    working_image->ReleaseData();
    working_image = closeFilter->GetOutput();
    StopCurrentStep<InternalImageType>(working_image);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Final Cast
  typedef itk::CastImageFilter<InternalImageType, MaskImageType> CastImageFilterType;
  typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
  caster->SetInput(working_image);
  caster->Update();
  working_image->ReleaseData();
  output = caster->GetOutput();

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional]
  if (GetAutoCrop()) {
  StartNewStep("AutoCrop");
    typedef clitk::AutoCropFilter<MaskImageType> CropFilterType;
    typename CropFilterType::Pointer cropFilter = CropFilterType::New();
    cropFilter->SetInput(output);
    cropFilter->SetBackgroundValue(GetBackgroundValue());
    cropFilter->Update();
    output->ReleaseData();
    output = cropFilter->GetOutput();
    StopCurrentStep<MaskImageType>(output);
  }
  else
  {
    // Remove Padding region
    typedef itk::CropImageFilter<MaskImageType, MaskImageType> CropFilterType;
    typename CropFilterType::Pointer cropFilter = CropFilterType::New();
    cropFilter->SetInput(output);
    cropFilter->SetLowerBoundaryCropSize(bounds);
    cropFilter->SetUpperBoundaryCropSize(bounds);
    cropFilter->Update();
    output->ReleaseData();
    output = cropFilter->GetOutput();
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType>
void
clitk::ExtractPatientFilter<TInputImageType>::
GenerateData() {
  // Final Graft
  this->GraftOutput(output);
  // Store image filename into AFDB
  GetAFDB()->SetImageFilename("Patient", this->GetOutputPatientFilename());
  WriteAFDB();
}
//--------------------------------------------------------------------


#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
