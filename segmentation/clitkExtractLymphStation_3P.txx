
#include <itkBinaryDilateImageFilter.h>
#include <itkMirrorPadImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_SetDefaultValues()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_SI_Limits() 
{
  /*
    Apex of the chest & Carina.
  */
  StartNewStep("[Station 3P] Inf/Sup limits with apex of the chest and carina");

  // Get Carina position (has been determined in Station8)
  m_CarinaZ = GetAFDB()->GetDouble("CarinaZ");
  
  // Get Apex of the Chest. The "lungs" structure is autocroped so we
  // consider the most superior point.
  MaskImagePointer Lungs = GetAFDB()->template GetImage<MaskImageType>("Lungs");
  MaskImageIndexType index = Lungs->GetLargestPossibleRegion().GetIndex();
  index += Lungs->GetLargestPossibleRegion().GetSize();
  MaskImagePointType p;
  Lungs->TransformIndexToPhysicalPoint(index, p);
  p[2] -= 5; // 5 mm below because the autocrop is slightly above the apex
  double m_ApexOfTheChest = p[2];

  /* Crop support :
     Superior limit = carina
     Inferior limit = Apex of the chest */
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                m_CarinaZ,
                                                m_ApexOfTheChest, true,
                                                GetBackgroundValue());

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_Remove_Structures() 
{
  /*
    3 - (sup) remove Aorta, VB, subcl
    not LR subcl ! -> a séparer LR ?
    (inf) remove Eso, Aorta, Azygousvein
  */

  StartNewStep("[Station 3P] Remove some structures.");

  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  clitk::AndNot<MaskImageType>(m_Working_Support, Aorta, GetBackgroundValue());

  MaskImagePointer VertebralBody = GetAFDB()->template GetImage<MaskImageType>("VertebralBody");
  clitk::AndNot<MaskImageType>(m_Working_Support, VertebralBody);

  MaskImagePointer SubclavianArtery = GetAFDB()->template GetImage<MaskImageType>("SubclavianArtery");
  clitk::AndNot<MaskImageType>(m_Working_Support, SubclavianArtery);

  MaskImagePointer Esophagus = GetAFDB()->template GetImage<MaskImageType>("Esophagus");
  clitk::AndNot<MaskImageType>(m_Working_Support, Esophagus);

  MaskImagePointer AzygousVein = GetAFDB()->template GetImage<MaskImageType>("AzygousVein");
  clitk::AndNot<MaskImageType>(m_Working_Support, AzygousVein);

  MaskImagePointer Thyroid = GetAFDB()->template GetImage<MaskImageType>("Thyroid");
  clitk::AndNot<MaskImageType>(m_Working_Support, Thyroid);

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_Ant_Limits() 
{
  /*
    Ant Post limit : 

    Anteriorly, it is in contact with the posterior aspect of Stations
    1–2 superiorly (Fig. 2A–C) and with Stations 4R and 4L inferiorly
    (Fig. 2D–I and 3A–C). The anterior limit of Station 3P is kept
    posterior to the trachea, which is defined by an imaginary
    horizontal line running along the posterior wall of the trachea
    (Fig. 2B,E, red line). Posteriorly, it is delineated along the
    anterior and lateral borders of the vertebral body until an
    imaginary horizontal line running 1 cm posteriorly to the
    anterior border of the vertebral body (Fig. 2D).

    1 - post to the trachea : define an imaginary line just post the
    Trachea and remove what is anterior. 

  */
  StartNewStep("[Station 3P] Ant limits with Trachea ");

  // Load Trachea
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  
  // Crop like current support (need by SliceBySliceSetBackgroundFromLineSeparation after)
  Trachea = 
    clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());
  
  // Slice by slice, determine the most post point of the trachea (A)
  // and consider a second point on the same horizontal line (B)
  std::vector<MaskImagePointType> p_A;
  std::vector<MaskImagePointType> p_B;
  std::vector<typename MaskSliceType::Pointer> slices;
  clitk::ExtractSlices<MaskImageType>(Trachea, 2, slices);
  MaskImagePointType p;
  typename MaskSliceType::PointType sp;
  for(uint i=0; i<slices.size(); i++) {
    // First only consider the main CCL (trachea, not bronchus)
    slices[i] = Labelize<MaskSliceType>(slices[i], 0, true, 100);
    slices[i] = KeepLabels<MaskSliceType>(slices[i], GetBackgroundValue(), 
                                          GetForegroundValue(), 1, 1, true);
    // Find most posterior point
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], GetBackgroundValue(), 
                                                            1, false, sp);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp, Trachea, i, p);
    p_A.push_back(p);
    p[0] -= 20;
    p_B.push_back(p);
  }
  clitk::WriteListOfLandmarks<MaskImageType>(p_A, "trachea-post.txt");

  // Remove Ant part above those lines
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_A, p_B,
                                                                    GetBackgroundValue(), 
                                                                    1, 10);
  // End, release memory
  GetAFDB()->template ReleaseImage<MaskImageType>("Trachea");
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_Post_Limits() 
{
  /*
    Ant Post limit : 

    Anteriorly, it is in contact with the posterior aspect of Stations
    1–2 superiorly (Fig. 2A–C) and with Stations 4R and 4L inferiorly
    (Fig. 2D–I and 3A–C). The anterior limit of Station 3P is kept
    posterior to the trachea, which is defined by an imaginary
    horizontal line running along the posterior wall of the trachea
    (Fig. 2B,E, red line). Posteriorly, it is delineated along the
    anterior and lateral borders of the vertebral body until an
    imaginary horizontal line running 1 cm posteriorly to the
    anterior border of the vertebral body (Fig. 2D).

    2 - post to the trachea : define an imaginary line just post the
    Trachea and remove what is anterior. 

  */
  StartNewStep("[Station 3P] Post limits with VertebralBody ");

  // Load VertebralBody
  MaskImagePointer VertebralBody = GetAFDB()->template GetImage<MaskImageType>("VertebralBody");
  
  // Crop like current support (need by SliceBySliceSetBackgroundFromLineSeparation after)
  VertebralBody = clitk::ResizeImageLike<MaskImageType>(VertebralBody, m_Working_Support, GetBackgroundValue());
  
  writeImage<MaskImageType>(VertebralBody, "vb.mhd");
  
  // Compute VertebralBody most Ant position (again because slices
  // changes). Slice by slice, determine the most post point of the
  // trachea (A) and consider a second point on the same horizontal
  // line (B)
  std::vector<MaskImagePointType> p_A;
  std::vector<MaskImagePointType> p_B;
  std::vector<typename MaskSliceType::Pointer> slices;
  clitk::ExtractSlices<MaskImageType>(VertebralBody, 2, slices);
  MaskImagePointType p;
  typename MaskSliceType::PointType sp;
  for(uint i=0; i<slices.size(); i++) {
    // Find most anterior point
    bool found = clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], GetBackgroundValue(), 
                                                                         1, true, sp);
    
    // If the VertebralBody stop superiorly before the end of
    // m_Working_Support, we consider the same previous point.
    if (!found) {
      p = p_A.back();
      p[2] += VertebralBody->GetSpacing()[2];
      p_A.push_back(p);
      p = p_B.back();
      p[2] += VertebralBody->GetSpacing()[2];
      p_B.push_back(p);
    }
    else {
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp, VertebralBody, i, p);
      p[1] += 10; // Consider 10 mm more post
      p_A.push_back(p);
      p[0] -= 20;
      p_B.push_back(p);
    }
  }
  clitk::WriteListOfLandmarks<MaskImageType>(p_A, "vb-ant.txt");
  

  // Remove Ant part above those lines
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_A, p_B,
                                                                    GetBackgroundValue(), 
                                                                    1, -10);
  writeImage<MaskImageType>(m_Working_Support, "a.mhd");

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_LR_sup_Limits() 
{
  /*
    "On the right side, the limit is deﬁned by the air–soft-tissue
    interface. On the left side, it is deﬁned by the air–tissue
    interface superiorly (Fig. 2A–C) and the aorta inferiorly
    (Figs. 2D–I and 3A–C)."

    sup : 
    Resizelike support : Trachea, SubclavianArtery
    Trachea, slice by slice, get centroid trachea
    SubclavianArtery, slice by slice, CCL
    prendre la 1ère à L et R, not at Left
    
  */
  StartNewStep("[Station 3P] Left/Right limits (superior part) ");

  // Load structures
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  MaskImagePointer SubclavianArtery = GetAFDB()->template GetImage<MaskImageType>("SubclavianArtery");
  
  // Crop like current support
  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());
  SubclavianArtery = clitk::ResizeImageLike<MaskImageType>(SubclavianArtery, m_Working_Support, GetBackgroundValue());
  
  writeImage<MaskImageType>(Trachea, "tr.mhd");
  writeImage<MaskImageType>(SubclavianArtery, "sca.mhd");
  
  // Get list of slices
  std::vector<MaskSlicePointer> slices_support;
  std::vector<MaskSlicePointer> slices_trachea;
  std::vector<MaskSlicePointer> slices_subclavianartery;
  clitk::ExtractSlices<MaskImageType>(m_Working_Support, 2, slices_support);
  clitk::ExtractSlices<MaskImageType>(Trachea, 2, slices_trachea);
  clitk::ExtractSlices<MaskImageType>(SubclavianArtery, 2, slices_subclavianartery);

  // Loop on slices
  std::vector<MaskImagePointType> points;
  MaskImagePointType p;
  for(uint i=0; i<slices_support.size(); i++) {
    // Get Trachea centroid
    std::vector<typename MaskSliceType::PointType> centroids;
    typename MaskSliceType::PointType c;
    ComputeCentroids<MaskSliceType>(slices_trachea[i], GetBackgroundValue(), centroids);
    c = centroids[1];

    // [debug] Store point
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(centroids[1], Trachea, i, p);
    points.push_back(p);
    
    // Get Right and Left CCL in SubclavianArtery
    slices_subclavianartery[i] = Labelize<MaskSliceType>(slices_subclavianartery[i], 0, true, 10);
    ComputeCentroids<MaskSliceType>(slices_subclavianartery[i], GetBackgroundValue(), centroids);

    if (centroids.size() > 1) {
      // Determine the one at Right/Left -> first after Trachea
      // centroid 
      typename MaskSliceType::PointType right;
      typename MaskSliceType::PointType left;
      int label_right=-1;
      int label_left=-1;
      right[0] = c[0]-100;
      left[0] = c[0]+100;
      for(uint j=1; j<centroids.size(); j++) {
        if (centroids[j][0] < c[0]) { // At Right of Trachea centroid
          if (centroids[j][0] >= right[0]) {
            right = centroids[j];
            label_right = j;
          }
        }
        if (centroids[j][0] > c[0]) { // At Left of Trachea centroid
          if (centroids[j][0] <= left[0]) {
            left = centroids[j];
            label_left = j;
          }
        }
      }

      if (label_right != -1) {
    
        // Debug points
        clitk::PointsUtils<MaskImageType>::Convert2DTo3D(centroids[label_right], SubclavianArtery, i, p);
        points.push_back(p);

        // Set Background and ForegroundValue according to label_right
        MaskSlicePointer object = 
          clitk::Binarize<MaskSliceType>(slices_subclavianartery[i], label_right, label_right, 
                                         GetBackgroundValue(), GetForegroundValue());
      
        // Relative Position : not at Right
        typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskSliceType> RelPosFilterType;
        typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        relPosFilter->WriteStepFlagOff();
        relPosFilter->SetBackgroundValue(GetBackgroundValue());
        relPosFilter->SetInput(slices_support[i]); 
        relPosFilter->SetInputObject(object); 
        relPosFilter->AddOrientationTypeString("R");
        relPosFilter->SetInverseOrientationFlag(true);
        //      relPosFilter->SetIntermediateSpacing(3);
        relPosFilter->SetIntermediateSpacingFlag(false);
        relPosFilter->SetFuzzyThreshold(0.7);
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->Update();
        slices_support[i] = relPosFilter->GetOutput();

        // Relative Position : not Anterior
        relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        relPosFilter->WriteStepFlagOff();
        relPosFilter->SetBackgroundValue(GetBackgroundValue());
        relPosFilter->SetInput(slices_support[i]); 
        relPosFilter->SetInputObject(object); 
        relPosFilter->AddOrientationTypeString("A");
        relPosFilter->SetInverseOrientationFlag(true);
        //      relPosFilter->SetIntermediateSpacing(3);
        relPosFilter->SetIntermediateSpacingFlag(false);
        relPosFilter->SetFuzzyThreshold(0.7);
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->Update();
        slices_support[i] = relPosFilter->GetOutput();

      } // End RelativePosition for Right


      if (label_left != -1) {
    
        // Debug points
        clitk::PointsUtils<MaskImageType>::Convert2DTo3D(centroids[label_left], SubclavianArtery, i, p);
        points.push_back(p);

        // Set Background and ForegroundValue according to label_right
        MaskSlicePointer object = 
          clitk::Binarize<MaskSliceType>(slices_subclavianartery[i], label_left, label_left, 
                                         GetBackgroundValue(), GetForegroundValue());
      
        // Relative Position : not at Right
        typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskSliceType> RelPosFilterType;
        typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        relPosFilter->WriteStepFlagOff();
        relPosFilter->SetBackgroundValue(GetBackgroundValue());
        relPosFilter->SetInput(slices_support[i]); 
        relPosFilter->SetInputObject(object); 
        relPosFilter->AddOrientationTypeString("L");
        relPosFilter->SetInverseOrientationFlag(true);
        //      relPosFilter->SetIntermediateSpacing(3);
        relPosFilter->SetIntermediateSpacingFlag(false);
        relPosFilter->SetFuzzyThreshold(0.7);
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->Update();
        slices_support[i] = relPosFilter->GetOutput();

        // Relative Position : not Anterior
        relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        relPosFilter->WriteStepFlagOff();
        relPosFilter->SetBackgroundValue(GetBackgroundValue());
        relPosFilter->SetInput(slices_support[i]); 
        relPosFilter->SetInputObject(object); 
        relPosFilter->AddOrientationTypeString("A");
        relPosFilter->SetInverseOrientationFlag(true);
        //      relPosFilter->SetIntermediateSpacing(3);
        relPosFilter->SetIntermediateSpacingFlag(false);
        relPosFilter->SetFuzzyThreshold(0.7);
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->Update();
        slices_support[i] = relPosFilter->GetOutput();

      }

    
    } // if centroids.size > 1
  } // End loop slices

  // Joint slices
  m_Working_Support = clitk::JoinSlices<MaskImageType>(slices_support, m_Working_Support, 2);

  // Save list points
  clitk::WriteListOfLandmarks<MaskImageType>(points, "subcl-lr.txt");


  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_LR_inf_Limits() 
{
  /*
    "On the right side, the limit is deﬁned by the air–soft-tissue
    interface. On the left side, it is deﬁned by the air–tissue
    interface superiorly (Fig. 2A–C) and the aorta inferiorly
    (Figs. 2D–I and 3A–C)."

    inf : not Right to Azygousvein    
  */
  StartNewStep("[Station 3P] Left/Right limits (inferior part) ");

  // Load structures
  MaskImagePointer AzygousVein = GetAFDB()->template GetImage<MaskImageType>("AzygousVein");

  typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->WriteStepFlagOff();
  relPosFilter->SetBackgroundValue(GetBackgroundValue());
  relPosFilter->SetInput(m_Working_Support); 
  relPosFilter->SetInputObject(AzygousVein); 
  relPosFilter->AddOrientationTypeString("R");
  relPosFilter->SetInverseOrientationFlag(true);
  //      relPosFilter->SetIntermediateSpacing(3);
  relPosFilter->SetIntermediateSpacingFlag(false);
  relPosFilter->SetFuzzyThreshold(0.7);
  relPosFilter->AutoCropFlagOn();
  relPosFilter->Update();
  m_Working_Support = relPosFilter->GetOutput();

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------
