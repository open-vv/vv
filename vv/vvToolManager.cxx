/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolManager.cxx,v $
  Language:  C++
  Date:      $Date: 2010/02/24 11:42:42 $
  Version:   $Revision: 1.3 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2008
  Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
  CREATIS-LRMN http://www.creatis.insa-lyon.fr

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  =========================================================================*/

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

