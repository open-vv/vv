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

