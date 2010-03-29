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

#include "vvToolManager.h"
#include "vvToolCreatorBase.h"
#include "vvMainWindowBase.h"

//------------------------------------------------------------------------------
/// Unique static instance 
vvToolManager* vvToolManager::mSingleton=0; 
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolManager * vvToolManager::GetInstance() {
  if (mSingleton == 0) {
    mSingleton = new vvToolManager;
  }
  return mSingleton;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolManager::AddTool(vvToolCreatorBase * v)  {
  std::cout << "Adding the tool <" << v->mToolName.toStdString() << ">." << std::endl;
  GetInstance()->mListOfTools.push_back(v);
}  
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolManager::InsertToolsInMenu(vvMainWindowBase * m) {
  for(unsigned int i=0; i<GetInstance()->mListOfTools.size(); i++) {
    GetInstance()->mListOfTools[i]->InsertToolInMenu(m);
  }
}
//------------------------------------------------------------------------------

