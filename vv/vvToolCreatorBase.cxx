/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCreatorBase.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/24 13:37:34 $
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

#include "vvToolCreatorBase.h"
#include "vvToolManager.h"
#include <QAction>
#include <QMenu>

//------------------------------------------------------------------------------
vvToolCreatorBase::vvToolCreatorBase(QString name): mExperimental(false) { 
  mToolName = name;
  vvToolManager::GetInstance()->AddTool(this); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolCreatorBase::InsertToolInMenu(vvMainWindowBase * m) { 
  mMainWindow = m; 
  // Create main action
  if (mToolIconFilename == "noicon") 
    mAction = new QAction(QString("&").append(mToolMenuName), this);
  else 
    mAction = new QAction(QIcon(mToolIconFilename), 
                          QString("&").append(mToolMenuName), this);
  mAction->setStatusTip(mToolTip);
  // Connect the action
  connect(mAction, SIGNAL(triggered()), this, SLOT(MenuToolSlot()));
  if (mExperimental)
      mMainWindow->GetExperimentalToolMenu()->addAction(mAction);
  else
      mMainWindow->GetToolMenu()->addAction(mAction);
} 
//------------------------------------------------------------------------------

