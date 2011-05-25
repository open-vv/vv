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

#ifndef CLITKEXTRACTLYMPHSTATIONSFILTER_TXX
#define CLITKEXTRACTLYMPHSTATIONSFILTER_TXX

// clitk
#include "clitkCommon.h"
#include "clitkExtractMediastinalVesselsFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkReconstructWithConditionalGrayscaleDilateImageFilter.h"

// itk
#include <itkBinaryThresholdImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

//--------------------------------------------------------------------
template <class TImageType>
clitk::ExtractMediastinalVesselsFilter<TImageType>::
ExtractMediastinalVesselsFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<TImageType, MaskImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0);
  SetForegroundValue(1);
  SetThresholdHigh(140);
  SetThresholdLow(55);
  SetErosionRadius(2);
  SetDilatationRadius(9);
  SetMaxDistancePostToCarina(10);
  SetMaxDistanceAntToCarina(40);
  SetMaxDistanceLeftToCarina(35);
  SetMaxDistanceRightToCarina(35);
  SetSoughtVesselSeedName("NoSeedNameGiven");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinalVesselsFilter<TImageType>::
GenerateOutputInformation() { 
  // Get inputs
  LoadAFDB();
  m_Input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  
  // ------------------------------------------------------------------
  // Crop the initial image superiorly and inferiorly. 
  // TODO : add options for x cm above/below
  CropInputImage();  

  // ------------------------------------------------------------------
  // Binarize the image. Need two thresholds, one high to select
  // structures (CCL) that are almost not connected (after erosion),
  // and one low thresholds to select the real contours. Will be
  // reconstructed later. 
  StartNewStep("Binarize with high threshold = "+toString(GetThresholdHigh()));
  typedef itk::BinaryThresholdImageFilter<ImageType, MaskImageType> BinarizeFilterType; 
  typename BinarizeFilterType::Pointer binarizeFilter = BinarizeFilterType::New();
  binarizeFilter->SetInput(m_Input);
  binarizeFilter->SetLowerThreshold(GetThresholdHigh());
  binarizeFilter->SetInsideValue(GetForegroundValue());
  binarizeFilter->SetOutsideValue(GetBackgroundValue());
  binarizeFilter->Update();
  m_Mask = binarizeFilter->GetOutput();
  StopCurrentStep<MaskImageType>(m_Mask);

  // ------------------------------------------------------------------
  StartNewStep("Binarize with low threshold = "+toString(GetThresholdLow()));
  binarizeFilter = BinarizeFilterType::New();
  binarizeFilter->SetInput(m_Input);
  binarizeFilter->SetLowerThreshold(GetThresholdLow());
  binarizeFilter->SetInsideValue(GetForegroundValue());
  binarizeFilter->SetOutsideValue(GetBackgroundValue());
  binarizeFilter->Update();
  MaskImagePointer m_Mask2 = binarizeFilter->GetOutput();
  StopCurrentStep<MaskImageType>(m_Mask2);

  // ------------------------------------------------------------------
  // Extract slices
  StartNewStep("Detect objects : erosion, then slice by slice reconstruction");
  std::vector<MaskSlicePointer> slices_mask;
  clitk::ExtractSlices<MaskImageType>(m_Mask, 2, slices_mask);
  std::vector<MaskSlicePointer> slices_mask2;
  clitk::ExtractSlices<MaskImageType>(m_Mask2, 2, slices_mask2);
  int radius = GetErosionRadius();

  // List of working slices (debug only)
  std::vector<MaskSlicePointer> debug_eroded;
  // std::vector<MaskSlicePointer> debug_labeled;
  
  // ------------------------------------------------------------------
  // Loop Slice by Slice in order to break connectivity between
  // CCL. Erode and reconstruct all labels at the same time without
  // merging them.
  for(uint i=0; i<slices_mask.size(); i++) {
    // Erosion kernel
    typedef itk::BinaryBallStructuringElement<MaskSliceType::PixelType,2> KernelType;
    KernelType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();
    
    // Erosion -> we break the connectivity between structure
    typedef itk::BinaryErodeImageFilter<MaskSliceType, MaskSliceType, KernelType> ErodeFilterType;
    typename ErodeFilterType::Pointer eroder = ErodeFilterType::New();
    eroder->SetInput(slices_mask[i]);
    eroder->SetBackgroundValue(GetBackgroundValue());
    eroder->SetForegroundValue(GetForegroundValue());
    eroder->SetBoundaryToForeground(true); // ??
    eroder->SetKernel(structuringElement);
    eroder->Update();
    MaskSlicePointer eroded = eroder->GetOutput();
    debug_eroded.push_back(eroded);
      
    // Labelize (CCL)
    MaskSlicePointer labeled = 
      clitk::Labelize<MaskSliceType>(eroded, GetBackgroundValue(), true, 1); // Fully connected !
    // debug_labeled.push_back(labeled);

    // Make Reconstruction filter : dilation all labels at the same
    // time, prevent to merge them.
    typedef clitk::ReconstructWithConditionalGrayscaleDilateImageFilter<MaskSliceType> ReconstructFilterType;
    typename ReconstructFilterType::Pointer reconstructor = ReconstructFilterType::New();
    reconstructor->SetInput(labeled);
    reconstructor->SetIterationNumber(radius+GetDilatationRadius());
    reconstructor->Update();
    MaskSlicePointer s = reconstructor->GetOutput();

    // Remove Initial BG of the second tresholded image
    s = clitk::SetBackground<MaskSliceType, MaskSliceType>(s, slices_mask2[i], 
                                                           GetBackgroundValue(), GetBackgroundValue(), true);
    m_slice_recon.push_back(s);

  } // end loop
  
  // Build 3D images from the slice by slice processing
  MaskImageType::Pointer eroded = clitk::JoinSlices<MaskImageType>(debug_eroded, m_Mask, 2);
  writeImage<MaskImageType>(eroded, "erode.mhd");
  //MaskImageType::Pointer l = clitk::JoinSlices<MaskImageType>(debug_labeled, m_Mask, 2);  
  MaskImageType::Pointer r = clitk::JoinSlices<MaskImageType>(m_slice_recon, m_Mask, 2);
  writeImage<MaskImageType>(r, "recon1.mhd");
  
  // ------------------------------------------------------------------
  // Track the SoughtVessel from the given first point
  // superiorly. This is done by TrackBifurcationFromPoint
  MaskImagePointType SoughtVesselSeedPoint;
  GetAFDB()->GetPoint3D(m_SoughtVesselSeedName, SoughtVesselSeedPoint);

  // Find the label with the maximum value to set the result
  typedef itk::MinimumMaximumImageCalculator<MaskImageType> MinMaxFilterType;
  MinMaxFilterType::Pointer ff = MinMaxFilterType::New();
  ff->SetImage(r);
  ff->ComputeMaximum();
  LabelType newLabel = ff->GetMaximum()+1; 

  // the following bifurcations point will the centroids of the
  // components obtain when (hopefully!) the SoughtVessel
  // split into CommonArtery and SubclavianArtery.
  std::vector<MaskImagePointType> bifurcations;
  TrackBifurcationFromPoint(r, m_slice_recon, SoughtVesselSeedPoint, newLabel, bifurcations);

  // Build the final 3D image from the previous slice by slice processing
  m_SoughtVessel = clitk::JoinSlices<MaskImageType>(m_slice_recon, m_Mask, 2);
  writeImage<MaskImageType>(m_SoughtVessel, "recon2.mhd");
  
  // Set binary image, (remove other labels).  
  // TODO: keep labeled image to track SubclavianArtery and CommonArtery
  m_SoughtVessel = 
    clitk::Binarize<MaskImageType>(m_SoughtVessel, newLabel, newLabel, 
                                   GetBackgroundValue(), GetForegroundValue());

  writeImage<MaskImageType>(m_SoughtVessel, "afterbinarize.mhd");

  m_SoughtVessel = clitk::AutoCrop<MaskImageType>(m_SoughtVessel, GetBackgroundValue());

  writeImage<MaskImageType>(m_SoughtVessel, "afterautocrop.mhd");

  // Clean the image : Opening (not in Z direction)
  typename MaskImageType::SizeType rad;
  rad[0] = rad[1] = 2;
  rad[2] = 0;
  m_SoughtVessel = clitk::Opening<MaskImageType>(m_SoughtVessel, rad, 
                                                          GetBackgroundValue(), GetForegroundValue());

  writeImage<MaskImageType>(m_SoughtVessel, "afteropen.mhd");

  // Clean the image : keep main CCL slice by slice
  m_SoughtVessel = clitk::SliceBySliceKeepMainCCL<MaskImageType>(m_SoughtVessel, 
                                                                          GetBackgroundValue(), 
                                                                          GetForegroundValue());  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinalVesselsFilter<TImageType>::
GenerateInputRequestedRegion() {
  //DD("GenerateInputRequestedRegion (nothing?)");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinalVesselsFilter<TImageType>::
GenerateData() {
  // Save in the AFDB (not write on the disk here)
  GetAFDB()->SetImageFilename(GetSoughtVesselName(), GetOutputFilename());  
  WriteAFDB();
  // Final Step -> graft output
  this->GraftNthOutput(0, m_SoughtVessel);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinalVesselsFilter<TImageType>::
CropInputImage() { 
  StartNewStep("Crop the input image: SI,AP limits with carina and crop with mediastinum");
  /*
    Need : Trachea, Carina (roi not point), 
   */
  // Get Trachea and Carina
  MaskImagePointer Trachea = GetAFDB()->template GetImage <MaskImageType>("Trachea");  
  
  // Compute Carina position
  double m_CarinaZ;
  MaskImagePointer Carina = GetAFDB()->template GetImage<MaskImageType>("Carina");
  std::vector<MaskImagePointType> centroids;
  clitk::ComputeCentroids<MaskImageType>(Carina, GetBackgroundValue(), centroids);
  m_CarinaZ = centroids[1][2];
  // add one slice to include carina 
  m_CarinaZ += Carina->GetSpacing()[2];
  // We dont need Carina structure from now
  Carina->Delete();
  GetAFDB()->SetPoint3D("CarinaPoint", centroids[1]);
  
  // Crop Inf, remove below Carina
  m_Input = 
    clitk::CropImageRemoveLowerThan<ImageType>(m_Input, 2, m_CarinaZ, false, GetBackgroundValue());  

  // Crop post
  double m_CarinaY = centroids[1][1];
  m_Input = clitk::CropImageRemoveGreaterThan<ImageType>(m_Input, 1,
                                                         m_CarinaY+GetMaxDistancePostToCarina(), 
                                                         false, GetBackgroundValue());  
  // Crop ant 
  m_Input = clitk::CropImageRemoveLowerThan<ImageType>(m_Input, 1, 
                                                       m_CarinaY-GetMaxDistanceAntToCarina(), 
                                                       false, GetBackgroundValue());  

  // Crop Right
  double m_CarinaX = centroids[1][0];
  m_Input = clitk::CropImageRemoveLowerThan<ImageType>(m_Input, 0, 
                                                       m_CarinaX-GetMaxDistanceRightToCarina(), 
                                                       false, GetBackgroundValue());  
  // Crop Left
  m_Input = clitk::CropImageRemoveGreaterThan<ImageType>(m_Input, 0, 
                                                         m_CarinaX+GetMaxDistanceLeftToCarina(), 
                                                         false, GetBackgroundValue());  

  /*
  // AutoCrop with Mediastinum, generaly only allow to remove few slices (superiorly)
  m_Mediastinum  = GetAFDB()->template GetImage<MaskImageType>("Mediastinum");
  // Resize like input (sup to carina)
  m_Mediastinum = clitk::ResizeImageLike<MaskImageType>(m_Mediastinum, m_Input, GetBackgroundValue());
  // Auto crop
  m_Mediastinum = clitk::AutoCrop<MaskImageType>(m_Mediastinum, GetBackgroundValue());
  // Resize input
  m_Input = clitk::ResizeImageLike<ImageType>(m_Input, m_Mediastinum, GetBackgroundValue());
  */

  // End
  StopCurrentStep<ImageType>(m_Input);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinalVesselsFilter<TImageType>::
TrackBifurcationFromPoint(MaskImagePointer & recon, 
                          std::vector<MaskSlicePointer> & slices_recon, 
                          MaskImagePointType point3D, 
                          LabelType newLabel,
                          std::vector<MaskImagePointType> & bifurcations) {
  StartNewStep("Track the SoughtVessel from the seed point");

  // Find first slice index
  MaskImageIndexType index;
  recon->TransformPhysicalPointToIndex(point3D, index);
  int numberOfBifurcation = 0;
  typedef typename MaskSliceType::PointType SlicePointType;
  SlicePointType previousCenter;

  // Get current label at the point3D of interest
  uint currentSlice=index[2]; 
  bool found = false;
  LabelType previous_slice_label=recon->GetPixel(index);
  //  DD(slices_recon.size());
  do {
    //    DD(currentSlice);
    // Consider current reconstructed slice
    MaskSlicePointer s = slices_recon[currentSlice];
    MaskSlicePointer previous;
    if (currentSlice == index[2]) previous = s;
    else {
      previous = slices_recon[currentSlice-1];
    }
    
    // Get centroids of the labels in the current slice
    static const unsigned int Dim = MaskSliceType::ImageDimension;
    typedef itk::ShapeLabelObject< LabelType, Dim > LabelObjectType;
    typedef itk::LabelMap< LabelObjectType > LabelMapType;
    typedef itk::LabelImageToLabelMapFilter<MaskSliceType, LabelMapType> ImageToMapFilterType;
    typename ImageToMapFilterType::Pointer imageToLabelFilter = ImageToMapFilterType::New(); 
    typedef itk::ShapeLabelMapFilter<LabelMapType, MaskSliceType> ShapeFilterType; 
    typename ShapeFilterType::Pointer statFilter = ShapeFilterType::New();
    imageToLabelFilter->SetBackgroundValue(GetBackgroundValue());
    imageToLabelFilter->SetInput(s);
    statFilter->SetInput(imageToLabelFilter->GetOutput());
    statFilter->Update();
    typename LabelMapType::Pointer labelMap = statFilter->GetOutput();

    // Look what centroid inside the previous largest one
    std::vector<SlicePointType> centroids;
    std::vector<LabelType> centroids_label;
    std::vector<double> labels_size;
    for(uint c=0; c<labelMap->GetNumberOfLabelObjects(); c++) {
      int label = labelMap->GetLabels()[c];
      //      DD(label);
      SlicePointType center = labelMap->GetLabelObject(label)->GetCentroid();
      //      DD(center);
      // Get label into previous slice
      typename MaskSliceType::IndexType centerIndex;
      previous->TransformPhysicalPointToIndex(center, centerIndex);
      LabelType labelInPreviousSlice = previous->GetPixel(centerIndex);
      // if this current centroid was in the current label, add it to bifurcations
      if (labelInPreviousSlice == previous_slice_label) {
        centroids.push_back(center);
        centroids_label.push_back(label);
        labels_size.push_back(labelMap->GetLabelObject(label)->GetPhysicalSize());
      }
    }

    if (centroids.size() == 0) {
      // Last attempt to find -> check if previous centroid is inside a CCL
      //      if in s -> get value, getcentroid add.
      DD(currentSlice);
      DD("Last change to find");
      typename MaskSliceType::IndexType previousCenterIndex;
      s->TransformPhysicalPointToIndex(previousCenter, previousCenterIndex);
      DD(previousCenter);
      LabelType labelInSlice = s->GetPixel(previousCenterIndex);
      DD(labelInSlice);
      if (labelInSlice != GetBackgroundValue()) {
        centroids.push_back(labelMap->GetLabelObject(labelInSlice)->GetCentroid());
        centroids_label.push_back(labelInSlice);
        labels_size.push_back(labelMap->GetLabelObject(labelInSlice)->GetPhysicalSize());
      }
    }

    
    //    DD(centroids.size());
    
    // If several centroids, we found a bifurcation
    if (centroids.size() > 1) {
      numberOfBifurcation++;
      // If the number of bifurcation is greater than the required one, we stop
      if (numberOfBifurcation > GetMaxNumberOfFoundBifurcation()) {
        found = true;
        DD("max bif reach");
        for(uint c=0; c<centroids.size(); c++) {
          ImagePointType bif;
          clitk::PointsUtils<MaskImageType>::Convert2DTo3D(centroids[c], m_Mask, currentSlice, bif);
          bifurcations.push_back(bif);
        }
      }
      // Else we continue along the main (largest) connected component
      else {
        int indexOfLargest = 0;
        for(uint b=0; b<centroids.size(); b++) {
          if (labels_size[b] > labels_size[indexOfLargest]) {
            indexOfLargest = b;
          }
        }
        SlicePointType c = centroids[indexOfLargest];
        LabelType l = centroids_label[indexOfLargest];
        centroids.clear();
        centroids.push_back(c);
        centroids_label.push_back(l);
      }
    }
    
    // if only one centroids, we change the current image with the current label 
    if (centroids.size() == 1) {
      s = clitk::SetBackground<MaskSliceType, MaskSliceType>(s, s, centroids_label[0], newLabel, true);
      slices_recon[currentSlice] = s;
      previous_slice_label = newLabel;
      // It can happend that several CCL share this same label. To
      // prevent this case, we only consider the one that contains
      // the centroid. 
      MaskSlicePointer temp = clitk::Binarize<MaskSliceType>(s, newLabel, newLabel, GetBackgroundValue(), GetForegroundValue());
      //      writeImage<MaskSliceType>(temp, "bin-"+toString(currentSlice)+".mhd");
      temp = clitk::Labelize<MaskSliceType>(temp, GetBackgroundValue(), true, 1);
      //writeImage<MaskSliceType>(temp, "label-"+toString(currentSlice)+".mhd");
      typename MaskSliceType::IndexType centroids_index;
      temp->TransformPhysicalPointToIndex(centroids[0], centroids_index);
      typename MaskSliceType::PixelType v = temp->GetPixel(centroids_index); 

      // It can happend that the centroid is inside the BG, so we keep
      // the largest CCL (the first);
      if (v == GetBackgroundValue()) {
        DD(currentSlice);
        DD("inside BG");
        DD(centroids[0]);
        v = 1; // largest one
      }

      //DD(v);
      temp = clitk::Binarize<MaskSliceType>(temp, v, v, GetBackgroundValue(), newLabel);      
      //writeImage<MaskSliceType>(temp, "relabel-"+toString(currentSlice)+".mhd");      
      s = temp;
      slices_recon[currentSlice] = s;

      // I need to recompute the centroid if we have removed some
      // connected component.
      clitk::ComputeCentroids<MaskSliceType>(s, GetBackgroundValue(), centroids);
      previousCenter = centroids[1];
    }

    if (centroids.size() == 0) {
      DD("ZERO");
      found = true;
    }

    if (currentSlice == slices_recon.size()-1) {
      DD("end of slices");
      found = true;
    }

    // iterate
    ++currentSlice;
  } while (!found);

  // End
  StopCurrentStep();
}
//--------------------------------------------------------------------


#endif //#define CLITKEXTRACTMEDIASTINALVESSELSFILTER_TXX
