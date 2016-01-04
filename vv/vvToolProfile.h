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

#include <QtUiPlugin/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvImageContour.h"
#include "ui_vvToolProfile.h"

#include "clitkProfileImage_ggo.h"
#include "clitkProfileImageGenericFilter.h"
#include <vtkTable.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>

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
  void GetArgsInfoFromGUI();
  virtual void InputIsSelected(vvSlicerManager * m);

  bool isPointsSelected();

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  
  void selectPoint1();
  void selectPoint2();
  void cancelPoints();
  void computeProfile();

 protected:
  void RemoveVTKObjects();
  virtual void closeEvent(QCloseEvent *event);
  Ui::vvToolProfile ui;
  args_info_clitkProfileImage mArgsInfo;
  
  int* mPoint1;
  int* mPoint2;
  bool mPoint1Selected;
  bool mPoint2Selected;
  vtkSmartPointer<vtkContextView> mView;
  clitk::ProfileImageGenericFilter::Pointer mFilter;


}; // end class vvToolProfile
//------------------------------------------------------------------------------

#endif

