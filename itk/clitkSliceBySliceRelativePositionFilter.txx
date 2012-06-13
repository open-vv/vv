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
#include "clitkCropLikeImageFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkExtractSliceFilter.h"
#include "clitkResampleImageWithOptionsFilter.h"

// itk
#include <itkJoinSeriesImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SliceBySliceRelativePositionFilter():
  clitk::AddRelativePositionConstraintToLabelImageFilter<ImageType>()
{
  SetDirection(2);
  UniqueConnectedComponentBySliceFlagOff();
  SetIgnoreEmptySliceObjectFlag(false);
  UseTheLargestObjectCCLFlagOff();
  this->VerboseStepFlagOff();
  this->WriteStepFlagOff();
  this->SetCombineWithOrFlag(false);
  ObjectCCLSelectionFlagOff();
  SetObjectCCLSelectionDimension(0);
  SetObjectCCLSelectionDirection(1);
  ObjectCCLSelectionIgnoreSingleCCLFlagOff();
  VerboseSlicesFlagOff();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SetInput(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
SetInputObject(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(1, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
PrintOptions(std::ostream & os) 
{
  os << "Slice direction = " << this->GetDirection() << std::endl
     << "BG value        = " << this->GetBackgroundValue() << std::endl;
  for(int i=0; i<this->GetNumberOfAngles(); i++) {
    os << "Orientation     = " << this->GetOrientationTypeString()[i] << std::endl;
    os << "Angles     = " << clitk::rad2deg(this->GetAngle1InRad(i)) 
       << " " << clitk::rad2deg(this->GetAngle2InRad(i)) << std::endl;
  }
  os << "InverseOrientationFlag  = " << this->GetInverseOrientationFlag() << std::endl        
     << "SpacingFlag     = " << this->GetIntermediateSpacingFlag() << std::endl
     << "Spacing         = " << this->GetIntermediateSpacing() << std::endl
     << "FuzzyThreshold  = " << this->GetFuzzyThreshold() << std::endl
     << "UniqueConnectedComponentBySliceFlag  = " << this->GetUniqueConnectedComponentBySliceFlag() << std::endl
     << "AutoCropFlag    = " << this->GetAutoCropFlag() << std::endl    
     << "RemoveObjectFlag= " << this->GetRemoveObjectFlag() << std::endl    
     << "CombineWithOrFlag = " << this->GetCombineWithOrFlag() << std::endl    
     << "UseTheLargestObjectCCLFlag = " << this->GetUseTheLargestObjectCCLFlag() << std::endl    
     << "ObjectCCLSelectionFlag = " << this->GetObjectCCLSelectionFlag() << std::endl    
     << "ObjectCCLSelectionDimension = " << this->GetObjectCCLSelectionDimension() << std::endl    
     << "ObjectCCLSelectionIgnoreSingleCCLFlag = " << this->GetObjectCCLSelectionIgnoreSingleCCLFlag() << std::endl    
     << "IgnoreEmptySliceObjectFlag = " << this->GetIgnoreEmptySliceObjectFlag() << std::endl
     << "(RP) FastFlag              = " << this->GetFastFlag() << std::endl
     << "(RP) Radius                = " << this->GetRadius() << std::endl;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
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
clitk::SliceBySliceRelativePositionFilter<ImageType>::
GenerateOutputInformation() 
{
  if (this->GetVerboseOptionFlag()) {
    PrintOptions();
  }

  //  if (this->GetFuzzyMapOnlyFlag()) this->ComputeFuzzyMapFlagOn();

  // Get input pointer
  input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  object = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  m_working_object = object;
  m_working_input = input;

  //--------------------------------------------------------------------
  // Resample object to the same spacing than input
  if (!clitk::HaveSameSpacing<ImageType, ImageType>(object, input)) {
    this->StartNewStep("Resample object to the same spacing than input");
    m_working_object = clitk::ResampleImageSpacing<ImageType>(object, input->GetSpacing());
    this->template StopCurrentStep<ImageType>(m_working_object);
  }
  
  //--------------------------------------------------------------------
  // Resize image according to common area (except in Z)
  if (!clitk::HaveSameSizeAndSpacing<ImageType, ImageType>(m_working_object, input)) {
    this->StartNewStep("Resize images (union in XY and like input in Z)");
    
    /* OLD STUFF
    this->StartNewStep("Pad object to the same size than input");
    m_working_object = clitk::ResizeImageLike<ImageType>(m_working_object, 
    input, 
    this->GetObjectBackgroundValue());
    this->template StopCurrentStep<ImageType>(m_working_object);
    */

    // Compute union of bounding boxes in X and Y
    static const unsigned int dim = ImageType::ImageDimension;
    typedef itk::BoundingBox<unsigned long, dim> BBType;
    typename BBType::Pointer bb1 = BBType::New();
    ComputeBBFromImageRegion<ImageType>(m_working_object, m_working_object->GetLargestPossibleRegion(), bb1);
    typename BBType::Pointer bb2 = BBType::New();
    ComputeBBFromImageRegion<ImageType>(input, input->GetLargestPossibleRegion(), bb2);
    typename BBType::Pointer bbo = BBType::New();
    ComputeBBUnion<dim>(bbo, bb1, bb2);

    //We set Z BB like input
    typename ImageType::PointType maxs = bbo->GetMaximum();
    typename ImageType::PointType mins = bbo->GetMinimum();
    maxs[2] = bb2->GetMaximum()[2];
    mins[2] = bb2->GetMinimum()[2];
    bbo->SetMaximum(maxs);
    bbo->SetMinimum(mins);

    // Crop
    m_working_input = clitk::ResizeImageLike<ImageType>(input, bbo, this->GetBackgroundValue());    
    m_working_object = clitk::ResizeImageLike<ImageType>(m_working_object, 
                                                         m_working_input, 
                                                         this->GetObjectBackgroundValue());
    
    // Index can be negative in some cases, and lead to problem with
    // some filter. So we correct it.
    m_working_input = clitk::RemoveNegativeIndexFromRegion<ImageType>(m_working_input);
    m_working_object = clitk::RemoveNegativeIndexFromRegion<ImageType>(m_working_object);

    // End
    this->template StopCurrentStep<ImageType>(m_working_input);  
  }
  
  //--------------------------------------------------------------------
  /* Steps : 
    - extract vector of slices in input, in object
    - slice by slice rel position
    - joint result
    - post process
  */

  //--------------------------------------------------------------------
  // Extract input slices
  this->StartNewStep("Extract input slices");
  typedef clitk::ExtractSliceFilter<ImageType> ExtractSliceFilterType;
  typename ExtractSliceFilterType::Pointer extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(m_working_input);
  extractSliceFilter->SetDirection(GetDirection());
  extractSliceFilter->Update();
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  std::vector<typename SliceType::Pointer> mInputSlices;
  extractSliceFilter->GetOutputSlices(mInputSlices);
  this->template StopCurrentStep<SliceType>(mInputSlices[0]);
  
  //--------------------------------------------------------------------
  // Extract object slices
  this->StartNewStep("Extract object slices");
  extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(m_working_object);
  extractSliceFilter->SetDirection(GetDirection());
  extractSliceFilter->Update();
  std::vector<typename SliceType::Pointer> mObjectSlices;
  extractSliceFilter->GetOutputSlices(mObjectSlices);
  this->template StopCurrentStep<SliceType>(mObjectSlices[0]);

  //--------------------------------------------------------------------
  // Prepare fuzzy slices (if needed)
  std::vector<typename FloatSliceType::Pointer> mFuzzyMapSlices;
  mFuzzyMapSlices.resize(mInputSlices.size());

  //--------------------------------------------------------------------
  // Perform slice by slice relative position
  this->StartNewStep("Perform slice by slice relative position ("+toString(mInputSlices.size())+")");
  for(unsigned int i=0; i<mInputSlices.size(); i++) {
    
    // Count the number of CCL (allow to ignore empty slice)
    int nb=0;
    mObjectSlices[i] = LabelizeAndCountNumberOfObjects<SliceType>(mObjectSlices[i], 0, true, 1, nb);

    // If no object and empty slices and if we need the full fuzzy map, create a dummy one.
    if ((nb==0) && (this->GetFuzzyMapOnlyFlag())) {
      typename FloatSliceType::Pointer one = FloatSliceType::New();
      one->CopyInformation(mObjectSlices[0]);
      one->SetRegions(mObjectSlices[0]->GetLargestPossibleRegion());
      one->Allocate();
      one->FillBuffer(2.0);
      mFuzzyMapSlices[i] = one;
    } // End nb==0 && GetComputeFuzzyMapFlag
    else {
      if ((!GetIgnoreEmptySliceObjectFlag()) || (nb!=0)) {
        
        // Select or not a single CCL ?
        if (GetUseTheLargestObjectCCLFlag()) {
          mObjectSlices[i] = KeepLabels<SliceType>(mObjectSlices[i], 0, 1, 1, 1, true);
        }

        // Select a single according to a position if more than one CCL
        if (GetObjectCCLSelectionFlag()) {
          // if several CCL, choose the most extrema according a direction, 
          // if not -> should we consider this slice ? 
          if (nb<2) {
            if (GetObjectCCLSelectionIgnoreSingleCCLFlag()) {
              mObjectSlices[i] = SetBackground<SliceType, SliceType>(mObjectSlices[i], mObjectSlices[i], 
                                                                     1, this->GetBackgroundValue(), 
                                                                     true);
            }
          }
          int dim = GetObjectCCLSelectionDimension();
          int direction = GetObjectCCLSelectionDirection();
          std::vector<typename SliceType::PointType> centroids;
          ComputeCentroids<SliceType>(mObjectSlices[i], this->GetBackgroundValue(), centroids);
          uint index=1;
          for(uint j=1; j<centroids.size(); j++) {
            if (direction == 1) {
              if (centroids[j][dim] > centroids[index][dim]) index = j;
            }
            else {
              if (centroids[j][dim] < centroids[index][dim]) index = j;
            }
          }
          for(uint v=1; v<centroids.size(); v++) {
            if (v != index) {
              mObjectSlices[i] = SetBackground<SliceType, SliceType>(mObjectSlices[i], mObjectSlices[i], 
                                                                     (char)v, this->GetBackgroundValue(), 
                                                                     true);
            }
          }
        } // end GetbjectCCLSelectionFlag = true

        // Relative position
        typedef clitk::AddRelativePositionConstraintToLabelImageFilter<SliceType> RelPosFilterType;
        typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        if (GetVerboseSlicesFlag()) {
          std::cout << "Slice " << i << std::endl;
          relPosFilter->VerboseStepFlagOn();
        }
        relPosFilter->WriteStepFlagOff();
        // relPosFilter->VerboseMemoryFlagOn();
        relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId()+"-"+toString(i));        
        relPosFilter->SetBackgroundValue(this->GetBackgroundValue());
        relPosFilter->SetInput(mInputSlices[i]); 
        relPosFilter->SetInputObject(mObjectSlices[i]); 
        relPosFilter->SetRemoveObjectFlag(this->GetRemoveObjectFlag());        
        // This flag (InverseOrientation) *must* be set before
        // AddOrientation because AddOrientation can change it.
        relPosFilter->SetInverseOrientationFlag(this->GetInverseOrientationFlag());
        for(int j=0; j<this->GetNumberOfAngles(); j++) {
          relPosFilter->AddAnglesInRad(this->GetAngle1InRad(j), this->GetAngle2InRad(j));
        }
        relPosFilter->SetIntermediateSpacing(this->GetIntermediateSpacing());
        relPosFilter->SetIntermediateSpacingFlag(this->GetIntermediateSpacingFlag());
        relPosFilter->SetFuzzyThreshold(this->GetFuzzyThreshold());
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->SetCombineWithOrFlag(this->GetCombineWithOrFlag()); 
        // should we stop after fuzzy map ?
        relPosFilter->SetFuzzyMapOnlyFlag(this->GetFuzzyMapOnlyFlag());
        //        relPosFilter->SetComputeFuzzyMapFlag(this->GetComputeFuzzyMapFlag());      
        relPosFilter->SetFastFlag(this->GetFastFlag());
        relPosFilter->SetRadius(this->GetRadius());

        // Go !
        relPosFilter->Update();

        // If we stop after the fuzzy map, store the fuzzy slices
        if (this->GetFuzzyMapOnlyFlag()) {
          mFuzzyMapSlices[i] = relPosFilter->GetFuzzyMap();
          // writeImage<FloatSliceType>(mFuzzyMapSlices[i], "slice_"+toString(i)+".mha");
        }

        // Set input slices
        if (!this->GetFuzzyMapOnlyFlag())  {
          mInputSlices[i] = relPosFilter->GetOutput();
          // Select main CC if needed
          if (GetUniqueConnectedComponentBySliceFlag()) {
            mInputSlices[i] = Labelize<SliceType>(mInputSlices[i], 0, true, 1);
            mInputSlices[i] = KeepLabels<SliceType>(mInputSlices[i], 0, 1, 1, 1, true);
          }          
        }

      }

      /*
      // Select unique CC according to the most in a given direction
      if (GetUniqueConnectedComponentBySliceAccordingToADirection()) {
      int nb;
      mInputSlices[i] = LabelizeAndCountNumberOfObjects<SliceType>(mInputSlices[i], 0, true, 1, nb);
      std::vector<typename ImageType::PointType> & centroids;
      ComputeCentroids
      }
      */

    } // End nb!=0 || GetComputeFuzzyMapFlagOFF

  } // end for i mInputSlices

  // Join the slices
  m_working_input = clitk::JoinSlices<ImageType>(mInputSlices, m_working_input, GetDirection());
  this->template StopCurrentStep<ImageType>(m_working_input);

  // Join the fuzzy map if needed
  if (this->GetFuzzyMapOnlyFlag()) {
    this->m_FuzzyMap = clitk::JoinSlices<FloatImageType>(mFuzzyMapSlices, m_working_input, GetDirection());
    this->template StopCurrentStep<FloatImageType>(this->m_FuzzyMap);
    if (this->GetFuzzyMapOnlyFlag()) return;
  }

  //--------------------------------------------------------------------
  // Step 7: autocrop
  if (this->GetAutoCropFlag()) {
    this->StartNewStep("Final AutoCrop");
    typedef clitk::AutoCropFilter<ImageType> CropFilterType;
    typename CropFilterType::Pointer cropFilter = CropFilterType::New();
    cropFilter->SetInput(m_working_input);
    cropFilter->ReleaseDataFlagOff();
    cropFilter->Update();   
    m_working_input = cropFilter->GetOutput();
    this->template StopCurrentStep<ImageType>(m_working_input);    
  }

  // Update output info
  this->GetOutput(0)->SetRegions(m_working_input->GetLargestPossibleRegion());  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::SliceBySliceRelativePositionFilter<ImageType>::
GenerateData() 
{
  // Get input pointer
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------  
  // Final Step -> set output
  //this->SetNthOutput(0, m_working_input);
  if (this->GetFuzzyMapOnlyFlag()) return; // no output in this case
  this->GraftOutput(m_working_input);
  return;
}
//--------------------------------------------------------------------

