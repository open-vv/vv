//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_4RL_SI_Limits() 
{
  /* SupInf limits : 
  - top of aortic arch
  - ends at the upper lobe bronchus or where the right pulmonary artery crosses the midline of the mediastinum
  */

  // Local variables
  double m_TopOfAorticArchInMM;
  double m_UpperLobeBronchusZPositionInMM;
  double m_RightPulmoArteyrCrossesMidMediastinumZPositionInMM;

  // Get Inputs
  m_TopOfAorticArchInMM = GetAFDB()->GetPoint3D("topOfAorticArch", 2);
  DD(m_TopOfAorticArchInMM);
  m_UpperLobeBronchusZPositionInMM = GetAFDB()->GetPoint3D("rightUpperLobeBronchus", 2);
  DD(m_UpperLobeBronchusZPositionInMM);
  m_RightPulmoArteyrCrossesMidMediastinumZPositionInMM = GetAFDB()->GetPoint3D("rightPulmoArteryCrossesMidMediastinum", 2);
  DD(m_RightPulmoArteyrCrossesMidMediastinumZPositionInMM);

  /* Crop support */
  StartNewStep("Inf/Sup mediastinum limits with aortic arch/upperLBronchus");
  double inf = std::max(m_UpperLobeBronchusZPositionInMM, m_RightPulmoArteyrCrossesMidMediastinumZPositionInMM);
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Support, 2, 
                                                inf,
                                                m_TopOfAorticArchInMM, true,
                                                GetBackgroundValue());
  StopCurrentStep<MaskImageType>(m_Working_Support);

  m_Station4RL = m_Working_Support;
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
  
  // Constraint at right from the SVC
  MaskImagePointer svc = GetAFDB()->template GetImage<MaskImageType>("SVC");
  typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskImageType> RelPosFilterType;
  typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
  relPosFilter->SetCurrentStepBaseId(this->GetCurrentStepId());
  relPosFilter->VerboseStepOff();
  relPosFilter->WriteStepOff();
  relPosFilter->SetInput(m_Working_Support); 
  relPosFilter->SetInputObject(svc); 
  relPosFilter->SetOrientationType(RelPosFilterType::RightTo);
  relPosFilter->SetIntermediateSpacing(2);
  relPosFilter->SetFuzzyThreshold(0.3);
  relPosFilter->Update();
  m_Working_Support = relPosFilter->GetOutput();
  m_Station4RL = m_Working_Support;
  
  // Left -> midline of the trachea
  // slice by slice, find X coord of 2D centroid (?)
  // check with previous line in order to not move too fast
  
  // skeleton ? -> need path description ? follow from slice to slice
  // OR CENTROID at each slice ?
  
  // Crop trachea
  // Extract list of slice from trachea
  // Loop slice -> Get centroid crop along line (BB limit) -> two supports

  // Crop the trachea like the current support
  MaskImagePointer crop_trachea = 
    clitk::ResizeImageLike<MaskImageType>(m_Trachea, m_Working_Support, GetBackgroundValue());
  writeImage<MaskImageType>(crop_trachea, "croptrachea.mhd");

  // Extract all the slices
  typedef clitk::ExtractSliceFilter<MaskImageType> ExtractSliceFilterType;
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  typename ExtractSliceFilterType::Pointer 
    extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(crop_trachea);
  extractSliceFilter->SetDirection(2);
  extractSliceFilter->Update();
  std::vector<typename SliceType::Pointer> trachea_slices;
  extractSliceFilter->GetOutputSlices(trachea_slices);



}
//--------------------------------------------------------------------
