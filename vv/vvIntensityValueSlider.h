#ifndef VVINTENSITYVALUESLIDER_H
#define VVINTENSITYVALUESLIDER_H
#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>

#include "ui_vvIntensityValueSlider.h"
#include "clitkCommon.h"
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

