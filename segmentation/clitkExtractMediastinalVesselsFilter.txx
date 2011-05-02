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
#include "clitkAutoCropFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkMorphoMathFilter.h"

// itk
#include <itkBinaryThresholdImageFilter.h>
#include <itkGrayscaleDilateImageFilter.h>
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
  SetThreshold(140);
  SetTemporaryForegroundValue(1);
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
  // Sup-Inf crop -> Carina
  CropSupInf();  

  // ------------------------------------------------------------------
  // Binarize
  StartNewStep("Binarize with treshold = "+toString(GetThreshold()));
  typedef itk::BinaryThresholdImageFilter<ImageType, MaskImageType> BinarizeFilterType; 
  typename BinarizeFilterType::Pointer binarizeFilter = BinarizeFilterType::New();
  binarizeFilter->SetInput(m_Input);
  binarizeFilter->SetLowerThreshold(GetThreshold());
  binarizeFilter->SetInsideValue(GetTemporaryForegroundValue());
  binarizeFilter->SetOutsideValue(GetBackgroundValue());
  binarizeFilter->Update();
  m_Mask = binarizeFilter->GetOutput();
  clitk::writeImage<MaskImageType>(m_Mask, "m.mhd");
  StopCurrentStep<MaskImageType>(m_Mask);

  // Keep main CCL ? 
  m_Mask = clitk::Labelize<MaskImageType>(m_Mask, GetBackgroundValue(), false, 10);
  m_Mask = KeepLabels<MaskImageType>(m_Mask, GetBackgroundValue(), GetTemporaryForegroundValue(), 1, 1, true);
  clitk::writeImage<MaskImageType>(m_Mask, "m2.mhd");
  
  // ------------------------------------------------------------------
  // Extract slices
  StartNewStep("Detect vessels (slice by slice reconstruction)");
  std::vector<MaskSlicePointer> slices_mask;
  clitk::ExtractSlices<MaskImageType>(m_Mask, 2, slices_mask);
  
  DD(slices_mask.size());
  
  std::vector<MaskSlicePointer> debug_eroded;
  std::vector<MaskSlicePointer> debug_labeled;
  std::vector<MaskSlicePointer> debug_slabeled;
  
  int radius = 3;
  DD(radius); // TO PUT IN OPTION

  // ------------------------------------------------------------------
  // Loop Slice by Slice -> erode find CCL and reconstruct
  clitk::MorphoMathFilter<MaskSliceType>::Pointer f= clitk::MorphoMathFilter<MaskSliceType>::New();
  for(uint i=0; i<slices_mask.size(); i++) {
    // Erosion
    f->SetInput(slices_mask[i]);
    f->SetBackgroundValue(GetBackgroundValue());
    f->SetForegroundValue(GetTemporaryForegroundValue());
    f->SetRadius(radius);
    f->SetOperationType(0); // Erode
    f->VerboseFlagOff();
    f->Update();
    MaskSlicePointer eroded = f->GetOutput();
    //    writeImage<MaskSliceType>(eroded, "er-"+toString(i)+".mhd");
    debug_eroded.push_back(eroded);
      
    // CCL
    int nb;
    MaskSlicePointer labeled = 
      clitk::LabelizeAndCountNumberOfObjects<MaskSliceType>(eroded, GetBackgroundValue(), false, 1, nb);

    // Relabel, large CCL with large label number
    for(int n=nb; n>0; n--) {
      //        DD(n);
      int li = n;
      int lo = 2*(nb+1)-li;
      labeled = clitk::SetBackground<MaskSliceType, MaskSliceType>(labeled, labeled, li, lo, true);
    }
    debug_labeled.push_back(labeled);

    // Create kernel for GrayscaleDilateImageFilter
    typedef itk::BinaryBallStructuringElement<MaskSliceType::PixelType,MaskSliceType::ImageDimension > KernelType;
    KernelType k;
    k.SetRadius(radius+1);
    k.CreateStructuringElement();
    
    // Keep the MAX -> we prefer the opposite su change the label
    typedef itk::GrayscaleDilateImageFilter<MaskSliceType, MaskSliceType, KernelType> FilterType;
    FilterType::Pointer m = FilterType::New();
    m->SetKernel(k);
    m->SetInput(labeled);
    // DD(m->GetAlgorithm());
    // m->SetAlgorithm(3);
    m->Update();
    MaskSlicePointer s = m->GetOutput();


    // Remove Initial BG
    s = clitk::SetBackground<MaskSliceType, MaskSliceType>(s, slices_mask[i], 
                                                   GetBackgroundValue(), GetBackgroundValue(), true);

    m_slice_recon.push_back(s);
  } // end loop
  DD("end loop");
  
  MaskImageType::Pointer eroded = clitk::JoinSlices<MaskImageType>(debug_eroded, m_Mask, 2);
  clitk::writeImage<MaskImageType>(eroded, "eroded.mhd");

  DD("l");
  MaskImageType::Pointer l = clitk::JoinSlices<MaskImageType>(debug_labeled, m_Mask, 2);
  clitk::writeImage<MaskImageType>(l, "labeled.mhd");
  
  DD("r");
  MaskImageType::Pointer r = clitk::JoinSlices<MaskImageType>(m_slice_recon, m_Mask, 2);
  clitk::writeImage<MaskImageType>(r, "recon.mhd");
  
  // ------------------------------------------------------------------
  // Loop Slice by Slice -> BCA not found yet
  /*  MaskImagePointType BCA_p;
  GetAFDB()->GetPoint3D("BrachioCephalicArteryFirstInferiorPoint", BCA_p);
  DD(BCA_p);
  MaskImagePointType bif1;
  MaskImagePointType bif2;
  TrackBifurcationFromPoint(r, BCA_p, bif1, bif2);
  DD(bif1);
  DD(bif2);
  */
  // Find max label
  typedef itk::MinimumMaximumImageCalculator<MaskImageType> MinMaxFilterType;
  MinMaxFilterType::Pointer ff = MinMaxFilterType::New();
  ff->SetImage(r);
  ff->ComputeMaximum();
  LabelType newLabel = ff->GetMaximum()+1; 
  DD(newLabel);

  // Get all centroids of the first slice
  std::vector<MaskSlicePointType> centroids2D;
  clitk::ComputeCentroids<MaskSliceType>(m_slice_recon[0], GetBackgroundValue(), centroids2D);
  DD(centroids2D.size());
  std::vector<MaskImagePointType> bifurcations;
  clitk::PointsUtils<MaskImageType>::Convert2DListTo3DList(centroids2D, 0, r, bifurcations);  
  DD(bifurcations.size());
  for(uint i=1; i<bifurcations.size()+1; i++) {
    DD(i);
    DD(bifurcations.size());
    TrackBifurcationFromPoint(r, m_slice_recon, bifurcations[i], newLabel+i, bifurcations);
    DD("end track");
    DD(bifurcations.size());
    MaskImageType::Pointer rr = clitk::JoinSlices<MaskImageType>(m_slice_recon, m_Mask, 2);
    clitk::writeImage<MaskImageType>(rr, "recon"+toString(i)+".mhd");
  }

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
  DD("GenerateData");
  // Final Step -> graft output (if SetNthOutput => redo)
  MaskImagePointer BrachioCephalicArtery = 
    GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicArtery");
  this->GraftNthOutput(0, BrachioCephalicArtery);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinalVesselsFilter<TImageType>::
CropSupInf() { 
  StartNewStep("Inf/Sup limits (carina) and crop with mediastinum");
  // Get Trachea and Carina
  MaskImagePointer Trachea = GetAFDB()->template GetImage <MaskImageType>("Trachea");  
  
  // Get or compute Carina
  double m_CarinaZ;
  // Get Carina Z position
  MaskImagePointer Carina = GetAFDB()->template GetImage<MaskImageType>("Carina");
  
  std::vector<MaskImagePointType> centroids;
  clitk::ComputeCentroids<MaskImageType>(Carina, GetBackgroundValue(), centroids);
  m_CarinaZ = centroids[1][2];
  // DD(m_CarinaZ);
  // add one slice to include carina ?
  m_CarinaZ += Carina->GetSpacing()[2];
  // We dont need Carina structure from now
  Carina->Delete();
  GetAFDB()->SetDouble("CarinaZ", m_CarinaZ);
  
  // Crop
  m_Input = clitk::CropImageRemoveLowerThan<ImageType>(m_Input, 2, 
                                                       m_CarinaZ, false, GetBackgroundValue());  

  // Crop not post to centroid
  double m_CarinaY = centroids[1][1];
  DD(m_CarinaY);
  m_Input = clitk::CropImageRemoveGreaterThan<ImageType>(m_Input, 1, // OLD ABOVE
                                                         m_CarinaY, false, GetBackgroundValue());  
  // Crop not ant to centroid
  m_Input = clitk::CropImageRemoveLowerThan<ImageType>(m_Input, 1, 
                                                         m_CarinaY-80, false, GetBackgroundValue());  

  // AutoCrop with Mediastinum
  m_Mediastinum  = GetAFDB()->template GetImage<MaskImageType>("Mediastinum");
  // Resize like input (sup to carina)
  m_Mediastinum = clitk::ResizeImageLike<MaskImageType>(m_Mediastinum, m_Input, GetBackgroundValue());
  // Auto crop
  m_Mediastinum = clitk::AutoCrop<MaskImageType>(m_Mediastinum, GetBackgroundValue());
  // Resize input
  m_Input = clitk::ResizeImageLike<ImageType>(m_Input, m_Mediastinum, GetBackgroundValue());

  // End
  StopCurrentStep<ImageType>(m_Input);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractMediastinalVesselsFilter<TImageType>::
//SearchBrachioCephalicArtery(int & BCA_first_slice, LabelType & BCA_first_label) { 
TrackBifurcationFromPoint(MaskImagePointer & recon, 
                          std::vector<MaskSlicePointer> & slices_recon, 
                          MaskImagePointType point3D, 
                          LabelType newLabel,
                          std::vector<MaskImagePointType> & bifurcations) {
  StartNewStep("Search for BCA first slice and label");
  DD(newLabel);

  // Extract slices
  //  std::vector<MaskSlicePointer> slices_recon;
  //clitk::ExtractSlices<MaskImageType>(recon, 2, slices_recon);

  // Find first slice
  MaskImageIndexType index;
  recon->TransformPhysicalPointToIndex(point3D, index);
  DD(point3D);
  DD(index);

  uint i=index[2]; 
  bool found = false;
  LabelType previous_largest_label=recon->GetPixel(index);
  DD(previous_largest_label);
  do {
    DD(i);
    // Consider current reconstructed slice
    MaskSlicePointer s = slices_recon[i];
    MaskSlicePointer previous;
    if (i==index[2]) previous = s;
    else previous = slices_recon[i-1];
    
    // Get centroids of the labels in the current slice
    typedef typename MaskSliceType::PointType SlicePointType;
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
    for(uint c=0; c<labelMap->GetNumberOfLabelObjects(); c++) {
      int label = labelMap->GetLabels()[c];
      DD(label);
      SlicePointType center = labelMap->GetLabelObject(label)->GetCentroid();
      SlicePointType center_previous = center;
      center_previous[2] -= m_Input->GetSpacing()[2];      
      // Get label into previous slice
      typename MaskSliceType::IndexType index;
      previous->TransformPhysicalPointToIndex(center_previous, index);
      LabelType l = previous->GetPixel(index);
      DD(l);
      if (l == previous_largest_label) {
        centroids.push_back(center);
        centroids_label.push_back(label);
      }
    }
    DD(centroids.size());
    
    // If several centroids, we found a bifurcation
    if (centroids.size() > 1) {
      found = true;
      for(uint c=0; c<centroids.size(); c++) {
        ImagePointType bif;
        clitk::PointsUtils<MaskImageType>::Convert2DTo3D(centroids[c], m_Mask, i, bif);
        bifurcations.push_back(bif);
        s = clitk::SetBackground<MaskSliceType, MaskSliceType>(s, s, centroids_label[c], newLabel+c+1, true);
        //        slices_recon[i] = s; // (useful ?)
      }
      DD("FOUND");
    }
    
    // if only one centroids, we change the current image with the current label 
    if (centroids.size() == 1) {
      s = clitk::SetBackground<MaskSliceType, MaskSliceType>(s, s, centroids_label[0], newLabel, true);
      previous_largest_label = newLabel;
      /*typedef itk::BinaryThresholdImageFilter<MaskSliceType, MaskSliceType> BinarizeFilterType; 
      typename BinarizeFilterType::Pointer binarizeFilter = BinarizeFilterType::New();
      binarizeFilter->SetInput(s);
      binarizeFilter->SetLowerThreshold(centroids_label[0]);
      binarizeFilter->SetUpperThreshold(centroids_label[0]+1);
      binarizeFilter->SetInsideValue(previous_largest_label);
      binarizeFilter->SetOutsideValue(GetBackgroundValue());
      binarizeFilter->Update();
      s = binarizeFilter->GetOutput();*/
      slices_recon[i] = s; // (not useful ?)
    }

    if (centroids.size() == 0) {
      DD("no centroid, I stop");
      found = true;
    }

    if (i == slices_recon.size()-1) found = true;

    // iterate
    ++i;
  } while (!found);


  //MaskImageType::Pointer rr = clitk::JoinSlices<MaskImageType>(slices_recon, m_Mask, 2);
  //clitk::writeImage<MaskImageType>(rr, "recon2.mhd");

  // End
  StopCurrentStep();
}
//--------------------------------------------------------------------



#endif //#define CLITKEXTRACTMEDIASTINALVESSELSFILTER_TXX
