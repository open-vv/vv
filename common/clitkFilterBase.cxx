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

// clitk
#include "clitkFilterBase.h"
#include "clitkCommon.h"

//--------------------------------------------------------------------
clitk::FilterBase::FilterBase() 
{
  SetVerboseOption(false);
  SetCurrentStepNumber(0);
  SetCurrentStepBaseId("");
  VerboseWarningOffOn(); // OffOn, it's cool no ?
  SetWarning("");
  m_IsCancelled = false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::FilterBase::SetWarning(std::string e)
{
  m_Warning = e;
  if (!GetVerboseWarningOff()) {
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
  if (m_VerboseStep) {
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

