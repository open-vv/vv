/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCreatorBase.cxx,v $
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

#include "vvToolCreatorBase.h"
#include "vvSlicerManager.h"
#include "vvToolManager.h"
#include <QAction>

//------------------------------------------------------------------------------
vvToolCreatorBase::vvToolCreatorBase(QString name) { 
  mToolName = name;
  vvToolManager::GetInstance()->AddTool(this); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCreatorBase::Initialize(vvMainWindow * m) { 
  mMainWindow = m;
 
  // Create main Action
  if (mToolIconFilename == "noicon") 
    mAction = new QAction(QString("&").append(mToolMenuName), this);
  else 
    mAction = new QAction(QIcon(mToolIconFilename), 
                          QString("&").append(mToolMenuName), this);
  mAction->setStatusTip(mToolTip);
  connect(mAction, SIGNAL(triggered()), this, SLOT(MenuToolSlot()));
  mMainWindow->GetInfoForTool()->mMenuTools->addAction(mAction);
  //UpdateEnabledTool();
} 
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCreatorBase::UpdateEnabledTool() { 
  DD("Update enabled tool"); 
  
  DD(mMainWindow->GetInfoForTool()->mSlicerManagers->size());
  if (mMainWindow->GetInfoForTool()->mSlicerManagers->size() < 1)
    mAction->setEnabled(false);
  else mAction->setEnabled(true);
}
//------------------------------------------------------------------------------
