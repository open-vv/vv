
#include <itkBinaryDilateImageFilter.h>
#include <itkMirrorPadImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_SetDefaultValues()
{
  SetDistanceMaxToAnteriorPartOfTheSpine(10);
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

  std::vector<MaskImagePointType> centroids;
  clitk::ComputeCentroids<MaskImageType>(Carina, GetBackgroundValue(), centroids);
  m_CarinaZ = centroids[1][2];
  // DD(m_CarinaZ);
  // add one slice to include carina ?
  m_CarinaZ += m_Mediastinum->GetSpacing()[2];
  // We dont need Carina structure from now
  Carina->Delete();
  GetAFDB()->SetDouble("CarinaZ", m_CarinaZ);
  
  // Found most inferior part of the lung
  MaskImagePointer Lungs = GetAFDB()->template GetImage<MaskImageType>("Lungs");
  // It should be already croped, so I took the origin and add 10mm above 
  m_DiaphragmInferiorLimit = Lungs->GetOrigin()[2]+10;
  //  Lungs->Delete(); // we don't need it, release memory -> it we want to release, also free in AFDB
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after reading lungs");
  GetAFDB()->template ReleaseImage<MaskImageType>("Lungs");
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after release lungs");

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
  /*
    
    DO NOT REMOVE AORTA YET (LATER)

    boolFilter->SetInput1(boolFilter->GetOutput());
    boolFilter->SetInput2(Aorta);    
    boolFilter->SetOperationType(BoolFilterType::AndNot);
    boolFilter->Update();    
  */ 
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
  clitk::PointsUtils<MaskImageType>::Convert2DMapTo3DList(vertebralAntPositionBySlice, 
                                                          VertebralBody, 
                                                          vertebralAntPositions);

  // DEBUG : write list of points
  clitk::WriteListOfLandmarks<MaskImageType>(vertebralAntPositions, 
                                             "S8-vertebralMostAntPositions-points.txt");

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
ExtractStation_8_Ant_Sup_Limits() 
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
 
  MaskImagePointer m_Working_Trachea = 
    clitk::CropImageAbove<MaskImageType>(Trachea, 2, m_CarinaZ, true, // AutoCrop
                                         GetBackgroundValue());

  // Seprate into two main bronchi
  MaskImagePointer RightBronchus;
  MaskImagePointer LeftBronchus;

  // Labelize and consider the two first (main) labels
  m_Working_Trachea = Labelize<MaskImageType>(m_Working_Trachea, 0, true, 1);

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

  ImagePixelType rightLabel;
  ImagePixelType leftLabel;  
  if (C1[0] < C2[0]) { rightLabel = 1; leftLabel = 2; }
  else { rightLabel = 2; leftLabel = 1; }

  // Select LeftLabel (set one label to Backgroundvalue)
  RightBronchus = 
    clitk::Binarize<MaskImageType>(m_Working_Trachea, rightLabel, rightLabel, 
                                   GetBackgroundValue(), GetForegroundValue());
  /*
    SetBackground<MaskImageType, MaskImageType>(m_Working_Trachea, m_Working_Trachea, 
    leftLabel, GetBackgroundValue(), false);
  */
  LeftBronchus = clitk::Binarize<MaskImageType>(m_Working_Trachea, leftLabel, leftLabel, 
                                                GetBackgroundValue(), GetForegroundValue());
  /*
    SetBackground<MaskImageType, MaskImageType>(m_Working_Trachea, m_Working_Trachea, 
    rightLabel, GetBackgroundValue(), false);
  */

  // Crop images
  RightBronchus = clitk::AutoCrop<MaskImageType>(RightBronchus, GetBackgroundValue()); 
  LeftBronchus = clitk::AutoCrop<MaskImageType>(LeftBronchus, GetBackgroundValue()); 

  // Insert int AFDB if need after 
  GetAFDB()->template SetImage <MaskImageType>("RightBronchus", "seg/rightBronchus.mhd", 
                                               RightBronchus, true);
  GetAFDB()->template SetImage <MaskImageType>("LeftBronchus", "seg/leftBronchus.mhd", 
                                               LeftBronchus, true);

  // Now crop below OriginOfRightMiddleLobeBronchusZ
  // It is not done before to keep entire bronchi.
  
  MaskImagePointer OriginOfRightMiddleLobeBronchus = 
    GetAFDB()->template GetImage<MaskImageType>("OriginOfRightMiddleLobeBronchus");
  std::vector<MaskImagePointType> centroids;
  clitk::ComputeCentroids<MaskImageType>(OriginOfRightMiddleLobeBronchus, GetBackgroundValue(), centroids);
  m_OriginOfRightMiddleLobeBronchusZ = centroids[1][2];
  // add one slice to include carina ?
  m_OriginOfRightMiddleLobeBronchusZ += RightBronchus->GetSpacing()[2];
  // We dont need Carina structure from now
  OriginOfRightMiddleLobeBronchus->Delete();

  RightBronchus = 
    clitk::CropImageBelow<MaskImageType>(RightBronchus, 2, 
                                         m_OriginOfRightMiddleLobeBronchusZ, 
                                         true, // AutoCrop
                                         GetBackgroundValue());
  LeftBronchus = 
    clitk::CropImageBelow<MaskImageType>(LeftBronchus, 2, 
                                         m_OriginOfRightMiddleLobeBronchusZ, 
                                         true, // AutoCrop
                                         GetBackgroundValue());

  // Search for points that are the most left/post/ant and most
  // right/post/ant of the left and right bronchus
  // 15  = not 15 mm more distance than the middle point.
  FindExtremaPointsInBronchus(RightBronchus, 0, 10, m_LeftMostInRightBronchus, 
			      m_AntMostInRightBronchus, m_PostMostInRightBronchus);

  FindExtremaPointsInBronchus(LeftBronchus, 1, 10, m_RightMostInLeftBronchus, 
			      m_AntMostInLeftBronchus, m_PostMostInLeftBronchus);

  // DEBUG : write the list of points
  ListOfPointsType v;
  v.reserve(m_LeftMostInRightBronchus.size()+m_AntMostInRightBronchus.size()+
            m_PostMostInRightBronchus.size());
  v.insert(v.end(), m_LeftMostInRightBronchus.begin(), m_LeftMostInRightBronchus.end() );
  v.insert(v.end(), m_AntMostInRightBronchus.begin(), m_AntMostInRightBronchus.end() );
  v.insert(v.end(), m_PostMostInRightBronchus.begin(), m_PostMostInRightBronchus.end() );
  clitk::WriteListOfLandmarks<MaskImageType>(v, "S8-RightBronchus-points.txt");

  v.clear();
  v.reserve(m_RightMostInLeftBronchus.size()+m_AntMostInLeftBronchus.size()+
            m_PostMostInLeftBronchus.size());
  v.insert(v.end(), m_RightMostInLeftBronchus.begin(), m_RightMostInLeftBronchus.end() );
  v.insert(v.end(), m_AntMostInLeftBronchus.begin(), m_AntMostInLeftBronchus.end() );
  v.insert(v.end(), m_PostMostInLeftBronchus.begin(), m_PostMostInLeftBronchus.end() );
  clitk::WriteListOfLandmarks<MaskImageType>(v, "S8-LeftBronchus-points.txt");

  v.clear();
  v.reserve(m_PostMostInLeftBronchus.size()+m_PostMostInRightBronchus.size());
  v.insert(v.end(), m_PostMostInLeftBronchus.begin(), m_PostMostInLeftBronchus.end() );
  v.insert(v.end(), m_PostMostInRightBronchus.begin(), m_PostMostInRightBronchus.end() );
  clitk::WriteListOfLandmarks<MaskImageType>(v, "S8-RightLeftBronchus-points.txt");


  // Now uses these points to limit, slice by slice 
  // line is mainly horizontal, so mainDirection=1
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    m_PostMostInRightBronchus,
                                                                    m_PostMostInLeftBronchus,
                                                                    GetBackgroundValue(), 1, 10); 

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
ExtractStation_8_Ant_Inf_Limits() 
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
  clitk::PrintMemory(GetVerboseMemoryFlag(), "after read Esophagus");

  // In images from the original article, Atlas – UM, the oesophagus
  //was included in nodal stations 3p and 8.  Having said that, in the
  //description for station 8, it indicates that “the delineation of
  //station 8 is limited to the soft tissues surrounding the
  //oesophagus”.  In the recent article, The IASLC Lung Cancer Staging
  //Project (J Thorac Oncol 4:5, 568-77), the images drawn by
  //Dr. Aletta Frasier exclude this structure.  From an oncological
  //prospective, the oesophagus should be excluded from these nodal
  //stations.

  /* NOT YET !! DO IT LATER

     typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BoolFilterType;
     typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
     boolFilter->InPlaceOn();
     boolFilter->SetInput1(m_Working_Support);
     boolFilter->SetInput2(m_Esophagus);    
     boolFilter->SetOperationType(BoolFilterType::AndNot);
     boolFilter->Update();    
     m_Working_Support = boolFilter->GetOutput();

  */

  // Crop Esophagus : keep only below the OriginOfRightMiddleLobeBronchusZ
  m_Esophagus = 
    clitk::CropImageAbove<MaskImageType>(m_Esophagus, 2, 
                                         m_OriginOfRightMiddleLobeBronchusZ, 
                                         true, // AutoCrop
                                         GetBackgroundValue());

  // Dilate to keep only not Anterior positions
  MaskImagePointType radiusInMM = GetEsophagusDiltationForAnt();

  //  m_Esophagus = EnlargeEsophagusDilatationRadiusInferiorly(m_Esophagus);

  m_Esophagus = clitk::Dilate<MaskImageType>(m_Esophagus, 
                                             radiusInMM, 
                                             GetBackgroundValue(), 
                                             GetForegroundValue(), true);

  // Remove Anterior part according to this dilatated esophagus. Note:
  // because we crop Esophagus with ORML, the support will also be
  // croped in the same way. Here it is a desired feature. If we dont
  // want, use SetIgnoreEmptySliceObject(true)

  // In the new IASCL definition, it is not clear if sup limits is
  //  around carina or On the right, it is “the lower border of the
  //  bronchus intermedius”, indicated on the image set as a point
  //  (“lower border of the bronchus intermedius”)

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
ExtractStation_8_Ant_Injected_Limits() 
{

  //--------------------------------------------------------------------
  StartNewStep("[Station8] Ant part (remove high density, injected part)");

  // Consider initial image, crop to current support
  ImagePointer working_input = 
    clitk::ResizeImageLike<ImageType>(m_Input, 
                                      m_Working_Support, 
                                      (short)GetBackgroundValue());
  
  // Threshold
  typedef itk::BinaryThresholdImageFilter<ImageType, MaskImageType> BinarizeFilterType; 
  typename BinarizeFilterType::Pointer binarizeFilter = BinarizeFilterType::New();
  binarizeFilter->SetInput(working_input);
  binarizeFilter->SetLowerThreshold(GetInjectedThresholdForS8());
  binarizeFilter->SetInsideValue(GetForegroundValue());
  binarizeFilter->SetOutsideValue(GetBackgroundValue());
  binarizeFilter->Update();
  MaskImagePointer injected = binarizeFilter->GetOutput();
  
  // Combine with current support
  clitk::AndNot<MaskImageType>(m_Working_Support, injected, GetBackgroundValue());

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_LR_1_Limits() 
{
  //--------------------------------------------------------------------
  StartNewStep("[Station8] Left and Right (from Carina to PulmonaryTrunk): Right to LeftPulmonaryArtery");
  
  /*
    We remove LeftPulmonaryArtery structure and what is at Left to
    this structure.
  */
  MaskImagePointer LeftPulmonaryArtery = GetAFDB()->template GetImage<MaskImageType>("LeftPulmonaryArtery");

  // Relative Position : not at Left
  typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->WriteStepFlagOff();
  relPosFilter->SetBackgroundValue(GetBackgroundValue());
  relPosFilter->SetInput(m_Working_Support); 
  relPosFilter->SetInputObject(LeftPulmonaryArtery); 
  relPosFilter->RemoveObjectFlagOn(); // remove the object too
  relPosFilter->AddOrientationTypeString("L");
  relPosFilter->InverseOrientationFlagOn(); // Not at Left
  relPosFilter->SetIntermediateSpacing(3);
  relPosFilter->IntermediateSpacingFlagOn();
  relPosFilter->SetFuzzyThreshold(0.7);
  relPosFilter->AutoCropFlagOn();
  relPosFilter->Update();   
  m_Working_Support = relPosFilter->GetOutput();

  // Release LeftPulmonaryArtery
  GetAFDB()->template ReleaseImage<MaskImageType>("LeftPulmonaryArtery");

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["8"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_8_LR_2_Limits() 
{
  //--------------------------------------------------------------------
  StartNewStep("[Station8] Left and Right (from PulmTrunk to OriginMiddleLobeBronchus) Right to line from Aorta to PulmonaryTrunk");

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
ExtractStation_8_LR_Limits_old2() 
{

  //--------------------------------------------------------------------
  StartNewStep("[Station8] Left and Right limits arround esophagus (below Carina)");

  // Estract slices for current support for slice by slice processing
  std::vector<typename MaskSliceType::Pointer> slices;
  clitk::ExtractSlices<MaskImageType>(m_Working_Support, 2, slices);
  
  // Dilate the Esophagus to consider a margins around
  MaskImagePointType radiusInMM = GetEsophagusDiltationForAnt();
  m_Esophagus = clitk::Dilate<MaskImageType>(m_Esophagus, 
                                             radiusInMM, 
                                             GetBackgroundValue(), 
                                             GetForegroundValue(), true);

  // Remove what is outside the mediastinum in this enlarged Esophagus -> it allows to select
  // 'better' extrema points (not too post).
  MaskImagePointer Lungs = GetAFDB()->template GetImage<MaskImageType>("Lungs");
  clitk::AndNot<MaskImageType>(m_Esophagus, Lungs, GetBackgroundValue());
  GetAFDB()->template ReleaseImage<MaskImageType>("Lungs");

  // Estract slices of Esophagus (resize like support before to have the same set of slices)
  MaskImagePointer EsophagusForSlice = clitk::ResizeImageLike<MaskImageType>(m_Esophagus, m_Working_Support, GetBackgroundValue());

  std::vector<typename MaskSliceType::Pointer> eso_slices;
  clitk::ExtractSlices<MaskImageType>(EsophagusForSlice, 2, eso_slices);

  // Estract slices of Vertebral (resize like support before to have the same set of slices)
  MaskImagePointer VertebralBody = GetAFDB()->template GetImage<MaskImageType>("VertebralBody");
  VertebralBody = clitk::ResizeImageLike<MaskImageType>(VertebralBody, m_Working_Support, GetBackgroundValue());
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
  std::vector<MaskImagePointType> p_RightMostAnt;
  std::vector<MaskImagePointType> p_RightMostPost;
  std::vector<MaskImagePointType> p_LeftMostAnt;
  std::vector<MaskImagePointType> p_LeftMostPost;
  std::vector<MaskImagePointType> p_AllPoints;
  std::vector<MaskImagePointType> p_LeftAorta;
  std::vector<MaskImagePointType> p_LeftEso;

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
      int j=i++;
      bool found = false;
      while (!found) {
        found = clitk::FindExtremaPointInAGivenDirection<SliceType>(vert_slices[j], GetBackgroundValue(), 1, true, p_slice_ant);
        //clitkExceptionMacro("No foreground pixels in this VertebralBody slices ??");
        j++;
      }
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
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxRight_Vertebra, m_Mediastinum, i, p);
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
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxLeft_Vertebra, m_Mediastinum, i, p);
    }
    p_LeftMostPost.push_back(p);
    p_AllPoints.push_back(p);

    // Find Eso slice centroid and do not consider what is post to
    // this centroid.
    std::vector<typename MaskSliceType::PointType> c;
    clitk::ComputeCentroids<MaskSliceType>(eso_slices[i], GetBackgroundValue(), c);
    if (c.size() >1) {
      eso_slices[i] = 
        clitk::CropImageAbove<MaskSliceType>(eso_slices[i], 1, c[1][1], false, GetBackgroundValue());
      eso_slices[i] = 
        clitk::ResizeImageLike<MaskSliceType>(eso_slices[i], aorta_slices[i], GetBackgroundValue());
      // writeImage<MaskSliceType>(eso_slices[i], "eso-slice-"+toString(i)+".mhd");
    }

    // Find right limit of Esophagus and Aorta
    bool f = 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(eso_slices[i], GetBackgroundValue(), 
                                                              0, true, sp_maxRight_Eso);
    f = f && 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(aorta_slices[i], GetBackgroundValue(), 
                                                              0, true, sp_maxRight_Aorta);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxRight_Eso, EsophagusForSlice, i, p);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxRight_Aorta, Aorta, i, pp);
    pp[0] -= 2; // Add a margin of 2 mm to include the Aorta 'wall'
    p_AllPoints.push_back(p);
    if (f) {
      p_AllPoints.push_back(pp);
      MaskImagePointType A = p_RightMostPost.back();
      MaskImagePointType B = p;
      MaskImagePointType C = pp;
      double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
      if (s>0) p_RightMostAnt.push_back(p);
      else p_RightMostAnt.push_back(pp);
    }
    else { // No more Esophagus in this slice : do nothing
      //      p_RightMostAnt.push_back(p); 
      p_RightMostPost.pop_back();
    }

    // --------------------------------------------------------------------------
    // Find the limit on the Left: most left point between Eso and
    // Eso. (Left is left on screen, coordinate increase)
    
    // Find left limit of Esophagus
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(eso_slices[i], GetBackgroundValue(), 0, false, sp_maxLeft_Eso);
    f = clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(aorta_slices[i], GetBackgroundValue(), 0, false, sp_maxLeft_Aorta);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxLeft_Eso, EsophagusForSlice, i, p);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_maxLeft_Aorta, Aorta, i, pp);
    p_AllPoints.push_back(p);
    pp[0] += 2; // Add a margin of 2 mm to include the 'wall'
    if (f) { // not below Aorta
      p_AllPoints.push_back(pp);
      MaskImagePointType A = p_LeftMostPost.back();
      MaskImagePointType B = p;
      MaskImagePointType C = pp;
      double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
      if (s<0) {
        p_LeftMostAnt.push_back(p); // Insert point most at Left, Eso
      }
      else {
        // in this case -> two lines !
        p_LeftMostAnt.push_back(pp);  // Insert point most at Left, Aorta (Vert to Aorta)
        // but also consider Aorta to Eso
        p_LeftAorta.push_back(pp);
        p_LeftEso.push_back(p);
      }
    }
    else { // No more Esophagus in this slice : do nothing
      p_LeftMostPost.pop_back();
      //p_LeftMostAnt.push_back(p);
    }
  } // End of slice loop
  
  clitk::WriteListOfLandmarks<MaskImageType>(p_AllPoints, "S8-LR-Eso-Vert.txt");
  clitk::WriteListOfLandmarks<MaskImageType>(p_LeftEso, "S8-Left-Eso.txt");
  clitk::WriteListOfLandmarks<MaskImageType>(p_LeftAorta, "S8-Left-Aorta.txt");

  // Now uses these points to limit, slice by slice 
  // Line is mainly vertical, so mainDirection=0
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_RightMostAnt, p_RightMostPost,
                                                                    GetBackgroundValue(), 0, 10);
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_LeftMostAnt, p_LeftMostPost,
                                                                    GetBackgroundValue(), 0, -10);
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    p_LeftEso,p_LeftAorta, 
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
ExtractStation_8_LR_Limits() 
{

  //--------------------------------------------------------------------
  StartNewStep("[Station8] Left and Right limits arround esophagus with lines from VertebralBody-Aorta-Esophagus");

  // Estract slices for current support for slice by slice processing
  std::vector<typename MaskSliceType::Pointer> slices;
  clitk::ExtractSlices<MaskImageType>(m_Working_Support, 2, slices);
  
  // Dilate the Esophagus to consider a margins around
  MaskImagePointType radiusInMM = GetEsophagusDiltationForAnt();
  m_Esophagus = clitk::Dilate<MaskImageType>(m_Esophagus, radiusInMM, 
                                             GetBackgroundValue(), GetForegroundValue(), true);
  //  m_Esophagus = EnlargeEsophagusDilatationRadiusInferiorly(m_Esophagus);

  // Remove what is outside the mediastinum in this enlarged Esophagus -> it allows to select
  // 'better' extrema points (not too post).
  MaskImagePointer Lungs = GetAFDB()->template GetImage<MaskImageType>("Lungs");
  clitk::AndNot<MaskImageType>(m_Esophagus, Lungs, GetBackgroundValue());
  GetAFDB()->template ReleaseImage<MaskImageType>("Lungs");

  // Estract slices of Esophagus (resize like support before to have the same set of slices)
  MaskImagePointer EsophagusForSlice = clitk::ResizeImageLike<MaskImageType>(m_Esophagus, m_Working_Support, GetBackgroundValue());
  std::vector<typename MaskSliceType::Pointer> eso_slices;
  clitk::ExtractSlices<MaskImageType>(EsophagusForSlice, 2, eso_slices);

  // Estract slices of Vertebral (resize like support before to have the same set of slices)
  MaskImagePointer VertebralBody = GetAFDB()->template GetImage<MaskImageType>("VertebralBody");
  VertebralBody = clitk::ResizeImageLike<MaskImageType>(VertebralBody, m_Working_Support, GetBackgroundValue());
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

  // Loop slices
  for(uint i=0; i<slices.size() ; i++) {
    // Declare all needed 2D points (sp = slice point)
    typename MaskSliceType::PointType sp_MostRightVertebralBody;
    typename MaskSliceType::PointType sp_MostLeftVertebralBody;
    typename MaskSliceType::PointType sp_MostLeftAorta;
    typename MaskSliceType::PointType sp_temp;
    typename MaskSliceType::PointType sp_MostLeftEsophagus;
    
    // Right is at left on screen, coordinate decrease
    // Left is at right on screen, coordinate increase
    
    /* -------------------------------------------------------------------------------------
       Find most Left point in VertebralBody. Consider only the
       horizontal line which is 'DistanceMaxToAnteriorPartOfTheSpine'
       away from the most ant point.
     */
    typename MaskSliceType::PointType sp_MostAntVertebralBody;
    bool found = false;
    int j=i;
    while (!found) {
      found = clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(vert_slices[j], GetBackgroundValue(), 1, true, sp_MostAntVertebralBody);
      if (!found) {
        // It should not happen ! But sometimes, a contour is missing or
        // the VertebralBody is not delineated enough inferiorly ... in
        // those cases, we consider the first found slice.
        std::cerr << "No foreground pixels in this VertebralBody slices !?? I try with the previous/next slice" << std::endl;
        j++;
      }
    }
    sp_MostAntVertebralBody[1] += GetDistanceMaxToAnteriorPartOfTheSpine(); // Consider offset

    // Crop the vertebralbody below this most post line
    vert_slices[j] = 
      clitk::CropImageAbove<MaskSliceType>(vert_slices[j], 1, sp_MostAntVertebralBody[1], false, GetBackgroundValue());
    vert_slices[j] = 
      clitk::ResizeImageLike<MaskSliceType>(vert_slices[j], aorta_slices[i], GetBackgroundValue());
    //    writeImage<MaskSliceType>(vert_slices[i], "vert-slice-"+toString(i)+".mhd");

    // Find first point not in mediastinum
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(vert_slices[j], GetBackgroundValue(), 0, false, sp_MostLeftVertebralBody);
    sp_MostLeftVertebralBody = clitk::FindExtremaPointInAGivenLine<MaskSliceType>(mediast_slices[i], 0, false, sp_MostLeftVertebralBody, GetBackgroundValue(), 30);
    sp_MostLeftVertebralBody[0] += 2; // 2mm margin
    clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(vert_slices[j], GetBackgroundValue(), 0, true, sp_MostRightVertebralBody);
    sp_MostRightVertebralBody = clitk::FindExtremaPointInAGivenLine<MaskSliceType>(mediast_slices[i], 0, true, sp_MostRightVertebralBody, GetBackgroundValue(),30);
    sp_MostRightVertebralBody[0] -= 2; // 2 mm margin

    // Convert 2D points into 3D
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostRightVertebralBody, VertebralBody, i, p);
    p_MostRightVertebralBody.push_back(p);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostLeftVertebralBody, VertebralBody, i, p);
    p_MostLeftVertebralBody.push_back(p);

    /* -------------------------------------------------------------------------------------
       Find most Left point in Esophagus
     */
    found = clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(eso_slices[i], GetBackgroundValue(), 0, false, sp_MostLeftEsophagus);
    if (!found) { // No more Esophagus, I remove the previous point

      // if (p_MostLeftEsophagus.size() < 1)  {
      p_MostLeftVertebralBody.pop_back();      
      // }
      // else {
      //   // Consider the previous point
      //   p = p_MostLeftEsophagus.back();
      //   p_MostLeftEsophagus.push_back(p);
      //   sp_MostLeftEsophagus = sp_temp; // Retrieve previous 2D position
      //   found = true;
      // }
    }
    else {
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(sp_MostLeftEsophagus, EsophagusForSlice, i, p);
      p_MostLeftEsophagus.push_back(p);
      // sp_temp = sp_MostLeftEsophagus; // Store previous 2D position
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
  clitk::WriteListOfLandmarks<MaskImageType>(p_MostRightVertebralBody, "S8-MostRight-VB-points.txt");
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
  StartNewStep("[Station8] remove some structures");

  Remove_Structures("8", "Aorta");
  Remove_Structures("8", "Esophagus");

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
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
  // Check if Esophagus is delineated at least until Diaphragm. Now,
  // because we use AutoCrop, Origin[2] gives this max inferior
  // position.

  DD("BUGGY DONT USE");
  exit(0);

  if (Esophagus->GetOrigin()[2] > m_DiaphragmInferiorLimit) {
    // crop first slice without mask
    MaskImagePointType pt;
    clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Esophagus, GetBackgroundValue(), 2, true, pt);
    DD(pt);
    Esophagus = 
      clitk::CropImageBelow<MaskImageType>(Esophagus, 2, 
                                           pt[2], 
                                           false, // AutoCrop
                                           GetBackgroundValue());
    writeImage<MaskImageType>(Esophagus, "crop-eso.mhd");

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
  relPosFilter->AddOrientationTypeString("NotLeftTo");
  //  relPosFilter->InverseOrientationFlagOn(); // Not Left to
  relPosFilter->SetDirection(2); // Z axis
  relPosFilter->UniqueConnectedComponentBySliceOff(); // important
  relPosFilter->SetIntermediateSpacing(4);
  relPosFilter->IntermediateSpacingFlagOn();
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
  relPosFilter->IntermediateSpacingFlagOn();
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
