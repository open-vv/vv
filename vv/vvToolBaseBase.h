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

#ifndef VVTOOLBASEBASE_H
#define VVTOOLBASEBASE_H

#include <algorithm>
#include "vvMainWindowBase.h"
#include "vvToolCreatorBase.h"
class QXmlStreamWriter;
class QXmlStreamReader;

//------------------------------------------------------------------------------
class vvToolBaseBase {
public:
  virtual ~vvToolBaseBase() { 
    std::vector<vvToolBaseBase*> & v = mCreator->GetListOfTool();
    v.erase(std::find(v.begin(), v.end(), this));
  };
#if __cplusplus > 199711L
  virtual void SaveState(std::shared_ptr<QXmlStreamWriter> & m_XmlWriter);
#else
  virtual void SaveState(std::auto_ptr<QXmlStreamWriter> & m_XmlWriter);
#endif
  virtual void InitializeNewTool(bool ReadStateFlag);   
  void SetCreator(vvToolCreatorBase * m) { mCreator = m; }
  void SetXmlReader(QXmlStreamReader * r, int index) { m_XmlReader = r; mImageIndex = index;}  

protected:
  vvMainWindowBase * mMainWindowBase;
  QXmlStreamReader * m_XmlReader;
  vvToolCreatorBase * mCreator;
  int mImageIndex;
};
//------------------------------------------------------------------------------
#endif

