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
#include "clitkExceptionObject.h"

//--------------------------------------------------------------------
clitk::ExceptionObject::
ExceptionObject(const char *file, unsigned int lineNumber, const char *desc):
  itk::ExceptionObject(file, lineNumber, desc) 
{ 
  m_Message = desc; 
  m_LineNumber = lineNumber;
  m_Filename = file;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const char* clitk::ExceptionObject::what() const throw() 
{
  std::ostringstream message;
  message << "clitk::ERROR in " << m_Filename << " line " << m_LineNumber
          << " : " << std::endl << "\t" << m_Message;
  char * m = new char[message.str().length()];
  strcpy(m, message.str().c_str());
  return m;
}
//--------------------------------------------------------------------
  
