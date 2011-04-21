//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_4RL_SI_Limits() 
{

  /*
    Station 4R: right lower paratracheal nodes From superior to
    inferior, the delineation of Station 4R starts at the top of the
    aortic arch (Fig. 2D) and ends at the upper lobe bronchus or where the
    right pulmonary artery crosses the midline of the mediastinum
    (Fig. 3E,F). On the left side, Station 4R is deﬁned by the midline of
    the trachea (Fig. 2D). On the right side, it is contained within the
    pleural envelope in the upper part, medial to the superior vena cava
    and the arch of the azygos vein in the intermediate section (Fig. 2I
    and 3A,B) and the right upper lobe pulmonary vein in its very caudal
    part. Anteriorly, it is limited most supe- riorly by the right
    brachiocephalic vein (Fig. 2D–H), fol- lowed by the superior vena cava
    and the arch or ascending section of the aorta (Figs. 2I and 3A–E). In
    between the superior vena cava and the aorta, we recommend delineating
    Station 4R so that it extends halfway between the two vessels where it
    will contact Station 3A or 6 (Figs. 2H,I and 3A–D). Posteriorly,
    Station 4R is deﬁned at its superior extent by an imaginary horizontal
    line running along the posterior wall of the trachea
    (Fig. 2E). Inferiorly, it remains anterior to the right main stem
    bronchus, ﬁlling the soft- tissue space between the vessels.
  */

  StartNewStep("[Station 4R]Inf/Sup mediastinum limits with aortic arch/upperLBronchus");
  /* SupInf limits : 
     - top of aortic arch
     - ends at the upper lobe bronchus or where the right pulmonary artery crosses the midline of the mediastinum
  */

  // Local variables
  double m_TopOfAorticArchInMM;
  double m_UpperLobeBronchusZPositionInMM;
  double m_RightPulmoArteyrCrossesMidMediastinumZPositionInMM;

  // Get Inputs
  m_TopOfAorticArchInMM = GetAFDB()->GetPoint3D("TopOfAorticArch", 2);
  DD(m_TopOfAorticArchInMM);
  m_UpperLobeBronchusZPositionInMM = GetAFDB()->GetPoint3D("RightUpperLobeBronchus", 2);
  DD(m_UpperLobeBronchusZPositionInMM);
  m_RightPulmoArteyrCrossesMidMediastinumZPositionInMM = GetAFDB()->GetPoint3D("RightPulmoArteryCrossesMidMediastinum", 2);
  DD(m_RightPulmoArteyrCrossesMidMediastinumZPositionInMM);

  /* Crop support */
  double inf = std::max(m_UpperLobeBronchusZPositionInMM, m_RightPulmoArteyrCrossesMidMediastinumZPositionInMM);
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Mediastinum, 2, 
                                                inf,
                                                m_TopOfAorticArchInMM, true,
                                                GetBackgroundValue());
  StopCurrentStep<MaskImageType>(m_Working_Support);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_4RL_LR_Limits() 
{
  // 4R first 

  // Left : midline of the trachea
  // Right : "- upper part : contained within the pleural envelope
  //- intermediate section : medial to the superior vena cava and the arch of the azygos vein
  // - very caudal part : right upper lobe pulmonary vein"
  // AAV ??
  
  // Left -> midline of the trachea
  // slice by slice, find X coord of 2D centroid (?)
  // check with previous line in order to not move too fast


  // ----------------------------------------------------------
  StartNewStep("[Station 4R] Left limits with midline of trachea ");
  
  /*
    Two possible approaches
    1) use trachea skeleton (clitkExtractAirwaysTreeInfo)
    -> but need to analyse the tree to remove "false" bifurcation
    -> need to track from top to bottom, with each bronchus
    -> how to stay on the main path ?
       
    2) analyse slice by slice the trachea, labelize and take the first
    or two first connected components -> find centroids. 
    -> not really "smooth" when bronchus slicing lead to "splated" region
       
    ==> we choose 2
  */

  // Crop the trachea like the current support
  MaskImagePointer Trachea = GetAFDB()->template GetImage <MaskImageType>("Trachea");    
  MaskImagePointer crop_trachea = 
    clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());
  writeImage<MaskImageType>(crop_trachea, "croptrachea.mhd");

  // Extract all the slices
  std::vector<typename MaskSliceType::Pointer> bronchi_slices;
  clitk::ExtractSlices<MaskImageType>(crop_trachea, 2, bronchi_slices);

  // List of midpoints
  std::vector<MaskImagePointType> midpoints;

  // Add mid points below carina, from foot to head
  for(uint i=0; i<m_LeftMostInRightBronchus.size(); i++) {
    MaskImagePointType p;
    p[0] = (m_LeftMostInRightBronchus[i][0]+m_RightMostInLeftBronchus[i][0])/2.0;
    p[1] = (m_LeftMostInRightBronchus[i][1]+m_RightMostInLeftBronchus[i][1])/2.0;
    p[2] = m_LeftMostInRightBronchus[i][2];
    midpoints.push_back(p);
  }

  // Crop the trachea above the carina. Find largest Z position
  MaskImageIndexType p = Trachea->GetLargestPossibleRegion().GetIndex();
  for(uint i=0; i<3; i++) {
    p[i] += Trachea->GetLargestPossibleRegion().GetSize()[i];
  }
  MaskImagePointType q;
  Trachea->TransformIndexToPhysicalPoint(p, q);
  double maxZ = q[2];
  double m_CarinaZ = GetAFDB()->GetPoint3D("Carina", 2);
  MaskImagePointer m_above_carina = 
    clitk::CropImageAlongOneAxis<MaskImageType>(Trachea, 2, 
                                                m_CarinaZ,
                                                maxZ, true,
                                                GetBackgroundValue());
  writeImage<MaskImageType>(m_above_carina, "above.mhd");

  // Extract all the slices
  std::vector<typename MaskSliceType::Pointer> trachea_slices;
  clitk::ExtractSlices<MaskImageType>(m_above_carina, 2, trachea_slices);

  // Find centroid of the trachea in each slice
  std::vector<MaskImagePointType> points;
  typedef typename MaskSliceType::PointType SlicePointType;
  SlicePointType previous;
  // Start from patient top (head)
  for(uint i=0; i<trachea_slices.size(); i++) {
    // Labelize 
    trachea_slices[i] = Labelize<MaskSliceType>(trachea_slices[i], GetBackgroundValue(), true, 10);
    // Get centroid
    std::vector<SlicePointType> c;
    clitk::ComputeCentroids<MaskSliceType>(trachea_slices[i], GetBackgroundValue(), c);
    // Keep first one (first connected component label) and convert to 3D
    MaskImagePointType p;
    p[2] = m_above_carina->GetOrigin()[2] + i*m_above_carina->GetSpacing()[2];
    p[0] = c[1][0];
    p[1] = c[1][1];
    midpoints.push_back(p);
  }

  // DEBUG POINTS
  std::ofstream osp;
  openFileForWriting(osp, "mp.txt");
  osp << "LANDMARKS1" << std::endl;
  for(uint i=0; i<midpoints.size(); i++) {
    osp << i << " " << midpoints[i][0] << " " 
	<< midpoints[i][1] << " " << midpoints[i][2] << " 0 0 " << std::endl;
  }
  osp.close();

  // Create and allocate left and right part of the support (we create
  // images because we will then crop them)
  m_LeftSupport = clitk::NewImageLike<MaskImageType>(m_Working_Support);
  m_RightSupport = clitk::NewImageLike<MaskImageType>(m_Working_Support);

  // Loop on current support, slice by slice
  typedef itk::ImageSliceIteratorWithIndex<MaskImageType> SliceIteratorType;
  SliceIteratorType iter = SliceIteratorType(m_Working_Support, 
                                             m_Working_Support->GetLargestPossibleRegion());
  SliceIteratorType iterL = SliceIteratorType(m_LeftSupport, 
					      m_LeftSupport->GetLargestPossibleRegion());
  SliceIteratorType iterR = SliceIteratorType(m_RightSupport, 
					      m_RightSupport->GetLargestPossibleRegion());
  iter.SetFirstDirection(0);
  iter.SetSecondDirection(1);
  iter.GoToBegin();
  iterL.SetFirstDirection(0);
  iterL.SetSecondDirection(1);
  iterL.GoToBegin();
  iterR.SetFirstDirection(0);
  iterR.SetSecondDirection(1);
  iterR.GoToBegin();

  int slice=0;
  // Assert starting of image has the same Z than first midpoints
  while (midpoints[slice][2] != m_Working_Support->GetOrigin()[2]) {
    slice++;
    if ((uint)slice >= midpoints.size()) {
      clitkExceptionMacro("Bug while searching for first midpoint to use");
    }
  }

  // Start loop
  while (!iter.IsAtEnd()) {
    ImageIndexType index;
    m_Working_Support->TransformPhysicalPointToIndex(midpoints[slice], index);
    while (!iter.IsAtEndOfSlice()) {
      // convert into index
      while (!iter.IsAtEndOfLine()) {
	// if indexcourant <index this is Right part. Left otherwise
	if (iter.GetIndex()[0]<index[0]) {
	  iterR.Set(iter.Get());
	  iterL.Set(GetBackgroundValue());
	}
	else {
	  iterL.Set(iter.Get());
	  iterR.Set(GetBackgroundValue());
	}
        ++iter;
	++iterL;
	++iterR;
      }
      iter.NextLine();
      iterL.NextLine();
      iterR.NextLine();

    }
    iter.NextSlice();
    iterL.NextSlice();
    iterR.NextSlice();
    ++slice;
  }

  // Auto crop !
  m_LeftSupport = clitk::AutoCrop<MaskImageType>(m_LeftSupport, GetBackgroundValue());               
  m_RightSupport = clitk::AutoCrop<MaskImageType>(m_RightSupport, GetBackgroundValue());               
  writeImage<MaskImageType>(m_LeftSupport, "lsac.mhd");
  writeImage<MaskImageType>(m_RightSupport, "rsac.mhd");

  StopCurrentStep<MaskImageType>(m_RightSupport);

  // -------------------------------------------------------------------------
  // Constraint at right from the SVC. Must be after MidTrachea limits
  // (if not, bronchus can be split, midposition is false)
  StartNewStep("[Station 4R] R limits with SVC ");
  MaskImagePointer svc = GetAFDB()->template GetImage<MaskImageType>("SVC");
  typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  /*  relPosFilter->SetVerboseStep(false);
  relPosFilter->SetInput(m_RightSupport);  // only right here ...
  relPosFilter->SetInputObject(svc); 
  relPosFilter->SetOrientationType(RelPosFilterType::RightTo);
  relPosFilter->SetIntermediateSpacing(2);
  relPosFilter->SetFuzzyThreshold(0.3);
  relPosFilter->Update();
  m_RightSupport = relPosFilter->GetOutput();
  */

  /*
    ==> TODO RIGHT TO MEDIAL TO SVC : 
    get centroid, cut in X direction to get medial

    ==> REDO OPERATOR to find extrma points ?
    centroid or skeleton ? 

  */

  relPosFilter = RelPosFilterType::New();
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->SetInput(m_RightSupport);  // only right here ...
  relPosFilter->SetInputObject(svc); 
  relPosFilter->AddOrientationType(RelPosFilterType::AntTo);
  relPosFilter->InverseOrientationFlagOn();
  relPosFilter->SetIntermediateSpacing(2); // this is important to put it low
  relPosFilter->SetFuzzyThreshold(0.6);
  relPosFilter->Update();
  m_RightSupport = relPosFilter->GetOutput();

  // AutoCrop
  m_RightSupport = clitk::AutoCrop<MaskImageType>(m_RightSupport, GetBackgroundValue());               

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_4RL_AP_Limits() 
{

  /*
    post of S4R
    - sup part -> cut line post wall trachea
    - inf part (where ???) -> remains anterior to the right main stem bronchus
    ==> bo
  */

  // Post (not Ant) to Aorta
  MaskImagePointer aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  // Crop according to current support
  aorta = clitk::ResizeImageLike<MaskImageType>(aorta, m_RightSupport, GetBackgroundValue());
  typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter = RelPosFilterType::New();
  relPosFilter->VerboseStepFlagOff();
  relPosFilter->SetInput(m_RightSupport);  // only right here ...
  relPosFilter->SetInputObject(aorta); 
  relPosFilter->AddOrientationType(RelPosFilterType::PostTo);
  //  relPosFilter->NotFlagOn();
  relPosFilter->SetIntermediateSpacing(2); // this is important to put it low
  relPosFilter->SetFuzzyThreshold(0.6);
  relPosFilter->Update();
  m_RightSupport = relPosFilter->GetOutput();

  m_ListOfStations["4R"] = m_RightSupport;
  StopCurrentStep<MaskImageType>(m_RightSupport);

  // POST -> horizontal lines bronchus --> points dectection in S7 to store.
}
//--------------------------------------------------------------------

