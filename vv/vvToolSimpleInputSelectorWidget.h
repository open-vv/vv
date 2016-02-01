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
#ifndef VVTOOLSIMPLEINPUTSELECTORWIDGET_H
#define VVTOOLSIMPLEINPUTSELECTORWIDGET_H
#if QT_VERSION < 0x050000
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif
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

