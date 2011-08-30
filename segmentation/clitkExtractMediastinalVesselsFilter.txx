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

template<class T> struct index_cmp {
  index_cmp(const T varr) : arr(varr) {}
  bool operator()(const size_t a, const size_t b) const
  { return arr[a] < arr[b]; }
  const T arr;
};


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
  SetFinalOpeningRadius(1);
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

    /*// Erosion kernel
      typedef itk::BinaryBallStructuringElement<MaskSliceType::PixelType,2> KernelType;
      KernelType structuringElement;
      structuringElement.SetRadius(radius);
      structuringElement.CreateStructuringElement();
    */
    
    // Erosion -> we break the connectivity between structure
    MaskSliceType::SizeType r;
    r[0] = r[1] = radius;
    MaskSlicePointer eroded = clitk::Opening<MaskSliceType>(slices_mask[i], 
                                                            r,
                                                            GetBackgroundValue(), 
                                                            GetForegroundValue());
    /*
    //typedef itk::BinaryErodeImageFilter<MaskSliceType, MaskSliceType, KernelType> ErodeFilterType;
    typedef itk::BinaryMorphologicalOpeningImageFilter<MaskSliceType, MaskSliceType, KernelType> ErodeFilterType;
    typename ErodeFilterType::Pointer eroder = ErodeFilterType::New();
    eroder->SetInput(slices_mask[i]);
    eroder->SetBackgroundValue(GetBackgroundValue());
    eroder->SetForegroundValue(GetForegroundValue());
    //eroder->SetBoundaryToForeground(true); // ?? for BinaryErodeImageFilter
    eroder->SetKernel(structuringElement);
    eroder->Update();
    MaskSlicePointer eroded = eroder->GetOutput();
    */

    // Keep slice for debug
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
  MaskImagePointType MaxSlicePoint;
  if (GetAFDB()->TagExist(m_SoughtVesselSeedName+"Max")) {
    GetAFDB()->GetPoint3D(m_SoughtVesselSeedName+"Max", MaxSlicePoint);
  }
  else {
    MaxSlicePoint = SoughtVesselSeedPoint;
    MaxSlicePoint[2] += 1000;
  }

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
  //  TrackBifurcationFromPoint(r, m_slice_recon, SoughtVesselSeedPoint, 
  //                         MaxSlicePoint, newLabel, bifurcations);

  TrackVesselsFromPoint(r, m_slice_recon, SoughtVesselSeedPoint, 
                        MaxSlicePoint, newLabel);

  // Build the final 3D image from the previous slice by slice processing
  m_SoughtVessel = clitk::JoinSlices<MaskImageType>(m_slice_recon, m_Mask, 2);
  writeImage<MaskImageType>(m_SoughtVessel, "recon2.mhd");
  
  // Set binary image, (remove other labels).  
  m_SoughtVessel = 
    clitk::Binarize<MaskImageType>(m_SoughtVessel, newLabel, newLabel, 
                                   GetBackgroundValue(), GetForegroundValue());

  //  writeImage<MaskImageType>(m_SoughtVessel, "afterbinarize.mhd");
  m_SoughtVessel = clitk::AutoCrop<MaskImageType>(m_SoughtVessel, GetBackgroundValue());

  //  writeImage<MaskImageType>(m_SoughtVessel, "afterautocrop.mhd");

  // Clean the image : Opening (not in Z direction)
  typename MaskImageType::SizeType rad;
  rad[0] = rad[1] = GetFinalOpeningRadius(); 
  rad[2] = 0;
  m_SoughtVessel = clitk::Opening<MaskImageType>(m_SoughtVessel, rad, 
                                                 GetBackgroundValue(), GetForegroundValue());

  //  writeImage<MaskImageType>(m_SoughtVessel, "afteropen.mhd");

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

  //  writeImage<ImageType>(m_Input, "crop.mhd");
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
                          MaskImagePointType pointMaxSlice,
                          LabelType newLabel,
                          std::vector<MaskImagePointType> & bifurcations) {
  StartNewStep("Track the SoughtVessel from the seed point");

  // Find first slice index
  MaskImageIndexType index;
  recon->TransformPhysicalPointToIndex(point3D, index);
  int numberOfBifurcation = 0;
  typedef typename MaskSliceType::PointType SlicePointType;
  SlicePointType previousCenter;
  
  // Max slice
  MaskImageIndexType indexMaxSlice;
  recon->TransformPhysicalPointToIndex(pointMaxSlice, indexMaxSlice);  
  uint maxSlice = indexMaxSlice[2];

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
    // statFilter->SetComputeFeretDiameter( true );
    statFilter->ComputePerimeterOn(); // To be able to get proper Roundness value
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
        //DD(labels_size.back());
        //DD(labelMap->GetLabelObject(label)->GetRoundness());
        // previousCenter = centroids.back();
      }
    }

    // -------------------------
    // If no centroid were found
    if (centroids.size() == 0) {
      // Last attempt to find -> check if previous centroid is inside a CCL
      //      if in s -> get value, getcentroid add.
      //      DD(currentSlice);
      //DD("Last change to find");
      //DD(previous_slice_label);
      //DD(newLabel);
      typename MaskSliceType::IndexType previousCenterIndex;
      s->TransformPhysicalPointToIndex(previousCenter, previousCenterIndex);
      //DD(previousCenter);
      LabelType labelInSlice = s->GetPixel(previousCenterIndex);
      //DD(labelInSlice);
      if (labelInSlice != GetBackgroundValue()) {
        centroids.push_back(labelMap->GetLabelObject(labelInSlice)->GetCentroid());
        centroids_label.push_back(labelInSlice);
        labels_size.push_back(labelMap->GetLabelObject(labelInSlice)->GetPhysicalSize());
      }
    }

    // Some centroid were found
    // If several centroids, we found a bifurcation
    if (centroids.size() > 1) {
      //      int n = centroids.size();
      // Debug point
      std::vector<ImagePointType> d;
      clitk::PointsUtils<MaskImageType>::Convert2DListTo3DList(centroids, currentSlice, m_Mask, d);
      //      DDV(d, d.size());

      /*
      // try one or all centroids
      std::vector<
      for(uint a<=0; a<nb++; a++) {
      DD(a);
      // Create the list of candidates
      std::vector<int> c;
      if (a==nb) { for(uint x=0; x<nb; x++) c.push_back(x); }
      else c.push_back(a);
      DD(a.size());

      // Test size
      double size=0.0;
      for(uint x=0; x<c.size(); c++) { size += labels_size[c[x]]; }
      DD(size);
      */        


      // new potential bifurcation found
      numberOfBifurcation++;
      // If the number of bifurcation is greater than the required one, we stop
      if (numberOfBifurcation > GetMaxNumberOfFoundBifurcation()) {
        found = true;
        //DD("max bif reach");
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
        //DD(indexOfLargest);
        //DD(labels_size[indexOfLargest]);
        SlicePointType c = centroids[indexOfLargest];
        LabelType l = centroids_label[indexOfLargest];
        //DD(l);
        //DD(c);
        centroids.clear();
        centroids.push_back(c);
        centroids_label.clear();
        centroids_label.push_back(l);
      }
    }


    /*    ==> here all centroid are considered as ok.*/
    
    // REMOVE IF CENTROID=1, REPLACE BY >0
    
    // if only one centroids, we change the current image with the current label 
    if (centroids.size() == 1) {
      //DD(centroids_label[0]);
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
        //        DD(currentSlice);
        //        DD("inside BG");
        //        DD(centroids[0]);
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
      //      DD("ZERO");
      found = true;
    }

    if (currentSlice == slices_recon.size()-1) {
      //      DD("end of slices");
      found = true;
    }
    
    if (currentSlice == maxSlice) {
      //      DD("end max slice");
      found = true;
    }

    // iterate
    ++currentSlice;
  } while (!found);

  // End
  StopCurrentStep();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinalVesselsFilter<TImageType>::
TrackVesselsFromPoint(MaskImagePointer & recon, 
                      std::vector<MaskSlicePointer> & slices, 
                      MaskImagePointType seedPoint, 
                      MaskImagePointType pointMaxSlice,
                      LabelType newLabel) {
  StartNewStep("Track the SoughtVessel from the seed point");

  // Find first slice index
  MaskImageIndexType seedIndex;
  recon->TransformPhysicalPointToIndex(seedPoint, seedIndex);
  //  int numberOfBifurcation = 0;
  typedef typename MaskSliceType::PointType SlicePointType;
  SlicePointType previousCentroid;
  previousCentroid[0] = seedPoint[0];
  previousCentroid[1] = seedPoint[1];
  
  // Max slice
  MaskImageIndexType indexMaxSlice;
  recon->TransformPhysicalPointToIndex(pointMaxSlice, indexMaxSlice);  
  uint maxSlice = std::min((uint)indexMaxSlice[2], (uint)slices.size());
  //  DD(maxSlice);

  // Get current label at the seedPoint of interest
  uint currentSlice=seedIndex[2]; 
  bool found = false;
  // LabelType previous_slice_label=recon->GetPixel(seedIndex);
  
  // Currrent label map variable
  typedef itk::ShapeLabelObject< LabelType, 2> LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;
  typename LabelMapType::Pointer labelMap;
  std::vector<typename LabelObjectType::Pointer> shapeObjectsList;
  std::vector<MaskSlicePointer> shapeObjectsSliceList; // keep slice, useful for 'union'
  typename LabelObjectType::Pointer previousShapeObject;

  do {
    // Debug
    std::cout << std::endl;
    //DD(currentSlice);
    ImagePointType c;
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(previousCentroid, m_Mask, currentSlice-1, c);
    //DD(c);

    // Consider current reconstructed slice
    MaskSlicePointer s = slices[currentSlice];
    MaskSlicePointer previous;
    shapeObjectsList.clear();
    shapeObjectsSliceList.clear();

    // Get shape of all labels in the current slice (it is already labelized)

    // Normal -> same CCL with different label
    // PB -> sometimes same label in different CCL ! car growing
    //ADD les deux l+s ? mais avec max entre chaque ccl number  (bof)
    /*
      for each label in s -> map avec label in l; if different -> change
     */
    MaskSlicePointer ll = clitk::Labelize<MaskSliceType>(s, GetBackgroundValue(), true, 1);
    //    writeImage<MaskSliceType>(s, "slice-"+toString(currentSlice)+".mhd");
    //writeImage<MaskSliceType>(ll, "slice-label-"+toString(currentSlice)+".mhd");
    typedef itk::ImageRegionIteratorWithIndex<MaskSliceType> IteratorType;
    IteratorType its(s, s->GetLargestPossibleRegion());
    IteratorType itl(ll, ll->GetLargestPossibleRegion());
    std::map<LabelType, LabelType> labelInL;
    std::map<LabelType, std::map<LabelType, LabelType> > labelToChange;
    its.GoToBegin();
    itl.GoToBegin();
    int currentLabel = newLabel+10;
    while (!its.IsAtEnd()) {
      LabelType labs = its.Get();
      if (labs != GetBackgroundValue()) {
        LabelType labl = itl.Get();
        if (labelInL.find(labs) == labelInL.end()) { // Not found in map, first time
          //          DD("first");
          labelInL[labs] = labl;
          //DD(labs);
          //DD(labl);
        }
        else {
          if (labelInL[labs] != labl) { // I found a labs with a different labl. Store it.
            if (labelToChange[labs].find(labl) == labelToChange[labs].end()) { // if not already found
              //DD("found");
              //DD(labs);
              //DD(labl);
              //DD(labelInL[labs]);
              //DD(currentLabel);
              labelToChange[labs][labl] = currentLabel;
              ++currentLabel;
            }
          }
        }
      }
      ++its;
      ++itl;
    }

    its.GoToBegin();
    itl.GoToBegin();
    while (!its.IsAtEnd()) {
      LabelType labs = its.Get();
      if (labs != GetBackgroundValue()) { // if not BG
        LabelType labl = itl.Get();
        if (labelToChange[labs].find(labl) != labelToChange[labs].end()) { // if some labs can change their label
          its.Set(labelToChange[labs][labl]); // change with the label for <labs-labl> 
        }
      }
      ++its;
      ++itl;
    } // end while 
    
    //    writeImage<MaskSliceType>(s, "slice-final"+toString(currentSlice)+".mhd");


    labelMap = clitk::ComputeLabelMap<MaskSliceType, LabelType>(s, GetBackgroundValue(), true);
    //    DD(labelMap->GetNumberOfLabelObjects());

    // If this is the first slice, get the object that contains the seed
    if (currentSlice == seedIndex[2]) {
      // DD("First slice");
      LabelType l = recon->GetPixel(seedIndex);
      // DD(l);
      shapeObjectsList.push_back(labelMap->GetLabelObject(l));
      shapeObjectsSliceList.push_back(s);
      previous = s;
      previousCentroid = shapeObjectsList[0]->GetCentroid();
      previousShapeObject = shapeObjectsList[0];
    }
    else {
      previous = slices[currentSlice-1];
      // Loop on labels to check if centroid is on the previous contour
      for(uint c=0; c<labelMap->GetNumberOfLabelObjects(); c++) {
        // Get the current label number
        int label = labelMap->GetLabels()[c];
        //DD(label);
        // Get the centroids
        SlicePointType centroid = labelMap->GetLabelObject(label)->GetCentroid();
        // Convert centroid into index in previous slice (same coordinate)
        typename MaskSliceType::IndexType centroidIndex;
        previous->TransformPhysicalPointToIndex(centroid, centroidIndex);
        LabelType labelInPreviousSlice = previous->GetPixel(centroidIndex);
        // if this current centroid was in the current label, we keep it
        //DD(labelInPreviousSlice);
        if (labelInPreviousSlice == newLabel) {
          shapeObjectsList.push_back(labelMap->GetLabelObject(label));
          shapeObjectsSliceList.push_back(s);
        }
      }
    }
    
    
    // Potentially the previous centroid could be inside another
    // labels, we consider i
    typename MaskSliceType::IndexType previousCentroidIndex;
    s->TransformPhysicalPointToIndex(previousCentroid, previousCentroidIndex);
    LabelType l = s->GetPixel(previousCentroidIndex);
    //DD(l);
    if (l != 0) { // if is not the background label
      int index = -1;
      for(uint c=0; c<shapeObjectsList.size(); c++) {
        if (shapeObjectsList[c]->GetLabel() == l) {
          index = c;
        }
      }
      if (index == -1) {
        //DD("not inside");
        shapeObjectsList.push_back(labelMap->GetLabelObject(l));
        shapeObjectsSliceList.push_back(s);
      }
      else {
        // DD("already inside");
      }
    }

    // for(uint c=0; c<shapeObjectsList.size(); c++) {
    //   std::cout << c << " " << shapeObjectsList[c]->GetLabel() << " " 
    //             << shapeObjectsList[c]->GetCentroid() << std::endl;
    // }


    // If several candidates, add one more with the union of all candidates
    MaskSlicePointer temp;
    if (shapeObjectsList.size() > 1) {
      //DD("add union");
      // Copy the slice
      temp = clitk::Clone<MaskSliceType>(s);
      // change label to a single label
      LabelType l = newLabel+1;
      for(uint c=0; c<shapeObjectsList.size(); c++) {
        LabelType ll = shapeObjectsList[c]->GetLabel();
        temp = clitk::SetBackground<MaskSliceType, MaskSliceType>(temp, temp, ll, l, true);
      }
      // Compute Label object properties
      labelMap = clitk::ComputeLabelMap<MaskSliceType, LabelType>(temp, GetBackgroundValue(), true);
      shapeObjectsList.push_back(labelMap->GetLabelObject(l));
      shapeObjectsSliceList.push_back(temp);
    }
    
    /*
    for(uint c=0; c<shapeObjectsList.size(); c++) {
      std::cout << c << " " << shapeObjectsList[c]->GetLabel() << " " 
                << shapeObjectsList[c]->GetCentroid() << std::endl;
    }
    */
    

    for(uint c=0; c<shapeObjectsList.size(); c++) {
      ImagePointType cc;
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(shapeObjectsList[c]->GetCentroid(), m_Mask, currentSlice, cc);
      // std::cout << c << " " << shapeObjectsList[c]->GetLabel() << " " 
      //   //                << shapeObjectsList[c]->GetCentroid() << " " 
      //           << cc << " " 
      //           << shapeObjectsList[c]->GetPhysicalSize() << " " 
      //           << shapeObjectsList[c]->GetRoundness() << std::endl;
    }


    if (shapeObjectsList.size() == 0) {
      found = true;
    }
    else {
      // Heuristic to select the good one. For each candidate, we consider the size
      std::vector<double> sizes;
      std::vector<double> roundness;
      std::vector<size_t> index_sizes;
      std::vector<size_t> index_roundness;    
      double previousSize = previousShapeObject->GetPhysicalSize();
      //DD(previousSize);
      for(uint c=0; c<shapeObjectsList.size(); c++) {
        double s = shapeObjectsList[c]->GetPhysicalSize();
        sizes.push_back(fabs(previousSize-s)/previousSize);
        roundness.push_back(fabs(1.0-shapeObjectsList[c]->GetRoundness()));
        index_sizes.push_back(c);
        index_roundness.push_back(c);
      }
      //DDV(sizes, sizes.size());
      //DDV(roundness, roundness.size());
      // DDV(index_sizes, index_sizes.size());
      // DDV(index_roundness, index_roundness.size());
      sort(index_sizes.begin(), index_sizes.end(), index_cmp<std::vector<double>&>(sizes));
      sort(index_roundness.begin(), index_roundness.end(), index_cmp<std::vector<double>&>(roundness));
      //DDV(index_sizes, index_sizes.size());
      // DDV(index_roundness, index_roundness.size());

      // TEMPORARY GET THE FIRST
      int best = index_sizes[0];
      // if (currentSlice == seedIndex[2]) { // first contour => idiot, first = single contour
      //   best = index_roundness[0]; // best is more round
      // }
      LabelType label = shapeObjectsList[best]->GetLabel();
      //      DD(label);
      s = shapeObjectsSliceList[best];
      s = clitk::SetBackground<MaskSliceType, MaskSliceType>(s, s, label, newLabel, true);

      // HERE

      // It can happend that several CCL share this same label. To
      // prevent this case, we only consider the one that contains
      // the centroid. 

      // TODO -> PREVIOUS CENTROID ???

      MaskSlicePointer temp = clitk::Binarize<MaskSliceType>(s, newLabel, newLabel, GetBackgroundValue(), GetForegroundValue());
      temp = clitk::Labelize<MaskSliceType>(temp, GetBackgroundValue(), true, 1);
      typename MaskSliceType::IndexType centroids_index;
      temp->TransformPhysicalPointToIndex(shapeObjectsList[best]->GetCentroid(), centroids_index);
      typename MaskSliceType::PixelType v = temp->GetPixel(centroids_index); 
        if (v == GetBackgroundValue()) {
          // DD(currentSlice);
          // DD("inside BG");
          //DD(centroids[0]);
          v = 1; // largest one
        }
        
        //DD(v);
        temp = clitk::Binarize<MaskSliceType>(temp, v, v, GetBackgroundValue(), newLabel);      
        //writeImage<MaskSliceType>(temp, "relabel-"+toString(currentSlice)+".mhd");      
        s = temp;

      
      // end 
      slices[currentSlice] = s;
      previousCentroid = shapeObjectsList[best]->GetCentroid();
      previousShapeObject = shapeObjectsList[best];
    }

    ++currentSlice;
    
    if (currentSlice == maxSlice) {
      // DD("end max slice");
      found = true;
    }

  } while (!found);
  
  /*
  // -------------------------
  // If no centroid were found
  if (shapeObjectsList.size() == 0) {
  // Last attempt to find -> check if previous centroid is inside a CCL
  //      if in s -> get value, getcentroid add.
  DD(currentSlice);
  DD("Last change to find");
  DD(previous_slice_label);
  DD(newLabel);
  typename MaskSliceType::IndexType previousCentroidIndex;
  s->TransformPhysicalPointToIndex(previousCentroid, previousCentroidIndex);
  DD(previousCentroid);
  LabelType labelInSlice = s->GetPixel(previousCentroidIndex);
  DD(labelInSlice);
  if (labelInSlice != GetBackgroundValue()) {
  centroids.push_back(labelMap->GetLabelObject(labelInSlice)->GetCentroid());
  centroids_label.push_back(labelInSlice);
  labels_size.push_back(labelMap->GetLabelObject(labelInSlice)->GetPhysicalSize());
  }
  }

  // Some centroid were found
  // If several centroids, we found a bifurcation
  if (centroids.size() > 1) {
  //      int n = centroids.size();
  // Debug point
  std::vector<ImagePointType> d;
  clitk::PointsUtils<MaskImageType>::Convert2DListTo3DList(centroids, currentSlice, m_Mask, d);
  DDV(d, d.size());

  // new potential bifurcation found
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
  DD(indexOfLargest);
  DD(labels_size[indexOfLargest]);
  SlicePointType c = centroids[indexOfLargest];
  LabelType l = centroids_label[indexOfLargest];
  DD(l);
  DD(c);
  centroids.clear();
  centroids.push_back(c);
  centroids_label.clear();
  centroids_label.push_back(l);
  }
  }
  */

  /*    ==> here all centroid are considered as ok.*/
    
  /*
  // REMOVE IF CENTROID=1, REPLACE BY >0
    
  // if only one centroids, we change the current image with the current label 
  if (centroids.size() == 1) {
  DD(centroids_label[0]);
  s = clitk::SetBackground<MaskSliceType, MaskSliceType>(s, s, centroids_label[0], newLabel, true);
  slices[currentSlice] = s;
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
  slices[currentSlice] = s;

  // I need to recompute the centroid if we have removed some
  // connected component.
  clitk::ComputeCentroids<MaskSliceType>(s, GetBackgroundValue(), centroids);
  previousCentroid = centroids[1];
  }

  if (centroids.size() == 0) {
  DD("ZERO");
  found = true;
  }

  if (currentSlice == slices.size()-1) {
  DD("end of slices");
  found = true;
  }
    
  if (currentSlice == maxSlice) {
  DD("end max slice");
  found = true;
  }

  // iterate
  ++currentSlice;
  } while (!found);
  */

  // End
  StopCurrentStep();
}
//--------------------------------------------------------------------


#endif //#define CLITKEXTRACTMEDIASTINALVESSELSFILTER_TXX
