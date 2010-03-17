/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolWidgetBase.h,v $
  Language:  C++
  Date:      $Date: 2010/03/17 11:22:18 $
  Version:   $Revision: 1.3 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2010
  Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
  CREATIS                   http://www.creatis.insa-lyon.fr

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

#ifndef VVTOOLWIDGETBASE_H
#define VVTOOLWIDGETBASE_H

#include <QtDesigner/QDesignerExportWidget>
#include "ui_vvToolWidgetBase.h"
#include "clitkImageToImageGenericFilter.h"
class vvMainWindowBase;

//------------------------------------------------------------------------------
class vvToolWidgetBase:
  public QDialog, 
  public Ui::vvToolWidgetBase 
{
  Q_OBJECT
  public:

  vvToolWidgetBase(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolWidgetBase();

  virtual void InputIsSelected(vvSlicerManager *m);
  virtual void InputIsSelected(std::vector<vvSlicerManager*> & l);
  void AddInputSelector(clitk::ImageToImageGenericFilterBase * f);
  void AddInputSelector();

public slots:
  virtual void apply() = 0;
  virtual bool close();
  void InputIsSelected();
  void AnImageIsBeingClosed(vvSlicerManager * m);
  void show();

protected:
  void InitializeInputs();
  Ui::vvToolWidgetBase ui;
  clitk::ImageToImageGenericFilterBase * mFilter;
  vvMainWindowBase * mMainWindowBase;
  std::vector<vvSlicerManager*> mSlicerManagersCompatible;
  vvSlicerManager * mCurrentSlicerManager;
  int mCurrentCompatibleIndex;
  vvImage * mCurrentImage;
  bool mIsInitialized;

}; // end class vvToolWidgetBase
//------------------------------------------------------------------------------

#endif

