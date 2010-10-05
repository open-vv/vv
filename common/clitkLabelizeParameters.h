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

#ifndef CLITKLABELIZEPARAMETERS_H
#define CLITKLABELIZEPARAMETERS_H

// clitk
//#include "clitkCommon.h"

// itk
#include "itkObject.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  template<class PixelType>
  class LabelizeParameters: public itk::Object {
  public:
    typedef LabelizeParameters<PixelType> Self;
    LabelizeParameters() { UseLastKeepOff(); }
    itkSetMacro(FirstKeep, PixelType);
    itkGetMacro(FirstKeep, PixelType);
    itkSetMacro(LastKeep, PixelType);
    itkGetMacro(LastKeep, PixelType);
    itkSetMacro(UseLastKeep, bool);
    itkGetConstMacro(UseLastKeep, bool);
    itkBooleanMacro(UseLastKeep);
    void AddLabelToRemove(PixelType l) { m_LabelsToRemove.push_back(l); }
    //    itkGetMacro(LabelsToRemove, std::vector<PixelType> &);
    std::vector<PixelType> & GetLabelsToRemove() { return m_LabelsToRemove; }
    
  protected:
    std::vector<PixelType> m_LabelsToRemove;
    PixelType m_FirstKeep;
    PixelType m_LastKeep;
    bool m_UseLastKeep; 
  };
  //--------------------------------------------------------------------


} // end namespace clitk
//--------------------------------------------------------------------

#endif // CLITKLABELIZEPARAMETERS_H
