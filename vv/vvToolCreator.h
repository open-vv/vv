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
#ifndef VVTOOLCREATOR_H
#define VVTOOLCREATOR_H

#include "vvToolCreatorBase.h"

//------------------------------------------------------------------------------
template<class ToolType>
class vvToolCreator: public vvToolCreatorBase {
public:
  static vvToolCreator<ToolType> *&GetInstance();
  virtual void InsertToolInMenu(vvMainWindowBase * m);
  virtual void MenuSpecificToolSlot() { CreateTool<ToolType>(); }
private:
  vvToolCreator():vvToolCreatorBase(mToolCreatorName) {;}
  static vvToolCreator<ToolType> * mSingleton;
  static const QString mToolCreatorName;
};
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#define CREATOR(CLASSNAME) vvToolCreator<CLASSNAME>::GetInstance()
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Initiliazes the static parameters and creates a dummy pointer (required for windows)
#define ADD_TOOL(NAME)                                                           \
  template<> const QString        vvToolCreator<NAME>::mToolCreatorName = #NAME; \
  template<> vvToolCreator<NAME> *vvToolCreator<NAME>::mSingleton       = NULL;  \
  const vvToolCreator<NAME> *dummy##NAME = vvToolCreator<NAME>::GetInstance();
//------------------------------------------------------------------------------

#include "vvToolCreator.txx"

#endif

