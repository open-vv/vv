/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolInputSelectorWidget.h,v $
  Language:  C++
  Date:      $Date: 2010/03/01 15:38:09 $
  Version:   $Revision: 1.4 $
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

#ifndef VVTOOLINPUTSELECTORWIDGET_H
#define VVTOOLINPUTSELECTORWIDGET_H

#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>
#include "ui_vvToolInputSelectorWidget.h"
#include "vvImage.h"

class vvSlicerManager;

//------------------------------------------------------------------------------
class vvToolInputSelectorWidget: public QWidget, private Ui::vvToolInputSelectorWidget 
{
  Q_OBJECT
    public:
  vvToolInputSelectorWidget(QWidget * parent=0, Qt::WindowFlags f=0);
  ~vvToolInputSelectorWidget() {}
  
  void Initialize(std::vector<vvSlicerManager*> l, int index);
  int GetSelectedInputIndex() { return mCurrentIndex; }

 public slots:
  void accept();
  void reject();
  void changeInput(int i);

 signals:
  void accepted();
  void rejected();

 protected:
  Ui::vvToolInputSelectorWidget ui;
  std::vector<vvSlicerManager*> mSlicerManagerList;
  int mCurrentIndex;
  vvSlicerManager * mCurrentSliceManager;

}; // end class vvToolInputSelectorWidget
//------------------------------------------------------------------------------

#endif

