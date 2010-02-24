/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCropImage.h,v $
  Language:  C++
  Date:      $Date: 2010/02/24 11:43:37 $
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

#ifndef VVTOOLCROPIMAGE_H
#define VVTOOLCROPIMAGE_H

#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvMainWindowBase.h"
#include "ui_vvToolCropImage.h"

//------------------------------------------------------------------------------
class vvToolCropImage:
  public vvToolWidgetBase,
  public vvToolBase<vvToolCropImage>, 
  private Ui::vvToolCropImage 
{
  Q_OBJECT
    public:
  vvToolCropImage(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolCropImage();

  //-----------------------------------------------------
  typedef vvToolCropImage Self;
  bool close() { return QWidget::close(); }
  virtual void InputIsSelected(vvSlicerManager *m);
  //void AnImageIsBeingClosed(vvSlicerManager * m) { vvToolWidgetBase::TTAnImageIsBeingClosed(m); }

  void bar() { DD("crop::bar"); }


public slots:
  void apply() { DD("Apply"); }

  //-----------------------------------------------------
  static void Initialize() {
    SetToolName("Crop");
    SetToolMenuName("Crop");
    SetToolIconFilename(":/new/prefix1/icons/binarize.png");
    SetToolTip("Crop image.");
  }

 protected:
  Ui::vvToolCropImage ui;


}; // end class vvToolCropImage
//------------------------------------------------------------------------------

#endif

