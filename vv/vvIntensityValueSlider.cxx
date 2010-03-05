/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvIntensityValueSlider.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/05 10:32:33 $
  Version:   $Revision: 1.5 $
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
  mIsInteger = true;
  mButtonPlus->setHidden(true);
  mButtonMinus->setHidden(true);
  
  // Connect signals & slots
  connect(mSpinBox, SIGNAL(valueChanged(double)), this, SLOT(valueChangedFromSpinBox(double)));
  connect(mSlider, SIGNAL(valueChanged(int)), this, SLOT(valueChangedFromSlider(int)));
  connect(mButtonPlus, SIGNAL(clicked()), this, SLOT(SingleStepPlusClicked()));
  connect(mButtonMinus, SIGNAL(clicked()), this, SLOT(SingleStepMinusClicked()));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvIntensityValueSlider::~vvIntensityValueSlider() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SingleStepMinusClicked() {
  mSpinBox->setSingleStep(mSpinBox->singleStep()*10);
  mSpinBox->setDecimals(mSpinBox->decimals()-1);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SetSingleStep(double step) {
  mSpinBox->setSingleStep(step);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SingleStepPlusClicked() {
  mSpinBox->setSingleStep(mSpinBox->singleStep()/10);
  mSpinBox->setDecimals(mSpinBox->decimals()+1);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::valueChangedFromSpinBox(double v) {
  if (v == mValue) return;
  mSpinBox->setValue(v);
  v = mSpinBox->value(); // this is needed to 'round' value according to spinBox precision
  double vv;
  if (!mIsInteger) {
    vv = ((v-mMin)/mWidth)/mSliderFactor;
  }
  else vv = v;
  mSlider->setValue(vv);
  mValue = v;
  emit valueChanged(v);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::valueChangedFromSlider(int vv) {
  double v;
  if (!mIsInteger) {
    v = ((double)vv*mSliderFactor)*mWidth+mMin;
  }
  else v= vv;

  // arrondir ! ?

  if (mValue == v) return;
  mSpinBox->setValue(v);
  mValue = v;
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
void vvIntensityValueSlider::SetValue(double v) { 
  mValue = v;
  mSpinBox->setValue(v);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SetMaximum(double max) {
  mMax = max;
  mSpinBox->setMaximum(max);
  
  // If integer values : update slider max
  if (mIsInteger == 1) {
    mSlider->setMaximum(max);
  }
  else {
    double step = mWidth/1000.0;
    mSpinBox->setSingleStep(step);
    mWidth = mMax-mMin;
  }
  if (mValue > max) { SetValue(max); }
  QString tip = QString("Min = %1    Max = %2").arg(mSpinBox->minimum()).arg(max);
  setToolTip(tip);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::SetMinimum(double min) {
  mMin = min;
  mSpinBox->setMinimum(min);

  if (mIsInteger == 1) {
    mSlider->setMinimum(min);
  }
  else {
    double step = mWidth/1000.0;
    mSpinBox->setSingleStep(step);
    mWidth = mMax-mMin;
  }

  if (mValue < min) { SetValue(min); }
  QString tip = QString("Min = %1    Max = %2").arg(min).arg(mSpinBox->maximum());
  setToolTip(tip);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvIntensityValueSlider::Update() {
  double range[2];
  mImage->GetFirstVTKImageData()->GetScalarRange(range);
  mMin = range[0];
  mMax = range[1];
  double step = (mMax-mMin)/1000.0;
  
  if (mImage->IsScalarTypeInteger()) {
    mIsInteger = true;
    mSpinBox->setSingleStep(1.0);
    mSpinBox->setDecimals(0);
    mSlider->setMaximum(mMax);
    mSlider->setMinimum(mMin);
    mSlider->setSingleStep(1);
    mSliderFactor = 1.0;
    mWidth = 1.0;
  }
  else {
    mIsInteger = false;
    mButtonPlus->setHidden(false);
    mButtonMinus->setHidden(false);
    mSpinBox->sizePolicy().setHorizontalPolicy(QSizePolicy::Expanding);
    mSpinBox->setSingleStep(step);
    mSpinBox->setDecimals(4);
    mSlider->setMaximum(1000);
    mSlider->setMinimum(0);
    mSlider->setSingleStep(1);
    mSliderFactor = 1.0/1000.0;
    mWidth = mMax-mMin;
  }
  
  mSpinBox->setMaximum(mMax);
  mSpinBox->setMinimum(mMin);
  mSpinBox->setValue((mMax-mMin)/2.0+mMin);
  
  QString tip = QString("Min = %1    Max = %2").arg(mMin).arg(mMax);
  setToolTip(tip);
}
//------------------------------------------------------------------------------


