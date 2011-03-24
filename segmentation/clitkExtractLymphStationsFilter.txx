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
ExtractStation_8() 
{
  if (CheckForStation("8")) {
    ExtractStation_8_SI_Limits();
    ExtractStation_8_Post_Limits();
    ExtractStation_8_Ant_Sup_Limits();
    ExtractStation_8_Ant_Inf_Limits();
    ExtractStation_8_LR_1_Limits();
    ExtractStation_8_LR_2_Limits();
    ExtractStation_8_LR_Limits();
    ExtractStation_8_Ant_Injected_Limits();
    ExtractStation_8_Single_CCL_Limits();
    ExtractStation_8_Remove_Structures();
    // Store image filenames into AFDB 
    writeImage<MaskImageType>(m_ListOfStations["8"], "seg/Station8.mhd");
    GetAFDB()->SetImageFilename("Station8", "seg/Station8.mhd");  
    WriteAFDB();
  }
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
ExtractStation_7() {
  if (CheckForStation("7")) {
  ExtractStation_7_SI_Limits();
  ExtractStation_7_RL_Limits();
  ExtractStation_7_Posterior_Limits();
  //  ExtractStation_8_Single_CCL_Limits(); // Yes the same than 8
  ExtractStation_7_Remove_Structures();
  // Store image filenames into AFDB 
  writeImage<MaskImageType>(m_ListOfStations["7"], "seg/Station7.mhd");
  GetAFDB()->SetImageFilename("Station7", "seg/Station7.mhd");  
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
Remove_Structures(std::string s)
{
  try {
    StartNewStep("[Station7] Remove "+s);  
    MaskImagePointer Structure = GetAFDB()->template GetImage<MaskImageType>(s);
    clitk::AndNot<MaskImageType>(m_Working_Support, Structure, GetBackgroundValue());
  }
  catch(clitk::ExceptionObject e) {
    std::cout << s << " not found, skip." << std::endl;
  }
}
//--------------------------------------------------------------------



#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
