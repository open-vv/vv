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

