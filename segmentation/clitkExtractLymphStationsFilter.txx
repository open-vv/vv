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

  // Station 8
  SetDistanceMaxToAnteriorPartOfTheSpine(10);
  MaskImagePointType p;
  p[0] = 15; p[1] = 2; p[2] = 1;
  SetEsophagusDiltationForAnt(p);
  p[0] = 5; p[1] = 10; p[2] = 1;
  SetEsophagusDiltationForRight(p);
  SetFuzzyThresholdForS8(0.5);

  // Station 7
  SetFuzzyThreshold(0.5);
  SetStation7Filename("station7.mhd");
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

  // Extract Station8
  StartNewStep("Station 8");
  StartSubStep(); 
  ExtractStation_8();
  StopSubStep();

  // Compute some interesting points in trachea
  // ( ALTERNATIVE -> SKELETON ANALYSIS ? 
  //    Pb : not sufficient for mostXX points ... ) 

  /* ==> todo (but why ???)
     ComputeTracheaCentroidsAboveCarina();
     ComputeBronchusExtremaPointsBelowCarina();
  */

  if (0) { // temporary suppress
    // Extract Station7
    StartNewStep("Station 7");
    StartSubStep();
    ExtractStation_7();
    StopSubStep();

    // Extract Station4RL
    StartNewStep("Station 4RL");
    StartSubStep();
    //ExtractStation_4RL();
    StopSubStep();
  }


  //
  //  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BFilter;
  //BFilter::Pointer merge = BFilter::New();  
  // writeImage<MaskImageType>(m_Output, "ouput.mhd");
  //writeImage<MaskImageType>(m_Working_Support, "ws.mhd");
  /*merge->SetInput1(m_Station7);
    merge->SetInput2(m_Station4RL); // support
    merge->SetOperationType(BFilter::AndNot); CHANGE OPERATOR
    merge->SetForegroundValue(4);
    merge->Update();
    m_Output = merge->GetOutput();
  */
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateInputRequestedRegion() {
  DD("GenerateInputRequestedRegion (nothing?)");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
GenerateData() {
  DD("GenerateData, graft output");

  // Final Step -> graft output (if SetNthOutput => redo)
  this->GraftOutput(m_ListOfStations["8"]);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_8() {

  // Check if m_ListOfStations["8"] exist. If yes -> use it as initial
  // support instead of m_Mediastinum
  if (m_ListOfStations["8"]) {
    DD("Station 8 support already exist -> use it");
    m_Working_Support = m_ListOfStations["8"];
  }
  else m_Working_Support = m_Mediastinum;

  ExtractStation_8_SI_Limits();
  ExtractStation_8_Post_Limits();
  ExtractStation_8_Ant_Limits();
  ExtractStation_8_LR_Limits();
  // ExtractStation_8_LR_Limits();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_7() {
  if (m_ListOfStations["7"]) {
    DD("Station 7 support already exist -> use it");
    m_Working_Support = m_ListOfStations["7"];
  }
  else m_Working_Support = m_Mediastinum;
  ExtractStation_7_SI_Limits();
  ExtractStation_7_RL_Limits();
  ExtractStation_7_Posterior_Limits();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_4RL() {
  /*
    WARNING ONLY 4R FIRST !!! (not same inf limits)
  */    
  ExtractStation_4RL_SI_Limits();
  ExtractStation_4RL_LR_Limits();
  ExtractStation_4RL_AP_Limits();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
FindExtremaPointsInBronchus(MaskImagePointer input, 
			    int direction,
			    double distance_max_from_center_point, 
			    ListOfPointsType & LR, 
			    ListOfPointsType & Ant, 
			    ListOfPointsType & Post)
{

  // Other solution ==> with auto bounding box ! (but pb to prevent to
  // be too distant from the center point

  // Extract slices
  std::vector<typename MaskSliceType::Pointer> slices;
  clitk::ExtractSlices<MaskImageType>(input, 2, slices);
  
  // Loop on slices
  bool found;
  for(uint i=0; i<slices.size(); i++) {
    /*
    // Keep main CCL
    slices[i] = Labelize<MaskSliceType>(slices[i], 0, true, 10);
    slices[i] = KeepLabels<MaskSliceType>(slices[i], 
					  GetBackgroundValue(), 
					  GetForegroundValue(), 1, 1, true);
    */

    // ------- Find rightmost or leftmost point  ------- 
    MaskSliceType::PointType LRMost;
    found = 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], 
                                                              GetBackgroundValue(), 
                                                              0, // axis XY
                                                              (direction==0?false:true),  // right or left according to direction
                                                              LRMost);
    // ------- Find postmost point  ------- 
    MaskSliceType::PointType postMost;
    found = 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], 
                                                              GetBackgroundValue(), 
                                                              1, false, LRMost, 
                                                              distance_max_from_center_point, 
                                                              postMost);
    // ------- Find antmost point  ------- 
    MaskSliceType::PointType antMost;
    found = 
      clitk::FindExtremaPointInAGivenDirection<MaskSliceType>(slices[i], 
                                                              GetBackgroundValue(), 
                                                              1, true, LRMost, 
                                                              distance_max_from_center_point, 
                                                              antMost);
    // Only add point if found
    if (found)  {
      // ------- Convert 2D to 3D points --------
      MaskImageType::PointType p;
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(LRMost, input, i, p);
      LR.push_back(p); 
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(antMost, input, i, p);
      Ant.push_back(p);
      clitk::PointsUtils<MaskImageType>::Convert2DTo3D(postMost, input, i, p);
      Post.push_back(p);
    }
  }
} 
//--------------------------------------------------------------------

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
