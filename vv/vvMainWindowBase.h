/*=========================================================================

 Program:   vv
  Module:   $RCSfile: vvMainWindowBase.h,v $
 Language:  C++
 Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

Copyright (C) 2010
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
