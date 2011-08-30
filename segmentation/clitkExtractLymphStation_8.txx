
#include <itkBinaryDilateImageFilter.h>
#include <itkMirrorPadImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_SetDefaultValues()
{
  MaskImagePointType p;
  p[0] = 15; p[1] = 2; p[2] = 1;
  SetEsophagusDiltationForAnt(p);
  p[0] = 5; p[1] = 10; p[2] = 1;
  SetEsophagusDiltationForRight(p);
  SetFuzzyThreshold("8", "Esophagus", 0.5);
  SetInjectedThresholdForS8(150);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_8() 
{
  if (CheckForStation("8")) {
    ExtractStation_8_SI_Limits();         // OK, validated
    ExtractStation_8_Ant_Limits();        // OK, validated
    ExtractStation_8_Left_Sup_Limits();   // OK, validated
    ExtractStation_8_Left_Inf_Limits();   // OK, validated
    ExtractStation_8_Single_CCL_Limits(); // OK, validated
    ExtractStation_8_Remove_Structures(); // OK, validated

    // Store image filenames into AFDB 
    writeImage<MaskImageType>(m_ListOfStations["8"], "seg/Station8.mhd");
    GetAFDB()->SetImageFilename("Station8", "seg/Station8.mhd");  
    WriteAFDB();
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_SI_Limits() 
{
  /*
    Station 8: paraeosphageal nodes
    
    "Superiorly, Station 8 begins at the level of the carina and,
    therefore, abuts Station 3P (Fig. 3C). Inferiorly, the lower limit
    of Station 8 was not speciﬁed in the Mountain and Dresler
    classiﬁcation (1). We delineate this volume until it reaches the
    gastroesphogeal junction. "
    
    => new classification IASCL 2009: 

    "Lower border: the diaphragm"
    
    "Upper border: the upper border of the lower lobe bronchus on the
    left; the lower border of the bronchus intermedius on the right"

  */
  StartNewStep("[Station8] Sup/Inf limits with LeftLower/RightMiddle Lobe and diaphragm");

  /* -----------------------------------------------
     NEW SUPERIOR LIMIT = LeftLowerLobeBronchus /
     RightMiddleLobeBronchus See FindLineForS7S8Separation
     -----------------------------------------------
  */
  ImagePointType A;
  ImagePointType B;
  FindLineForS7S8Separation(A, B);

  // add one slice to be adjacent to Station7
  B[2] += m_Working_Support->GetSpacing()[2];
  A[2] += m_Working_Support->GetSpacing()[2];

  // Use the line to remove the inferior part
  m_Working_Support =
    SliceBySliceSetBackgroundFromSingleLine<MaskImageType>(m_Working_Support, 
                                                           GetBackgroundValue(), A, B, 2, 0, true);
  
  /* -----------------------------------------------
     INFERIOR LIMIT = Diaphragm
     -----------------------------------------------
  */  

  // Found most inferior part of the lung
  MaskImagePointer Lungs = GetAFDB()->template GetImage<MaskImageType>("Lungs");
  // It should be already croped, so I took the origin and add 10mm above 
  m_DiaphragmInferiorLimit = Lungs->GetOrigin()[2]+10;
  GetAFDB()->template ReleaseImage<MaskImageType>("Lungs");

  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                m_DiaphragmInferiorLimit,
                                                B[2], true,
                                                GetBackgroundValue());
  // Done.
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_Ant_Limits() 
{
  //--------------------------------------------------------------------
  StartNewStep("[Station8] Ant part: not post to Esophagus");
  /*
    Consider Esophagus, dilate it and remove ant part. It remains part
    on L & R, than can be partly removed by cutting what remains at
    right of vertebral body.
  */
  
  // Get Esophagus
  m_Esophagus = GetAFDB()->template GetImage<MaskImageType>("Esophagus");

  // In images from the original article, Atlas – UM, the oesophagus
  //was included in nodal stations 3p and 8.  Having said that, in the
  //description for station 8, it indicates that “the delineation of
  //station 8 is limited to the soft tissues surrounding the
  //oesophagus”.  In the recent article, The IASLC Lung Cancer Staging
  //Project (J Thorac Oncol 4:5, 568-77), the images drawn by
  //Dr. Aletta Frasier exclude this structure.  From an oncological
  //prospective, the oesophagus should be excluded from these nodal
  //stations.

  // Resize Esophagus like current support
  m_Esophagus = 
    clitk::ResizeImageLike<MaskImageType>(m_Esophagus, m_Working_Support, GetBackgroundValue()); // Needed ?

  // Dilate to keep only not Anterior positions
  MaskImagePointType radiusInMM = GetEsophagusDiltationForAnt();
  m_Esophagus = clitk::Dilate<MaskImageType>(m_Esophagus, 
                                             radiusInMM, 
                                             GetBackgroundValue(), 
                                             GetForegroundValue(), true);
  // Keep what is Posterior to Esophagus
  typedef clitk::SliceBySliceRelativePositionFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->WriteStepFlagOff();
  relPosFilter->SetInput(m_Working_Support);
  relPosFilter->SetInputObject(m_Esophagus);
  relPosFilter->AddOrientationTypeString("PostTo");
  //  relPosFilter->InverseOrientationFlagOff();
  relPosFilter->SetDirection(2); // Z axis
  relPosFilter->UniqueConnectedComponentBySliceOff();
  relPosFilter->SetIntermediateSpacing(3);
  relPosFilter->IntermediateSpacingFlagOn();
  relPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("8", "Esophagus"));
  relPosFilter->RemoveObjectFlagOff(); // Do not exclude here because it is dilated
  relPosFilter->CombineWithOrFlagOff(); // NO !
  relPosFilter->IgnoreEmptySliceObjectFlagOn();
  relPosFilter->Update();
  m_Working_Support = relPosFilter->GetOutput();

  // AutoCrop (OR SbS ?)
  m_Working_Support = clitk::AutoCrop<MaskImageType>(m_Working_Support, GetBackgroundValue()); 

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_Left_Sup_Limits() 
{
  //--------------------------------------------------------------------
  StartNewStep("[Station8] Left limits: remove Left to line from Aorta to PulmonaryTrunk");

  /*
    We consider a line from Left part of Aorta to left part of
    PulmonaryTrunk and remove what is at Left.
  */
    
  // Load the structures
  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  MaskImagePointer PulmonaryTrunk = GetAFDB()->template GetImage<MaskImageType>("PulmonaryTrunk");
  
  // Resize like the PT and define the slices
  MaskImagePointType min, max;
  clitk::GetMinMaxPointPosition<MaskImageType>(PulmonaryTrunk, min, max);
  Aorta = clitk::CropImageAlongOneAxis<MaskImageType>(Aorta, 2, min[2], max[2], false, GetBackgroundValue());
  std::vector<MaskSlicePointer> slices_aorta;
  clitk::ExtractSlices<MaskImageType>(Aorta, 2, slices_aorta);
  std::vector<MaskSlicePointer> slices_PT;
  clitk::ExtractSlices<MaskImageType>(PulmonaryTrunk, 2, slices_PT);
  
  // Find the points at left
  std::vector<MaskImagePointType> p_A;
  std::vector<MaskImagePointType> p_B;
  MaskImagePointType pA;
  MaskImagePointType pB;
  for(uint i=0; i<slices_PT.size(); i++) {    
    typename MaskSliceType::PointType ps;
    // In Aorta (assume a single CCL)
    bool found = 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices_aorta[i], GetBackgroundValue(), 0, false, ps);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(ps, Aorta, i, pA);
    
    if (found) {
      // In PT : generally 2 CCL, we keep the most at left
      found = 
        clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices_PT[i], GetBackgroundValue(), 0, false, ps);
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(ps, PulmonaryTrunk, i, pB);
    }
    
    if (found) {
      p_A.push_back(pA);
      p_B.push_back(pB);
    }
  }
  clitk::WriteListOfLandmarks<MaskImageType>(p_A, "S8-Aorta-Left-points.txt");
  clitk::WriteListOfLandmarks<MaskImageType>(p_B, "S8-PT-Left-points.txt");

  // Remove part at Left  
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_A, p_B,
                                                                    GetBackgroundValue(), 
                                                                    0, -10);

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_Single_CCL_Limits() 
{
  //--------------------------------------------------------------------
  StartNewStep("[Station8 or 3P] Slice by slice, keep a single CCL (the closest to VertebralBody)");

  // Consider slices
  std::vector<typename MaskSliceType::Pointer> slices;
  std::vector<typename MaskSliceType::Pointer> slices_vb;
  clitk::ExtractSlices<MaskImageType>(m_Working_Support, 2, slices);
  MaskImagePointer VertebralBody = 
    GetAFDB()->template GetImage <MaskImageType>("VertebralBody");  
  VertebralBody = clitk::ResizeImageLike<MaskImageType>(VertebralBody, m_Working_Support, GetBackgroundValue());
  clitk::ExtractSlices<MaskImageType>(VertebralBody, 2, slices_vb);

  for(uint i=0; i<slices.size(); i++) {
    // Decompose in labels
    slices[i] = Labelize<MaskSliceType>(slices[i], 0, true, 100);
    // Compute centroids coordinate
    std::vector<typename MaskSliceType::PointType> centroids;
    std::vector<typename MaskSliceType::PointType> c;
    clitk::ComputeCentroids<MaskSliceType>(slices[i], GetBackgroundValue(), centroids);
    clitk::ComputeCentroids<MaskSliceType>(slices_vb[i], GetBackgroundValue(), c);
    if ((c.size() > 1) && (centroids.size() > 1)) {
      // keep the one which is the closer to vertebralbody centroid
      double distance=1000000;
      int index=0;
      for(uint j=1; j<centroids.size(); j++) {
        double d = centroids[j].EuclideanDistanceTo(c[1]);
        if (d<distance) {
          distance = d;
          index = j;
        }
      }
      // remove all others label
      slices[i] = KeepLabels<MaskSliceType>(slices[i], GetBackgroundValue(), 
                                            GetForegroundValue(), index, index, true);
    }
  }
  m_Working_Support = clitk::JoinSlices<MaskImageType>(slices, m_Working_Support, 2);
  
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_Left_Inf_Limits() 
{
  //--------------------------------------------------------------------
  StartNewStep("[Station8] Left limits around esophagus with lines from VertebralBody-Aorta-Esophagus");

  // Estract slices for current support for slice by slice processing
  std::vector<typename MaskSliceType::Pointer> slices;
  clitk::ExtractSlices<MaskImageType>(m_Working_Support, 2, slices);

  // Remove what is outside the mediastinum in this enlarged Esophagus -> it allows to select
  // 'better' extrema points (not too post).
  MaskImagePointer Lungs = GetAFDB()->template GetImage<MaskImageType>("Lungs");
  clitk::AndNot<MaskImageType>(m_Esophagus, Lungs, GetBackgroundValue());
  GetAFDB()->template ReleaseImage<MaskImageType>("Lungs");

  // Estract slices of Esophagus (resize like support before to have the same set of slices)
  MaskImagePointer EsophagusForSlice = 
    clitk::ResizeImageLike<MaskImageType>(m_Esophagus, m_Working_Support, GetBackgroundValue());
  std::vector<typename MaskSliceType::Pointer> eso_slices;
  clitk::ExtractSlices<MaskImageType>(EsophagusForSlice, 2, eso_slices);

  // Estract slices of Vertebral (resize like support before to have the same set of slices)
  MaskImagePointer VertebralBody = GetAFDB()->template GetImage<MaskImageType>("VertebralBody");
  VertebralBody = clitk::ResizeImageLike<MaskImageType>(VertebralBody, m_Working_Support, GetBackgroundValue());
  // Remove what is outside the support to not consider what is to
  // posterior in the VertebralBody (post the horizontal line)
  clitk::And<MaskImageType>(VertebralBody, m_Working_Support, GetBackgroundValue());
  // writeImage<MaskImageType>(VertebralBody, "vb.mhd");
  std::vector<typename MaskSliceType::Pointer> vert_slices;
  clitk::ExtractSlices<MaskImageType>(VertebralBody, 2, vert_slices);

  // Estract slices of Aorta (resize like support before to have the same set of slices)
  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  Aorta = clitk::ResizeImageLike<MaskImageType>(Aorta, m_Working_Support, GetBackgroundValue());
  std::vector<typename MaskSliceType::Pointer> aorta_slices;
  clitk::ExtractSlices<MaskImageType>(Aorta, 2, aorta_slices);

  // Extract slices of Mediastinum (resize like support before to have the same set of slices)
  m_Mediastinum = GetAFDB()->template GetImage<MaskImageType>("Mediastinum");
  m_Mediastinum = clitk::ResizeImageLike<MaskImageType>(m_Mediastinum, m_Working_Support, GetBackgroundValue());
  std::vector<typename MaskSliceType::Pointer> mediast_slices;
  clitk::ExtractSlices<MaskImageType>(m_Mediastinum, 2, mediast_slices);

  // List of points
  std::vector<MaskImagePointType> p_MostLeftVertebralBody;
  std::vector<MaskImagePointType> p_MostRightVertebralBody;
  std::vector<MaskImagePointType> p_MostLeftAorta;
  std::vector<MaskImagePointType> p_MostLeftEsophagus;

  /*
    In the following, we search for the LeftRight limits.  We search
    for the most Right points in Esophagus and in VertebralBody and
    consider a line between those to most right points. All points in
    the support which are most right to this line are discarded. Same
    for the left part. The underlying assumption is that the support
    is concave between Eso/VertebralBody. Esophagus is a bit
    dilatated. On VertebralBody we go right (or left) until we reach
    the lung (but no more 20 mm).
  */

  // Temporary 3D point
  MaskImagePointType p;

  typename MaskSliceType::PointType minSlicePoint;
  typename MaskSliceType::PointType maxSlicePoint;
  clitk::GetMinMaxPointPosition<MaskSliceType>(vert_slices[0], minSlicePoint, maxSlicePoint);

  // Loop slices
  for(uint i=0; i<slices.size() ; i++) {
    // Declare all needed 2D points (sp = slice point)
    //    typename MaskSliceType::PointType sp_MostRightVertebralBody;
    typename MaskSliceType::PointType sp_MostLeftVertebralBody;
    typename MaskSliceType::PointType sp_MostLeftAorta;
    typename MaskSliceType::PointType sp_temp;
    typename MaskSliceType::PointType sp_MostLeftEsophagus;

    /* -------------------------------------------------------------------------------------
        Find first point not in mediastinum at LEFT
    */
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(vert_slices[i], GetBackgroundValue(), 
                                                            0, false, sp_MostLeftVertebralBody);
    // clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostLeftVertebralBody, VertebralBody, i, p);
    // DD(p);

    sp_MostLeftVertebralBody = 
      clitk::FindExtremaPointInAGivenLine<MaskSliceType>(mediast_slices[i], 0, false, 
                                                         sp_MostLeftVertebralBody, GetBackgroundValue(), 30);
    // clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostLeftVertebralBody, VertebralBody, i, p);
    // DD(p);

    sp_MostLeftVertebralBody[0] += 2; // 2mm margin
    // clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostLeftVertebralBody, VertebralBody, i, p);
    // DD(p);

    // Convert 2D points into 3D
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostLeftVertebralBody, VertebralBody, i, p);
    p_MostLeftVertebralBody.push_back(p);

    /* -------------------------------------------------------------------------------------
       Find first point not in mediastinum at RIGHT. Not used yet.
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(vert_slices[i], GetBackgroundValue(), 
                                                            0, true, sp_MostRightVertebralBody);
    sp_MostRightVertebralBody = 
      clitk::FindExtremaPointInAGivenLine<MaskSliceType>(mediast_slices[i], 0, true, 
                                                         sp_MostRightVertebralBody, GetBackgroundValue(),30);
    sp_MostRightVertebralBody[0] -= 2; // 2 mm margin
    
    // Convert 2D points into 3D
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostRightVertebralBody, VertebralBody, i, p);
    p_MostRightVertebralBody.push_back(p);
    */


    /* -------------------------------------------------------------------------------------
       Find most Left point in Esophagus
     */
    bool found = clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(eso_slices[i], GetBackgroundValue(), 0, false, sp_MostLeftEsophagus);
    if (!found) { // No more Esophagus, I remove the previous point
      //DD("no eso pop back");
      p_MostLeftVertebralBody.pop_back();
    }
    else {
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostLeftEsophagus, EsophagusForSlice, i, p);
      p_MostLeftEsophagus.push_back(p);
    }
      
    /* -------------------------------------------------------------------------------------
       Find most Left point in Aorta 
    */
    if (found) {
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(aorta_slices[i], GetBackgroundValue(), 0, false, sp_MostLeftAorta);
      sp_MostLeftAorta = clitk::FindExtremaPointInAGivenLine<MaskSliceType>(mediast_slices[i], 0, false, sp_MostLeftAorta, GetBackgroundValue(), 10);
      typename MaskSliceType::PointType temp=sp_MostLeftEsophagus;
      temp[0] -= 10;
      if (clitk::IsOnTheSameLineSide(sp_MostLeftAorta, sp_MostLeftVertebralBody, sp_MostLeftEsophagus, temp)) { 
        // sp_MostLeftAorta is on the same side than temp (at Right) -> ignore Aorta
        sp_MostLeftAorta = sp_MostLeftEsophagus;
      }
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostLeftAorta, Aorta, i, p);
      p_MostLeftAorta.push_back(p);
    }

  } // End of slice loop
  
  clitk::WriteListOfLandmarks<MaskImageType>(p_MostLeftVertebralBody, "S8-MostLeft-VB-points.txt");
  //  clitk::WriteListOfLandmarks<MaskImageType>(p_MostRightVertebralBody, "S8-MostRight-VB-points.txt");
  clitk::WriteListOfLandmarks<MaskImageType>(p_MostLeftAorta, "S8-MostLeft-Aorta-points.txt");
  clitk::WriteListOfLandmarks<MaskImageType>(p_MostLeftEsophagus, "S8-MostLeft-eso-points.txt");

  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_MostLeftVertebralBody, p_MostLeftAorta,
                                                                    GetBackgroundValue(), 0, -10);

  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_MostLeftAorta, p_MostLeftEsophagus,
                                                                    GetBackgroundValue(), 0, -10);
  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
  return;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_Remove_Structures()
{
  //--------------------------------------------------------------------
  Remove_Structures("8", "Aorta");
  Remove_Structures("8", "Esophagus");
  Remove_Structures("8", "VertebralBody");  

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
  return;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
FindExtremaPointsInBronchus(MaskImagePointer input, 
			    int direction,
			    double distance_max_from_center_point, 
			    ListOfPointsType & LR, 
			    ListOfPointsType & Ant, 
			    ListOfPointsType & Post)
{

  // Other solution ==> with auto bounding box ! (but pb to prevent to
  // be too distant from the center point

  // Extract slices
  std::vector<typename MaskSliceType::Pointer> slices;
  clitk::ExtractSlices<MaskImageType>(input, 2, slices);
  
  // Loop on slices
  bool found;
  for(uint i=0; i<slices.size(); i++) {
    /*
    // Keep main CCL
    slices[i] = Labelize<MaskSliceType>(slices[i], 0, true, 10);
    slices[i] = KeepLabels<MaskSliceType>(slices[i], 
    GetBackgroundValue(), 
    GetForegroundValue(), 1, 1, true);
    */

    // ------- Find rightmost or leftmost point  ------- 
    MaskSliceType::PointType LRMost;
    found = 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], 
                                                              GetBackgroundValue(), 
                                                              0, // axis XY
                                                              (direction==0?false:true),  // right or left according to direction
                                                              LRMost);
    // ------- Find postmost point  ------- 
    MaskSliceType::PointType postMost;
    found = 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], 
                                                              GetBackgroundValue(), 
                                                              1, false, LRMost, 
                                                              distance_max_from_center_point, 
                                                              postMost);
    // ------- Find antmost point  ------- 
    MaskSliceType::PointType antMost;
    found = 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], 
                                                              GetBackgroundValue(), 
                                                              1, true, LRMost, 
                                                              distance_max_from_center_point, 
                                                              antMost);
    // Only add point if found
    if (found)  {
      // ------- Convert 2D to 3D points --------
      MaskImageType::PointType p;
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(LRMost, input, i, p);
      LR.push_back(p); 
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(antMost, input, i, p);
      Ant.push_back(p);
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(postMost, input, i, p);
      Post.push_back(p);
    }
  }
} 
//--------------------------------------------------------------------
