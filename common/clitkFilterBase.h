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

#ifndef CLITKFILTERBASE_H
#define CLITKFILTERBASE_H

// clitk
#include "clitkCommon.h"
#include "clitkTimer.h"
#include "clitkFilterMacros.txx"
#include "clitkLabelizeParameters.h"

// itk
#include "itkObject.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Convenient class to manage options from GGO (gengetopt) to filter
  */
  //--------------------------------------------------------------------
  class FilterBase
  {

  public:
    // Standard class typedefs
    typedef FilterBase  Self;
    
    // Run-time type information (and related methods)
    itkTypeMacro(FilterBase, Object);

    // Needed by itkSetMacro (cannot inherit from itkObject because of
    // multiple inheritance)
    virtual void Modified() {} 
    virtual bool GetDebug() const { return false; }

    // To put in class that inherit from FilterBase
#define FILTERBASE_INIT                                                 \
    virtual void Modified() { Superclass::Modified(); }                 \
    virtual bool GetDebug() const { return Superclass::GetDebug(); }

    // Verbose options management
    itkSetMacro(VerboseOption, bool);
    itkGetConstMacro(VerboseOption, bool);
    itkBooleanMacro(VerboseOption);
    GGO_DefineOption_Flag(verboseOption, SetVerboseOption);

    // Steps management
    itkSetMacro(NumberOfSteps, int);
    itkGetConstMacro(NumberOfSteps, int);
    itkSetMacro(VerboseStep, bool);
    itkGetConstMacro(VerboseStep, bool);
    itkBooleanMacro(VerboseStep);
    GGO_DefineOption_Flag(verboseStep, SetVerboseStep);

    itkSetMacro(WriteStep, bool);
    itkGetConstMacro(WriteStep, bool);
    itkBooleanMacro(WriteStep);
    GGO_DefineOption_Flag(writeStep, SetWriteStep);

    itkSetMacro(CurrentStepNumber, int);
    itkGetConstMacro(CurrentStepNumber, int);
    itkSetMacro(CurrentStepId, std::string);
    itkGetConstMacro(CurrentStepId, std::string);
    itkSetMacro(CurrentStepBaseId, std::string);
    itkGetConstMacro(CurrentStepBaseId, std::string);
    itkSetMacro(CurrentStepName, std::string);
    itkGetConstMacro(CurrentStepName, std::string);

    // Convenient function for verbose option
    template<class OptionType>
    void VerboseOption(std::string name, OptionType value);    
    template<class OptionType>
    void VerboseOption(std::string name, int nb, OptionType value);
    template<class OptionType>
    void VerboseOptionV(std::string name, int nb, OptionType * value);

    void SetWarning(std::string e);
    itkGetConstMacro(Warning, std::string);
    itkSetMacro(VerboseWarningOff, bool);
    itkGetConstMacro(VerboseWarningOff, bool);
    itkBooleanMacro(VerboseWarningOff);
    GGO_DefineOption_Flag(verboseWarningOff, SetVerboseWarningOff);
    
    // Use this function to cancel the filter between step
    void Cancel();
    bool Cancelled();

  protected:
    FilterBase();
    virtual ~FilterBase() {}    
    void StartNewStep(std::string s);
    template<class TInternalImageType>
    void StopCurrentStep(typename TInternalImageType::Pointer p);
    void StopCurrentStep();

    bool m_VerboseOption;  
    bool m_VerboseStep;
    bool m_WriteStep;
    int m_CurrentStepNumber;
    int m_NumberOfSteps;
    std::string m_CurrentStepId;
    std::string m_CurrentStepBaseId;
    std::string m_CurrentStepName;
    std::string m_Warning;
    bool m_VerboseWarningOff;
    bool m_IsCancelled;
    Timer m_CurrentStepTimer;
    

  private:
    FilterBase(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkFilterBase.txx"
#endif

#endif // CLITKFILTERBASE_H
