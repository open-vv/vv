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
#ifndef VVTOOLINPUTSELECTORWIDGET_H
#define VVTOOLINPUTSELECTORWIDGET_H

// qt
#include <QObject>
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif
#include <QDialog>

// vv
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

