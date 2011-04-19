/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/

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

  virtual vvSlicerManager * AddImage(vvImage::Pointer image,std::string filename) = 0;
  virtual void UpdateCurrentSlicer() = 0;
  virtual QTabWidget * GetTab() = 0;
  const std::vector<vvSlicerManager*> & GetSlicerManagers() const { return mSlicerManagers; }
  QMenu * GetToolMenu() const { return mMenuTools; }
  QMenu * GetMenu(std::string menuName) const;// { return mMenuTools; }
  QMenu * GetContextMenu() const { return mContextMenu; }
  QMenu * GetExperimentalToolMenu() const { return mMenuExperimentalTools; }
  int GetSlicerManagerCurrentIndex() const { return mSlicerManagerCurrentIndex; }
  QWidget * GetMainWidget() { return mMainWidget; }
  const QString & GetInputPathName() { return mInputPathName; }

signals:
  void AnImageIsBeingClosed(vvSlicerManager *);
  void SelectedImageHasChanged(vvSlicerManager *);

protected:
  std::vector<vvSlicerManager*> mSlicerManagers;
  QMenu * mMenuTools;
  QMenu * mMenuSegmentation;
  QMenu * mContextMenu;
  QMenu * mMenuExperimentalTools;
  int mSlicerManagerCurrentIndex;
  QWidget* mMainWidget;
  QString mInputPathName;

};
//------------------------------------------------------------------------------

#endif
