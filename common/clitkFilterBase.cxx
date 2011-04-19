/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

// clitk
#include "clitkFilterBase.h"
#include "clitkCommon.h"

//--------------------------------------------------------------------
clitk::FilterBase::FilterBase() 
{
  SetVerboseOptionFlag(false);
  SetCurrentStepNumber(0);
  SetCurrentStepBaseId("");
  VerboseWarningFlagOn();
  VerboseWarningFlagOff();
  VerboseMemoryFlagOff();
  SetWarning("");
  VerboseWarningFlagOn();
  m_IsCancelled = false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterBase::SetWarning(std::string e)
{
  m_Warning = e;
  if (GetVerboseWarningFlag()) {
    std::cout << GetWarning() << std::endl;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterBase::StartNewStep(std::string s) 
{
  if (Cancelled()) {
    throw clitk::ExceptionObject("Filter is canceled.");
  }

  m_CurrentStepNumber++;
  if (GetCurrentStepBaseId() != "") {
    std::ostringstream oss;
    oss << GetCurrentStepBaseId() << "." << m_CurrentStepNumber;
    SetCurrentStepId(oss.str());
  }
  else {
    std::ostringstream oss;
    oss << m_CurrentStepNumber;
    SetCurrentStepId(oss.str());
  }

  m_CurrentStepName = "Step "+GetCurrentStepId()+" -- "+s;
  if (GetVerboseStepFlag()) {
    std::cout << m_CurrentStepName << std::endl;
    //"Step " << GetCurrentStepId() << " -- " << s << std::endl;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterBase::StopCurrentStep() 
{
  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterBase::Cancel()
{
  m_IsCancelled = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool clitk::FilterBase::Cancelled()
{
  return m_IsCancelled;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterBase::StartSubStep() {
  m_SubstepNumbers.push_back(GetCurrentStepNumber());
  m_SubstepID.push_back(GetCurrentStepId());
  SetCurrentStepBaseId(GetCurrentStepId());
  SetCurrentStepNumber(0);;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterBase::StopSubStep() {
  int s = m_SubstepNumbers.back();
  m_SubstepNumbers.pop_back();
  SetCurrentStepNumber(s);
  m_SubstepID.pop_back();
  if (m_SubstepID.size() != 0) {
    SetCurrentStepBaseId(m_SubstepID.back());  }
  else SetCurrentStepBaseId("");
}
