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
#include "clitkMeshToBinaryImageFilter.h"

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

// vtk
#include <vtkAppendPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>

//--------------------------------------------------------------------
template <class TImageType>
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractLymphStationsFilter():
  clitk::StructuresExtractionFilter<ImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0);
  SetForegroundValue(1);
  ComputeStationsSupportsFlagOn();

  // Default values
  ExtractStation_3P_SetDefaultValues();
  ExtractStation_2RL_SetDefaultValues();
  ExtractStation_3A_SetDefaultValues();
  ExtractStation_1RL_SetDefaultValues();
  ExtractStation_4RL_SetDefaultValues();
  ExtractStation_5_SetDefaultValues();
  ExtractStation_6_SetDefaultValues();

  // TODO
  ExtractStation_7_SetDefaultValues();
  ExtractStation_8_SetDefaultValues();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateOutputInformation() { 
  // Get inputs
  this->LoadAFDB();
  m_Input = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
  m_Mediastinum = this->GetAFDB()->template GetImage <MaskImageType>("Mediastinum");

  // Clean some computer landmarks to force the recomputation
  this->GetAFDB()->RemoveTag("AntPostVesselsSeparation");

  // Global supports for stations
  bool supportsExist = true;
  try {
    m_ListOfSupports["S1R"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S1R");
    m_ListOfSupports["S1L"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S1L");
    m_ListOfSupports["S2R"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S2R");
    m_ListOfSupports["S2L"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S2L");
    m_ListOfSupports["S4R"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S4R");
    m_ListOfSupports["S4L"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S4L");
    
    m_ListOfSupports["S3A"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S3A");
    m_ListOfSupports["S3P"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S3P");
    m_ListOfSupports["S5"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S5");
    m_ListOfSupports["S6"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S6");
    m_ListOfSupports["S7"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S7");
    m_ListOfSupports["S8"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S8");
    m_ListOfSupports["S9"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S9");
    m_ListOfSupports["S10"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S10");
    m_ListOfSupports["S11"] = this->GetAFDB()->template GetImage<MaskImageType>("Support_S11");
  } catch(clitk::ExceptionObject o) {
    supportsExist = false;
  }

  if (!supportsExist || GetComputeStationsSupportsFlag()) {
    this->StartNewStep("Supports for stations");
    this->StartSubStep(); 
    this->GetAFDB()->RemoveTag("CarinaZ");
    this->GetAFDB()->RemoveTag("ApexOfTheChestZ");
    this->GetAFDB()->RemoveTag("ApexOfTheChest");
    this->GetAFDB()->RemoveTag("RightBronchus");
    this->GetAFDB()->RemoveTag("LeftBronchus");
    this->GetAFDB()->RemoveTag("SuperiorBorderOfAorticArchZ");
    this->GetAFDB()->RemoveTag("SuperiorBorderOfAorticArch");
    this->GetAFDB()->RemoveTag("InferiorBorderOfAorticArchZ");
    this->GetAFDB()->RemoveTag("InferiorBorderOfAorticArch");
    ExtractStationSupports();
    this->StopSubStep();  
  }

  // Extract Stations
  ExtractStation_1RL();
  ExtractStation_2RL();
  ExtractStation_3P();
  ExtractStation_3A();
  ExtractStation_4RL();
  ExtractStation_5();
  ExtractStation_6();

  // ---------- TODO -----------------------

  // Extract Station8
  //  ExtractStation_8();

  // Extract Station7
  //this->StartNewStep("Station 7");
  //this->StartSubStep();
  //ExtractStation_7();
  //this->StopSubStep();
  
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
  if (this->GetAFDB()->TagExist(s)) {
    m_ListOfStations[station] = this->GetAFDB()->template GetImage<MaskImageType>(s);
    found = true;
  }

  // Define the starting support 
  if (found && GetComputeStation(station)) {
    std::cout << "Station " << station << " already exists, but re-computation forced." << std::endl;
  }
  if (!found || GetComputeStation(station)) {
    m_Working_Support = m_Mediastinum = this->GetAFDB()->template GetImage<MaskImageType>("Mediastinum", true);
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
template<class PointType>
class comparePointsX {
public:
  bool operator() (PointType i, PointType j) { return (i[0]<j[0]); }
};
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PairType>
class comparePointsWithAngle {
public:
  bool operator() (PairType i, PairType j) { return (i.second < j.second); } 
};
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<int Dim>
void HypercubeCorners(std::vector<itk::Point<double, Dim> > & out) {
  std::vector<itk::Point<double, Dim-1> > previous;
  HypercubeCorners<Dim-1>(previous);
  out.resize(previous.size()*2);
  for(unsigned int i=0; i<out.size(); i++) {
    itk::Point<double, Dim> p;
    if (i<previous.size()) p[0] = 0; 
    else p[0] = 1;
    for(int j=0; j<Dim-1; j++) 
      {
        p[j+1] = previous[i%previous.size()][j];
      }
    out[i] = p;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<>
void HypercubeCorners<1>(std::vector<itk::Point<double, 1> > & out) {
  out.resize(2);
  out[0][0] = 0;
  out[1][0] = 1;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void ComputeImageBoundariesCoordinates(typename ImageType::Pointer image, 
                                       std::vector<typename ImageType::PointType> & bounds) 
{
  // Get image max/min coordinates
  const unsigned int dim=ImageType::ImageDimension;
  typedef typename ImageType::PointType PointType;
  typedef typename ImageType::IndexType IndexType;
  PointType min_c, max_c;
  IndexType min_i, max_i;
  min_i = image->GetLargestPossibleRegion().GetIndex();
  for(unsigned int i=0; i<dim; i++)
    max_i[i] = image->GetLargestPossibleRegion().GetSize()[i] + min_i[i];
  image->TransformIndexToPhysicalPoint(min_i, min_c);
  image->TransformIndexToPhysicalPoint(max_i, max_c);
  
  // Get corners coordinates
  HypercubeCorners<ImageType::ImageDimension>(bounds);
  for(unsigned int i=0; i<bounds.size(); i++) {
    for(unsigned int j=0; j<dim; j++) {
      if (bounds[i][j] == 0) bounds[i][j] = min_c[j];
      if (bounds[i][j] == 1) bounds[i][j] = max_c[j];
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
Remove_Structures(std::string station, std::string s)
{
  try {
    this->StartNewStep("[Station"+station+"] Remove "+s);  
    MaskImagePointer Structure = this->GetAFDB()->template GetImage<MaskImageType>(s);
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
void 
clitk::ExtractLymphStationsFilter<TImageType>::
SetThreshold(std::string station, std::string tag, double value)
{
  m_Threshold[station][tag] = value;
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
double 
clitk::ExtractLymphStationsFilter<TImageType>::
GetThreshold(std::string station, std::string tag)
{
  if (m_Threshold.find(station) == m_Threshold.end()) {
    clitkExceptionMacro("Could not find options for station "+station+" in the list (while searching for tag "+tag+").");
    return 0.0;
  }

  if (m_Threshold[station].find(tag) == m_Threshold[station].end()) {
    clitkExceptionMacro("Could not find options "+tag+" in the list of Threshold for station "+station+".");
    return 0.0;
  }
  
  return m_Threshold[station][tag]; 
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
    this->GetAFDB()->GetPoint3D("LineForS7S8Separation_Begin", A); 
    this->GetAFDB()->GetPoint3D("LineForS7S8Separation_End", B);
  }
  catch(clitk::ExceptionObject & o) {
    
    DD("FindLineForS7S8Separation");
    // Load LeftLowerLobeBronchus and get centroid point
    MaskImagePointer LeftLowerLobeBronchus = 
      this->GetAFDB()->template GetImage <MaskImageType>("LeftLowerLobeBronchus");
    std::vector<MaskImagePointType> c;
    clitk::ComputeCentroids<MaskImageType>(LeftLowerLobeBronchus, GetBackgroundValue(), c);
    A = c[1];
    
    // Load RightMiddleLobeBronchus and get superior point (not centroid here)
    MaskImagePointer RightMiddleLobeBronchus = 
      this->GetAFDB()->template GetImage <MaskImageType>("RightMiddleLobeBronchus");
    bool b = FindExtremaPointInAGivenDirection<MaskImageType>(RightMiddleLobeBronchus, 
                                                              GetBackgroundValue(), 
                                                              2, false, B);
    if (!b) {
      clitkExceptionMacro("Error while searching most superior point in RightMiddleLobeBronchus. Abort");
    }
    
    // Insert into the DB
    this->GetAFDB()->SetPoint3D("LineForS7S8Separation_Begin", A);
    this->GetAFDB()->SetPoint3D("LineForS7S8Separation_End", B);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
double 
clitk::ExtractLymphStationsFilter<TImageType>::
FindCarina()
{
  double z;
  try {
    z = this->GetAFDB()->GetDouble("CarinaZ");
  }
  catch(clitk::ExceptionObject e) {
    DD("FindCarinaSlicePosition");
    // Get Carina
    MaskImagePointer Carina = this->GetAFDB()->template GetImage<MaskImageType>("Carina");
    
    // Get Centroid and Z value
    std::vector<MaskImagePointType> centroids;
    clitk::ComputeCentroids<MaskImageType>(Carina, GetBackgroundValue(), centroids);

    // We dont need Carina structure from now
    this->GetAFDB()->template ReleaseImage<MaskImageType>("Carina");
    
    // Put inside the AFDB
    this->GetAFDB()->SetPoint3D("CarinaPoint", centroids[1]);
    this->GetAFDB()->SetDouble("CarinaZ", centroids[1][2]);
    this->WriteAFDB();
    z = centroids[1][2];
  }
  return z;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
double 
clitk::ExtractLymphStationsFilter<TImageType>::
FindApexOfTheChest()
{
  double z;
  try {
    z = this->GetAFDB()->GetDouble("ApexOfTheChestZ");
  }
  catch(clitk::ExceptionObject e) {
    DD("FindApexOfTheChestPosition");
    MaskImagePointer Lungs = this->GetAFDB()->template GetImage<MaskImageType>("Lungs");
    MaskImagePointType p;
    p[0] = p[1] = p[2] =  0.0; // to avoid warning
    clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Lungs, GetBackgroundValue(), 2, false, p);

    // We dont need Lungs structure from now
    this->GetAFDB()->template ReleaseImage<MaskImageType>("Lungs");
    
    // Put inside the AFDB
    this->GetAFDB()->SetPoint3D("ApexOfTheChest", p);
    p[2] -= 5; // We consider 5 mm lower 
    this->GetAFDB()->SetDouble("ApexOfTheChestZ", p[2]);
    this->WriteAFDB();
    z = p[2];
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
    m_RightBronchus = this->GetAFDB()->template GetImage <MaskImageType>("RightBronchus");
    m_LeftBronchus = this->GetAFDB()->template GetImage <MaskImageType>("LeftBronchus");
  }
  catch(clitk::ExceptionObject & o) {

    DD("FindLeftAndRightBronchi");
    // The goal is to separate the trachea inferiorly to the carina into
    // a Left and Right bronchus.
  
    // Get the trachea
    MaskImagePointer Trachea = this->GetAFDB()->template GetImage<MaskImageType>("Trachea");

    // Get the Carina position
    double m_CarinaZ = FindCarina();

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
    this->GetAFDB()->template SetImage <MaskImageType>("RightBronchus", "seg/rightBronchus.mhd", 
                                                 RightBronchus, true);
    this->GetAFDB()->template SetImage <MaskImageType>("LeftBronchus", "seg/leftBronchus.mhd", 
                                                 LeftBronchus, true);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
double 
clitk::ExtractLymphStationsFilter<TImageType>::
FindSuperiorBorderOfAorticArch()
{
  double z;
  try {
    z = this->GetAFDB()->GetDouble("SuperiorBorderOfAorticArchZ");
  }
  catch(clitk::ExceptionObject e) {
    DD("FindSuperiorBorderOfAorticArch");
    MaskImagePointer Aorta = this->GetAFDB()->template GetImage<MaskImageType>("Aorta");
    MaskImagePointType p;
    p[0] = p[1] = p[2] =  0.0; // to avoid warning
    clitk::FindExtremaPointInAGivenDirection<MaskImageType>(Aorta, GetBackgroundValue(), 2, false, p);
    p[2] += Aorta->GetSpacing()[2]; // the slice above
    
    // We dont need Lungs structure from now
    this->GetAFDB()->template ReleaseImage<MaskImageType>("Aorta");
    
    // Put inside the AFDB
    this->GetAFDB()->SetPoint3D("SuperiorBorderOfAorticArch", p);
    this->GetAFDB()->SetDouble("SuperiorBorderOfAorticArchZ", p[2]);
    this->WriteAFDB();
    z = p[2];
  }
  return z;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
double 
clitk::ExtractLymphStationsFilter<TImageType>::
FindInferiorBorderOfAorticArch()
{
  double z;
  try {
    z = this->GetAFDB()->GetDouble("InferiorBorderOfAorticArchZ");
  }
  catch(clitk::ExceptionObject e) {
    DD("FindInferiorBorderOfAorticArch");
    MaskImagePointer Aorta = this->GetAFDB()->template GetImage<MaskImageType>("Aorta");
    std::vector<MaskSlicePointer> slices;
    clitk::ExtractSlices<MaskImageType>(Aorta, 2, slices);
    bool found=false;
    uint i = slices.size()-1;
    while (!found) {
      slices[i] = Labelize<MaskSliceType>(slices[i], 0, false, 10);
      std::vector<typename MaskSliceType::PointType> c;
      clitk::ComputeCentroids<MaskSliceType>(slices[i], GetBackgroundValue(), c);
      if (c.size()>2) {
        found = true;
      }
      else {
        i--;
      }
    }
    MaskImageIndexType index;
    index[0] = index[1] = 0.0;
    index[2] = i+1;
    MaskImagePointType lower;
    Aorta->TransformIndexToPhysicalPoint(index, lower);
    
    // We dont need Lungs structure from now
    this->GetAFDB()->template ReleaseImage<MaskImageType>("Aorta");
    
    // Put inside the AFDB
    this->GetAFDB()->SetPoint3D("InferiorBorderOfAorticArch", lower);
    this->GetAFDB()->SetDouble("InferiorBorderOfAorticArchZ", lower[2]);
    this->WriteAFDB();
    z = lower[2];
  }
  return z;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
typename clitk::ExtractLymphStationsFilter<ImageType>::MaskImagePointer 
clitk::ExtractLymphStationsFilter<ImageType>::
FindAntPostVesselsOLD()
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

  // Check if no already done
  bool found = true;
  MaskImagePointer binarizedContour;
  try {
    binarizedContour = this->GetAFDB()->template GetImage <MaskImageType>("AntPostVesselsSeparation");
  }
  catch(clitk::ExceptionObject e) {
    found = false;
  }
  if (found) {
    return binarizedContour;
  }

  /* Here, we consider the vessels form a kind of anterior barrier. We
     link all vessels centroids and remove what is post to it.
    - select the list of structure
            vessel1 = BrachioCephalicArtery
            vessel2 = BrachioCephalicVein (warning several CCL, keep most at Right)
            vessel3 = CommonCarotidArtery
            vessel4 = SubclavianArtery
            other   = Thyroid
            other   = Aorta 
     - crop images as needed
     - slice by slice, choose the CCL and extract centroids
     - slice by slice, sort according to polar angle wrt Trachea centroid.
     - slice by slice, link centroids and close contour
     - remove outside this contour
     - merge with support 
  */

  // Read structures
  MaskImagePointer BrachioCephalicArtery = this->GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicArtery");
  MaskImagePointer BrachioCephalicVein = this->GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  MaskImagePointer CommonCarotidArtery = this->GetAFDB()->template GetImage<MaskImageType>("CommonCarotidArtery");
  MaskImagePointer SubclavianArtery = this->GetAFDB()->template GetImage<MaskImageType>("SubclavianArtery");
  MaskImagePointer Thyroid = this->GetAFDB()->template GetImage<MaskImageType>("Thyroid");
  MaskImagePointer Aorta = this->GetAFDB()->template GetImage<MaskImageType>("Aorta");
  MaskImagePointer Trachea = this->GetAFDB()->template GetImage<MaskImageType>("Trachea");
  
  // Create a temporay support
  // From first slice of BrachioCephalicVein to end of 3A
  MaskImagePointer support = this->GetAFDB()->template GetImage<MaskImageType>("Support_Sup_Carina");
  MaskImagePointType p;
  p[0] = p[1] = p[2] =  0.0; // to avoid warning
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(BrachioCephalicVein, GetBackgroundValue(), 2, true, p);
  double inf = p [2];
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(this->GetAFDB()->template GetImage<MaskImageType>("Support_S3A"), 
                                                          GetBackgroundValue(), 2, false, p);
  double sup = p [2];  
  support = clitk::CropImageAlongOneAxis<MaskImageType>(support, 2, inf, sup, 
                                                        false, GetBackgroundValue());

  // Resize all structures like support
  BrachioCephalicArtery = 
    clitk::ResizeImageLike<MaskImageType>(BrachioCephalicArtery, support, GetBackgroundValue());
  CommonCarotidArtery = 
    clitk::ResizeImageLike<MaskImageType>(CommonCarotidArtery, support, GetBackgroundValue());
  SubclavianArtery = 
    clitk::ResizeImageLike<MaskImageType>(SubclavianArtery, support, GetBackgroundValue());
  Thyroid = 
    clitk::ResizeImageLike<MaskImageType>(Thyroid, support, GetBackgroundValue());
  Aorta = 
    clitk::ResizeImageLike<MaskImageType>(Aorta, support, GetBackgroundValue());
  BrachioCephalicVein = 
    clitk::ResizeImageLike<MaskImageType>(BrachioCephalicVein, support, GetBackgroundValue());
  Trachea = 
    clitk::ResizeImageLike<MaskImageType>(Trachea, support, GetBackgroundValue());

  // Extract slices
  std::vector<MaskSlicePointer> slices_BrachioCephalicArtery;
  clitk::ExtractSlices<MaskImageType>(BrachioCephalicArtery, 2, slices_BrachioCephalicArtery);
  std::vector<MaskSlicePointer> slices_BrachioCephalicVein;
  clitk::ExtractSlices<MaskImageType>(BrachioCephalicVein, 2, slices_BrachioCephalicVein);
  std::vector<MaskSlicePointer> slices_CommonCarotidArtery;
  clitk::ExtractSlices<MaskImageType>(CommonCarotidArtery, 2, slices_CommonCarotidArtery);
  std::vector<MaskSlicePointer> slices_SubclavianArtery;
  clitk::ExtractSlices<MaskImageType>(SubclavianArtery, 2, slices_SubclavianArtery);
  std::vector<MaskSlicePointer> slices_Thyroid;
  clitk::ExtractSlices<MaskImageType>(Thyroid, 2, slices_Thyroid);
  std::vector<MaskSlicePointer> slices_Aorta;
  clitk::ExtractSlices<MaskImageType>(Aorta, 2, slices_Aorta);
  std::vector<MaskSlicePointer> slices_Trachea;
  clitk::ExtractSlices<MaskImageType>(Trachea, 2, slices_Trachea);
  unsigned int n= slices_BrachioCephalicArtery.size();
  
  // Get the boundaries of one slice
  std::vector<MaskSlicePointType> bounds;
  ComputeImageBoundariesCoordinates<MaskSliceType>(slices_BrachioCephalicArtery[0], bounds);

  // For all slices, for all structures, find the centroid and build the contour
  // List of 3D points (for debug)
  std::vector<MaskImagePointType> p3D;

  vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  for(unsigned int i=0; i<n; i++) {
    // Labelize the slices
    slices_CommonCarotidArtery[i] = Labelize<MaskSliceType>(slices_CommonCarotidArtery[i], 
                                                            GetBackgroundValue(), true, 1);
    slices_SubclavianArtery[i] = Labelize<MaskSliceType>(slices_SubclavianArtery[i], 
                                                         GetBackgroundValue(), true, 1);
    slices_BrachioCephalicArtery[i] = Labelize<MaskSliceType>(slices_BrachioCephalicArtery[i], 
                                                             GetBackgroundValue(), true, 1);
    slices_BrachioCephalicVein[i] = Labelize<MaskSliceType>(slices_BrachioCephalicVein[i], 
                                                            GetBackgroundValue(), true, 1);
    slices_Thyroid[i] = Labelize<MaskSliceType>(slices_Thyroid[i], 
                                                GetBackgroundValue(), true, 1);
    slices_Aorta[i] = Labelize<MaskSliceType>(slices_Aorta[i], 
                                              GetBackgroundValue(), true, 1);

    // Search centroids
    std::vector<MaskSlicePointType> points2D;
    std::vector<MaskSlicePointType> centroids1;
    std::vector<MaskSlicePointType> centroids2;
    std::vector<MaskSlicePointType> centroids3;
    std::vector<MaskSlicePointType> centroids4;
    std::vector<MaskSlicePointType> centroids5;
    std::vector<MaskSlicePointType> centroids6;
    ComputeCentroids<MaskSliceType>(slices_CommonCarotidArtery[i], GetBackgroundValue(), centroids1);
    ComputeCentroids<MaskSliceType>(slices_SubclavianArtery[i], GetBackgroundValue(), centroids2);
    ComputeCentroids<MaskSliceType>(slices_BrachioCephalicArtery[i], GetBackgroundValue(), centroids3);
    ComputeCentroids<MaskSliceType>(slices_Thyroid[i], GetBackgroundValue(), centroids4);
    ComputeCentroids<MaskSliceType>(slices_Aorta[i], GetBackgroundValue(), centroids5);
    ComputeCentroids<MaskSliceType>(slices_BrachioCephalicVein[i], GetBackgroundValue(), centroids6);

    // BrachioCephalicVein -> when it is separated into two CCL, we
    // only consider the most at Right one
    if (centroids6.size() > 2) {
      if (centroids6[1][0] < centroids6[2][0]) centroids6.erase(centroids6.begin()+2);
      else centroids6.erase(centroids6.begin()+1);
    }
    
    // BrachioCephalicVein -> when SubclavianArtery has 2 CCL
    // (BrachioCephalicArtery is divided) -> forget BrachioCephalicVein
    if ((centroids3.size() ==1) && (centroids2.size() > 2)) {
      centroids6.clear();
    }

    for(unsigned int j=1; j<centroids1.size(); j++) points2D.push_back(centroids1[j]);
    for(unsigned int j=1; j<centroids2.size(); j++) points2D.push_back(centroids2[j]);
    for(unsigned int j=1; j<centroids3.size(); j++) points2D.push_back(centroids3[j]);
    for(unsigned int j=1; j<centroids4.size(); j++) points2D.push_back(centroids4[j]);
    for(unsigned int j=1; j<centroids5.size(); j++) points2D.push_back(centroids5[j]);
    for(unsigned int j=1; j<centroids6.size(); j++) points2D.push_back(centroids6[j]);
    
    // Sort by angle according to trachea centroid and vertical line,
    // in polar coordinates :
    // http://en.wikipedia.org/wiki/Polar_coordinate_system
    std::vector<MaskSlicePointType> centroids_trachea;
    ComputeCentroids<MaskSliceType>(slices_Trachea[i], GetBackgroundValue(), centroids_trachea);
    typedef std::pair<MaskSlicePointType, double> PointAngleType;
    std::vector<PointAngleType> angles;
    for(unsigned int j=0; j<points2D.size(); j++) {
      //double r = centroids_trachea[1].EuclideanDistanceTo(points2D[j]);
      double x = (points2D[j][0]-centroids_trachea[1][0]); // X : Right to Left
      double y = (centroids_trachea[1][1]-points2D[j][1]); // Y : Post to Ant
      double angle = 0;
      if (x>0) angle = atan(y/x);
      if ((x<0) && (y>=0)) angle = atan(y/x)+M_PI;
      if ((x<0) && (y<0)) angle = atan(y/x)-M_PI;
      if (x==0) {
        if (y>0) angle = M_PI/2.0;
        if (y<0) angle = -M_PI/2.0;
        if (y==0) angle = 0;
      }
      angle = clitk::rad2deg(angle);
      // Angle is [-180;180] wrt the X axis. We change the X axis to
      // be the vertical line, because we want to sort from Right to
      // Left from Post to Ant.
      if (angle>0) 
        angle = (270-angle);
      if (angle<0) {
        angle = -angle-90;
        if (angle<0) angle = 360-angle;
      }
      PointAngleType a;
      a.first = points2D[j];
      a.second = angle;
      angles.push_back(a);
    }

    // Do nothing if less than 2 points --> n
    if (points2D.size() < 3) { //continue;
      continue;
    }

    // Sort points2D according to polar angles
    std::sort(angles.begin(), angles.end(), comparePointsWithAngle<PointAngleType>());
    for(unsigned int j=0; j<angles.size(); j++) {
      points2D[j] = angles[j].first;
    }
    //    DDV(points2D, points2D.size());

    /* When vessels are far away, we try to replace the line segment
       with a curved line that join the two vessels but stay
       approximately at the same distance from the trachea centroids
       than the vessels.

       For that: 
       - let a and c be two successive vessels centroids
       - id distance(a,c) < threshold, next point

       TODO HERE
       
       - compute mid position between two successive points -
       compute dist to trachea centroid for the 3 pts - if middle too
       low, add one point
    */
    std::vector<MaskSlicePointType> toadd;
    unsigned int index = 0;
    double dmax = 5;
    while (index<points2D.size()-1) {
      MaskSlicePointType a = points2D[index];
      MaskSlicePointType c = points2D[index+1];

      double dac = a.EuclideanDistanceTo(c);
      if (dac>dmax) {
        
        MaskSlicePointType b;
        b[0] = a[0]+(c[0]-a[0])/2.0;
        b[1] = a[1]+(c[1]-a[1])/2.0;
        
        // Compute distance to trachea centroid
        MaskSlicePointType m = centroids_trachea[1];
        double da = m.EuclideanDistanceTo(a);
        double db = m.EuclideanDistanceTo(b);
        //double dc = m.EuclideanDistanceTo(c);
        
        // Mean distance, find point on the line from trachea centroid
        // to b
        double alpha = (da+db)/2.0;
        MaskSlicePointType v;
        double n = sqrt( pow(b[0]-m[0], 2) + pow(b[1]-m[1], 2));
        v[0] = (b[0]-m[0])/n;
        v[1] = (b[1]-m[1])/n;
        MaskSlicePointType r;
        r[0] = m[0]+alpha*v[0];
        r[1] = m[1]+alpha*v[1];
        points2D.insert(points2D.begin()+index+1, r);
      }
      else {
        index++;
      }
    }
    //    DDV(points2D, points2D.size());

    // Add some points to close the contour 
    // - H line towards Right
    MaskSlicePointType p = points2D[0]; 
    p[0] = bounds[0][0];
    points2D.insert(points2D.begin(), p);
    // - corner Right/Post
    p = bounds[0];
    points2D.insert(points2D.begin(), p);
    // - H line towards Left
    p = points2D.back(); 
    p[0] = bounds[2][0];
    points2D.push_back(p);
    // - corner Right/Post
    p = bounds[2];
    points2D.push_back(p);
    // Close contour with the first point
    points2D.push_back(points2D[0]);
    //    DDV(points2D, points2D.size());
      
    // Build 3D points from the 2D points
    std::vector<ImagePointType> points3D;
    clitk::PointsUtils<MaskImageType>::Convert2DListTo3DList(points2D, i, support, points3D);
    for(unsigned int x=0; x<points3D.size(); x++) p3D.push_back(points3D[x]);

    // Build the mesh from the contour's points
    vtkSmartPointer<vtkPolyData> mesh = Build3DMeshFrom2DContour(points3D);
    append->AddInput(mesh);
  }

  // DEBUG: write points3D
  clitk::WriteListOfLandmarks<MaskImageType>(p3D, "vessels-centroids.txt");

  // Build the final 3D mesh form the list 2D mesh
  append->Update();
  vtkSmartPointer<vtkPolyData> mesh = append->GetOutput();
  
  // Debug, write the mesh
  /*
    vtkSmartPointer<vtkPolyDataWriter> w = vtkSmartPointer<vtkPolyDataWriter>::New();
    w->SetInput(mesh);
    w->SetFileName("bidon.vtk");
    w->Write();    
  */

  // Compute a single binary 3D image from the list of contours
  clitk::MeshToBinaryImageFilter<MaskImageType>::Pointer filter = 
    clitk::MeshToBinaryImageFilter<MaskImageType>::New();
  filter->SetMesh(mesh);
  filter->SetLikeImage(support);
  filter->Update();
  binarizedContour = filter->GetOutput();  

  // Crop
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(binarizedContour, GetForegroundValue(), 2, true, p);
  inf = p[2];
  DD(p);
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(binarizedContour, GetForegroundValue(), 2, false, p);
  sup = p[2];
  DD(p);
  binarizedContour = clitk::CropImageAlongOneAxis<MaskImageType>(binarizedContour, 2, inf, sup, 
                                                        false, GetBackgroundValue());
  // Update the AFDB
  writeImage<MaskImageType>(binarizedContour, "seg/AntPostVesselsSeparation.mha");
  this->GetAFDB()->SetImageFilename("AntPostVesselsSeparation", "seg/AntPostVesselsSeparation.mha");
  this->WriteAFDB();
  return binarizedContour;

  /*
  // Inverse binary mask if needed. We test a point that we know must be in FG. If it is not, inverse
  ImagePointType p = p3D[2]; // This is the first centroid of the first slice
  p[1] += 50; // 50 mm Post from this point must be kept
  ImageIndexType index;
  binarizedContour->TransformPhysicalPointToIndex(p, index);
  bool isInside = (binarizedContour->GetPixel(index) != GetBackgroundValue());

  // remove from support
  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BoolFilterType;
  typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
  boolFilter->InPlaceOn();
  boolFilter->SetInput1(m_Working_Support);
  boolFilter->SetInput2(binarizedContour);
  boolFilter->SetBackgroundValue1(GetBackgroundValue());
  boolFilter->SetBackgroundValue2(GetBackgroundValue());
  if (isInside)
    boolFilter->SetOperationType(BoolFilterType::And);
  else
    boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update();
  m_Working_Support = boolFilter->GetOutput();
  */

  // End
  //StopCurrentStep<MaskImageType>(m_Working_Support);
  //m_ListOfStations["2R"] = m_Working_Support;
  //m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
typename clitk::ExtractLymphStationsFilter<ImageType>::MaskImagePointer 
clitk::ExtractLymphStationsFilter<ImageType>::
FindAntPostVessels2()
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

  // Check if no already done
  bool found = true;
  MaskImagePointer binarizedContour;
  try {
    binarizedContour = this->GetAFDB()->template GetImage <MaskImageType>("AntPostVesselsSeparation");
  }
  catch(clitk::ExceptionObject e) {
    found = false;
  }
  if (found) {
    return binarizedContour;
  }

  /* Here, we consider the vessels form a kind of anterior barrier. We
     link all vessels centroids and remove what is post to it.
    - select the list of structure
            vessel1 = BrachioCephalicArtery
            vessel2 = BrachioCephalicVein (warning several CCL, keep most at Right)
            vessel3 = CommonCarotidArtery
            vessel4 = SubclavianArtery
            other   = Thyroid
            other   = Aorta 
     - crop images as needed
     - slice by slice, choose the CCL and extract centroids
     - slice by slice, sort according to polar angle wrt Trachea centroid.
     - slice by slice, link centroids and close contour
     - remove outside this contour
     - merge with support 
  */

  // Read structures
  std::map<std::string, MaskImagePointer> MapOfStructures;  
  typedef std::map<std::string, MaskImagePointer>::iterator MapIter;
  MapOfStructures["BrachioCephalicArtery"] = this->GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicArtery");
  MapOfStructures["BrachioCephalicVein"] = this->GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  MapOfStructures["CommonCarotidArteryLeft"] = this->GetAFDB()->template GetImage<MaskImageType>("LeftCommonCarotidArtery");
  MapOfStructures["CommonCarotidArteryRight"] = this->GetAFDB()->template GetImage<MaskImageType>("RightCommonCarotidArtery");
  MapOfStructures["SubclavianArteryLeft"] = this->GetAFDB()->template GetImage<MaskImageType>("LeftSubclavianArtery");
  MapOfStructures["SubclavianArteryRight"] = this->GetAFDB()->template GetImage<MaskImageType>("RightSubclavianArtery");
  MapOfStructures["Thyroid"] = this->GetAFDB()->template GetImage<MaskImageType>("Thyroid");
  MapOfStructures["Aorta"] = this->GetAFDB()->template GetImage<MaskImageType>("Aorta");
  MapOfStructures["Trachea"] = this->GetAFDB()->template GetImage<MaskImageType>("Trachea");
  
  std::vector<std::string> ListOfStructuresNames;

  // Create a temporay support
  // From first slice of BrachioCephalicVein to end of 3A or end of 2RL
  MaskImagePointer support = this->GetAFDB()->template GetImage<MaskImageType>("Support_Sup_Carina");
  MaskImagePointType p;
  p[0] = p[1] = p[2] =  0.0; // to avoid warning
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(MapOfStructures["BrachioCephalicVein"], 
                                                          GetBackgroundValue(), 2, true, p);
  double inf = p[2];
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(this->GetAFDB()->template GetImage<MaskImageType>("Support_S3A"), 
                                                          GetBackgroundValue(), 2, false, p);
  MaskImagePointType p2;
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(this->GetAFDB()->template GetImage<MaskImageType>("Support_S2L"), 
                                                          GetBackgroundValue(), 2, false, p2);
  if (p2[2] > p[2]) p = p2;

  double sup = p[2]+support->GetSpacing()[2];//one slice more ?
  support = clitk::CropImageAlongOneAxis<MaskImageType>(support, 2, inf, sup, 
                                                        false, GetBackgroundValue());

  // Resize all structures like support
  for (MapIter it = MapOfStructures.begin(); it != MapOfStructures.end(); ++it) {
    it->second = clitk::ResizeImageLike<MaskImageType>(it->second, support, GetBackgroundValue());
  }

  // Extract slices
  typedef std::vector<MaskSlicePointer> SlicesType;
  std::map<std::string, SlicesType> MapOfSlices;
  for (MapIter it = MapOfStructures.begin(); it != MapOfStructures.end(); ++it) {
    SlicesType s;
    clitk::ExtractSlices<MaskImageType>(it->second, 2, s);    
    MapOfSlices[it->first] = s;
  }

  unsigned int n= MapOfSlices["Trachea"].size();
  
  // Get the boundaries of one slice
  std::vector<MaskSlicePointType> bounds;
  ComputeImageBoundariesCoordinates<MaskSliceType>(MapOfSlices["Trachea"][0], bounds);

  // LOOP ON SLICES
  // For all slices, for all structures, find the centroid and build the contour
  // List of 3D points (for debug)
  std::vector<MaskImagePointType> p3D;
  vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  for(unsigned int i=0; i<n; i++) {
    
    // Labelize the slices
    for (MapIter it = MapOfStructures.begin(); it != MapOfStructures.end(); ++it) {
      MaskSlicePointer & s = MapOfSlices[it->first][i];
      s = clitk::Labelize<MaskSliceType>(s, GetBackgroundValue(), true, 1);
    }

    // Search centroids
    std::vector<MaskSlicePointType> points2D;
    typedef std::vector<MaskSlicePointType> CentroidsType;
    std::map<std::string, CentroidsType> MapOfCentroids;
    for (MapIter it = MapOfStructures.begin(); it != MapOfStructures.end(); ++it) {
      std::string structure = it->first;
      MaskSlicePointer & s = MapOfSlices[structure][i];
      CentroidsType c;
      clitk::ComputeCentroids<MaskSliceType>(s, GetBackgroundValue(), c);
      MapOfCentroids[structure] = c;
    }


    // BrachioCephalicVein -> when it is separated into two CCL, we
    // only consider the most at Right one
    CentroidsType & c = MapOfCentroids["BrachioCephalicVein"];
    if (c.size() > 2) {
      if (c[1][0] < c[2][0]) c.erase(c.begin()+2);
      else c.erase(c.begin()+1);
    }
    
    /*
    // BrachioCephalicVein -> when SubclavianArtery has 2 CCL
    // (BrachioCephalicArtery is divided) -> forget BrachioCephalicVein
    if ((MapOfCentroids["BrachioCephalicArtery"].size() ==1) 
        && (MapOfCentroids["SubclavianArtery"].size() > 2)) {
      MapOfCentroids["BrachioCephalicVein"].clear();
    }
    */
    
    // Add all 2D points
    for (MapIter it = MapOfStructures.begin(); it != MapOfStructures.end(); ++it) {
      std::string structure = it->first;
      if (structure != "Trachea") { // do not add centroid of trachea
        CentroidsType & c = MapOfCentroids[structure];
        for(unsigned int j=1; j<c.size(); j++) points2D.push_back(c[j]);
      }
    }
    
    // Sort by angle according to trachea centroid and vertical line,
    // in polar coordinates :
    // http://en.wikipedia.org/wiki/Polar_coordinate_system
    //    std::vector<MaskSlicePointType> centroids_trachea;
    //ComputeCentroids<MaskSliceType>(MapOfSlices["Trachea"][i], GetBackgroundValue(), centroids_trachea);
    CentroidsType centroids_trachea = MapOfCentroids["Trachea"];
    typedef std::pair<MaskSlicePointType, double> PointAngleType;
    std::vector<PointAngleType> angles;
    for(unsigned int j=0; j<points2D.size(); j++) {
      //double r = centroids_trachea[1].EuclideanDistanceTo(points2D[j]);
      double x = (points2D[j][0]-centroids_trachea[1][0]); // X : Right to Left
      double y = (centroids_trachea[1][1]-points2D[j][1]); // Y : Post to Ant
      double angle = 0;
      if (x>0) angle = atan(y/x);
      if ((x<0) && (y>=0)) angle = atan(y/x)+M_PI;
      if ((x<0) && (y<0)) angle = atan(y/x)-M_PI;
      if (x==0) {
        if (y>0) angle = M_PI/2.0;
        if (y<0) angle = -M_PI/2.0;
        if (y==0) angle = 0;
      }
      angle = clitk::rad2deg(angle);
      // Angle is [-180;180] wrt the X axis. We change the X axis to
      // be the vertical line, because we want to sort from Right to
      // Left from Post to Ant.
      if (angle>0) 
        angle = (270-angle);
      if (angle<0) {
        angle = -angle-90;
        if (angle<0) angle = 360-angle;
      }
      PointAngleType a;
      a.first = points2D[j];
      a.second = angle;
      angles.push_back(a);
    }

    // Do nothing if less than 2 points --> n
    if (points2D.size() < 3) { //continue;
      continue;
    }

    // Sort points2D according to polar angles
    std::sort(angles.begin(), angles.end(), comparePointsWithAngle<PointAngleType>());
    for(unsigned int j=0; j<angles.size(); j++) {
      points2D[j] = angles[j].first;
    }
    //    DDV(points2D, points2D.size());

    /* When vessels are far away, we try to replace the line segment
       with a curved line that join the two vessels but stay
       approximately at the same distance from the trachea centroids
       than the vessels.

       For that: 
       - let a and c be two successive vessels centroids
       - id distance(a,c) < threshold, next point

       TODO HERE
       
       - compute mid position between two successive points -
       compute dist to trachea centroid for the 3 pts - if middle too
       low, add one point
    */
    std::vector<MaskSlicePointType> toadd;
    unsigned int index = 0;
    double dmax = 5;
    while (index<points2D.size()-1) {
      MaskSlicePointType a = points2D[index];
      MaskSlicePointType c = points2D[index+1];

      double dac = a.EuclideanDistanceTo(c);
      if (dac>dmax) {
        
        MaskSlicePointType b;
        b[0] = a[0]+(c[0]-a[0])/2.0;
        b[1] = a[1]+(c[1]-a[1])/2.0;
        
        // Compute distance to trachea centroid
        MaskSlicePointType m = centroids_trachea[1];
        double da = m.EuclideanDistanceTo(a);
        double db = m.EuclideanDistanceTo(b);
        //double dc = m.EuclideanDistanceTo(c);
        
        // Mean distance, find point on the line from trachea centroid
        // to b
        double alpha = (da+db)/2.0;
        MaskSlicePointType v;
        double n = sqrt( pow(b[0]-m[0], 2) + pow(b[1]-m[1], 2));
        v[0] = (b[0]-m[0])/n;
        v[1] = (b[1]-m[1])/n;
        MaskSlicePointType r;
        r[0] = m[0]+alpha*v[0];
        r[1] = m[1]+alpha*v[1];
        points2D.insert(points2D.begin()+index+1, r);
      }
      else {
        index++;
      }
    }
    //    DDV(points2D, points2D.size());

    // Add some points to close the contour 
    // - H line towards Right
    MaskSlicePointType p = points2D[0]; 
    p[0] = bounds[0][0];
    points2D.insert(points2D.begin(), p);
    // - corner Right/Post
    p = bounds[0];
    points2D.insert(points2D.begin(), p);
    // - H line towards Left
    p = points2D.back(); 
    p[0] = bounds[2][0];
    points2D.push_back(p);
    // - corner Right/Post
    p = bounds[2];
    points2D.push_back(p);
    // Close contour with the first point
    points2D.push_back(points2D[0]);
    //    DDV(points2D, points2D.size());
      
    // Build 3D points from the 2D points
    std::vector<ImagePointType> points3D;
    clitk::PointsUtils<MaskImageType>::Convert2DListTo3DList(points2D, i, support, points3D);
    for(unsigned int x=0; x<points3D.size(); x++) p3D.push_back(points3D[x]);

    // Build the mesh from the contour's points
    vtkSmartPointer<vtkPolyData> mesh = Build3DMeshFrom2DContour(points3D);
    append->AddInput(mesh);
    // if (i ==n-1) { // last slice
    //   clitk::PointsUtils<MaskImageType>::Convert2DListTo3DList(points2D, i+1, support, points3D);
    //   vtkSmartPointer<vtkPolyData> mesh = Build3DMeshFrom2DContour(points3D);
    //   append->AddInput(mesh);
    // }
  }

  // DEBUG: write points3D
  clitk::WriteListOfLandmarks<MaskImageType>(p3D, "vessels-centroids.txt");

  // Build the final 3D mesh form the list 2D mesh
  append->Update();
  vtkSmartPointer<vtkPolyData> mesh = append->GetOutput();
  
  // Debug, write the mesh
  /*
  vtkSmartPointer<vtkPolyDataWriter> w = vtkSmartPointer<vtkPolyDataWriter>::New();
  w->SetInput(mesh);
  w->SetFileName("bidon.vtk");
  w->Write();    
  */

  // Compute a single binary 3D image from the list of contours
  clitk::MeshToBinaryImageFilter<MaskImageType>::Pointer filter = 
    clitk::MeshToBinaryImageFilter<MaskImageType>::New();
  filter->SetMesh(mesh);
  filter->SetLikeImage(support);
  filter->Update();
  binarizedContour = filter->GetOutput();  

  // Crop
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(binarizedContour, 
                                                          GetForegroundValue(), 2, true, p);
  inf = p[2];
  clitk::FindExtremaPointInAGivenDirection<MaskImageType>(binarizedContour, 
                                                          GetForegroundValue(), 2, false, p);
  sup = p[2]+binarizedContour->GetSpacing()[2]; // extend to include the last slice
  binarizedContour = clitk::CropImageAlongOneAxis<MaskImageType>(binarizedContour, 2, inf, sup, 
                                                        false, GetBackgroundValue());

  // Update the AFDB
  writeImage<MaskImageType>(binarizedContour, "seg/AntPostVesselsSeparation.mha");
  this->GetAFDB()->SetImageFilename("AntPostVesselsSeparation", "seg/AntPostVesselsSeparation.mha");
  this->WriteAFDB();
  return binarizedContour;

  /*
  // Inverse binary mask if needed. We test a point that we know must be in FG. If it is not, inverse
  ImagePointType p = p3D[2]; // This is the first centroid of the first slice
  p[1] += 50; // 50 mm Post from this point must be kept
  ImageIndexType index;
  binarizedContour->TransformPhysicalPointToIndex(p, index);
  bool isInside = (binarizedContour->GetPixel(index) != GetBackgroundValue());

  // remove from support
  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BoolFilterType;
  typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
  boolFilter->InPlaceOn();
  boolFilter->SetInput1(m_Working_Support);
  boolFilter->SetInput2(binarizedContour);
  boolFilter->SetBackgroundValue1(GetBackgroundValue());
  boolFilter->SetBackgroundValue2(GetBackgroundValue());
  if (isInside)
    boolFilter->SetOperationType(BoolFilterType::And);
  else
    boolFilter->SetOperationType(BoolFilterType::AndNot);
  boolFilter->Update();
  m_Working_Support = boolFilter->GetOutput();
  */

  // End
  //StopCurrentStep<MaskImageType>(m_Working_Support);
  //m_ListOfStations["2R"] = m_Working_Support;
  //m_ListOfStations["2L"] = m_Working_Support;
}
//--------------------------------------------------------------------



#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
