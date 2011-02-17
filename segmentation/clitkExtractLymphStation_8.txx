
#include <itkBinaryDilateImageFilter.h>
#include <itkMirrorPadImageFilter.h>

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
  StartNewStep("[Station8] Inf/Sup mediastinum limits with carina/diaphragm junction");

  // Get Carina Z position
  MaskImagePointer Carina = GetAFDB()->template GetImage<MaskImageType>("Carina");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after read Carina"); 
  std::vector<MaskImagePointType> centroids;
  clitk::ComputeCentroids<MaskImageType>(Carina, GetBackgroundValue(), centroids);
  DD(centroids[1]);
  m_CarinaZ = centroids[1][2];
  DD(m_CarinaZ);
  // add one slice to include carina ?
  m_CarinaZ += m_Mediastinum->GetSpacing()[2];
  DD(m_CarinaZ);
  // We dont need Carina structure from now
  Carina->Delete();
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after delete Carina"); 
  
  // Get left lower lobe bronchus (L), take the upper border
  // Get right bronchus intermedius (RML), take the lower border

  /*
    double m_CarinaZ = GetAFDB()->GetPoint3D("Carina", 2) + 
    m_Mediastinum->GetSpacing()[2]; // add one slice to include carina
    // double GOjunctionZ = GetAFDB()->GetPoint3D("GOjunction", 2);
    */

  // Found most inferior part of the lung
  MaskImagePointer Lungs = GetAFDB()->template GetImage<MaskImageType>("Lungs");
  // It should be already croped, so I took the origin and add 10mm above 
  m_DiaphragmInferiorLimit = Lungs->GetOrigin()[2]+10;
  DD(m_DiaphragmInferiorLimit);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after read Lung");  
  Lungs->Delete(); // we don't need it, release memory
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after release Lung");  

  /* Crop support :
     Superior limit = carina
     Inferior limit = DiaphragmInferiorLimit (old=Gastroesphogeal junction) */
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                m_DiaphragmInferiorLimit, //GOjunctionZ, 
                                                m_CarinaZ, true,
                                                GetBackgroundValue());
  
  // Remove some structures that we know are excluded 
  MaskImagePointer VertebralBody = 
    GetAFDB()->template GetImage <MaskImageType>("VertebralBody");  
  MaskImagePointer Aorta = 
    GetAFDB()->template GetImage <MaskImageType>("Aorta");  

  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BoolFilterType;
  typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
  boolFilter->InPlaceOn();
  boolFilter->SetInput1(m_Working_Support);
  boolFilter->SetInput2(VertebralBody);    
  boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update();    
  boolFilter->SetInput1(boolFilter->GetOutput());
  boolFilter->SetInput2(Aorta);    
  boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update();    
  m_Working_Support = boolFilter->GetOutput();

  // Done.
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_Post_Limits() 
{
  /*
    Station 8: paraeosphageal nodes

    Anteriorly, it is in contact with Station 7 and the
    left main stem bronchus in its superior aspect (Fig. 3C–H) and
    with the heart more inferiorly. Posteriorly, Station 8 abuts the
    descending aorta and the anterior aspect of the vertebral body
    until an imaginary horizontal line running 1 cm posterior to the
    anterior border of the vertebral body (Fig. 3C). 

    New classification IASCL 2009 :

    "Nodes lying adjacent to the wall of the esophagus and to the
    right or left of the midline, excluding subcarinal nodes (S7)
    Upper"

  */

  StartNewStep("[Station8] Post limits with vertebral body");
  MaskImagePointer VertebralBody = 
    GetAFDB()->template GetImage <MaskImageType>("VertebralBody");  

  // Consider vertebral body slice by slice
  typedef clitk::ExtractSliceFilter<MaskImageType> ExtractSliceFilterType;
  typename ExtractSliceFilterType::Pointer extractSliceFilter = ExtractSliceFilterType::New();
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  std::vector<typename SliceType::Pointer> vertebralSlices;
  extractSliceFilter->SetInput(VertebralBody);
  extractSliceFilter->SetDirection(2);
  extractSliceFilter->Update();
  extractSliceFilter->GetOutputSlices(vertebralSlices);

  // For each slice, compute the most anterior point
  std::map<int, typename SliceType::PointType> vertebralAntPositionBySlice;
  for(uint i=0; i<vertebralSlices.size(); i++) {
    typename SliceType::PointType p;
    bool found = clitk::FindExtremaPointInAGivenDirection<SliceType>(vertebralSlices[i], 
                                                                     GetBackgroundValue(), 
                                                                     1, true, p);
    if (found) {
      vertebralAntPositionBySlice[i] = p;
    }
    else { 
      // no Foreground in this slice (it appends generally at the
      // beginning and the end of the volume). Do nothing in this
      // case.
    }
  }

  // Convert 2D points in slice into 3D points
  std::vector<MaskImagePointType> vertebralAntPositions;
  clitk::PointsUtils<MaskImageType>::Convert2DTo3DList(vertebralAntPositionBySlice, 
                                                       VertebralBody, 
                                                       vertebralAntPositions);

  // DEBUG : write list of points
  clitk::WriteListOfLandmarks<MaskImageType>(vertebralAntPositions, 
                                             "vertebralMostAntPositions.txt");

  // Cut support posteriorly 1cm the most anterior point of the
  // VertebralBody. Do nothing below and above the VertebralBody. To
  // do that compute several region, slice by slice and fill. 
  MaskImageRegionType region;
  MaskImageSizeType size;
  MaskImageIndexType start;
  size[2] = 1; // a single slice
  start[0] = m_Working_Support->GetLargestPossibleRegion().GetIndex()[0];
  size[0] = m_Working_Support->GetLargestPossibleRegion().GetSize()[0];
  for(uint i=0; i<vertebralAntPositions.size(); i++) {
    typedef typename itk::ImageRegionIterator<MaskImageType> IteratorType;
    IteratorType iter = 
      IteratorType(m_Working_Support, m_Working_Support->GetLargestPossibleRegion());
    MaskImageIndexType index;
    // Consider some cm posterior to most anterior positions (usually
    // 1 cm).
    vertebralAntPositions[i][1] += GetDistanceMaxToAnteriorPartOfTheSpine();
    // Get index of this point
    m_Working_Support->TransformPhysicalPointToIndex(vertebralAntPositions[i], index);
    // Compute region (a single slice)
    start[2] = index[2];
    start[1] = m_Working_Support->GetLargestPossibleRegion().GetIndex()[1]+index[1];
    size[1] = m_Working_Support->GetLargestPossibleRegion().GetSize()[1]-start[1];
    region.SetSize(size);
    region.SetIndex(start);
    // Fill region
    if (m_Working_Support->GetLargestPossibleRegion().IsInside(start))  {
      itk::ImageRegionIterator<MaskImageType> it(m_Working_Support, region);
      it.GoToBegin();
      while (!it.IsAtEnd()) {
        it.Set(GetBackgroundValue());
        ++it;
      }
    }
  }

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
  StartNewStep("[Station8] Ant limits with S7 above Carina");
  /*
    Anteriorly, it is in contact with Station 7 and the
    left main stem bronchus in its superior aspect (Fig. 3C–H) and
    with the heart more inferiorly. 

    1) line post wall bronchi (S7), till originRMLB
    - LUL bronchus : to detect in trachea. But not needed here ??
    2) heart ! -> not delineated.
    ==> below S7, crop CT not to far away (ant), then try with threshold
    
    1) ==> how to share with S7 ? Prepare both support at the same time !
    NEED vector of initial support for each station ? No -> map if it exist before, take it !!

  */

  // Ant limit from carina (start) to end of S7 = originRMLB
  // Get Trachea
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after read Trachea");
 
  MaskImagePointer m_Working_Trachea = 
    clitk::CropImageAbove<MaskImageType>(Trachea, 2, m_CarinaZ, true, // AutoCrop
                                         GetBackgroundValue());
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after crop");

  // Seprate into two main bronchi
  MaskImagePointer LeftBronchus;
  MaskImagePointer RightBronchus;

  // Labelize and consider the two first (main) labels
  m_Working_Trachea = Labelize<MaskImageType>(m_Working_Trachea, 0, true, 1);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after labelize");

  // Carina position must at the first slice that separate the two
  // main bronchus (not superiorly). We thus first check that the
  // upper slice is composed of at least two labels
  typedef itk::ImageSliceIteratorWithIndex<MaskImageType> SliceIteratorType;
  SliceIteratorType iter(m_Working_Trachea, m_Working_Trachea->GetLargestPossibleRegion());
  iter.SetFirstDirection(0);
  iter.SetSecondDirection(1);
  iter.GoToReverseBegin(); // Start from the end (because image is IS not SI)
  int maxLabel=0;
  while (!iter.IsAtReverseEndOfSlice()) {
    while (!iter.IsAtReverseEndOfLine()) {    
      if (iter.Get() > maxLabel) maxLabel = iter.Get();
      --iter;
    }
    iter.PreviousLine();
  }
  if (maxLabel < 2) {
    clitkExceptionMacro("First slice form Carina does not seems to seperate the two main bronchus. Abort");
  }

  // Compute 3D centroids of both parts to identify the left from the
  // right bronchus
  std::vector<ImagePointType> c;
  clitk::ComputeCentroids<MaskImageType>(m_Working_Trachea, GetBackgroundValue(), c);
  ImagePointType C1 = c[1];
  ImagePointType C2 = c[2];

  ImagePixelType leftLabel;
  ImagePixelType rightLabel;  
  if (C1[0] < C2[0]) { leftLabel = 1; rightLabel = 2; }
  else { leftLabel = 2; rightLabel = 1; }

  // Select LeftLabel (set one label to Backgroundvalue)
  LeftBronchus = 
    SetBackground<MaskImageType, MaskImageType>(m_Working_Trachea, m_Working_Trachea, 
                                                rightLabel, GetBackgroundValue(), false);
  RightBronchus  = 
    SetBackground<MaskImageType, MaskImageType>(m_Working_Trachea, m_Working_Trachea, 
                                                leftLabel, GetBackgroundValue(), false);
  // Crop images
  LeftBronchus = clitk::AutoCrop<MaskImageType>(LeftBronchus, GetBackgroundValue()); 
  RightBronchus = clitk::AutoCrop<MaskImageType>(RightBronchus, GetBackgroundValue()); 

  // Insert int AFDB if need after 
  GetAFDB()->template SetImage <MaskImageType>("LeftBronchus", "seg/leftBronchus.mhd", 
                                               LeftBronchus, true);
  GetAFDB()->template SetImage <MaskImageType>("RightBronchus", "seg/rightBronchus.mhd", 
                                               RightBronchus, true);

  // Now crop below OriginOfRightMiddleLobeBronchusZ
  // It is not done before to keep entire bronchi.
  
  MaskImagePointer OriginOfRightMiddleLobeBronchus = 
    GetAFDB()->template GetImage<MaskImageType>("OriginOfRightMiddleLobeBronchus");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after read OriginOfRightMiddleLobeBronchus"); 
  std::vector<MaskImagePointType> centroids;
  clitk::ComputeCentroids<MaskImageType>(OriginOfRightMiddleLobeBronchus, GetBackgroundValue(), centroids);
  DD(centroids.size());
  DD(centroids[0]); // BG
  DD(centroids[1]);
  m_OriginOfRightMiddleLobeBronchusZ = centroids[1][2];
  DD(m_OriginOfRightMiddleLobeBronchusZ);
  // add one slice to include carina ?
  m_OriginOfRightMiddleLobeBronchusZ += LeftBronchus->GetSpacing()[2];
  DD(m_OriginOfRightMiddleLobeBronchusZ);
  DD(OriginOfRightMiddleLobeBronchus->GetReferenceCount());
  // We dont need Carina structure from now
  OriginOfRightMiddleLobeBronchus->Delete();
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after delete OriginOfRightMiddleLobeBronchus"); 

  LeftBronchus = 
    clitk::CropImageBelow<MaskImageType>(LeftBronchus, 2, 
                                         m_OriginOfRightMiddleLobeBronchusZ, 
                                         true, // AutoCrop
                                         GetBackgroundValue());
  RightBronchus = 
    clitk::CropImageBelow<MaskImageType>(RightBronchus, 2, 
                                         m_OriginOfRightMiddleLobeBronchusZ, 
                                         true, // AutoCrop
                                         GetBackgroundValue());

  // Search for points that are the most left/post/ant and most
  // right/post/ant of the left and right bronchus
  // 15  = not 15 mm more distance than the middle point.
  FindExtremaPointsInBronchus(LeftBronchus, 0, 10, m_RightMostInLeftBronchus, 
			      m_AntMostInLeftBronchus, m_PostMostInLeftBronchus);
  FindExtremaPointsInBronchus(RightBronchus, 1, 10, m_LeftMostInRightBronchus, 
			      m_AntMostInRightBronchus, m_PostMostInRightBronchus);

  // DEBUG : write the list of points
  ListOfPointsType v;
  v.reserve(m_RightMostInLeftBronchus.size()+m_AntMostInLeftBronchus.size()+
            m_PostMostInLeftBronchus.size());
  v.insert(v.end(), m_RightMostInLeftBronchus.begin(), m_RightMostInLeftBronchus.end() );
  v.insert(v.end(), m_AntMostInLeftBronchus.begin(), m_AntMostInLeftBronchus.end() );
  v.insert(v.end(), m_PostMostInLeftBronchus.begin(), m_PostMostInLeftBronchus.end() );
  clitk::WriteListOfLandmarks<MaskImageType>(v, "LeftBronchusPoints.txt");

  v.clear();
  v.reserve(m_LeftMostInRightBronchus.size()+m_AntMostInRightBronchus.size()+
            m_PostMostInRightBronchus.size());
  v.insert(v.end(), m_LeftMostInRightBronchus.begin(), m_LeftMostInRightBronchus.end() );
  v.insert(v.end(), m_AntMostInRightBronchus.begin(), m_AntMostInRightBronchus.end() );
  v.insert(v.end(), m_PostMostInRightBronchus.begin(), m_PostMostInRightBronchus.end() );
  clitk::WriteListOfLandmarks<MaskImageType>(v, "RightBronchusPoints.txt");


  // Now uses these points to limit, slice by slice 
  // line is mainly horizontal, so mainDirection=1
  writeImage<MaskImageType>(m_Working_Support, "before.mhd");
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    m_PostMostInLeftBronchus,
                                                                    m_PostMostInRightBronchus,
                                                                    GetBackgroundValue(), 1, 10); 
  writeImage<MaskImageType>(m_Working_Support, "after.mhd");

HERE

  // Keep main 3D CCL :
  m_Working_Support = Labelize<MaskImageType>(m_Working_Support, 0, false, 10);
  m_Working_Support = KeepLabels<MaskImageType>(m_Working_Support, 
                                                GetBackgroundValue(), 
                                                GetForegroundValue(), 1, 1, true);
  
  // Autocrop
  m_Working_Support = clitk::AutoCrop<MaskImageType>(m_Working_Support, GetBackgroundValue()); 

  // End of step
  StopCurrentStep<MaskImageType>(m_Working_Support);
  //  m_ListOfStations["8"] = m_Working_Support;

}

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_LR_Limits() 
{

  //--------------------------------------------------------------------
  StartNewStep("[Station8] Left and Right limits arround esophagus (below Carina)");
  /*
    Consider Esophagus, dilate it and remove ant part. It remains part
    on L & R, than can be partly removed by cutting what remains at
    right of vertebral body.
  */
  
  // Get Esophagus
  MaskImagePointer Esophagus = GetAFDB()->template GetImage<MaskImageType>("Esophagus");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after read Esophagus");

  // Crop Esophagus : keep only below the OriginOfRightMiddleLobeBronchusZ
  Esophagus = 
    clitk::CropImageAbove<MaskImageType>(Esophagus, 2, 
                                         m_OriginOfRightMiddleLobeBronchusZ, 
                                         true, // AutoCrop
                                         GetBackgroundValue());

  // Dilate to keep only not Anterior positions
  MaskImagePointType radiusInMM = GetEsophagusDiltationForAnt();
  Esophagus = EnlargeEsophagusDilatationRadiusInferiorly(Esophagus);
  Esophagus = clitk::Dilate<MaskImageType>(Esophagus, 
                                           radiusInMM, 
                                           GetBackgroundValue(), 
                                           GetForegroundValue(), true);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after dilate Esophagus");
  writeImage<MaskImageType>(Esophagus, "enlarged-eso.mhd");

  // Remove Anterior part according to this dilatated esophagus
  typedef clitk::SliceBySliceRelativePositionFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->SetInput(m_Working_Support);
  relPosFilter->SetInputObject(Esophagus);
  relPosFilter->AddOrientationTypeString("P");
  relPosFilter->InverseOrientationFlagOff();
  relPosFilter->SetDirection(2); // Z axis
  relPosFilter->UniqueConnectedComponentBySliceOff();
  relPosFilter->SetIntermediateSpacing(3);
  relPosFilter->ResampleBeforeRelativePositionFilterOn();
  relPosFilter->SetFuzzyThreshold(GetFuzzyThresholdForS8());
  relPosFilter->RemoveObjectFlagOff();
  relPosFilter->Update();
  m_Working_Support = relPosFilter->GetOutput();
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after SbS Rel pos Post");

  // AutoCrop (OR SbS ?)
  m_Working_Support = clitk::AutoCrop<MaskImageType>(m_Working_Support, GetBackgroundValue()); 

  writeImage<MaskImageType>(m_Working_Support, "step1.4.1.mhd");

  clitk::PrintMemory(GetVerboseMemoryFlag(), "after autocrop");

  // Estract slices for current support for slice by slice processing
  std::vector<typename MaskSliceType::Pointer> slices;
  clitk::ExtractSlices<MaskImageType>(m_Working_Support, 2, slices);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after support slices");
  
  // Estract slices of Esophagus (resize like support before to have the same set of slices)
  MaskImagePointer EsophagusForSlice = clitk::ResizeImageLike<MaskImageType>(Esophagus, m_Working_Support, GetBackgroundValue());
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after Eso resize");
  std::vector<typename MaskSliceType::Pointer> eso_slices;
  clitk::ExtractSlices<MaskImageType>(EsophagusForSlice, 2, eso_slices);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after Eso slices");

  // Estract slices of Vertebral (resize like support before to have the same set of slices)
  MaskImagePointer VertebralBody = GetAFDB()->template GetImage<MaskImageType>("VertebralBody");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after Read VertebralBody");
  VertebralBody = clitk::ResizeImageLike<MaskImageType>(VertebralBody, m_Working_Support, GetBackgroundValue());
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after VertebralBody Resize");
  std::vector<typename MaskSliceType::Pointer> vert_slices;
  clitk::ExtractSlices<MaskImageType>(VertebralBody, 2, vert_slices);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after VertebralBody slices");

  // Estract slices of Aorta (resize like support before to have the same set of slices)
  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after Read Aorta");
  Aorta = clitk::ResizeImageLike<MaskImageType>(Aorta, m_Working_Support, GetBackgroundValue());
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after Aorta Resize");
  std::vector<typename MaskSliceType::Pointer> aorta_slices;
  clitk::ExtractSlices<MaskImageType>(Aorta, 2, aorta_slices);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after Aorta slices");

  // Extract slices of Mediastinum (resize like support before to have the same set of slices)
  m_Mediastinum = GetAFDB()->template GetImage<MaskImageType>("Mediastinum");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after read Mediastinum");
  m_Mediastinum = clitk::ResizeImageLike<MaskImageType>(m_Mediastinum, m_Working_Support, GetBackgroundValue());
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after resize Mediastinum");
  std::vector<typename MaskSliceType::Pointer> mediast_slices;
  clitk::ExtractSlices<MaskImageType>(m_Mediastinum, 2, mediast_slices);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after Mediastinum slices");

  writeImage<MaskImageType>(EsophagusForSlice, "slices_eso.mhd");
  writeImage<MaskImageType>(VertebralBody, "slices_vert.mhd");
  writeImage<MaskImageType>(Aorta, "slices_aorta.mhd");
  writeImage<MaskImageType>(m_Mediastinum, "slices_medias.mhd");
  writeImage<MaskImageType>(m_Working_Support, "slices_support.mhd");


  // List of points
  std::vector<MaskImagePointType> p_RightMostAnt;
  std::vector<MaskImagePointType> p_RightMostPost;
  std::vector<MaskImagePointType> p_LeftMostAnt;
  std::vector<MaskImagePointType> p_LeftMostPost;
  std::vector<MaskImagePointType> p_AllPoints;

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

  // Loop slices
  MaskImagePointType p;
  MaskImagePointType pp;
  for(uint i=0; i<slices.size() ; i++) {
    // Declare all needed points (sp = slice point)
    typename MaskSliceType::PointType sp_maxRight_Eso;    
    typename MaskSliceType::PointType sp_maxRight_Aorta;    
    typename MaskSliceType::PointType sp_maxRight_Vertebra;
    typename MaskSliceType::PointType sp_maxLeft_Eso; 
    typename MaskSliceType::PointType sp_maxLeft_Aorta;   
    typename MaskSliceType::PointType sp_maxLeft_Vertebra;
    
    // Right is at left on screen, coordinate decrease
    // Left is at right on screen, coordinate increase
    
    // Find right limit of Esophagus and Aorta
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(eso_slices[i], GetBackgroundValue(), 0, true, sp_maxRight_Eso);
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(aorta_slices[i], GetBackgroundValue(), 0, true, sp_maxRight_Aorta);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxRight_Eso, EsophagusForSlice, i, p);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxRight_Aorta, Aorta, i, pp);
    pp[0] -= 2; // Add a margin of 2 mm to include the 'wall'
    p_AllPoints.push_back(p);
    p_AllPoints.push_back(pp);
    if (p[0]<pp[0]) p_RightMostAnt.push_back(p); // Insert point most at right
    else p_RightMostAnt.push_back(pp);

    // Find limit of Vertebral -> only at most Post part of current
    // slice support.  First found most ant point in VertebralBody
    typedef MaskSliceType SliceType;
    typename SliceType::PointType p_slice_ant;
    bool found = clitk::FindExtremaPointInAGivenDirection<SliceType>(vert_slices[i], GetBackgroundValue(), 1, true, p_slice_ant);
    if (!found) {
      // It should not happen ! But sometimes, a contour is missing or
      // the VertebralBody is not delineated enough inferiorly ... in
      // those cases, we consider the first found slice.
      std::cerr << "No foreground pixels in this VertebralBody slices !?? I try with the previous/next slice" << std::endl;
      DD(i);
      int j=i++;
      bool found = false;
      while (!found) {
        found = clitk::FindExtremaPointInAGivenDirection<SliceType>(vert_slices[j], GetBackgroundValue(), 1, true, p_slice_ant);
        //clitkExceptionMacro("No foreground pixels in this VertebralBody slices ??");
        j++;
      }
      DD(j);        
    }
    p_slice_ant[1] += GetDistanceMaxToAnteriorPartOfTheSpine(); // Consider offset
    
    // The, find most Right and Left points on that AP position
    typename SliceType::IndexType indexR;
    typename SliceType::IndexType indexL;
    vert_slices[i]->TransformPhysicalPointToIndex(p_slice_ant, indexR);
    indexL = indexR;
    // Check that is inside the mask
    indexR[1] = std::min(indexR[1], (long)vert_slices[i]->GetLargestPossibleRegion().GetSize()[1]-1);
    indexL[1] = indexR[1];
    while (vert_slices[i]->GetPixel(indexR) != GetBackgroundValue()) {
      indexR[0] --; // Go to the right
    }
    while (vert_slices[i]->GetPixel(indexL) != GetBackgroundValue()) {
      indexL[0] ++; // Go to the left
    }
    vert_slices[i]->TransformIndexToPhysicalPoint(indexR, sp_maxRight_Vertebra);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxRight_Vertebra, VertebralBody, i, p);
    p_AllPoints.push_back(p);
    vert_slices[i]->TransformIndexToPhysicalPoint(indexL, sp_maxLeft_Vertebra);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxLeft_Vertebra, VertebralBody, i, p);
    p_AllPoints.push_back(p);
    
    // Find last point out of the mediastinum on this line, Right :
    mediast_slices[i]->TransformPhysicalPointToIndex(sp_maxRight_Vertebra, indexR);
    double distance = 0.0;
    while (mediast_slices[i]->GetPixel(indexR) != GetBackgroundValue()) {
      indexR[0] --;
      distance += mediast_slices[i]->GetSpacing()[0];
    }
    if (distance < 30) { // Ok in this case, we found limit with lung
      mediast_slices[i]->TransformIndexToPhysicalPoint(indexR, sp_maxRight_Vertebra);
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxRight_Vertebra, m_Mediastinum, i, p);
    }
    else { // in that case, we are probably below the diaphragm, so we
           // add aribtrarly few mm
      sp_maxRight_Vertebra[0] -= 2; // Leave 2 mm around the VertebralBody 
    }
    p_RightMostPost.push_back(p);
    p_AllPoints.push_back(p);

    // Find last point out of the mediastinum on this line, Left :
    mediast_slices[i]->TransformPhysicalPointToIndex(sp_maxLeft_Vertebra, indexL);
    distance = 0.0;
    while (mediast_slices[i]->GetPixel(indexL) != GetBackgroundValue()) {
      indexL[0] ++;
      distance += mediast_slices[i]->GetSpacing()[0];
    }
    if (distance < 30) { // Ok in this case, we found limit with lung
      mediast_slices[i]->TransformIndexToPhysicalPoint(indexL, sp_maxLeft_Vertebra);
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxLeft_Vertebra, m_Mediastinum, i, p);
    }
    else { // in that case, we are probably below the diaphragm, so we
           // add aribtrarly few mm
      sp_maxLeft_Vertebra[0] += 2; // Leave 2 mm around the VertebralBody 
    }
    p_LeftMostPost.push_back(p);
    p_AllPoints.push_back(p);

    // --------------------------------------------------------------------------
    // Find the limit on the Left: most left point between Eso and
    // Vertebra. (Left is left on screen, coordinate increase)
    
    // Find left limit of Esophagus
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(eso_slices[i], GetBackgroundValue(), 0, false, sp_maxLeft_Eso);
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(aorta_slices[i], GetBackgroundValue(), 0, false, sp_maxLeft_Aorta);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxLeft_Eso, EsophagusForSlice, i, p);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxLeft_Aorta, Aorta, i, pp);
    p_AllPoints.push_back(p);
    pp[0] += 2; // Add a margin of 2 mm to include the 'wall'
    p_AllPoints.push_back(pp);
    if (p[0]>pp[0]) p_LeftMostAnt.push_back(p); // Insert point most at right
    else p_LeftMostAnt.push_back(pp);
  } // End of slice loop
  
  clitk::WriteListOfLandmarks<MaskImageType>(p_AllPoints, "LR-Eso-Vert.txt");

  // Now uses these points to limit, slice by slice 
  // Line is mainly vertical, so mainDirection=0
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_RightMostAnt, p_RightMostPost,
                                                                    GetBackgroundValue(), 0, 10);
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_LeftMostAnt, p_LeftMostPost,
                                                                    GetBackgroundValue(), 0, -10);
  // DEBUG
  m_ListOfStations["8"] = m_Working_Support;
  return;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
typename clitk::ExtractLymphStationsFilter<ImageType>::MaskImagePointer
clitk::ExtractLymphStationsFilter<ImageType>::
EnlargeEsophagusDilatationRadiusInferiorly(MaskImagePointer & Esophagus)
{
  // Check if Esophagus is delineated at least until GOjunction. Now,
  // because we use AutoCrop, Origin[2] gives this max inferior
  // position.
  //  double GOjunctionZ = GetAFDB()->GetPoint3D("GOjunction", 2);

  if (Esophagus->GetOrigin()[2] > m_DiaphragmInferiorLimit) {
    std::cout << "Warning Esophagus is not delineated until Diaphragm. I mirror-pad it." 
              << std::endl;
    double extraSize = Esophagus->GetOrigin()[2]-m_DiaphragmInferiorLimit; 
    
    // Pad with few more slices
    typedef itk::MirrorPadImageFilter<MaskImageType, MaskImageType> PadFilterType;
    typename PadFilterType::Pointer padFilter = PadFilterType::New();
    padFilter->SetInput(Esophagus);
    MaskImageSizeType b;
    b[0] = 0; b[1] = 0; b[2] = (uint)ceil(extraSize/Esophagus->GetSpacing()[2])+1;
    padFilter->SetPadLowerBound(b);
    padFilter->Update();
    Esophagus = padFilter->GetOutput();
  }
  return Esophagus;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_LR_Limits_old() 
{
  /*
    Station 8: paraeosphageal nodes

    Laterally, it is within the pleural envelope and again abuts the
    descending aorta on the left. Reasonably, the delineation of
    Station 8 is limited to the soft tissue surrounding the esophagus
    (Fig.  3C–H). 
  */

  StartNewStep("[Station8] Right limits (around esophagus)");
  // Get Esophagus
  MaskImagePointer Esophagus = GetAFDB()->template GetImage<MaskImageType>("Esophagus");

  // Autocrop to get first slice with starting Esophagus
  Esophagus = clitk::AutoCrop<MaskImageType>(Esophagus, GetBackgroundValue()); 

  // Dilate 
  // LR dilatation -> large to keep point inside
  // AP dilatation -> few mm 
  // SI dilatation -> enough to cover Diaphragm (old=GOjunctionZ)
  MaskImagePointType radiusInMM = GetEsophagusDiltationForRight();
  Esophagus = EnlargeEsophagusDilatationRadiusInferiorly(Esophagus);
  Esophagus = clitk::Dilate<MaskImageType>(Esophagus, 
                                           radiusInMM, 
                                           GetBackgroundValue(), 
                                           GetForegroundValue(), true);
  writeImage<MaskImageType>(Esophagus, "dilateEso2.mhd");

  writeImage<MaskImageType>(m_Working_Support, "before-relpos2.mhd");

  // Remove Right (Left on the screen) part according to this
  // dilatated esophagus
  typedef clitk::SliceBySliceRelativePositionFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->WriteStepFlagOff();
  relPosFilter->SetInput(m_Working_Support);
  relPosFilter->SetInputObject(Esophagus);
  relPosFilter->AddOrientationTypeString("L");
  relPosFilter->InverseOrientationFlagOn(); // Not Left to
  relPosFilter->SetDirection(2); // Z axis
  relPosFilter->UniqueConnectedComponentBySliceOff(); // important
  relPosFilter->SetIntermediateSpacing(4);
  relPosFilter->ResampleBeforeRelativePositionFilterOn();
  relPosFilter->SetFuzzyThreshold(0.9); // remove few part only
  relPosFilter->RemoveObjectFlagOff();
  relPosFilter->Update();
  m_Working_Support = relPosFilter->GetOutput();

  // Get a single 3D CCL
  m_Working_Support = Labelize<MaskImageType>(m_Working_Support, 0, false, 10);
  m_Working_Support = KeepLabels<MaskImageType>(m_Working_Support, 
                                                GetBackgroundValue(), 
                                                GetForegroundValue(), 1, 1, true);


  /*
  // Re-Add post to Esophagus -> sometimes previous relpos remove some
  // correct part below esophagus.
  MaskImagePointer Esophagus = GetAFDB()->template GetImage<MaskImageType>("Esophagus");
  EnlargeEsophagusDilatationRadiusInferiorly(Esophagus);
  writeImage<MaskImageType>(Esophagus, "e-again.mhd");
  typedef clitk::SliceBySliceRelativePositionFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInput(m_Working_Support);
  relPosFilter->SetInputObject(Esophagus);
  relPosFilter->SetOrientationTypeString("P");
  relPosFilter->InverseOrientationFlagOff(); 
  relPosFilter->SetDirection(2); // Z axis
  relPosFilter->UniqueConnectedComponentBySliceOff(); // important
  relPosFilter->SetIntermediateSpacing(4);
  relPosFilter->ResampleBeforeRelativePositionFilterOn();
  relPosFilter->CombineWithOrFlagOn();
  relPosFilter->SetFuzzyThreshold(0.9); // remove few part only
  relPosFilter->RemoveObjectFlagOff();
  relPosFilter->Update();
  m_Working_Support = relPosFilter->GetOutput();
  */

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
}
//--------------------------------------------------------------------
