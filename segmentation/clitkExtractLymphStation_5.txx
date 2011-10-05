

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_5_SetDefaultValues()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_5() {
  if ((!CheckForStation("5")) && (!CheckForStation("5"))) return;

  StartNewStep("Stations 5");
  StartSubStep(); 

  // Get the current support 
  StartNewStep("[Station 5] Get the current 5 suppport");
  m_ListOfStations["5"] = m_ListOfSupports["S5"];
  StopCurrentStep<MaskImageType>(m_ListOfStations["5"]);
    
  // Generic RelativePosition processes
  m_ListOfStations["5"] = this->ApplyRelativePositionList("Station_5", m_ListOfStations["5"]);

  // Separation Ant/Post -> Like 4SL !
  m_Working_Support = m_ListOfStations["5"];
  ExtractStation_S4L_S5_Limits_Aorta_LeftPulmonaryArtery(-10);
  m_ListOfStations["5"] = m_Working_Support;

  // Ant limit
  m_Working_Support = m_ListOfStations["5"];
  ExtractStation_5_Limits_AscendingAorta_Ant();
  m_ListOfStations["5"] = m_Working_Support;

  // Keep only one single CCL by slice
  StartNewStep("[Station 5] Keep only one CCL by slice");  
  m_ListOfStations["5"] = SliceBySliceKeepMainCCL<MaskImageType>(m_ListOfStations["5"], 
                                                                  GetBackgroundValue(), 
                                                                  GetForegroundValue());

  // AutoCrop
  m_ListOfStations["5"] = clitk::AutoCrop<MaskImageType>(m_ListOfStations["5"], GetBackgroundValue());
  StopCurrentStep<MaskImageType>(m_ListOfStations["5"]);

  // Store image filenames into AFDB 
  writeImage<MaskImageType>(m_ListOfStations["5"], "seg/Station5.mhd");
  GetAFDB()->SetImageFilename("Station5", "seg/Station5.mhd"); 
  WriteAFDB(); 
  StopSubStep();

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_5_Limits_AscendingAorta_Ant() 
{
  // ----------------------------------------------------------
  StartNewStep("[Station 5] Limits with Ant part of AscendingAorta");

  // Get AscendingAorta
  MaskImagePointer AscendingAorta = GetAFDB()->template GetImage<MaskImageType>("AscendingAorta");

  // Crop and select most Ant points
  AscendingAorta = 
    clitk::ResizeImageLike<MaskImageType>(AscendingAorta, m_Working_Support, GetBackgroundValue());
  std::vector<MaskImagePointType> A;
  std::vector<MaskImagePointType> B;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(AscendingAorta, 
                                                                               GetBackgroundValue(), 
                                                                               2, 1, true, 0, -1, A, B);
  //1 mm margin

  // Separate according to AB lines
  // clitk::WriteListOfLandmarks<MaskImageType>(A, "A.txt");
  // clitk::WriteListOfLandmarks<MaskImageType>(B, "B.txt");
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, A, B,
                                                                    GetBackgroundValue(), 
                                                                    1, 10); // Keep point along axis 1
  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
}
//--------------------------------------------------------------------


