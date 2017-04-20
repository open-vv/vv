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

// vv
#include "vvToolBaseBase.h"

//------------------------------------------------------------------------------
#if __cplusplus > 199711L
void vvToolBaseBase::SaveState(std::shared_ptr<QXmlStreamWriter> & m_XmlWriter)
#else
void vvToolBaseBase::SaveState(std::auto_ptr<QXmlStreamWriter> & m_XmlWriter)
#endif
{ 
  std::cerr << "SaveState not implemented for this tool." << std::endl; 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolBaseBase::InitializeNewTool(bool ReadStateFlag) { 
  if (ReadStateFlag == true) {
    std::cerr << "ReadState not implemented for this tool." << std::endl;
  }
}
//------------------------------------------------------------------------------
