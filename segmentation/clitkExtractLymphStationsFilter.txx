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

  // Station 7
  SetFuzzyThreshold(0.5);
  SetStation7Filename("station7.mhd");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
template <class ArgsInfoType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
SetArgsInfo(ArgsInfoType & argsinfo) {
  DD("SetArgsInfo");
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
  m_Support = GetAFDB()->template GetImage <MaskImageType>("mediastinum");
  
  //
  typedef clitk::BooleanOperatorLabelImageFilter<MaskImageType> BFilter;
  BFilter::Pointer merge = BFilter::New();  

  // Extract Station7
  ExtractStation_7();
  m_Output = m_Station7;

  // Extract Station4RL
  ExtractStation_4RL();

  writeImage<MaskImageType>(m_Station4RL, "s4rl.mhd");
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
  this->GraftOutput(m_Output);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_7() {
  DD("ExtractStation_7");
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
  DD("ExtractStation_4RL");
  writeImage<MaskImageType>(m_Support, "essai.mhd"); // OK

  /*
    WARNING ONLY 4R FIRST !!! (not same inf limits)
   */
    
  ExtractStation_4RL_SI_Limits();
  ExtractStation_4RL_LR_Limits();

}
//--------------------------------------------------------------------



#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
