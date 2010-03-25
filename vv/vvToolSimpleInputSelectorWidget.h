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
  void SetText(QString & s);
  void EnableAllowSkip(bool b);

 public slots:
  void accept();
  void reject();
  void setEnabled(bool b);
  void skip(QAbstractButton*);

 signals:
  void accepted();
  void rejected();
  void sigskip();

 protected slots:
  void changeInput(int i);
  
 protected:
  Ui::vvToolSimpleInputSelectorWidget ui;
  std::vector<vvSlicerManager*> mSlicerManagerList;
  int mCurrentIndex;
  vvSlicerManager * mCurrentSliceManager;
  bool mAllowSkip;

}; // end class vvToolSimpleInputSelectorWidget
//------------------------------------------------------------------------------

#endif

