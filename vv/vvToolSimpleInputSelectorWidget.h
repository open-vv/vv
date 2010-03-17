/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolSimpleInputSelectorWidget.h,v $
  Language:  C++
  Date:      $Date: 2010/03/17 11:23:45 $
  Version:   $Revision: 1.1 $
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

#ifndef VVTOOLSIMPLEINPUTSELECTORWIDGET_H
#define VVTOOLSIMPLEINPUTSELECTORWIDGET_H

#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>
#include "ui_vvToolSimpleInputSelectorWidget.h"

class vvSlicerManager;

//------------------------------------------------------------------------------
class vvToolSimpleInputSelectorWidget: public QWidget, private Ui::vvToolSimpleInputSelectorWidget 
{
  Q_OBJECT
    public:
  vvToolSimpleInputSelectorWidget(QWidget * parent=0, Qt::WindowFlags f=0);
  ~vvToolSimpleInputSelectorWidget() {}
  
  void SetInputList(const std::vector<vvSlicerManager*> & l, int index);
  void Initialize();
  int GetSelectedInputIndex() { return mCurrentIndex; }
  vvSlicerManager * GetSelectedInput();

 public slots:
  void accept();
  void reject();
  void setEnabled(bool b);

 signals:
  void accepted();
  void rejected();

 protected slots:
  void changeInput(int i);
  
 protected:
  Ui::vvToolSimpleInputSelectorWidget ui;
  std::vector<vvSlicerManager*> mSlicerManagerList;
  int mCurrentIndex;
  vvSlicerManager * mCurrentSliceManager;

}; // end class vvToolSimpleInputSelectorWidget
//------------------------------------------------------------------------------

#endif

