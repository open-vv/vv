
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
  DD(centroids.size());
  DD(centroids[0]); // BG
  DD(centroids[1]);
  m_CarinaZ = centroids[1][2];
  DD(m_CarinaZ);
  // add one slice to include carina ?
  m_CarinaZ += m_Mediastinum->GetSpacing()[2];
  DD(m_CarinaZ);
  DD(Carina->GetReferenceCount());
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
ExtractStation_8_AP_Limits() 
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
 
  // Crop above Carina
  //double m_CarinaZ = GetAFDB()->GetPoint3D("Carina", 2) + 
  //    Trachea->GetSpacing()[2]; // add one slice to include carina;

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
  
  /*
    double OriginOfRightMiddleLobeBronchusZ = 
    GetAFDB()->GetPoint3D("OriginOfRightMiddleLobeBronchus", 2)+
    LeftBronchus->GetSpacing()[2]; 
    // ^--> Add one slice because the origin is the first slice without S7
    DD(OriginOfRightMiddleLobeBronchusZ);
    DD(OriginOfRightMiddleLobeBronchusZ-LeftBronchus->GetSpacing()[2]);  
  */

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
  // http://www.gamedev.net/community/forums/topic.asp?topic_id=542870
  /*
    Assuming the points are (Ax,Ay) (Bx,By) and (Cx,Cy), you need to compute:
    (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax)
    This will equal zero if the point C is on the line formed by
    points A and B, and will have a different sign depending on the
    side. Which side this is depends on the orientation of your (x,y)
    coordinates, but you can plug test values for A,B and C into this
    formula to determine whether negative values are to the left or to
    the right.
    => to accelerate, start with formula, when change sign -> stop and fill
  */
  typedef itk::ImageSliceIteratorWithIndex<MaskImageType> SliceIteratorType;
  SliceIteratorType siter = SliceIteratorType(m_Working_Support, 
                                              m_Working_Support->GetLargestPossibleRegion());
  siter.SetFirstDirection(0);
  siter.SetSecondDirection(1);
  siter.GoToBegin();
  int i=0;
  MaskImageType::PointType A;
  MaskImageType::PointType B;
  MaskImageType::PointType C;
  while (!siter.IsAtEnd()) {
    // Check that the current slice correspond to the current point
    m_Working_Support->TransformIndexToPhysicalPoint(siter.GetIndex(), C);
    if (C[2] != m_PostMostInLeftBronchus[i][2]) {
      // m_Working_Support start from GOjunction while list of point
      // start at OriginOfRightMiddleLobeBronchusZ, so we must skip some slices.
    }
    else {
      // Define A,B,C points
      A = m_PostMostInLeftBronchus[i];
      B = m_PostMostInRightBronchus[i];
      C = A;
      C[1] += 10; // I know I must keep this point
      double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
      bool isPositive = s<0;
      while (!siter.IsAtEndOfSlice()) {
        while (!siter.IsAtEndOfLine()) {
          // Very slow, I know ... but image should be very small
          m_Working_Support->TransformIndexToPhysicalPoint(siter.GetIndex(), C);
          double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
          if (s == 0) siter.Set(2);
          if (isPositive) {
            if (s > 0) siter.Set(GetBackgroundValue());
          }
          else {
            if (s < 0) siter.Set(GetBackgroundValue());
          }
          ++siter;
        }
        siter.NextLine();
      }
      ++i;
    }
    siter.NextSlice();
  }

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

  //--------------------------------------------------------------------
  StartNewStep("[Station8] Ant limits arround esophagus below Carina");
  /*
    Consider Esophagus, dilate it and remove ant part. It remains part
    on L & R, than can be partly removed by cutting what remains at
    right of vertebral body.
  */
  
  // Get Esophagus
  DD("Esophagus");
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
  VertebralBody = GetAFDB()->template GetImage<MaskImageType>("VertebralBody");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after Read VertebralBody");
  VertebralBody = clitk::ResizeImageLike<MaskImageType>(VertebralBody, m_Working_Support, GetBackgroundValue());
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after VertebralBody Resize");
  std::vector<typename MaskSliceType::Pointer> vert_slices;
  clitk::ExtractSlices<MaskImageType>(VertebralBody, 2, vert_slices);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after VertebralBody slices");

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
  writeImage<MaskImageType>(m_Mediastinum, "slices_medias.mhd");
  writeImage<MaskImageType>(m_Working_Support, "slices_support.mhd");

  // Find common slices between Eso and m_Working_Support
  // int s=0;
  // MaskImageIndexType z_Eso = Esophagus->GetLargestPossibleRegion().GetIndex();
  // MaskImagePointType p_Eso; 
  // Esophagus->TransformIndexToPhysicalPoint(z_Eso, p_Eso);
  // MaskImageIndexType z_Support;  
  // z_Support = m_Working_Support->GetLargestPossibleRegion().GetIndex();
  // MaskImagePointType p_Support; 
  // m_Working_Support->TransformIndexToPhysicalPoint(z_Support, p_Support);
  // while (p_Eso[2] < p_Support[2]) {
  //   z_Eso[2] ++;
  //   Esophagus->TransformIndexToPhysicalPoint(z_Eso, p_Eso);    
  // }
  // s = z_Eso[2] - Esophagus->GetLargestPossibleRegion().GetIndex()[2];
  // DD(s);

  // Find common slices between m_Working_Support and Mediastinum
  // int sm=0;
  // MaskImageIndexType z_Mediast = m_Mediastinum->GetLargestPossibleRegion().GetIndex();
  // MaskImagePointType p_Mediast; 
  // m_Mediastinum->TransformIndexToPhysicalPoint(z_Mediast, p_Mediast);
  // z_Support = m_Working_Support->GetLargestPossibleRegion().GetIndex();
  // m_Working_Support->TransformIndexToPhysicalPoint(z_Support, p_Support);
  // while (p_Mediast[2] < p_Support[2]) {
  //   z_Mediast[2] ++;
  //   m_Mediastinum->TransformIndexToPhysicalPoint(z_Mediast, p_Mediast);    
  // }
  // sm = z_Mediast[2] - m_Mediastinum->GetLargestPossibleRegion().GetIndex()[2];
  // DD(sm);
  
  DD(EsophagusForSlice->GetLargestPossibleRegion().GetSize()[2]);
  DD(m_Mediastinum->GetLargestPossibleRegion().GetSize()[2]);
  DD(slices.size());
  DD(vert_slices.size());
  DD(eso_slices.size());
  DD(mediast_slices.size());

  // uint max = std::min(slices.size(), std::min(eso_slices.size()-s, mediast_slices.size()-sm));
  // DD(max);

  // DEBUG POINTS
  std::ofstream osp;
  openFileForWriting(osp, "point_s8.txt");
  osp << "LANDMARKS1" << std::endl;

  // Loop slices
  MaskImagePointType p;
  int pi=0;
  for(uint i=0; i<slices.size() ; i++) {
    DD(i);

    // --------------------------------------------------------------------------
    // Find the limit on the Right: most right point between Eso and
    // Vertebra. (Right is left on screen, coordinate decrease)
    typename MaskSliceType::PointType p_maxRight;
    
    // Find right limit of Esophagus
    typename MaskSliceType::PointType p_maxRight_Eso;    
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(eso_slices[i], GetBackgroundValue(), 
                                                            1, false, p_maxRight_Eso);
    // Debug point 
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p_maxRight_Eso, EsophagusForSlice, i, p);
    osp << pi << " " << p[0] << " " << p[1] << " " << p[2] << " 0 0 " << std::endl;
    ++pi;

    // Find right limit of Vertebra
    typename MaskSliceType::PointType p_maxRight_Vertebra;
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(vert_slices[i], GetBackgroundValue(), 
                                                            1, false, p_maxRight_Vertebra);
    // Debug point 
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p_maxRight_Vertebra, VertebralBody, i, p);
    osp << pi << " " << p[0] << " " << p[1] << " " << p[2] << " 0 0 " << std::endl;
    ++pi;
    
    // Find last point out of the mediastinum on this line :
    typename MaskSliceType::IndexType index;
    mediast_slices[i]->TransformPhysicalPointToIndex(p_maxRight_Vertebra, index);
    double distance = 0.0;
    while (mediast_slices[i]->GetPixel(index) != GetBackgroundValue()) {
      index[0] --;
      distance += mediast_slices[i]->GetSpacing()[0];
    }
    DD(distance);
    if (distance < 20) { // Ok in this case, we found limit with lung
      mediast_slices[i]->TransformIndexToPhysicalPoint(index, p_maxRight_Vertebra);
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p_maxRight_Vertebra, m_Mediastinum, i, p);
      osp << pi << " " << p[0] << " " << p[1] << " " << p[2] << " 0 0 " << std::endl;
      ++pi;
    }
    
    // Choose the most extrema one
    if (p_maxRight_Vertebra[0] < p_maxRight_Eso[0]) {
      p_maxRight = p_maxRight_Vertebra;
    }
    else p_maxRight = p_maxRight_Eso;

    // --------------------------------------------------------------------------


    /*
    // Get most post of dilated Esophagus
    typename MaskSliceType::PointType p_post;
    bool f = clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(eso_slices[i], GetBackgroundValue(), 1, false, p_post);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p_post, EsophagusForSlice, i, p);
    osp << pi << " " << p[0] << " " << p[1] << " " << p[2] << " 0 0 " << std::endl;
    ++pi;
    // DD(f);
    // DD(p);

    // Get most left of the vertebral body
    typename MaskSliceType::PointType s_left;
    f = f && clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(vert_slices[i], GetBackgroundValue(), 0, false, s_left);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(s_left, VertebralBody, i, p);
    osp << pi << " " << p[0] << " " << p[1] << " " << p[2] << " 0 0 " << std::endl;
    ++pi;
    // DD(f);
    // DD(p);

    // Find last point out of the mediastinum on this line :
    typename MaskSliceType::IndexType index_left;
    mediast_slices[i]->TransformPhysicalPointToIndex(s_left, index_left);
    index_left[0] ++; // on more left to be inside the support
    while (mediast_slices[i]->GetPixel(index_left) != GetBackgroundValue()) {
    index_left[0] ++;
    }
    mediast_slices[i]->TransformIndexToPhysicalPoint(index_left, s_left);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(s_left, m_Mediastinum, i, p);
    osp << pi << " " << p[0] << " " << p[1] << " " << p[2] << " 0 0 " << std::endl;
    ++pi;
    // DD(f);
    // DD(p);
    */


    // Loop to suppress
    // if (f) {
    typename MaskSliceType::PointType p;
    typedef itk::ImageRegionIteratorWithIndex<MaskSliceType> IteratorType;
    IteratorType iter(slices[i], slices[i]->GetLargestPossibleRegion());
    iter.GoToBegin();
    while (!iter.IsAtEnd()) {
      if (iter.Get() != GetBackgroundValue()) {
        slices[i]->TransformIndexToPhysicalPoint(iter.GetIndex(), p);

        // Remove point too at RIGHT
        if (p[0] < p_maxRight[0]) {
          iter.Set(GetBackgroundValue());
        }

        /*
        // Remove point from foreground if too right or to high
        if ((p[1] > p_post[1]) && (p[0] > s_left[0])) {
        iter.Set(GetBackgroundValue());
        }
        */

      }
      ++iter;
    }
    // } // end if f
    // s++;
    // sm++;
  }
  osp.close();
  

  // Joint slices
  DD("after loop");
  m_Working_Support = clitk::JoinSlices<MaskImageType>(slices, m_Working_Support, 2);
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after JoinSlices");

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
ExtractStation_8_LR_Limits() 
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
