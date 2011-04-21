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
  SetFuzzyThreshold(0.5);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateOutputInformation() { 
  //  Superclass::GenerateOutputInformation();
  
  // Get input
  LoadAFDB();
  m_mediastinum = GetAFDB()->template GetImage <MaskImageType>("mediastinum");  
  m_trachea = GetAFDB()->template GetImage <MaskImageType>("trachea");  
  ImagePointType carina;
  GetAFDB()->GetPoint3D("carina", carina);
  m_CarinaZPositionInMM = carina[2];
  DD(m_CarinaZPositionInMM);
  ImagePointType mlb;
  GetAFDB()->GetPoint3D("middleLobeBronchus", mlb);
  m_MiddleLobeBronchusZPositionInMM = mlb[2];
  DD(m_MiddleLobeBronchusZPositionInMM);

  // ----------------------------------------------------------------
  // ----------------------------------------------------------------
  // Superior limit = carina
  // Inferior limit = origin right middle lobe bronchus
  StartNewStep("Inf/Sup mediastinum limits with carina/bronchus");
  m_working_mediastinum = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_mediastinum, 2, 
                                                m_MiddleLobeBronchusZPositionInMM, 
                                                m_CarinaZPositionInMM, true,
                                                GetBackgroundValue());
  StopCurrentStep<MaskImageType>(m_working_mediastinum);
  m_output = m_working_mediastinum;

  // ----------------------------------------------------------------
  // ----------------------------------------------------------------
  // Superior limit = carina
  // Inferior limit = origin right middle lobe bronchus
  StartNewStep("Inf/Sup trachea limits with carina/bronchus");
  m_working_trachea = 
    clitk::CropImageAlongOneAxis<MaskImageType>(m_trachea, 2, 
                                                m_MiddleLobeBronchusZPositionInMM, 
                                                m_CarinaZPositionInMM, true,
                                                GetBackgroundValue());
  StopCurrentStep<MaskImageType>(m_working_trachea);

  // ----------------------------------------------------------------
  // ----------------------------------------------------------------
  // Separate trachea in two CCL
  StartNewStep("Separate trachea under carina");

  // Labelize and consider two main labels
  m_working_trachea = Labelize<MaskImageType>(m_working_trachea, 0, true, 1);

  // Carina position must at the first slice that separate the two main bronchus (not superiorly) 
  // Check that upper slice is composed of at least two labels
  typedef itk::ImageSliceIteratorWithIndex<MaskImageType> SliceIteratorType;
  SliceIteratorType iter(m_working_trachea, m_working_trachea->GetLargestPossibleRegion());
  iter.SetFirstDirection(0);
  iter.SetSecondDirection(1);
  iter.GoToReverseBegin(); // Start from the end (because image is IS not SI)
  int maxLabel=0;
  while (!iter.IsAtReverseEndOfSlice()) {
    while (!iter.IsAtReverseEndOfLine()) {    
      //  DD(iter.GetIndex());
      if (iter.Get() > maxLabel) maxLabel = iter.Get();
      --iter;
    }
    iter.PreviousLine();
  }
  if (maxLabel < 2) {
    clitkExceptionMacro("First slice form Carina does not seems to seperate the two main bronchus. Abort");
  }

  // Compute centroid of both parts to identify the left from the right bronchus
  typedef long LabelType;
  static const unsigned int Dim = ImageType::ImageDimension;
  typedef itk::ShapeLabelObject< LabelType, Dim > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;
  typedef itk::LabelImageToLabelMapFilter<MaskImageType, LabelMapType> ImageToMapFilterType;
  typename ImageToMapFilterType::Pointer imageToLabelFilter = ImageToMapFilterType::New(); 
  typedef itk::ShapeLabelMapFilter<LabelMapType, MaskImageType> ShapeFilterType; 
  typename ShapeFilterType::Pointer statFilter = ShapeFilterType::New();
  imageToLabelFilter->SetBackgroundValue(GetBackgroundValue());
  imageToLabelFilter->SetInput(m_working_trachea);
  statFilter->SetInput(imageToLabelFilter->GetOutput());
  statFilter->Update();
  typename LabelMapType::Pointer labelMap = statFilter->GetOutput();

  ImagePointType C1 = labelMap->GetLabelObject(1)->GetCentroid();
  ImagePointType C2 = labelMap->GetLabelObject(2)->GetCentroid();

  ImagePixelType leftLabel;
  ImagePixelType rightLabel;  
  if (C1[0] < C2[0]) { leftLabel = 1; rightLabel = 2; }
  else { leftLabel = 2; rightLabel = 1; }
  DD(leftLabel);
  DD(rightLabel);

  StopCurrentStep<MaskImageType>(m_working_trachea);

  //-----------------------------------------------------
  StartNewStep("Left limits with bronchus (slice by slice)");  
  // Select LeftLabel (set one label to Backgroundvalue)
  MaskImagePointer leftBronchus = 
    SetBackground<MaskImageType, MaskImageType>(m_working_trachea, m_working_trachea, 
                                                rightLabel, GetBackgroundValue());
  MaskImagePointer rightBronchus  = 
    SetBackground<MaskImageType, MaskImageType>(m_working_trachea, m_working_trachea, 
                                                leftLabel, GetBackgroundValue());
  writeImage<MaskImageType>(leftBronchus, "left.mhd");
  writeImage<MaskImageType>(rightBronchus, "right.mhd");

  m_working_mediastinum = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_working_mediastinum, 
						       leftBronchus, 2, 
                                                       GetFuzzyThreshold(), "RightTo", 
                                                       true, 4);
  m_working_mediastinum = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_working_mediastinum, 
						       rightBronchus, 
						       2, GetFuzzyThreshold(), "LeftTo", 
                                                       true, 4);
  m_working_mediastinum = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_working_mediastinum, 
						       leftBronchus, 
						       2, GetFuzzyThreshold(), "AntTo", 
                                                       true, 4, true); // NOT
  m_working_mediastinum = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_working_mediastinum, 
						       rightBronchus, 
						       2, GetFuzzyThreshold(), "AntTo", 
                                                       true, 4, true);
  m_working_mediastinum = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_working_mediastinum, 
						       leftBronchus, 
						       2, GetFuzzyThreshold(), "PostTo", 
                                                       true, 4, true);
  m_working_mediastinum = 
    clitk::SliceBySliceRelativePosition<MaskImageType>(m_working_mediastinum, 
						       rightBronchus, 
						       2, GetFuzzyThreshold(), "PostTo", 
                                                       true, 4, true);
  m_output = m_working_mediastinum;
  StopCurrentStep<MaskImageType>(m_output);

  //-----------------------------------------------------
  //-----------------------------------------------------
  StartNewStep("Posterior limits");  

  // Left Bronchus slices
  typedef clitk::ExtractSliceFilter<MaskImageType> ExtractSliceFilterType;
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  typename ExtractSliceFilterType::Pointer 
    extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(leftBronchus);
  extractSliceFilter->SetDirection(2);
  extractSliceFilter->Update();
  std::vector<typename SliceType::Pointer> leftBronchusSlices;
  extractSliceFilter->GetOutputSlices(leftBronchusSlices);
  
  // Right Bronchus slices
  extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(rightBronchus);
  extractSliceFilter->SetDirection(2);
  extractSliceFilter->Update();
  std::vector<typename SliceType::Pointer> rightBronchusSlices ;
  extractSliceFilter->GetOutputSlices(rightBronchusSlices);
  
  assert(leftBronchusSlices.size() == rightBronchusSlices.size());
  
  std::vector<MaskImageType::PointType> leftPoints;
  std::vector<MaskImageType::PointType> rightPoints;
  for(uint i=0; i<leftBronchusSlices.size(); i++) {
    // Keep main CCL
    leftBronchusSlices[i] = Labelize<SliceType>(leftBronchusSlices[i], 0, true, 10);
    leftBronchusSlices[i] = KeepLabels<SliceType>(leftBronchusSlices[i], 
                                                  GetBackgroundValue(), 
                                                  GetForegroundValue(), 1, 1, true);
    rightBronchusSlices[i] = Labelize<SliceType>(rightBronchusSlices[i], 0, true, 10);
    rightBronchusSlices[i] = KeepLabels<SliceType>(rightBronchusSlices[i], 
                                                   GetBackgroundValue(), 
                                                   GetForegroundValue(), 1, 1, true);
    double distance_max_from_center_point = 15;

    // ------- Find point in left Bronchus ------- 
    // find right most point in left  = rightMost
    SliceType::PointType a;
    SliceType::PointType rightMost = 
      clitk::FindExtremaPointInAGivenDirection<SliceType>(leftBronchusSlices[i], 
                                                          GetBackgroundValue(), 
                                                          0, false, a, 0);
    // find post most point in left, not far away from rightMost
    SliceType::PointType p = 
      clitk::FindExtremaPointInAGivenDirection<SliceType>(leftBronchusSlices[i], 
                                                          GetBackgroundValue(), 
                                                          1, false, rightMost, 
                                                          distance_max_from_center_point);
    MaskImageType::PointType pp;
    pp[0] = p[0]; pp[1] = p[1];
    pp[2] = i*leftBronchus->GetSpacing()[2] + leftBronchus->GetOrigin()[2];
    leftPoints.push_back(pp);

    // ------- Find point in right Bronchus ------- 
    // find left most point in right  = leftMost
    SliceType::PointType leftMost = 
      clitk::FindExtremaPointInAGivenDirection<SliceType>(rightBronchusSlices[i], 
                                                          GetBackgroundValue(), 
                                                          0, true, a, 0);
    // find post most point in left, not far away from leftMost
    p = clitk::FindExtremaPointInAGivenDirection<SliceType>(rightBronchusSlices[i], 
                                                            GetBackgroundValue(), 
                                                            1, false, leftMost, 
                                                            distance_max_from_center_point);
    pp[0] = p[0]; pp[1] = p[1];
    pp[2] = i*rightBronchus->GetSpacing()[2] + rightBronchus->GetOrigin()[2];
    rightPoints.push_back(pp);
  }

  // DEBUG
  std::ofstream osl;
  openFileForWriting(osl, "left.txt");
  osl << "LANDMARKS1" << std::endl;
  std::ofstream osr;
  openFileForWriting(osr, "right.txt");
  osr << "LANDMARKS1" << std::endl;
  for(uint i=0; i<leftBronchusSlices.size(); i++) {
    osl << i << " " << leftPoints[i][0] << " " << leftPoints[i][1] 
        << " " << leftPoints[i][2] << " 0 0 " << std::endl;
    osr << i << " " << rightPoints[i][0] << " " << rightPoints[i][1] 
        << " " << rightPoints[i][2] << " 0 0 " << std::endl;
  }
  osl.close();
  osr.close();

  // Now uses these points to limit, slice by slice 
  // http://www.gamedev.net/community/forums/topic.asp?topic_id=542870
  /*
    Assuming the points are (Ax,Ay) (Bx,By) and (Cx,Cy), you need to compute:
    (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax)
    This will equal zero if the point C is on the line formed by points A and B, and will have a different sign depending on the side. Which side this is depends on the orientation of your (x,y) coordinates, but you can plug test values for A,B and C into this formula to determine whether negative values are to the left or to the right.

    => to accelerate, start with formula, when change sign -> stop and fill
  */
  //  typedef itk::ImageSliceIteratorWithIndex<MaskImageType> SliceIteratorType;
  iter = SliceIteratorType(m_working_mediastinum, m_working_mediastinum->GetLargestPossibleRegion());
  iter.SetFirstDirection(0);
  iter.SetSecondDirection(1);
  iter.GoToBegin();
  int i=0;
  MaskImageType::PointType A;
  MaskImageType::PointType B;
  MaskImageType::PointType C;
  while (!iter.IsAtEnd()) {
    A = leftPoints[i];
    B = rightPoints[i];
    C = A;
    C[1] -= 10; // I know I must keep this point
    double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
    bool isPositive = s<0;
    while (!iter.IsAtEndOfSlice()) {
      while (!iter.IsAtEndOfLine()) {
        // Very slow, I know ... but image should be very small
        m_working_mediastinum->TransformIndexToPhysicalPoint(iter.GetIndex(), C);
        double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
        if (s == 0) iter.Set(2);
        if (isPositive) {
          if (s > 0) iter.Set(GetBackgroundValue());
        }
        else {
          if (s < 0) iter.Set(GetBackgroundValue());
        }
        ++iter;
      }
      iter.NextLine();
    }
    iter.NextSlice();
    ++i;
  }

  //-----------------------------------------------------
  //-----------------------------------------------------
  // StartNewStep("Anterior limits");  
 

  // MISSING FROM NOW 
  
  // Station 4R, Station 4L, the right pulmonary artery, and/or the
  // left superior pulmonary vein


  //-----------------------------------------------------
  //-----------------------------------------------------
  // ALSO SUBSTRACT ARTERY/VEIN (initially in the support)

  
  // Set output image information (required)
  MaskImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(m_working_mediastinum->GetLargestPossibleRegion());
  outputImage->SetOrigin(m_working_mediastinum->GetOrigin());
  outputImage->SetRequestedRegion(m_working_mediastinum->GetLargestPossibleRegion());

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateInputRequestedRegion() {
  DD("GenerateInputRequestedRegion");
  // Call default
  //  Superclass::GenerateInputRequestedRegion();
  // Following needed because output region can be greater than input (trachea)
  //ImagePointer mediastinum = dynamic_cast<TImageType*>(itk::ProcessObject::GetInput(0));
  //ImagePointer trachea = dynamic_cast<TImageType*>(itk::ProcessObject::GetInput(1));
  DD("set reg");
  m_mediastinum->SetRequestedRegion(m_mediastinum->GetLargestPossibleRegion());
  m_trachea->SetRequestedRegion(m_trachea->GetLargestPossibleRegion());
  DD("end");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateData() {
  DD("GenerateData");
  // Final Step -> graft output (if SetNthOutput => redo)
  this->GraftOutput(m_output);
}
//--------------------------------------------------------------------
  

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
