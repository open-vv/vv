/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvIntensityValueSlider.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/01 07:37:25 $
  Version:   $Revision: 1.4 $
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

#include "vvIntensityValueSlider.h"

//------------------------------------------------------------------------------
vvIntensityValueSlider::vvIntensityValueSlider(QWidget * parent, Qt::WindowFlags f)
  :QWidget(parent,f),  Ui::vvIntensityValueSlider() 
{
  // GUI Initialization
  setupUi(this);  
  mValue = 0;
  SetMaximum(1000);
  SetMinimum(-1000);
  
  // Connect signals & slots
  connect(mSpinBox, SIGNAL(valueChanged(double)), this, SLOT(valueChangedFromSpinBox(double)));
  connect(mSlider, SIGNAL(valueChanged(int)), this, SLOT(valueChangedFromSlider(int)));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvIntensityValueSlider::~vvIntensityValueSlider() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::valueChangedFromSpinBox(double v) {
  mSlider->setValue(v);
  mValue = v;
  emit valueChanged(v);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::valueChangedFromSlider(int v) {
  mSpinBox->setValue(v*mSliderFactor);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SetText(QString t) {
  mLabel->setText(t);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SetImage(vvImage * im) { 
  mImage = im; 
  Update(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SetValue(double d) { 
  mSpinBox->setValue(d);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SetMaximum(double max) {
  mSlider->setMaximum(max);
  mSpinBox->setMaximum(max);
  if (mValue > max) { SetValue(max); }
  QString tip = QString("Min = %1    Max = %2").arg(mSpinBox->minimum()).arg(max);
  setToolTip(tip);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SetMinimum(double min) {
  mSlider->setMinimum(min);
  mSpinBox->setMinimum(min);
  if (mValue < min) { SetValue(min); }
  QString tip = QString("Min = %1    Max = %2").arg(min).arg(mSpinBox->maximum());
  setToolTip(tip);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::Update() {
  if (mImage->IsScalarTypeInteger()) {

    mSpinBox->setSingleStep(1.0);
    mSpinBox->setDecimals(0);
    mSliderFactor = 1.0;

    double range[2];
    mImage->GetFirstVTKImageData()->GetScalarRange(range);
    mMin = range[0];
    mMax = range[1];
    mSlider->setMaximum(mMax);
    mSlider->setMinimum(mMin);
    mSpinBox->setMaximum(mMax);
    mSpinBox->setMinimum(mMin);

    QString tip = QString("Min = %1    Max = %2").arg(mMin).arg(mMax);
    setToolTip(tip);
  }
  else {
    std::cerr << "NO floating point image yet !!" << std::endl;
    exit(0);
  }
}

//------------------------------------------------------------------------------
