
//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_7_SI_Limits() 
{
  // Get Inputs
  MaskImagePointer Trachea = GetAFDB()->template GetImage <MaskImageType>("Trachea");  
  double m_CarinaZ = GetAFDB()->GetPoint3D("Carina", 2);
  DD(m_CarinaZ);
  double m_OriginOfRightMiddleLobeBronchusZ = GetAFDB()->GetPoint3D("OriginOfRightMiddleLobeBronchus", 2);
  DD(m_OriginOfRightMiddleLobeBronchusZ);

  /* Crop support :
     Superior limit = carina
     Inferior limit = origin right middle lobe bronchus */
  StartNewStep("[Station7] Inf/Sup mediastinum limits with carina/bronchus");
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                m_OriginOfRightMiddleLobeBronchusZ, 
                                                m_CarinaZ, true,
                                                GetBackgroundValue());
  /* Crop trachea
     Superior limit = carina
     Inferior limit = origin right middle lobe bronchus*/
  m_working_trachea = 
    clitk::CropImageAlongOneAxis<MaskImageType>(Trachea, 2, 
                                                m_OriginOfRightMiddleLobeBronchusZ, 
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
  // Separate trachea in two CCL
  StartNewStep("[Station7] Separate trachea under carina");

  // Labelize and consider two main labels
  m_working_trachea = Labelize<MaskImageType>(m_working_trachea, 0, true, 1);

  // Carina position must at the first slice that separate the two
  // main bronchus (not superiorly) Check that upper slice is composed
  // of at least two labels
  typedef itk::ImageSliceIteratorWithIndex<MaskImageType> SliceIteratorType;
  SliceIteratorType iter(m_working_trachea, m_working_trachea->GetLargestPossibleRegion());
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
  clitk::ComputeCentroids<MaskImageType>(m_working_trachea, GetBackgroundValue(), c);
  ImagePointType C1 = c[1];
  ImagePointType C2 = c[2];

  ImagePixelType leftLabel;
  ImagePixelType rightLabel;  
  if (C1[0] < C2[0]) { leftLabel = 1; rightLabel = 2; }
  else { leftLabel = 2; rightLabel = 1; }

  StopCurrentStep<MaskImageType>(m_working_trachea);

  //-----------------------------------------------------
  // Select LeftLabel (set one label to Backgroundvalue)
  m_LeftBronchus = 
    SetBackground<MaskImageType, MaskImageType>(m_working_trachea, m_working_trachea, 
                                                rightLabel, GetBackgroundValue(), false);
  m_RightBronchus  = 
    SetBackground<MaskImageType, MaskImageType>(m_working_trachea, m_working_trachea, 
                                                leftLabel, GetBackgroundValue(), false);

  StartNewStep("[Station7] Limits with bronchus (slice by slice) : RightTo left bronchus");  
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, 
						       m_LeftBronchus, 2, 
                                                       GetFuzzyThreshold(), "RightTo", 
                                                       true, 4);

  StartNewStep("[Station7] Limits with bronchus (slice by slice) : LeftTo right bronchus");  
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, 
						       m_RightBronchus, 
						       2, GetFuzzyThreshold(), "LeftTo", 
                                                       true, 4);

  StartNewStep("[Station7] Limits with bronchus (slice by slice) : not AntTo left bronchus");  
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, 
						       m_LeftBronchus, 
						       2, GetFuzzyThreshold(), "AntTo", 
                                                       true, 4, true); // NOT

  StartNewStep("[Station7] Limits with bronchus (slice by slice) : not AntTo right bronchus");  
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, 
						       m_RightBronchus, 
						       2, GetFuzzyThreshold(), "AntTo", 
                                                       true, 4, true);

  StartNewStep("[Station7] Limits with bronchus (slice by slice) : not PostTo left bronchus");  
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, 
						       m_LeftBronchus, 
						       2, GetFuzzyThreshold(), "PostTo", 
                                                       true, 4, true);

  StartNewStep("[Station7] Limits with bronchus (slice by slice) : not PostTo right bronchus");  
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, 
						       m_RightBronchus, 
						       2, GetFuzzyThreshold(), "PostTo", 
                                                       true, 4, true);
  m_Station7 = m_Working_Support;
  StopCurrentStep<MaskImageType>(m_Station7);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_7_Posterior_Limits() 
{
  StartNewStep("[Station7] Posterior limits -> must be AntTo post wall of the bronchi");  

  // Search for points that are the most left/post/ant and most
  // right/post/ant of the left and right bronchus

  // extract, loop slices, label/keep, find extrema x 3
  FindExtremaPointsInBronchus(m_LeftBronchus, 0, 15,
			      m_RightMostInLeftBronchus, 
			      m_AntMostInLeftBronchus, 
			      m_PostMostInLeftBronchus);
  FindExtremaPointsInBronchus(m_RightBronchus, 1, 15,
			      m_LeftMostInRightBronchus, 
			      m_AntMostInRightBronchus, 
			      m_PostMostInRightBronchus);
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
  SliceIteratorType iter = SliceIteratorType(m_Working_Support, 
                                             m_Working_Support->GetLargestPossibleRegion());
  iter.SetFirstDirection(0);
  iter.SetSecondDirection(1);
  iter.GoToBegin();
  int i=0;
  MaskImageType::PointType A;
  MaskImageType::PointType B;
  MaskImageType::PointType C;
  while (!iter.IsAtEnd()) {
    A = m_PostMostInLeftBronchus[i];
    B = m_PostMostInRightBronchus[i];
    C = A;
    C[1] -= 10; // I know I must keep this point
    double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
    bool isPositive = s<0;
    while (!iter.IsAtEndOfSlice()) {
      while (!iter.IsAtEndOfLine()) {
        // Very slow, I know ... but image should be very small
        m_Working_Support->TransformIndexToPhysicalPoint(iter.GetIndex(), C);
        double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
        if (s == 0) iter.Set(2);
        if (isPositive) {
          if (s > 0) iter.Set(GetBackgroundValue());
        }
        else {
          if (s < 0) iter.Set(GetBackgroundValue());
        }
        ++iter;
      }
      iter.NextLine();
    }
    iter.NextSlice();
    ++i;
  }

  //-----------------------------------------------------
  // StartNewStep("[Station7] Anterior limits");  
 

  // MISSING FROM NOW 
  
  // Station 4R, Station 4L, the right pulmonary artery, and/or the
  // left superior pulmonary vein


  //-----------------------------------------------------
  //-----------------------------------------------------
  // ALSO SUBSTRACT ARTERY/VEIN (initially in the support)


  // Set output
  m_Station7 = m_Working_Support;
}
//--------------------------------------------------------------------


