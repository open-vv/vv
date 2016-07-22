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
#ifndef VVINTENSITYVALUESLIDER_H
#define VVINTENSITYVALUESLIDER_H

// qt
#include <QtGlobal>
#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>

// clitk 
#include "clitkCommon.h"

// vv
#include "ui_vvIntensityValueSlider.h"
#include "vvImage.h"
#include "vtkImageData.h"

//------------------------------------------------------------------------------
class vvIntensityValueSlider:
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
  void SetSingleStep(double step);                         
  void resetMinimum() { SetMinimum(mMin); }
  void resetMaximum() { SetMaximum(mMax); }

 public slots:
  void valueChangedFromSpinBox(double v);
  void valueChangedFromSlider(int v);
  void SingleStepPlusClicked();
  void SingleStepMinusClicked();

 signals:
  void valueChanged(double);

 protected:
  Ui::vvIntensityValueSlider ui;
  vvImage * mImage;
  double mMin;
  double mMax;
  double mWidth;
  double mSliderFactor;
  double mValue;
  bool mIsInteger;

}; // end class vvIntensityValueSlider
//------------------------------------------------------------------------------

#endif

