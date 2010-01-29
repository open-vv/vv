/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvIntensityValueSlider.h,v $
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

#ifndef VVINTENSITYVALUESLIDER_H
#define VVINTENSITYVALUESLIDER_H

#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>

#include "ui_vvIntensityValueSlider.h"
#include "clitkCommon.h"
#include "vvImage.h"
#include "vtkImageData.h"

//------------------------------------------------------------------------------
class QDESIGNER_WIDGET_EXPORT vvIntensityValueSlider: 
  public QWidget, private Ui::vvIntensityValueSlider {
  Q_OBJECT
    public:
  vvIntensityValueSlider(QWidget * parent=0, Qt::WindowFlags f=0);
  ~vvIntensityValueSlider();

  void SetText(QString t);
  void SetImage(vvImage * im);
  void Update();
  double GetValue() const { return mValue; }
  void SetValue(double d);
  void SetMaximum(double max);
  void SetMinimum(double min);                         
  void resetMinimum() { SetMinimum(mMin); }
  void resetMaximum() { SetMaximum(mMax); }

 public slots:
  void valueChangedFromSpinBox(double v);
  void valueChangedFromSlider(int v);

 signals:
  void valueChanged(double);

 protected:
  Ui::vvIntensityValueSlider ui;
  vvImage * mImage;
  double mSliderFactor;
  double mValue;
  double mMax;
  double mMin;

}; // end class vvIntensityValueSlider
//------------------------------------------------------------------------------

#endif

