/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCreatorBase.h,v $
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

#ifndef VVTOOLCREATORBASE_H
#define VVTOOLCREATORBASE_H

#include "clitkCommon.h"
#include <QObject>

// For vvMainWindowToolInfo
#include "vvMainWindow.h"
class vvMainWindow;
// struct vvMainWindowToolInfo;
class QAction;

//------------------------------------------------------------------------------
class vvToolCreatorBase: public QObject {
  Q_OBJECT
  public:

  vvToolCreatorBase(QString name);
  virtual ~vvToolCreatorBase() {;}
  
  virtual void Initialize(vvMainWindow * m);
  virtual void UpdateEnabledTool();
  template<class ToolType> void CreateTool();
  virtual void MenuSpecificToolSlot() = 0;
  
  QString mToolName;
  QString mToolMenuName;
  QString mToolIconFilename;
  QString mToolTip;
  QAction * mAction;

  vvMainWindow * mMainWindow;

public slots:
  virtual void MenuToolSlot() { MenuSpecificToolSlot(); }
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template<class ToolType>
class vvToolCreator: public vvToolCreatorBase {
public:
  vvToolCreator(QString name):vvToolCreatorBase(name) {;}
  virtual void Initialize(vvMainWindow * m);
  static vvToolCreator<ToolType> * mSingleton;
  virtual void MenuSpecificToolSlot() { CreateTool<ToolType>(); }
};
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#define CREATOR(CLASSNAME) vvToolCreator<CLASSNAME>::mSingleton
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
#define ADD_TOOL(NAME)                                          \
  template<>                                                    \
  vvToolCreator<NAME> * vvToolCreator<NAME>::mSingleton =       \
    new vvToolCreator<NAME>(#NAME);
//------------------------------------------------------------------------------

#include "vvToolCreatorBase.txx"

#endif

