/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCreatorBase.h,v $
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

