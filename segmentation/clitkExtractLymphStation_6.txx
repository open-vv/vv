

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_6_SetDefaultValues()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_6() {
  if ((!CheckForStation("6")) && (!CheckForStation("6"))) return;

  StartNewStep("Stations 6");
  StartSubStep(); 

  // Get the current support 
  StartNewStep("[Station 6] Get the current 6 suppport");
  m_ListOfStations["6"] = m_ListOfSupports["S6"];
  StopCurrentStep<MaskImageType>(m_ListOfStations["6"]);
    
  // Generic RelativePosition processes
  m_ListOfStations["6"] = this->ApplyRelativePositionList("Station_6", m_ListOfStations["6"]);

  // Remove some structures
  m_Working_Support = m_ListOfStations["6"];
  Remove_Structures(" 6", "Aorta");
  Remove_Structures(" 6", "SVC");
  Remove_Structures(" 6", "BrachioCephalicVein");
  m_ListOfStations["6"] = m_Working_Support;

  // Keep only one single CCL by slice
  StartNewStep("[Station 6] Keep only one CCL by slice");  
  m_ListOfStations["6"] = SliceBySliceKeepMainCCL<MaskImageType>(m_ListOfStations["6"], 
                                                                  GetBackgroundValue(), 
                                                                  GetForegroundValue());
  StopCurrentStep<MaskImageType>(m_ListOfStations["6"]);

  // Store image filenames into AFDB 
  writeImage<MaskImageType>(m_ListOfStations["6"], "seg/Station6.mhd");
  GetAFDB()->SetImageFilename("Station6", "seg/Station6.mhd"); 
  WriteAFDB(); 
  StopSubStep();

}
//--------------------------------------------------------------------


