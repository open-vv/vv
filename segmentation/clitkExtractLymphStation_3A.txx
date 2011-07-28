
//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_SetDefaultValues()
{
  SetFuzzyThreshold("3A", "Sternum", 0.5);
  SetFuzzyThreshold("3A", "SubclavianArtery", 0.5);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_3A()
{
  if (!CheckForStation("3A")) return;

  StartNewStep("Station 3A");
  StartSubStep();   

  // Get the current support 
  StartNewStep("[Station 3A] Get the current 3A suppport");
  m_Working_Support = m_ListOfSupports["S3A"];
  m_ListOfStations["3A"] = m_Working_Support;
  StopCurrentStep<MaskImageType>(m_Working_Support);
  
  ExtractStation_3A_AntPost_S5();
  ExtractStation_3A_AntPost_S6();
  ExtractStation_3A_AntPost_Superiorly();
  ExtractStation_3A_Remove_Structures();

  Remove_Structures("3A", "Aorta");
  Remove_Structures("3A", "SubclavianArteryLeft");
  Remove_Structures("3A", "SubclavianArteryRight");
  Remove_Structures("3A", "Thyroid");
  Remove_Structures("3A", "CommonCarotidArteryLeft");
  Remove_Structures("3A", "CommonCarotidArteryRight");
  Remove_Structures("3A", "BrachioCephalicArtery");

  ExtractStation_3A_PostToBones();
  
  

  // ExtractStation_3A_Ant_Limits(); --> No, already in support; to remove
  // ExtractStation_3A_Post_Limits(); --> No, more complex, see Vessels etc

  // Store image filenames into AFDB 
  writeImage<MaskImageType>(m_ListOfStations["3A"], "seg/Station3A.mhd");
  GetAFDB()->SetImageFilename("Station3A", "seg/Station3A.mhd"); 
  WriteAFDB(); 
  StopSubStep();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_Ant_Limits() 
{
  StartNewStep("[Station 3A] Ant limits with Sternum");

  // Get Sternum, keep posterior part.
  MaskImagePointer Sternum = GetAFDB()->template GetImage<MaskImageType>("Sternum");
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, Sternum, 2, 
                                                       GetFuzzyThreshold("3A", "Sternum"), "PostTo", 
                                                       false, 3, true, false);
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3A"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_Post_Limits() 
{
  StartNewStep("[Station 3A] Post limits with SubclavianArtery");

  // Get Sternum, keep posterior part.
  MaskImagePointer SubclavianArteryLeft = 
    GetAFDB()->template GetImage<MaskImageType>("SubclavianArteryLeft");
  MaskImagePointer SubclavianArteryRight = 
    GetAFDB()->template GetImage<MaskImageType>("SubclavianArteryRight");

  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, SubclavianArteryLeft, 2, 
                                                       GetFuzzyThreshold("3A", "SubclavianArtery"), "AntTo", 
                                                       false, 3, true, false);
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, SubclavianArteryRight, 2, 
                                                       GetFuzzyThreshold("3A", "SubclavianArtery"), "AntTo", 
                                                       false, 3, true, false);
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3A"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_AntPost_S5() 
{
  StartNewStep("[Station 3A] Post limits around S5");

  // First remove post to SVC
  MaskImagePointer SVC = GetAFDB()->template GetImage <MaskImageType>("SVC");

  // Trial in 3D -> difficulties superiorly. Stay slice by slice.
  // Slice by slice not post to SVC. Use initial spacing
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, SVC, 2, 
                                                       GetFuzzyThreshold("3A", "SVC"), 
                                                       "NotPostTo", true, 
                                                       SVC->GetSpacing()[0], false, false);  

  // Consider Aorta, remove Left/Post part ; only around S5
  // Get S5 support and Aorta
  MaskImagePointer S5 = m_ListOfSupports["S5"];
  MaskImagePointer Aorta = GetAFDB()->template GetImage <MaskImageType>("Aorta");
  Aorta = clitk::ResizeImageLike<MaskImageType>(Aorta, S5, GetBackgroundValue());
  
  // Inferiorly, Aorta has two CCL that merge into a single one when
  // S6 appears. Loop on Aorta slices, select the most ant one, detect
  // the most ant point.
  std::vector<MaskSlicePointer> slices;
  clitk::ExtractSlices<MaskImageType>(Aorta, 2, slices);
  std::vector<MaskImagePointType> points;
  for(uint i=0; i<slices.size(); i++) {
    // Select most ant CCL
    slices[i] = clitk::Labelize<MaskSliceType>(slices[i], GetBackgroundValue(), false, 1);
    std::vector<MaskSlicePointType> c;
    clitk::ComputeCentroids<MaskSliceType>(slices[i], GetBackgroundValue(), c);
    assert(c.size() == 3); // only 2 CCL
    typename MaskSliceType::PixelType l;
    if (c[1][1] > c[2][1]) { // We will remove the label=1
      l = 1;
    }
    else {
      l = 2;// We will remove the label=2
    }
    slices[i] = clitk::SetBackground<MaskSliceType, MaskSliceType>(slices[i], slices[i], l, 
                                                                   GetBackgroundValue(), true);
    
    // Detect the most ant point
    MaskSlicePointType p;
    MaskImagePointType pA;
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], GetBackgroundValue(), 1, true, p);
    // Set the X coordinate to the X coordinate of the centroid
    if (l==1) p[0] = c[2][0];
    else p[0] = c[1][0];
    
    // Convert in 3D and store
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p, Aorta, i, pA);
    points.push_back(pA);
  }
  
  // DEBUG
  // MaskImagePointer o = clitk::JoinSlices<MaskImageType>(slices, Aorta, 2);
  // writeImage<MaskImageType>(o, "o.mhd");
  // clitk::WriteListOfLandmarks<MaskImageType>(points, "Ant-Aorta.txt");

  // Remove Post/Left to this point
  m_Working_Support = 
    clitk::SliceBySliceSetBackgroundFromPoints<MaskImageType>(m_Working_Support, 
                                                              GetBackgroundValue(), 2,
                                                              points, 
                                                              true, // Set BG if X greater than point[x], and 
                                                              true); // if Y greater than point[y]
  
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3A"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_AntPost_S6() 
{
  StartNewStep("[Station 3A] Post limits around S6");

  // Consider Aorta
  MaskImagePointer Aorta = GetAFDB()->template GetImage <MaskImageType>("Aorta");
  
  // Limits the support to S6
  MaskImagePointer S6 = m_ListOfSupports["S6"];
  Aorta = clitk::ResizeImageLike<MaskImageType>(Aorta, S6, GetBackgroundValue());
  
  // Extend 1cm anteriorly
  MaskImagePointType radius; // in mm
  radius[0] = 10;
  radius[1] = 10;
  radius[2] = 0; // required
  Aorta = clitk::Dilate<MaskImageType>(Aorta, radius, GetBackgroundValue(), GetForegroundValue(), false);
  
  // Not Post to
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, Aorta, 2, 
                                                       GetFuzzyThreshold("3A", "Aorta"), 
                                                       "NotPostTo", true, 
                                                       Aorta->GetSpacing()[0], false, false);
  
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3A"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_AntPost_Superiorly() 
{
  StartNewStep("[Station 3A] Post limits superiorly");

  /*
 MaskImagePointer BrachioCephalicVein = GetAFDB()->template GetImage <MaskImageType>("BrachioCephalicVein");
 MaskImagePointer BrachioCephalicArtery = GetAFDB()->template GetImage <MaskImageType>("BrachioCephalicArtery");
 MaskImagePointer CommonCarotidArteryLeft = GetAFDB()->template GetImage <MaskImageType>("CommonCarotidArteryLeft");
 MaskImagePointer CommonCarotidArteryRight = GetAFDB()->template GetImage <MaskImageType>("CommonCarotidArteryRight");
 MaskImagePointer SubclavianArteryLeft = GetAFDB()->template GetImage <MaskImageType>("SubclavianArteryLeft");
 MaskImagePointer SubclavianArteryRight = GetAFDB()->template GetImage <MaskImageType>("SubclavianArteryRight");

  // Not Post to
#define RP(STRUCTURE)                                                   \
 m_Working_Support =                                                    \
   clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, STRUCTURE, 2, \
                                                      0.5,              \
                                                      "NotPostTo", true, \
                                                      STRUCTURE->GetSpacing()[0], false, false);
 
 // RP(BrachioCephalicVein);
 RP(BrachioCephalicArtery);
 RP(CommonCarotidArteryRight);
 RP(CommonCarotidArteryLeft);
 RP(SubclavianArteryRight);
 RP(SubclavianArteryLeft);
  */
  
  // Get or compute the binary mask that separate Ant/Post part
  // according to vessels
  MaskImagePointer binarizedContour = FindAntPostVessels2();
  binarizedContour = clitk::ResizeImageLike<MaskImageType>(binarizedContour, 
                                                           m_Working_Support, 
                                                           GetBackgroundValue());

  // remove from support
  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BoolFilterType;
  typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
  boolFilter->InPlaceOn();
  boolFilter->SetInput1(m_Working_Support);
  boolFilter->SetInput2(binarizedContour);
  boolFilter->SetBackgroundValue1(GetBackgroundValue());
  boolFilter->SetBackgroundValue2(GetBackgroundValue());
  boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update();
  m_Working_Support = boolFilter->GetOutput();
  
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3A"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3A_Remove_Structures() 
{
  Remove_Structures("3A", "Aorta");
  Remove_Structures("3A", "SubclavianArteryLeft");
  Remove_Structures("3A", "SubclavianArteryRight");
  Remove_Structures("3A", "Thyroid");
  Remove_Structures("3A", "CommonCarotidArteryLeft");
  Remove_Structures("3A", "CommonCarotidArteryRight");
  Remove_Structures("3A", "BrachioCephalicArtery");
  //  Remove_Structures("3A", "BrachioCephalicVein"); ?

  StartNewStep("[Station 3A] Remove part of BrachioCephalicVein");
  // resize like support, extract slices
  // while single CCL -> remove
  // when two remove only the most post
  MaskImagePointer BrachioCephalicVein = 
    GetAFDB()->template GetImage <MaskImageType>("BrachioCephalicVein");
  BrachioCephalicVein = clitk::ResizeImageLike<MaskImageType>(BrachioCephalicVein, 
                                                              m_Working_Support, 
                                                              GetBackgroundValue());
  std::vector<MaskSlicePointer> slices;
  std::vector<MaskSlicePointer> slices_BCV;
  clitk::ExtractSlices<MaskImageType>(m_Working_Support, 2, slices);
  clitk::ExtractSlices<MaskImageType>(BrachioCephalicVein, 2, slices_BCV);
  for(uint i=0; i<slices.size(); i++) {
    // Labelize slices_BCV
    slices_BCV[i] = Labelize<MaskSliceType>(slices_BCV[i], 0, true, 1);

    // Compute centroids
    std::vector<typename MaskSliceType::PointType> centroids;
    ComputeCentroids<MaskSliceType>(slices_BCV[i], GetBackgroundValue(), centroids);

    // If several centroid, select the one most anterior
    if (centroids.size() > 2) {
      // Only keep the one most post
      typename MaskSliceType::PixelType label;
      if (centroids[1][1] > centroids[2][1]) {
        label = 2;
      }
      else {
        label = 1;
      }      
      // "remove" the CCL 
      slices_BCV[i] = clitk::SetBackground<MaskSliceType, MaskSliceType>(slices_BCV[i], 
                                                                         slices_BCV[i], 
                                                                         label, 
                                                                         GetBackgroundValue(), 
                                                                         true);
    }
    
    // Remove from the support
    clitk::AndNot<MaskSliceType>(slices[i], slices_BCV[i], GetBackgroundValue());
  }
  
  // Joint
  m_Working_Support = clitk::JoinSlices<MaskImageType>(slices, m_Working_Support, 2);

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3A"] = m_Working_Support;
}
//--------------------------------------------------------------------
