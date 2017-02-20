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
#ifndef VVTOOLHISTOGRAM_H
#define VVTOOLHISTOGRAM_H

#include <QtGlobal>
#include <QtDesigner/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "ui_vvToolHistogram.h"

#include "clitkHistogramImage_ggo.h"
#include "clitkHistogramImageGenericFilter.h"
#include <vtkTable.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkInteractorObserver.h>
#include <vtkEventQtSlotConnect.h>

//------------------------------------------------------------------------------
class vvToolHistogram:
  public vvToolWidgetBase,
  public vvToolBase<vvToolHistogram>, 
  private Ui::vvToolHistogram
{
  Q_OBJECT
    public:
  vvToolHistogram(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolHistogram();

  //-----------------------------------------------------
  static void Initialize();
  void GetArgsInfoFromGUI();
  virtual void InputIsSelected(vvSlicerManager * m);

  void computeHistogram();
  void changeWindowLevel();
  void computeWindowLevel();
  void computeMinMax();
  void SetPoints();

  void displayHistogram();

  QVTKWidget* GetHistogramWidget();

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();

  void windowLevelChanged();
  void reduceWindow();
  void expandWindow();
  void translateWindow(double x);

  void SaveAs();

 protected:
  void RemoveVTKObjects();
  virtual void closeEvent(QCloseEvent *event);
  Ui::vvToolHistogram ui;
  args_info_clitkHistogramImage mArgsInfo;

  double mMinWindowLevel;
  double mMaxWindowLevel;
  double originalWindow;
  double originalLevel;
  bool mModificationsDone;

  vtkSmartPointer<vtkContextView> mView;

  vtkSmartPointer<vtkTable> mTableWindowLevel;
  vtkSmartPointer<vtkTable> mTable;
  //vtkSmartPointer<vtkContextView> mView;
  clitk::HistogramImageGenericFilter::Pointer mFilter;
  std::string mTextFileName;

}; // end class vvToolHistogram
//------------------------------------------------------------------------------

#endif

