#ifndef VVTOOLCREATORBASE_H
#define VVTOOLCREATORBASE_H
#include <QObject>
#include "vvSlicerManager.h"
#include "vvMainWindowBase.h"

class QAction;
class vvToolBaseBase;

//------------------------------------------------------------------------------
class vvToolCreatorBase: public QObject {
  Q_OBJECT
  public:

  vvToolCreatorBase(QString name);
  virtual ~vvToolCreatorBase() {;}  
  virtual void InsertToolInMenu(vvMainWindowBase * m);
  template<class ToolType> void CreateTool();
  virtual void MenuSpecificToolSlot() = 0;
  vvMainWindowBase * GetMainWindow() const { return mMainWindow; }

  QString mToolName;
  QString mToolMenuName;
  QString mToolIconFilename;
  QString mToolTip;
  QAction * mAction;
  bool mExperimental;

public slots:
  virtual void MenuToolSlot() { MenuSpecificToolSlot(); }

protected:
  vvMainWindowBase * mMainWindow;
  std::vector<vvToolBaseBase*> mListOfTool;
};
//------------------------------------------------------------------------------

#include "vvToolCreatorBase.txx"

#endif

