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
#ifndef VVTOOLPROFILE_H
#define VVTOOLPROFILE_H

#include <QtGlobal>
#include <QtDesigner/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvImageContour.h"
#include "vvLandmarks.h"
#include "ui_vvToolProfile.h"
#include "vvBinaryImageOverlayActor.h"

#include "clitkProfileImage_ggo.h"
#include "clitkProfileImageGenericFilter.h"
#include <vtkTable.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkTransformPolyDataFilter.h>

//------------------------------------------------------------------------------
class vvToolProfile:
  public vvToolWidgetBase,
  public vvToolBase<vvToolProfile>, 
  private Ui::vvToolProfile
{
  Q_OBJECT
    public:
  vvToolProfile(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolProfile();

  //-----------------------------------------------------
  static void Initialize();
  void InitializeLine();
  void GetArgsInfoFromGUI();
  virtual void InputIsSelected(vvSlicerManager * m);

  bool isPointsSelected();
  void computeProfile();
  void SetPoints();

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  
  void selectPoint1();
  void selectPoint2();
  void cancelPoints();
  void SaveAs();
  void DisplayLine(int);
  void DeleteLine(int);

 protected:
  void RemoveVTKObjects();
  virtual void closeEvent(QCloseEvent *event);
  Ui::vvToolProfile ui;
  args_info_clitkProfileImage mArgsInfo;
  
  double* mPoint1;
  double* mPoint2;
  bool mPoint1Selected;
  bool mPoint2Selected;
  vtkSmartPointer<vtkContextView> mView;
  clitk::ProfileImageGenericFilter::Pointer mFilter;
  std::string mTextFileName;
  std::vector<vtkSmartPointer<vtkActor> > mLineActors;
  vtkSmartPointer<vtkPolyData> mLinesPolyData;
  vtkSmartPointer<vtkTransformPolyDataFilter> mLineTransform;


}; // end class vvToolProfile
//------------------------------------------------------------------------------

#endif

