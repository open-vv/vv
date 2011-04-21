
//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_7_SetDefaultValues()
{
  SetFuzzyThreshold("7", "Bronchi", 0.1);
  SetFuzzyThreshold("7", "LeftSuperiorPulmonaryVein", 0.3);
  SetFuzzyThreshold("7", "RightSuperiorPulmonaryVein", 0.2);
  SetFuzzyThreshold("7", "RightPulmonaryArtery", 0.3);
  SetFuzzyThreshold("7", "LeftPulmonaryArtery", 0.5);
  SetFuzzyThreshold("7", "SVC", 0.2);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_7_SI_Limits() 
{
  StartNewStep("[Station7] Inf/Sup mediastinum limits with carina/LLLBronchus");
  // Get Inputs
  MaskImagePointer Trachea = GetAFDB()->template GetImage <MaskImageType>("Trachea");  
  
  // We suppoe that CarinaZ was already computed (S8)
  double m_CarinaZ = GetAFDB()->GetDouble("CarinaZ");
  
  //  double m_OriginOfRightMiddleLobeBronchusZ = GetAFDB()->GetPoint3D("OriginOfRightMiddleLobeBronchus", 2);
  // DD(m_OriginOfRightMiddleLobeBronchusZ);
  MaskImagePointer UpperBorderOfLLLBronchus = GetAFDB()->template GetImage<MaskImageType>("UpperBorderOfLLLBronchus");

  // Search most inf point (WHY ? IS IT THE RIGHT STRUCTURE ??)
  MaskImagePointType ps = UpperBorderOfLLLBronchus->GetOrigin(); // initialise to avoid warning 
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(UpperBorderOfLLLBronchus, GetBackgroundValue(), 2, true, ps);
  double m_UpperBorderOfLLLBronchusZ = ps[2];

  /*
  std::vector<MaskImagePointType> centroids;
  clitk::ComputeCentroids<MaskImageType>(UpperBorderOfLLLBronchus, GetBackgroundValue(), centroids);
  double m_UpperBorderOfLLLBronchusZ = centroids[1][2];
  DD(m_UpperBorderOfLLLBronchusZ)
  */

  /* Crop support */
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                m_UpperBorderOfLLLBronchusZ, 
                                                m_CarinaZ, true,
                                                GetBackgroundValue());
  /* Crop trachea */
  m_Working_Trachea = 
    clitk::CropImageAlongOneAxis<MaskImageType>(Trachea, 2, 
                                                m_UpperBorderOfLLLBronchusZ, 
                                                m_CarinaZ, true,
                                                GetBackgroundValue());

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["7"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_7_RL_Limits() 
{
  // ----------------------------------------------------------------
  StartNewStep("[Station7] Limits with bronchus : RightTo the left bronchus");  

  // First consider bronchus and keep the main CCL by slice
  m_RightBronchus = GetAFDB()->template GetImage <MaskImageType>("RightBronchus");
  m_LeftBronchus = GetAFDB()->template GetImage <MaskImageType>("LeftBronchus");

  // Extract slices, Label, compute centroid, keep most central connected component
  std::vector<MaskSlicePointer> slices_leftbronchus;
  std::vector<MaskSlicePointer> slices_rightbronchus;
  clitk::ExtractSlices<MaskImageType>(m_LeftBronchus, 2, slices_leftbronchus);
  clitk::ExtractSlices<MaskImageType>(m_RightBronchus, 2, slices_rightbronchus);
  
  // Loop on slices
  for(uint i=0; i<slices_leftbronchus.size(); i++) {
    slices_leftbronchus[i] = Labelize<MaskSliceType>(slices_leftbronchus[i], 0, false, 10);
    std::vector<typename MaskSliceType::PointType> c;
    clitk::ComputeCentroids<MaskSliceType>(slices_leftbronchus[i], GetBackgroundValue(), c);
    if (c.size() > 1) {
      double most_at_left = c[1][0];
      int most_at_left_index=1;
      for(uint j=1; j<c.size(); j++) {
        if (c[j][0] < most_at_left) {
          most_at_left = c[j][0];
          most_at_left_index = j;
        }
      }
      // Put all other CCL to Background
      slices_leftbronchus[i] = 
        clitk::Binarize<MaskSliceType>(slices_leftbronchus[i], most_at_left_index, 
                                       most_at_left_index, GetBackgroundValue(), GetForegroundValue());
    } // end c.size
  }
  
  for(uint i=0; i<slices_rightbronchus.size(); i++) {
    slices_rightbronchus[i] = Labelize<MaskSliceType>(slices_rightbronchus[i], 0, false, 10);
    std::vector<typename MaskSliceType::PointType> c;
    clitk::ComputeCentroids<MaskSliceType>(slices_rightbronchus[i], GetBackgroundValue(), c);
    if (c.size() > 1) {
      double most_at_right = c[1][0];
      int most_at_right_index=1;
      for(uint j=1; j<c.size(); j++) {
        if (c[j][0] > most_at_right) {
          most_at_right = c[j][0];
          most_at_right_index = j;
        }
      }
      // Put all other CCL to Background
      slices_rightbronchus[i] = 
        clitk::Binarize<MaskSliceType>(slices_rightbronchus[i], most_at_right_index, 
                                       most_at_right_index, GetBackgroundValue(), GetForegroundValue());
    } // end c.size
  }
  
  // Joint slices
  m_LeftBronchus = clitk::JoinSlices<MaskImageType>(slices_leftbronchus, m_LeftBronchus, 2);
  m_RightBronchus = clitk::JoinSlices<MaskImageType>(slices_rightbronchus, m_RightBronchus, 2);

  writeImage<MaskImageType>(m_LeftBronchus, "step-left.mhd");
  writeImage<MaskImageType>(m_RightBronchus, "step-right.mhd");

  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, m_LeftBronchus, 2, 
                                                       GetFuzzyThreshold("7", "Bronchi"), "RightTo", 
                                                       false, 3, false);
  StopCurrentStep<MaskImageType>(m_Working_Support);


  // ----------------------------------------------------------------
  StartNewStep("[Station7] Limits with bronchus : LeftTo the right bronchus");
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, m_RightBronchus, 2, 
                                                       GetFuzzyThreshold("7", "Bronchi"), "LeftTo", 
                                                       false, 3, false); 
  StopCurrentStep<MaskImageType>(m_Working_Support);


  // ----------------------------------------------------------------
  StartNewStep("[Station7] Limits with LeftSuperiorPulmonaryVein");
  try {
    MaskImagePointer LeftSuperiorPulmonaryVein = GetAFDB()->template GetImage<MaskImageType>("LeftSuperiorPulmonaryVein");
    typedef SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
    typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
    sliceRelPosFilter->SetInput(m_Working_Support);
    sliceRelPosFilter->SetInputObject(LeftSuperiorPulmonaryVein);
    sliceRelPosFilter->SetDirection(2);
    sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("7", "LeftSuperiorPulmonaryVein"));
    sliceRelPosFilter->AddOrientationTypeString("NotLeftTo");
    sliceRelPosFilter->AddOrientationTypeString("NotAntTo");
    sliceRelPosFilter->SetIntermediateSpacingFlag(true);
    sliceRelPosFilter->SetIntermediateSpacing(3);
    sliceRelPosFilter->SetUniqueConnectedComponentBySlice(false);
    sliceRelPosFilter->SetAutoCropFlag(false); 
    sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
    sliceRelPosFilter->Update();
    m_Working_Support = sliceRelPosFilter->GetOutput();
    StopCurrentStep<MaskImageType>(m_Working_Support);
  }
  catch (clitk::ExceptionObject e) {
    std::cout << "Not LeftSuperiorPulmonaryVein, skip" << std::endl;
  }

  // ----------------------------------------------------------------
  StartNewStep("[Station7] Limits with RightSuperiorPulmonaryVein");
  try {
    MaskImagePointer RightSuperiorPulmonaryVein = GetAFDB()->template GetImage<MaskImageType>("RightSuperiorPulmonaryVein");
    typedef SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
    typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
    sliceRelPosFilter->SetInput(m_Working_Support);
    sliceRelPosFilter->SetInputObject(RightSuperiorPulmonaryVein);
    sliceRelPosFilter->SetDirection(2);
    sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("7", "RightSuperiorPulmonaryVein"));
    sliceRelPosFilter->AddOrientationTypeString("NotRightTo");
    sliceRelPosFilter->AddOrientationTypeString("NotAntTo");
    sliceRelPosFilter->AddOrientationTypeString("NotPostTo");
    sliceRelPosFilter->SetIntermediateSpacingFlag(true);
    sliceRelPosFilter->SetIntermediateSpacing(3);
    sliceRelPosFilter->SetUniqueConnectedComponentBySlice(false);
    sliceRelPosFilter->SetAutoCropFlag(false); 
    sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
    sliceRelPosFilter->Update();
    m_Working_Support = sliceRelPosFilter->GetOutput();
    StopCurrentStep<MaskImageType>(m_Working_Support);
  }
  catch (clitk::ExceptionObject e) {
    std::cout << "Not RightSuperiorPulmonaryVein, skip" << std::endl;
  }

  // ----------------------------------------------------------------
  StartNewStep("[Station7] Limits with RightPulmonaryArtery");
  MaskImagePointer RightPulmonaryArtery = GetAFDB()->template GetImage<MaskImageType>("RightPulmonaryArtery");
  typedef SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(RightPulmonaryArtery);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("7", "RightPulmonaryArtery"));
  sliceRelPosFilter->AddOrientationTypeString("NotAntTo");
  sliceRelPosFilter->SetIntermediateSpacingFlag(true);
  sliceRelPosFilter->SetIntermediateSpacing(3);
  sliceRelPosFilter->SetUniqueConnectedComponentBySlice(false);
  sliceRelPosFilter->SetAutoCropFlag(false); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();
  StopCurrentStep<MaskImageType>(m_Working_Support);

  // ----------------------------------------------------------------
  StartNewStep("[Station7] Limits with LeftPulmonaryArtery");
  MaskImagePointer LeftPulmonaryArtery = GetAFDB()->template GetImage<MaskImageType>("LeftPulmonaryArtery");
  sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(LeftPulmonaryArtery);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("7", "LeftPulmonaryArtery"));
  sliceRelPosFilter->AddOrientationTypeString("NotAntTo");
  sliceRelPosFilter->SetIntermediateSpacingFlag(true);
  sliceRelPosFilter->SetIntermediateSpacing(3);
  sliceRelPosFilter->SetUniqueConnectedComponentBySlice(false);
  sliceRelPosFilter->SetAutoCropFlag(false); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();
  StopCurrentStep<MaskImageType>(m_Working_Support);

  StartNewStep("[Station7] Limits with SVC");
  MaskImagePointer SVC = GetAFDB()->template GetImage<MaskImageType>("SVC");
  sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(SVC);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("7", "SVC"));
  sliceRelPosFilter->AddOrientationTypeString("NotRightTo");
  sliceRelPosFilter->AddOrientationTypeString("NotAntTo");
  sliceRelPosFilter->SetIntermediateSpacingFlag(true);
  sliceRelPosFilter->SetIntermediateSpacing(3);
  sliceRelPosFilter->SetUniqueConnectedComponentBySlice(false);
  sliceRelPosFilter->SetAutoCropFlag(true); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();
  StopCurrentStep<MaskImageType>(m_Working_Support);
  
  // End
  m_ListOfStations["7"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_7_Posterior_Limits() 
{
  StartNewStep("[Station7] Posterior limits -> must be AntTo post wall of the bronchi (OLD CLASSIF)");  

  // Search for points that are the most left/post/ant and most
  // right/post/ant of the left and right bronchus

  // extract, loop slices, label/keep, find extrema x 3
  /*  FindExtremaPointsInBronchus(m_LeftBronchus, 0, 15, m_RightMostInLeftBronchus, 
			      m_AntMostInLeftBronchus, m_PostMostInLeftBronchus);
  FindExtremaPointsInBronchus(m_RightBronchus, 1, 15, m_LeftMostInRightBronchus, 
			      m_AntMostInRightBronchus, m_PostMostInRightBronchus);
  */
  
  // First cut bronchus to the correct sup/inf support 
  MaskImagePointer RightBronchus = clitk::ResizeImageLike<MaskImageType>(m_RightBronchus, m_Working_Support, GetBackgroundValue());
  MaskImagePointer LeftBronchus = clitk::ResizeImageLike<MaskImageType>(m_LeftBronchus, m_Working_Support, GetBackgroundValue());

  // Find extrema points
  FindExtremaPointsInBronchus(RightBronchus, 0, 10, m_LeftMostInRightBronchus, 
			      m_AntMostInRightBronchus, m_PostMostInRightBronchus);

  FindExtremaPointsInBronchus(LeftBronchus, 1, 10, m_RightMostInLeftBronchus, 
			      m_AntMostInLeftBronchus, m_PostMostInLeftBronchus);



  // DEBUG
  std::ofstream osrl; openFileForWriting(osrl, "osrl.txt"); osrl << "LANDMARKS1" << std::endl;
  std::ofstream osal; openFileForWriting(osal, "osal.txt"); osal << "LANDMARKS1" << std::endl;
  std::ofstream ospl; openFileForWriting(ospl, "ospl.txt"); ospl << "LANDMARKS1" << std::endl;
  std::ofstream osrr; openFileForWriting(osrr, "osrr.txt"); osrr << "LANDMARKS1" << std::endl;
  std::ofstream osar; openFileForWriting(osar, "osar.txt"); osar << "LANDMARKS1" << std::endl;
  std::ofstream ospr; openFileForWriting(ospr, "ospr.txt"); ospr << "LANDMARKS1" << std::endl;

  for(uint i=0; i<m_RightMostInLeftBronchus.size(); i++) {
    osrl << i << " " << m_RightMostInLeftBronchus[i][0] << " " << m_RightMostInLeftBronchus[i][1] 
	 << " " << m_RightMostInLeftBronchus[i][2] << " 0 0 " << std::endl;
    osal << i << " " << m_AntMostInLeftBronchus[i][0] << " " << m_AntMostInLeftBronchus[i][1] 
	 << " " << m_AntMostInLeftBronchus[i][2] << " 0 0 " << std::endl;
    ospl << i << " " << m_PostMostInLeftBronchus[i][0] << " " << m_PostMostInLeftBronchus[i][1] 
	 << " " << m_PostMostInLeftBronchus[i][2] << " 0 0 " << std::endl;
  }

  for(uint i=0; i<m_LeftMostInRightBronchus.size(); i++) {
    osrr << i << " " << m_LeftMostInRightBronchus[i][0] << " " << m_LeftMostInRightBronchus[i][1] 
	 << " " << m_LeftMostInRightBronchus[i][2] << " 0 0 " << std::endl;
    osar << i << " " << m_AntMostInRightBronchus[i][0] << " " << m_AntMostInRightBronchus[i][1] 
	 << " " << m_AntMostInRightBronchus[i][2] << " 0 0 " << std::endl;
    ospr << i << " " << m_PostMostInRightBronchus[i][0] << " " << m_PostMostInRightBronchus[i][1] 
	 << " " << m_PostMostInRightBronchus[i][2] << " 0 0 " << std::endl;
  }
  osrl.close();
  osal.close();
  ospl.close();
  osrr.close();
  osar.close();
  ospr.close();

  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    m_PostMostInRightBronchus,
                                                                    m_PostMostInLeftBronchus,
                                                                    GetBackgroundValue(), 1, -10);
  // If needed -> can do the same with AntMost.

  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["7"] = m_Working_Support;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_7_Remove_Structures()
{

  //--------------------------------------------------------------------
  StartNewStep("[Station7] remove some structures");

  Remove_Structures("7", "AzygousVein");
  Remove_Structures("7", "Aorta");
  Remove_Structures("7", "Esophagus");
  Remove_Structures("7", "RightPulmonaryArtery");
  Remove_Structures("7", "LeftPulmonaryArtery");
  Remove_Structures("7", "LeftSuperiorPulmonaryVein");
  Remove_Structures("7", "PulmonaryTrunk");
  Remove_Structures("7", "VertebralBody");

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["7"] = m_Working_Support;
  return;
}
//--------------------------------------------------------------------



