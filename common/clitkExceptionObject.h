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

#ifndef CLITKEXCEPTIONOBJECT_H
#define CLITKEXCEPTIONOBJECT_H

#include <sstream>

// clitk
#include "clitkCommon.h"

// itk
#include "itkMacro.h"

namespace clitk {

  //--------------------------------------------------------------------
  class ExceptionObject: public itk::ExceptionObject {
  public:
    ExceptionObject(const char *file, 
                    unsigned int lineNumber=0,
                    const char *desc="None");
    virtual ~ExceptionObject() throw() {}
    virtual const char* what() const throw();
    std::string m_Message;
    int m_LineNumber;
    std::string m_Filename;
  };
  //--------------------------------------------------------------------
  
  //--------------------------------------------------------------------
  // Largely inspired from itkExceptionMacro, but work without
  // itkObject
#define clitkExceptionMacro(x)                                          \
  {                                                                     \
    std::ostringstream message;                                       \
    message << x;                                                       \
    ::clitk::ExceptionObject e_(__FILE__, __LINE__, message.str().c_str()); \
    throw e_; /* Explicit naming to work around Intel compiler bug.  */ \
  }
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#endif // CLITKEXCEPTIONOBJECT_H
