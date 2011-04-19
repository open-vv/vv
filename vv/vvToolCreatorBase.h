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
#ifndef VVTOOLCREATORBASE_H
#define VVTOOLCREATORBASE_H

//qt
#include <QObject>

// vv
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
  virtual void InsertToolInContextMenu();
  template<class ToolType> void CreateTool();
  virtual void MenuSpecificToolSlot() = 0;
  vvMainWindowBase * GetMainWindow() const { return mMainWindow; }
  void addMenuToContextMenu(QMenu * m);
  void SetMenuName(std::string m) { m_MenuName = m; }

  QString mToolName;
  QString mToolMenuName;
  QString mToolIconFilename;
  QString mToolTip;
  QAction * mAction;
  bool mExperimental;
  bool mUseContextMenu;
  QMenu * mToolMenu;
  std::vector<QAction*> mListOfActions;

public slots:
  virtual void MenuToolSlot();

protected:
  vvMainWindowBase * mMainWindow;
  std::vector<vvToolBaseBase*> mListOfTool;
  QObject * mSender;
  std::string m_MenuName;
};
//------------------------------------------------------------------------------

#include "vvToolCreatorBase.txx"

#endif

