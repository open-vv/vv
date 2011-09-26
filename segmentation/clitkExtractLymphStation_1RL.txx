

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
ExtractStation_1RL_SetDefaultValues()
{
  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_1RL()
{
  if ((!CheckForStation("1R")) && (!CheckForStation("1L"))) return;

  StartNewStep("Stations 1RL");
  StartSubStep(); 

  // Get the current support 
  StartNewStep("[Station 1RL] Get the current 1RL suppport");
  m_ListOfStations["1R"] = m_ListOfSupports["S1R"];
  m_ListOfStations["1L"] = m_ListOfSupports["S1L"];
  StopCurrentStep<MaskImageType>(m_ListOfStations["1R"]);
    
  // Specific processes
  ExtractStation_1RL_Ant_Limits();
  ExtractStation_1RL_Post_Limits();
  m_Working_Support = m_ListOfStations["1R"];
  Remove_Structures(" 1R", "ScaleneMuscleAnt");
  Remove_Structures(" 1R", "CommonCarotidArteryRight");  
  m_Working_Support = m_ListOfStations["1L"];
  Remove_Structures(" 1L", "ScaleneMuscleAnt");
  Remove_Structures(" 1L", "CommonCarotidArteryLeft");  
 
  // Generic RelativePosition processes
  m_ListOfStations["1R"] = this->ApplyRelativePositionList("Station_1R", m_ListOfStations["1R"]);
  m_ListOfStations["1L"] = this->ApplyRelativePositionList("Station_1L", m_ListOfStations["1L"]);

  // Store image filenames into AFDB 
  writeImage<MaskImageType>(m_ListOfStations["1R"], "seg/Station1R.mhd");
  writeImage<MaskImageType>(m_ListOfStations["1L"], "seg/Station1L.mhd");
  GetAFDB()->SetImageFilename("Station1R", "seg/Station1R.mhd"); 
  GetAFDB()->SetImageFilename("Station1L", "seg/Station1L.mhd"); 
  WriteAFDB(); 
  StopSubStep();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_1RL_Ant_Limits()
{
  // -----------------------------------------------------
  StartNewStep("[Station 1RL] anterior limits with Trachea and Thyroid");
  
  /*
    The idea here it to consider the most anterior points int the
    Trachea or the Thyroid and cut all parts anterior to it. This is
    an heuristic, not written explicitely in the articles.
  */

  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  MaskImagePointer Thyroid = GetAFDB()->template GetImage<MaskImageType>("Thyroid");
  MaskImagePointer S1R = m_ListOfStations["1R"];
  MaskImagePointer S1L = m_ListOfStations["1L"];
  MaskImagePointer support = m_ListOfSupports["S1RL"];
 
  // Resize like S1R. Warning: for Thyroid, only Right part is thus
  // taken into account
  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, S1R, GetBackgroundValue());
  Thyroid = clitk::ResizeImageLike<MaskImageType>(Thyroid, S1R, GetBackgroundValue());

  // Search for most Ant point, slice by slice, between Trachea and Thyroid
  std::vector<MaskSlicePointer> Trachea_slices;
  clitk::ExtractSlices<MaskImageType>(Trachea, 2, Trachea_slices);
  std::vector<MaskSlicePointer> Thyroid_slices;
  clitk::ExtractSlices<MaskImageType>(Thyroid, 2, Thyroid_slices);
  std::vector<typename ImageType::PointType> A;
  std::vector<typename ImageType::PointType> B;
  for(uint i=0; i<Trachea_slices.size(); i++) {
    MaskSlicePointType p;
    MaskSlicePointType q;
    FindExtremaPointInAGivenDirection<MaskSliceType>(Trachea_slices[i], 
                                                     GetBackgroundValue(), 
                                                     1, true, p);
    FindExtremaPointInAGivenDirection<MaskSliceType>(Thyroid_slices[i], 
                                                     GetBackgroundValue(), 
                                                     1, true, q);
    if (q[1] < p[1]) p = q; // Now p is the most ant.
    // Add a little margin, 3mm
    p[1] -= 3;
    // Convert in 3D
    ImagePointType x; //dummy
    A.push_back(x);
    B.push_back(x);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p, Trachea, i, A[i]);
    p[0] += 10;    
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p, Trachea, i, B[i]);
  }  

  // Remove anterior to this line (keep +10 offset)
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(S1R, A, B, 
                                                                    GetBackgroundValue(), 1, +10);
  m_ListOfStations["1R"] = S1R;

  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(S1L, A, B, 
                                                                    GetBackgroundValue(), 1, +10);
  StopCurrentStep<MaskImageType>(S1L);
  m_ListOfStations["1L"] = S1L;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_1RL_Post_Limits()
{
  // -----------------------------------------------------
  StartNewStep("[Station 1RL] Posterior limits with VertebralArtery");
  
  MaskImagePointer VertebralArtery = GetAFDB()->template GetImage<MaskImageType>("VertebralArtery");
  MaskImagePointer S1R = m_ListOfStations["1R"];
  MaskImagePointer S1L = m_ListOfStations["1L"];

  // Resize like S1R.
  VertebralArtery = clitk::ResizeImageLike<MaskImageType>(VertebralArtery, S1R, GetBackgroundValue());

  // Search for most Ant point, slice by slice, between Trachea and Thyroid
  std::vector<MaskSlicePointer> VertebralArtery_slices;
  clitk::ExtractSlices<MaskImageType>(VertebralArtery, 2, VertebralArtery_slices);
  std::vector<typename ImageType::PointType> A;
  std::vector<typename ImageType::PointType> B;
  for(uint i=0; i<VertebralArtery_slices.size(); i++) {
    MaskSlicePointType p;
    FindExtremaPointInAGivenDirection<MaskSliceType>(VertebralArtery_slices[i], 
                                                     GetBackgroundValue(), 
                                                     1, false, p);
    // Add a little margin ? No.
    //p[1] += 0;
    //DD(p);
    // Convert in 3D
    ImagePointType x; //dummy
    A.push_back(x);
    B.push_back(x);
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p, VertebralArtery, i, A[i]);
    p[0] += 10;    
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(p, VertebralArtery, i, B[i]);
  }  

  // Remove anterior to this line (keep -10 offset)
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(S1R, A, B, 
                                                                    GetBackgroundValue(), 1, -10);
  m_ListOfStations["1R"] = S1R;

  // Remove anterior to this line (keep -10 offset)
  clitk::SliceBySliceSetBackgroundFromLineSeparation<MaskImageType>(S1L, A, B, 
                                                                    GetBackgroundValue(), 1, -10);
  StopCurrentStep<MaskImageType>(S1L);
  m_ListOfStations["1L"] = S1L;
}
//--------------------------------------------------------------------


