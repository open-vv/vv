

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
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_2RL()
{
  if ((!CheckForStation("2R")) && (!CheckForStation("2L"))) return;

  StartNewStep("Stations 2RL");
  StartSubStep(); 

  // Get the current support 
  StartNewStep("[Station 2RL] Get the current 2RL suppport");
  m_ListOfStations["2R"] = m_ListOfSupports["S2R"];
  m_ListOfStations["2L"] = m_ListOfSupports["S2L"];
  StopCurrentStep<MaskImageType>(m_ListOfStations["2R"]);
    
  // Do the same limits for R & L
  m_Working_Support = m_ListOfStations["2R"];
  ExtractStation_2RL_Ant_Limits("2R"); 
  ExtractStation_2RL_Remove_Structures(" 2R");
  m_ListOfStations["2R"] = m_Working_Support;

  m_Working_Support = m_ListOfStations["2L"];
  ExtractStation_2RL_Ant_Limits("2L");
  ExtractStation_2RL_Remove_Structures(" 2L");
  m_ListOfStations["2L"] = m_Working_Support;

  // Remove superior part to BrachioCephalicVein (used then by RelPos)
  ExtractStation_2RL_Cut_BrachioCephalicVein_superiorly_when_it_split();

  // Generic RelativePosition processes
  m_ListOfStations["2R"] = this->ApplyRelativePositionList("Station_2R", m_ListOfStations["2R"]);
  m_ListOfStations["2L"] = this->ApplyRelativePositionList("Station_2L", m_ListOfStations["2L"]);
  
  // Store image filenames into AFDB 
  writeImage<MaskImageType>(m_ListOfStations["2R"], "seg/Station2R.mhd");
  writeImage<MaskImageType>(m_ListOfStations["2L"], "seg/Station2L.mhd");
  GetAFDB()->SetImageFilename("Station2R", "seg/Station2R.mhd"); 
  GetAFDB()->SetImageFilename("Station2L", "seg/Station2L.mhd"); 
  WriteAFDB(); 
  StopSubStep();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Ant_Limits(std::string s)
{
  // -----------------------------------------------------
  StartNewStep("[Station "+s+"] Ant limits with vessels centroids");
  
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
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_2RL_Cut_BrachioCephalicVein_superiorly_when_it_split() 
{
  // -----------------------------------------------------
  StartNewStep("[Station 2RL] Cut BrachioCephalicVein superiorly (when it split)");
  
  // Get BrachioCephalicVein
  MaskImagePointer BrachioCephalicVein = GetAFDB()->template GetImage<MaskImageType>("BrachioCephalicVein");
  
  // Remove the part superior to the slice where BrachioCephalicVein
  // divide in two CCL
  std::vector<MaskSlicePointer> BCV_slices;
  clitk::ExtractSlices<MaskImageType>(BrachioCephalicVein, 2, BCV_slices);
  bool stop = false;
  uint i=0;
  while (!stop) {
    // Count the number of CCL
    int nb;
    clitk::LabelizeAndCountNumberOfObjects<MaskSliceType>(BCV_slices[i], GetBackgroundValue(), true, 1, nb);
    if (nb>1) stop = true;
    i++; 
  }
  // Convert slice into coordinate
  MaskImagePointType p;
  MaskImageIndexType index;
  index[0] = index[1] = 0;
  index[2] = i;
  BrachioCephalicVein->TransformIndexToPhysicalPoint(index, p);
  BrachioCephalicVein = 
    clitk::CropImageRemoveGreaterThan<MaskImageType>(BrachioCephalicVein, 2, 
                                                     p[2], true,
                                                     GetBackgroundValue());

  // Now, insert this image in the AFDB (but do not store on disk)
  GetAFDB()->template SetImage<MaskImageType>("BrachioCephalicVein_ForS2RL", "bidon", 
                                              BrachioCephalicVein, false);
  // End
  StopCurrentStep<MaskImageType>(BrachioCephalicVein);
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
ExtractStation_2RL_Remove_Structures(std::string s)
{
  // m_Working_Support must be set
  Remove_Structures(s, "BrachioCephalicVein");
  Remove_Structures(s, "BrachioCephalicArtery");
  Remove_Structures(s, "LeftCommonCarotidArtery");
  Remove_Structures(s, "RightCommonCarotidArtery");
  Remove_Structures(s, "LeftSubclavianArtery");
  Remove_Structures(s, "RightSubclavianArtery");
  Remove_Structures(s, "Thyroid");
  Remove_Structures(s, "Aorta");
}
//--------------------------------------------------------------------


