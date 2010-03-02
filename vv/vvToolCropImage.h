/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolCropImage.h,v $
  Language:  C++
  Date:      $Date: 2010/03/02 13:28:43 $
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

  virtual void InputIsSelected(vvSlicerManager *m);

public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  void sliderXMinValueChanged(int s);
  void sliderXMaxValueChanged(int s);
  void sliderYMinValueChanged(int s);
  void sliderYMaxValueChanged(int s);
  void sliderZMinValueChanged(int s);
  void sliderZMaxValueChanged(int s);
  // void sliderTMinValueChanged(int s);
  // void sliderTMaxValueChanged(int s);
  void autoCropValueChanged(double v);

  //-----------------------------------------------------
  static void Initialize() {
    SetToolName("Crop");
    SetToolMenuName("Crop");
    SetToolIconFilename(":/new/prefix1/icons/crop.png");
    SetToolTip("Crop image.");
  }

 protected:
  Ui::vvToolCropImage ui;
  int * mReducedExtent;
  int * mInitialExtent;
  int mExtentSize;
  void UpdateExtent();

}; // end class vvToolCropImage
//------------------------------------------------------------------------------

#endif

