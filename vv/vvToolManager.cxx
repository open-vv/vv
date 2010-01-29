/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolManager.cxx,v $
  Language:  C++
  Date:      $Date: 2010/01/29 13:54:37 $
  Version:   $Revision: 1.1 $
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
#include "vvMainWindow.h"

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
  mListOfTools.push_back(v);
}  
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolManager::Initialize(vvMainWindow * m) {
  for(unsigned int i=0; i<mListOfTools.size(); i++) {
    mListOfTools[i]->Initialize(m);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolManager::UpdateEnabledTool() {
  for(unsigned int i=0; i<mListOfTools.size(); i++) {
    mListOfTools[i]->UpdateEnabledTool();
  }
}
//------------------------------------------------------------------------------
