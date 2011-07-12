/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ======================================================================-====*/

#ifndef CLITKEXTRACTLYMPHSTATIONSFILTER_TXX
#define CLITKEXTRACTLYMPHSTATIONSFILTER_TXX

// clitk
#include "clitkCommon.h"
#include "clitkExtractLymphStationsFilter.h"
#include "clitkAddRelativePositionConstraintToLabelImageFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkAutoCropFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkSliceBySliceRelativePositionFilter.h"

// itk
#include <itkStatisticsLabelMapFilter.h>
#include <itkLabelImageToStatisticsLabelMapFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkImageSliceConstIteratorWithIndex.h>
#include <itkImageSliceIteratorWithIndex.h>
#include <itkBinaryThinningImageFilter.h>

// itk ENST
#include "RelativePositionPropImageFilter.h"

//--------------------------------------------------------------------
template <class TImageType>
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractLymphStationsFilter():
  clitk::FilterBase(),
  clitk::FilterWithAnatomicalFeatureDatabaseManagement(),
  itk::ImageToImageFilter<TImageType, MaskImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0);
  SetForegroundValue(1);

  // Default values
  ExtractStation_8_SetDefaultValues();
  ExtractStation_3P_SetDefaultValues();
  ExtractStation_2RL_SetDefaultValues();
  ExtractStation_3A_SetDefaultValues();
  ExtractStation_7_SetDefaultValues();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateOutputInformation() { 
  // Get inputs
  LoadAFDB();
  m_Input = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
  m_Mediastinum = GetAFDB()->template GetImage <MaskImageType>("Mediastinum");

  // Global supports for stations
  StartNewStep("Supports for stations");
  StartSubStep(); 
  ExtractStationSupports();
  StopSubStep();  

  // Extract Station8
  StartNewStep("Station 8");
  StartSubStep(); 
  ExtractStation_8();
  StopSubStep();

  // Extract Station3P
  StartNewStep("Station 3P");
  StartSubStep(); 
  ExtractStation_3P();
  StopSubStep();

  // Extract Station2RL
  /*
    StartNewStep("Station 2RL");
    StartSubStep(); 
    ExtractStation_2RL();
    StopSubStep();
  */

  // Extract Station3A
  StartNewStep("Station 3A");
  StartSubStep(); 
  ExtractStation_3A();
  StopSubStep();

  // Extract Station7
  StartNewStep("Station 7");
  StartSubStep();
  ExtractStation_7();
  StopSubStep();

  if (0) { // temporary suppress
    // Extract Station4RL
    StartNewStep("Station 4RL");
    StartSubStep();
    //ExtractStation_4RL();
    StopSubStep();
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateInputRequestedRegion() {
  //DD("GenerateInputRequestedRegion (nothing?)");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateData() {
  // Final Step -> graft output (if SetNthOutput => redo)
  this->GraftOutput(m_ListOfStations["8"]);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class TImageType>
bool 
clitk::ExtractLymphStationsFilter<TImageType>::
CheckForStation(std::string station) 
{
  // Compute Station name
  std::string s = "Station"+station;
  

  // Check if station already exist in DB
  bool found = false;
  if (GetAFDB()->TagExist(s)) {
    m_ListOfStations[station] = GetAFDB()->template GetImage<MaskImageType>(s);
    found = true;
  }

  // Define the starting support 
  if (found && GetComputeStation(station)) {
    std::cout << "Station " << station << " already exists, but re-computation forced." << std::endl;
  }
  if (!found || GetComputeStation(station)) {
    m_Working_Support = m_Mediastinum = GetAFDB()->template GetImage<MaskImageType>("Mediastinum", true);
    return true;
  }
  else {
    std::cout << "Station " << station << " found. I used it" << std::endl;
    return false;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
bool
clitk::ExtractLymphStationsFilter<TImageType>::
GetComputeStation(std::string station) 
{
  return (m_ComputeStationMap.find(station) != m_ComputeStationMap.end());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void
clitk::ExtractLymphStationsFilter<TImageType>::
AddComputeStation(std::string station) 
{
  m_ComputeStationMap[station] = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_3P()
{
  if (CheckForStation("3P")) {
    ExtractStation_3P_SI_Limits();
    ExtractStation_3P_Ant_Limits();
    ExtractStation_3P_Post_Limits();
    ExtractStation_3P_LR_sup_Limits();
    //    ExtractStation_3P_LR_sup_Limits_2();
    ExtractStation_3P_LR_inf_Limits();
    ExtractStation_8_Single_CCL_Limits(); // YES 8 !
    ExtractStation_3P_Remove_Structures(); // after CCL
    // Store image filenames into AFDB 
    writeImage<MaskImageType>(m_ListOfStations["3P"], "seg/Station3P.mhd");
    GetAFDB()->SetImageFilename("Station3P", "seg/Station3P.mhd"); 
    WriteAFDB(); 
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_3A()
{
  if (CheckForStation("3A")) {
    ExtractStation_3A_SI_Limits();
    ExtractStation_3A_Ant_Limits();
    ExtractStation_3A_Post_Limits();
    // Store image filenames into AFDB 
    writeImage<MaskImageType>(m_ListOfStations["3A"], "seg/Station3A.mhd");
    GetAFDB()->SetImageFilename("Station3A", "seg/Station3A.mhd"); 
    WriteAFDB(); 
  }
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
    ExtractStation_2RL_Ant_Limits(); 
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
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_4RL() {
  DD("TODO");
  exit(0);
  /*
    WARNING ONLY 4R FIRST !!! (not same inf limits)
  */    
  ExtractStation_4RL_SI_Limits();
  ExtractStation_4RL_LR_Limits();
  ExtractStation_4RL_AP_Limits();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Remove_Structures(std::string station, std::string s)
{
  try {
    StartNewStep("[Station"+station+"] Remove "+s);  
    MaskImagePointer Structure = GetAFDB()->template GetImage<MaskImageType>(s);
    clitk::AndNot<MaskImageType>(m_Working_Support, Structure, GetBackgroundValue());
  }
  catch(clitk::ExceptionObject e) {
    std::cout << s << " not found, skip." << std::endl;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
SetFuzzyThreshold(std::string station, std::string tag, double value)
{
  m_FuzzyThreshold[station][tag] = value;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
double 
clitk::ExtractLymphStationsFilter<TImageType>::
GetFuzzyThreshold(std::string station, std::string tag)
{
  if (m_FuzzyThreshold.find(station) == m_FuzzyThreshold.end()) {
    clitkExceptionMacro("Could not find options for station "+station+" in the list (while searching for tag "+tag+").");
    return 0.0;
  }

  if (m_FuzzyThreshold[station].find(tag) == m_FuzzyThreshold[station].end()) {
    clitkExceptionMacro("Could not find options "+tag+" in the list of FuzzyThreshold for station "+station+".");
    return 0.0;
  }
  
  return m_FuzzyThreshold[station][tag]; 
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void
clitk::ExtractLymphStationsFilter<TImageType>::
FindLineForS7S8Separation(MaskImagePointType & A, MaskImagePointType & B)
{
  // Create line from A to B with
  // A = upper border of LLL at left
  // B = lower border of bronchus intermedius (BI) or RightMiddleLobeBronchus
  
  try {
    GetAFDB()->GetPoint3D("LineForS7S8Separation_Begin", A); 
    GetAFDB()->GetPoint3D("LineForS7S8Separation_End", B);
  }
  catch(clitk::ExceptionObject & o) {
    
    DD("FindLineForS7S8Separation");
    // Load LeftLowerLobeBronchus and get centroid point
    MaskImagePointer LeftLowerLobeBronchus = 
      GetAFDB()->template GetImage <MaskImageType>("LeftLowerLobeBronchus");
    std::vector<MaskImagePointType> c;
    clitk::ComputeCentroids<MaskImageType>(LeftLowerLobeBronchus, GetBackgroundValue(), c);
    A = c[1];
    
    // Load RightMiddleLobeBronchus and get superior point (not centroid here)
    MaskImagePointer RightMiddleLobeBronchus = 
      GetAFDB()->template GetImage <MaskImageType>("RightMiddleLobeBronchus");
    bool b = FindExtremaPointInAGivenDirection<MaskImageType>(RightMiddleLobeBronchus, 
                                                              GetBackgroundValue(), 
                                                              2, false, B);
    if (!b) {
      clitkExceptionMacro("Error while searching most superior point in RightMiddleLobeBronchus. Abort");
    }
    
    // Insert into the DB
    GetAFDB()->SetPoint3D("LineForS7S8Separation_Begin", A);
    GetAFDB()->SetPoint3D("LineForS7S8Separation_End", B);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
double 
clitk::ExtractLymphStationsFilter<TImageType>::
FindCarinaSlicePosition()
{
  double z;
  try {
    z = GetAFDB()->GetDouble("CarinaZ");
  }
  catch(clitk::ExceptionObject e) {
    DD("FindCarinaSlicePosition");
    // Get Carina
    MaskImagePointer Carina = GetAFDB()->template GetImage<MaskImageType>("Carina");
    
    // Get Centroid and Z value
    std::vector<MaskImagePointType> centroids;
    clitk::ComputeCentroids<MaskImageType>(Carina, GetBackgroundValue(), centroids);

    // We dont need Carina structure from now
    Carina->Delete();
    
    // Put inside the AFDB
    GetAFDB()->SetPoint3D("CarinaPoint", centroids[1]);
    GetAFDB()->SetDouble("CarinaZ", centroids[1][2]);
    WriteAFDB();
    z = centroids[1][2];
  }
  return z;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void
clitk::ExtractLymphStationsFilter<TImageType>::
FindLeftAndRightBronchi()
{
  try {
    m_RightBronchus = GetAFDB()->template GetImage <MaskImageType>("RightBronchus");
    m_LeftBronchus = GetAFDB()->template GetImage <MaskImageType>("LeftBronchus");
  }
  catch(clitk::ExceptionObject & o) {

    DD("FindLeftAndRightBronchi");
    // The goal is to separate the trachea inferiorly to the carina into
    // a Left and Right bronchus.
  
    // Get the trachea
    MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");

    // Get the Carina position
    m_CarinaZ = FindCarinaSlicePosition();

    // Consider only inferiorly to the Carina
    MaskImagePointer m_Working_Trachea = 
      clitk::CropImageRemoveGreaterThan<MaskImageType>(Trachea, 2, m_CarinaZ, true, // AutoCrop
                                                       GetBackgroundValue());

    // Labelize the trachea
    m_Working_Trachea = Labelize<MaskImageType>(m_Working_Trachea, 0, true, 1);

    // Carina position must at the first slice that separate the two
    // main bronchus (not superiorly). We thus first check that the
    // upper slice is composed of at least two labels
    MaskImagePointer RightBronchus;
    MaskImagePointer LeftBronchus;
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
      clitkExceptionMacro("First slice from Carina does not seems to seperate the two main bronchus. Abort");
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
  }
}
//--------------------------------------------------------------------

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
