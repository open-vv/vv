

// vtk
#include <vtkAppendPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>

// clitk
#include "clitkMeshToBinaryImageFilter.h"

// itk
#include <itkImageDuplicator.h>

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_SetDefaultValues()
{
  SetFuzzyThreshold("2RL", "CommonCarotidArtery", 0.7);
  SetFuzzyThreshold("2RL", "BrachioCephalicArtery", 0.7);
  SetFuzzyThreshold("2RL", "BrachioCephalicVein", 0.3);
  SetFuzzyThreshold("2RL", "Aorta", 0.7);
  SetFuzzyThreshold("2RL", "SubclavianArteryRight", 0.5);
  SetFuzzyThreshold("2RL", "SubclavianArteryLeft", 0.8);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_2RL()
{
  if (CheckForStation("2RL")) {
    ExtractStation_2RL_SI_Limits();
    ExtractStation_2RL_Post_Limits();

    ExtractStation_2RL_Ant_Limits2();
    //ExtractStation_2RL_Ant_Limits(); 

    ExtractStation_2RL_LR_Limits(); 
    ExtractStation_2RL_Remove_Structures(); 
    ExtractStation_2RL_SeparateRL(); 
    
    // Store image filenames into AFDB 
    writeImage<MaskImageType>(m_ListOfStations["2R"], "seg/Station2R.mhd");
    writeImage<MaskImageType>(m_ListOfStations["2L"], "seg/Station2L.mhd");
    GetAFDB()->SetImageFilename("Station2R", "seg/Station2R.mhd"); 
    GetAFDB()->SetImageFilename("Station2L", "seg/Station2L.mhd"); 
    WriteAFDB(); 
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_SI_Limits() 
{
  // Apex of the chest or Sternum & Carina.
  StartNewStep("[Station 2RL] Inf/Sup limits with Sternum and TopOfAorticArch/CaudalMarginOfLeftBrachiocephalicVein");

  /* Rod says: "For the inferior border, unlike in Atlas – UM, there
   is now a difference between 2R and 2L.  2R stops at the
   intersection of the caudal margin of the innominate vein with the
   trachea.  2L extends less inferiorly to the superior border of the
   aortic arch." */

  /* Get TopOfAorticArch and CaudalMarginOfLeftBrachiocephalicVein 
     - TopOfAorticArch -> can be obtain from Aorta -> most sup part.  
     - CaudalMarginOfLeftBrachiocephalicVein -> must inf part of BrachicephalicVein
   */
  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  MaskImagePointType p = Aorta->GetOrigin(); // initialise to avoid warning 
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Aorta, GetBackgroundValue(), 2, false, p);
  double TopOfAorticArchZ=p[2];
  GetAFDB()->SetDouble("TopOfAorticArchZ", TopOfAorticArchZ);

  MaskImagePointer BrachioCephalicVein = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(BrachioCephalicVein, GetBackgroundValue(), 2, true, p);
  double CaudalMarginOfLeftBrachiocephalicVeinZ=p[2];
  GetAFDB()->SetDouble("CaudalMarginOfLeftBrachiocephalicVeinZ", CaudalMarginOfLeftBrachiocephalicVeinZ);
  
  // First, cut on the most inferior part. Add one slice because this
  // inf slice should not be included.
  double inf = std::min(CaudalMarginOfLeftBrachiocephalicVeinZ, TopOfAorticArchZ) + m_Working_Support->GetSpacing()[2];

  // Get Sternum and search for the upper position
  MaskImagePointer Sternum = GetAFDB()->template GetImage<MaskImageType>("Sternum");

  // Search most sup point
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Sternum, GetBackgroundValue(), 2, false, p);
  double m_SternumZ = p[2];
  // +Sternum->GetSpacing()[2]; // One more slice, because it is below this point // NOT HERE ?? WHY DIFFERENT FROM 3A ??

  //* Crop support :
  m_Working_Support = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_Working_Support, 2, 
                                                inf, m_SternumZ, true,
                                                GetBackgroundValue());

  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Ant_Limits() 
{
  // -----------------------------------------------------
  /* Rod says: "The anterior border, as with the Atlas – UM, is
    posterior to the vessels (right subclavian vein, left
    brachiocephalic vein, right brachiocephalic vein, left subclavian
    artery, left common carotid artery and brachiocephalic trunk).
    These vessels are not included in the nodal station.  The anterior
    border is drawn to the midpoint of the vessel and an imaginary
    line joins the middle of these vessels.  Between the vessels,
    station 2 is in contact with station 3a." */

  // -----------------------------------------------------
  StartNewStep("[Station 2RL] Ant limits with CommonCarotidArtery");

  // Get CommonCarotidArtery
  MaskImagePointer CommonCarotidArtery = GetAFDB()->template GetImage<MaskImageType>("CommonCarotidArtery");
  
  // Remove Ant to CommonCarotidArtery
  typedef SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(CommonCarotidArtery);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("2RL", "CommonCarotidArtery"));
  sliceRelPosFilter->AddOrientationTypeString("NotAntTo");
  sliceRelPosFilter->IntermediateSpacingFlagOn();
  sliceRelPosFilter->SetIntermediateSpacing(2);
  sliceRelPosFilter->UniqueConnectedComponentBySliceOff();
  sliceRelPosFilter->UseASingleObjectConnectedComponentBySliceFlagOff();
  sliceRelPosFilter->AutoCropFlagOn(); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->RemoveObjectFlagOff();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();

  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // -----------------------------------------------------
  // Remove Ant to H line from the Ant most part of the
  // CommonCarotidArtery until we reach the first slice of
  // BrachioCephalicArtery
  StartNewStep("[Station 2RL] Ant limits with CommonCarotidArtery, H line");

  // First, find the first slice of BrachioCephalicArtery
  MaskImagePointer BrachioCephalicArtery = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicArtery");
  MaskImagePointType p = BrachioCephalicArtery->GetOrigin(); // initialise to avoid warning 
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(BrachioCephalicArtery, GetBackgroundValue(), 2, false, p);
  double TopOfBrachioCephalicArteryZ=p[2] + BrachioCephalicArtery->GetSpacing()[2]; // Add one slice

  // Remove CommonCarotidArtery below this point
  CommonCarotidArtery = clitk::CropImageRemoveLowerThan<MaskImageType>(CommonCarotidArtery, 2, TopOfBrachioCephalicArteryZ, true, GetBackgroundValue());  

  // Find most Ant points
  std::vector<MaskImagePointType> ccaAntPositionA;
  std::vector<MaskImagePointType> ccaAntPositionB;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(CommonCarotidArtery, 
                                                                               GetBackgroundValue(), 2,
                                                                               1, true, // Ant
                                                                               0, // Horizontal line
                                                                               -3, // margin
                                                                               ccaAntPositionA, 
                                                                               ccaAntPositionB);
  // Cut ant to this line
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    ccaAntPositionA,
                                                                    ccaAntPositionB,
                                                                    GetBackgroundValue(), 1, 10); 

  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // -----------------------------------------------------
  // Ant limit with the BrachioCephalicArtery
  StartNewStep("[Station 2RL] Ant limits with BrachioCephalicArtery line");

  // Remove Ant to BrachioCephalicArtery
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, BrachioCephalicArtery, 2, 
                                                       GetFuzzyThreshold("2RL", "BrachioCephalicArtery"), "NotAntTo", false, 2, true, false);
  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // -----------------------------------------------------
  // Ant limit with the BrachioCephalicArtery H line
  StartNewStep("[Station 2RL] Ant limits with BrachioCephalicArtery, Horizontal line");
  
  // Find most Ant points
  std::vector<MaskImagePointType> bctAntPositionA;
  std::vector<MaskImagePointType> bctAntPositionB;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(BrachioCephalicArtery, 
                                                                               GetBackgroundValue(), 2,
                                                                               1, true, // Ant
                                                                               0, // Horizontal line
                                                                               -1, // margin
                                                                               bctAntPositionA, 
                                                                               bctAntPositionB);
  // Cut ant to this line
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    bctAntPositionA,
                                                                    bctAntPositionB,
                                                                    GetBackgroundValue(), 1, 10); 
 // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // -----------------------------------------------------
  // Ant limit with the BrachioCephalicVein
  StartNewStep("[Station 2RL] Ant limits with BrachioCephalicVein");

  // Remove Ant to BrachioCephalicVein
  MaskImagePointer BrachioCephalicVein = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, BrachioCephalicVein, 2, 
                                                       GetFuzzyThreshold("2RL", "BrachioCephalicVein"), "NotAntTo", false, 2, true, false);
  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Ant_Limits2()
{
  // -----------------------------------------------------
  StartNewStep("[Station 2RL] Ant limits with vessels centroids");
  
  // WARNING, as I used "And" after, empty slice in binarizedContour
  // lead to remove part of the support, although we want to keep
  // unchanged. So we decide to ResizeImageLike but pad with
  // ForegroundValue instead of BG

  // Get or compute the binary mask that separate Ant/Post part
  // according to vessels
  MaskImagePointer binarizedContour = FindAntPostVessels2();
  binarizedContour = clitk::ResizeImageLike<MaskImageType>(binarizedContour, 
                                                           m_Working_Support, 
                                                           GetForegroundValue());
  
  // remove from support
  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BoolFilterType;
  typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
  boolFilter->InPlaceOn();
  boolFilter->SetInput1(m_Working_Support);
  boolFilter->SetInput2(binarizedContour);
  boolFilter->SetBackgroundValue1(GetBackgroundValue());
  boolFilter->SetBackgroundValue2(GetBackgroundValue());
  boolFilter->SetOperationType(BoolFilterType::And);
  boolFilter->Update();
  m_Working_Support = boolFilter->GetOutput();

  // End
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Post_Limits() 
{
  StartNewStep("[Station 2RL] Post limits with post wall of Trachea");

  // Get Trachea
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  
  // Resize like the current support (to have the same number of slices)
  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());

  // Find extrema post positions
  std::vector<MaskImagePointType> tracheaPostPositionsA;
  std::vector<MaskImagePointType> tracheaPostPositionsB;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(Trachea, 
                                                                               GetBackgroundValue(), 2, 
                                                                               1, false, // Post
                                                                               0, // Horizontal line 
                                                                               1, 
                                                                               tracheaPostPositionsA, 
                                                                               tracheaPostPositionsB);
  // Cut post to this line
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    tracheaPostPositionsA,
                                                                    tracheaPostPositionsB,
                                                                    GetBackgroundValue(), 1, -10); 
  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Build a vtk mesh from a list of slice number/closed-contours
template <class ImageType>
vtkSmartPointer<vtkPolyData> 
clitk::ExtractLymphStationsFilter<ImageType>::
Build3DMeshFrom2DContour(const std::vector<ImagePointType> & points)
{
  // create a contour, polydata. 
  vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();
  mesh->Allocate(); //for cell structures
  mesh->SetPoints(vtkPoints::New());
  vtkIdType ids[2];
  int point_number = points.size();
  for (unsigned int i=0; i<points.size(); i++) {
    mesh->GetPoints()->InsertNextPoint(points[i][0],points[i][1],points[i][2]);
    ids[0]=i;
    ids[1]=(ids[0]+1)%point_number; //0-1,1-2,...,n-1-0
    mesh->GetLines()->InsertNextCell(2,ids);
  }
  // Return
  return mesh;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_LR_Limits()
{
  // ---------------------------------------------------------------------------
  StartNewStep("[Station 2RL] Left/Right limits with Aorta");
  MaskImagePointer Aorta = GetAFDB()->template GetImage<MaskImageType>("Aorta");
  //  DD(GetFuzzyThreshold("2RL", "BrachioCephalicVein"));
  m_Working_Support = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_Working_Support, Aorta, 2, 
                                                       GetFuzzyThreshold("2RL", "Aorta"),
                                                       "RightTo", false, 2, true, false);  
  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;

  // ---------------------------------------------------------------------------
  StartNewStep("[Station 2RL] Left/Right limits with SubclavianArtery (Right)");
  
  // SliceBySliceRelativePosition + select CCL most at Right
  MaskImagePointer SubclavianArtery = GetAFDB()->template GetImage<MaskImageType>("SubclavianArtery");
  typedef SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(SubclavianArtery);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("2RL", "SubclavianArteryRight"));
  sliceRelPosFilter->AddOrientationTypeString("NotRightTo");
  sliceRelPosFilter->IntermediateSpacingFlagOn();
  sliceRelPosFilter->SetIntermediateSpacing(2);
  sliceRelPosFilter->UniqueConnectedComponentBySliceOff();
  sliceRelPosFilter->UseASingleObjectConnectedComponentBySliceFlagOff();

  sliceRelPosFilter->CCLSelectionFlagOn(); // select one CCL by slice
  sliceRelPosFilter->SetCCLSelectionDimension(0); // select according to X (0) axis
  sliceRelPosFilter->SetCCLSelectionDirection(-1); // select most at Right
  sliceRelPosFilter->CCLSelectionIgnoreSingleCCLFlagOn(); // ignore if only one CCL

  sliceRelPosFilter->AutoCropFlagOn(); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->RemoveObjectFlagOff();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;


  // ---------------------------------------------------------------------------
  StartNewStep("[Station 2RL] Left/Right limits with SubclavianArtery (Left)");
  
  // SliceBySliceRelativePosition + select CCL most at Right
   sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(m_Working_Support);
  sliceRelPosFilter->SetInputObject(SubclavianArtery);
  sliceRelPosFilter->SetDirection(2);
  sliceRelPosFilter->SetFuzzyThreshold(GetFuzzyThreshold("2RL", "SubclavianArteryLeft"));
  sliceRelPosFilter->AddOrientationTypeString("NotLeftTo");
  sliceRelPosFilter->IntermediateSpacingFlagOn();
  sliceRelPosFilter->SetIntermediateSpacing(2);
  sliceRelPosFilter->UniqueConnectedComponentBySliceOff();
  sliceRelPosFilter->UseASingleObjectConnectedComponentBySliceFlagOff();

  sliceRelPosFilter->CCLSelectionFlagOn(); // select one CCL by slice
  sliceRelPosFilter->SetCCLSelectionDimension(0); // select according to X (0) axis
  sliceRelPosFilter->SetCCLSelectionDirection(+1); // select most at Left
  sliceRelPosFilter->CCLSelectionIgnoreSingleCCLFlagOff(); // do not ignore if only one CCL

  sliceRelPosFilter->AutoCropFlagOn(); 
  sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
  sliceRelPosFilter->RemoveObjectFlagOff();
  sliceRelPosFilter->Update();
  m_Working_Support = sliceRelPosFilter->GetOutput();

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Remove_Structures()
{
  Remove_Structures("2RL", "BrachioCephalicVein");
  Remove_Structures("2RL", "CommonCarotidArtery");
  Remove_Structures("2RL", "SubclavianArtery");
  Remove_Structures("2RL", "Thyroid");
  Remove_Structures("2RL", "Aorta");
  
  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_SeparateRL()
{
  // ---------------------------------------------------------------------------
  StartNewStep("[Station 2RL] Separate 2R/2L according to Trachea");

  /*Rod says: 
    
   "For station 2 there is a shift, dividing 2R from 2L, from midline
   to the left paratracheal border."

   Algo: 
    - Consider Trachea SliceBySlice
    - find extrema at Left
    - add margins towards Right
    - remove what is at Left of this line
   */

  // Get Trachea
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  
  // Resize like the current support (to have the same number of slices)
  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());

  // Find extrema post positions
  std::vector<MaskImagePointType> tracheaLeftPositionsA;
  std::vector<MaskImagePointType> tracheaLeftPositionsB;
  clitk::SliceBySliceBuildLineSegmentAccordingToExtremaPosition<MaskImageType>(Trachea, 
                                                                               GetBackgroundValue(), 2, 
                                                                               0, false, // Left
                                                                               1, // Vertical line 
                                                                               1, // margins 
                                                                               tracheaLeftPositionsA, 
                                                                               tracheaLeftPositionsB);
  // Copy support for R and L
  typedef itk::ImageDuplicator<MaskImageType> DuplicatorType;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(m_Working_Support);
  duplicator->Update();
  MaskImageType::Pointer m_Working_Support2 = duplicator->GetOutput();
  
  // Cut post to this line for Right part
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support, 
                                                                    tracheaLeftPositionsA,
                                                                    tracheaLeftPositionsB,
                                                                    GetBackgroundValue(), 0, -10); 
  writeImage<MaskImageType>(m_Working_Support, "R.mhd");

  // Cut post to this line for Left part
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(m_Working_Support2, 
                                                                    tracheaLeftPositionsA,
                                                                    tracheaLeftPositionsB,
                                                                    GetBackgroundValue(), 0, +10); 
  writeImage<MaskImageType>(m_Working_Support2, "L.mhd");

  // END
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["2R"] = m_Working_Support;
  m_ListOfStations["2L"] = m_Working_Support2;
}
//--------------------------------------------------------------------
