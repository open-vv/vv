#ifndef VVMAINWINDOWBASE_H
#define VVMAINWINDOWBASE_H
#include "clitkCommon.h"
#include "vvImage.h"
#include <qmainwindow.h>

class vvSlicerManager;
class QMenu;

//------------------------------------------------------------------------------
class vvMainWindowBase : public QMainWindow
{
   Q_OBJECT

public:
  vvMainWindowBase() { mSlicerManagerCurrentIndex = -1; }
  ~vvMainWindowBase() { }

  virtual void AddImage(vvImage::Pointer image,std::string filename) = 0;
  virtual void UpdateCurrentSlicer() = 0;
  const std::vector<vvSlicerManager*> & GetSlicerManagers() const { return mSlicerManagers; }
  QMenu * GetToolMenu() const { return mMenuTools; }
  QMenu * GetExperimentalToolMenu() const { return mMenuExperimentalTools; }
  int GetSlicerManagerCurrentIndex() const { return mSlicerManagerCurrentIndex; }
  QWidget * GetMainWidget() { return mMainWidget; }

signals:
  void AnImageIsBeingClosed(vvSlicerManager *);

protected:
  std::vector<vvSlicerManager*> mSlicerManagers;
  QMenu * mMenuTools;
  QMenu * mMenuExperimentalTools;
  int mSlicerManagerCurrentIndex;
  QWidget* mMainWidget;
};
//------------------------------------------------------------------------------

#endif
