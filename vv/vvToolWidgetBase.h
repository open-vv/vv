/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

#ifndef VVTOOLWIDGETBASE_H
#define VVTOOLWIDGETBASE_H

#include <QtDesigner/QDesignerExportWidget>
#include "ui_vvToolWidgetBase.h"
#include "clitkImageToImageGenericFilter.h"

// qt
 #include <QCloseEvent>

class vvMainWindowBase;

//------------------------------------------------------------------------------
class vvToolWidgetBase:
  public QWidget, 
  public Ui::vvToolWidgetBase 
{
  Q_OBJECT
  public:

  vvToolWidgetBase(vvMainWindowBase * parent=0, Qt::WindowFlags f=Qt::Dialog, bool initialize=true); // default is a dialog
  virtual ~vvToolWidgetBase();

  virtual void InputIsSelected(vvSlicerManager *m);
  virtual void InputIsSelected(std::vector<vvSlicerManager*> & l);
  void AddInputSelector(QString s, clitk::ImageToImageGenericFilterBase * f, bool allowSkip=false);
  void AddInputSelector(QString s, bool allowSkip=false);
  void HideInputSelector();
  void accept();
  void reject();

public slots:
  virtual void apply() = 0;
  virtual bool close();
  void InputIsSelected();
  virtual void AnImageIsBeingClosed(vvSlicerManager * m);
  virtual void SelectedImageHasChanged(vvSlicerManager * m);
  void show();
  virtual void keyPressEvent(QKeyEvent *event);

protected:
  virtual void Initialization();
  void InitializeInputs();
  Ui::vvToolWidgetBase ui;
  clitk::ImageToImageGenericFilterBase::Pointer mFilter;
  vvMainWindowBase * mMainWindow;
  std::vector<vvSlicerManager*> mSlicerManagersCompatible;
  vvSlicerManager * mCurrentSlicerManager;
  int mCurrentCompatibleIndex;
  vvImage * mCurrentImage;
  bool mIsInitialized;
  QWidget * mWidgetForTab;
  
  // The static members manage all tool instances
  static QWidget * mStaticWidgetForTab; // <-- MUST BE CHANGED IN VECTOR 
  static int mTabNumber;
  static QVBoxLayout * mStaticVerticalLayout;  // <-- MUST BE CHANGED IN VECTOR 
  static bool mIsAnotherToolWaitInput;
  virtual void CheckInputList(std::vector<vvSlicerManager*> & l, int & index) { }
  virtual void closeEvent(QCloseEvent *event);
  void SwapCurrentWidget();
  bool mPreventToUseTwoToolsOnSameInput;

}; // end class vvToolWidgetBase
//------------------------------------------------------------------------------

#endif

