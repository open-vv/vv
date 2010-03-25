#ifndef VVTOOLINPUTSELECTORWIDGET_H
#define VVTOOLINPUTSELECTORWIDGET_H
#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>
#include "ui_vvToolInputSelectorWidget.h"

class vvSlicerManager;
class vvToolSimpleInputSelectorWidget;

//------------------------------------------------------------------------------
class vvToolInputSelectorWidget: public QWidget, private Ui::vvToolInputSelectorWidget 
{
  Q_OBJECT
    public:
  vvToolInputSelectorWidget(QWidget * parent=0, Qt::WindowFlags f=0);
  ~vvToolInputSelectorWidget() {}
  
  void AddInputSelector(QString & s, const std::vector<vvSlicerManager*> & l, int index, bool allowSkip=false);  
  void Initialize();
  std::vector<vvSlicerManager*> & GetSelectedInputs();
  void AnImageIsBeingClosed(vvSlicerManager * m);
  int GetNumberOfInput();

 public slots:
  void accept(); // to change ! in something like acceptOneMoreInput
  void reject();
  void skip();

 signals:
  void accepted();
  void rejected();

 protected:
  void AddInputSelector(vvToolSimpleInputSelectorWidget * input, 
			std::vector<vvSlicerManager*> l, int index);
  Ui::vvToolInputSelectorWidget ui;
  std::vector<vvSlicerManager*> mSlicerManagerList;
  int mCurrentIndex;
  vvSlicerManager * mCurrentSliceManager;
  int mNumberOfAcceptedInputs;
  std::vector<vvToolSimpleInputSelectorWidget *> mListOfSimpleInputWidget;
  std::vector<bool> mSkipInput;
}; // end class vvToolInputSelectorWidget
//------------------------------------------------------------------------------

#endif

