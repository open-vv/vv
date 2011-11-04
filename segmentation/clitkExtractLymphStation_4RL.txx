

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_4RL_SetDefaultValues()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_4R() {
  if (!CheckForStation("4R")) return;
  StartNewStep("Stations 4R");
  StartSubStep(); 

  // Get the current support 
  StartNewStep("[Station 4R] Get the current 4RL suppport");
  m_ListOfStations["4R"] = m_ListOfSupports["S4R"];
  StopCurrentStep<MaskImageType>(m_ListOfStations["4R"]);
    
  // Generic RelativePosition processes
  m_ListOfStations["4R"] = this->ApplyRelativePositionList("Station_4R", m_ListOfStations["4R"]);

  // Store image filenames into AFDB 
  WriteImageStation("4R");
  StopSubStep();
  ComputeOverlapWithRef("4R");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_4L() {
  if (!CheckForStation("4L")) return;
  StartNewStep("Stations 4L");
  StartSubStep(); 

  // Get the current support 
  StartNewStep("[Station 4L] Get the current 4RL suppport");
  m_ListOfStations["4L"] = m_ListOfSupports["S4L"];
  StopCurrentStep<MaskImageType>(m_ListOfStations["4L"]);
    
  // Generic RelativePosition processes
  m_ListOfStations["4L"] = this->ApplyRelativePositionList("Station_4L", m_ListOfStations["4L"]);

  // Separation Ant/Post
  m_Working_Support = m_ListOfStations["4L"];
  ExtractStation_S4L_S5_Limits_Aorta_LeftPulmonaryArtery(10);
  m_ListOfStations["4L"] = m_Working_Support;
  
  // Keep only one single CCL by slice
  StartNewStep("[Station 4L] Keep only one CCL by slice");  
  m_ListOfStations["4L"] = SliceBySliceKeepMainCCL<MaskImageType>(m_ListOfStations["4L"], 
                                                                  GetBackgroundValue(), 
                                                                  GetForegroundValue());
  StopCurrentStep<MaskImageType>(m_ListOfStations["4L"]);

  // Store image filenames into AFDB 
  WriteImageStation("4L");
  StopSubStep();
  ComputeOverlapWithRef("4L");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_S4L_S5_Limits_Aorta_LeftPulmonaryArtery(int KeepPoint) 
{
  // ----------------------------------------------------------
  StartNewStep("[Station 4L] Limits between Asc and Desc Aorta, and with LeftPulmonaryArtery ");

  // Separate Aorta into DescendingAorta AscendingAorta
  MaskImagePointer AscendingAorta = GetAFDB()->template GetImage<MaskImageType>("AscendingAorta");
  MaskImagePointer DescendingAorta = GetAFDB()->template GetImage<MaskImageType>("DescendingAorta");  

  // Crop like support (inferiorly)
  m_Working_Support = clitk::AutoCrop<MaskImageType>(m_Working_Support, GetBackgroundValue());
  AscendingAorta = 
    clitk::ResizeImageLike<MaskImageType>(AscendingAorta, m_Working_Support, GetBackgroundValue());
  DescendingAorta = 
    clitk::ResizeImageLike<MaskImageType>(DescendingAorta, m_Working_Support, GetBackgroundValue());

  // Crop superior part (when AscendingAorta and DescendingAorta join)
  MaskImagePointType p;
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(AscendingAorta, 
                                                          GetBackgroundValue(), 2, false, p);
  double max = p[2]+AscendingAorta->GetSpacing()[2];
  AscendingAorta = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(AscendingAorta, 2, max, false, GetBackgroundValue());
  DescendingAorta = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(DescendingAorta, 2, max, false, GetBackgroundValue());

  // Crop inferior part, where LeftPulmonaryArtery start
  MaskImagePointer AscendingAortaCopy = clitk::Clone<MaskImageType>(AscendingAorta);
  MaskImagePointer LeftPulmonaryArtery = GetAFDB()->template GetImage<MaskImageType>("LeftPulmonaryArtery");
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(LeftPulmonaryArtery, 
                                                          GetBackgroundValue(), 2, false, p);
  max = p[2]+LeftPulmonaryArtery->GetSpacing()[2];
  AscendingAorta = 
    clitk::CropImageRemoveLowerThan<MaskImageType>(AscendingAorta, 2, max, false, GetBackgroundValue());
  DescendingAorta = 
    clitk::CropImageRemoveLowerThan<MaskImageType>(DescendingAorta, 2, max, false, GetBackgroundValue());

  // Find closest points
  std::vector<MaskImagePointType> A;
  std::vector<MaskImagePointType> B;
  clitk::SliceBySliceBuildLineSegmentAccordingToMinimalDistanceBetweenStructures<MaskImageType>(AscendingAorta, 
                                                                                                DescendingAorta, 
                                                                                                GetBackgroundValue(), 
                                                                                                2, A, B);
  // Separate according to AB lines
  // clitk::WriteListOfLandmarks<MaskImageType>(A, "A.txt");
  // clitk::WriteListOfLandmarks<MaskImageType>(B, "B.txt");
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, A, B,
                                                                    GetBackgroundValue(), 
                                                                    1, KeepPoint); // Keep point along axis 1

  // Now, same principle between AscendingAorta and LeftPulmonaryArtery
  AscendingAorta = 
    clitk::ResizeImageLike<MaskImageType>(AscendingAortaCopy, m_Working_Support, GetBackgroundValue());
  LeftPulmonaryArtery = 
    clitk::ResizeImageLike<MaskImageType>(LeftPulmonaryArtery, m_Working_Support, GetBackgroundValue());
  AscendingAorta = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(AscendingAorta, 2, max, false, GetBackgroundValue());
  LeftPulmonaryArtery = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(LeftPulmonaryArtery, 2, max, false, GetBackgroundValue());
  A.clear();
  B.clear();
  clitk::SliceBySliceBuildLineSegmentAccordingToMinimalDistanceBetweenStructures<MaskImageType>(AscendingAorta, 
                                                                                                LeftPulmonaryArtery, 
                                                                                                GetBackgroundValue(), 
                                                                                                2, A, B);
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, A, B,
                                                                    GetBackgroundValue(), 
                                                                    1, KeepPoint); // Keep point along axis 1

  // AutoCrop
  m_Working_Support = clitk::AutoCrop<MaskImageType>(m_Working_Support, GetBackgroundValue());

  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
}
//--------------------------------------------------------------------


