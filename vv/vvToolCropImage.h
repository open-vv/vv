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
#ifndef VVTOOLCROPIMAGE_H
#define VVTOOLCROPIMAGE_H

//qt
#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif
#include <QDialog>
#include <QSlider>
// vv
#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvMainWindowBase.h"
#include "ui_vvToolCropImage.h"
//vtk
#include <vtkTransform.h>

//------------------------------------------------------------------------------
class vvToolCropImage:
  public vvToolWidgetBase,
  public vvToolBase<vvToolCropImage>, 
  private Ui::vvToolCropImage 
{
  Q_OBJECT
    public:
  vvToolCropImage(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolCropImage();

  virtual void InputIsSelected(vvSlicerManager *m);

public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  void autoCropValueChanged(double v);
  void sliderValueChanged(int id);


  //-----------------------------------------------------
  static void Initialize() {
    SetToolName("Crop");
    SetToolMenuName("Crop");
    SetToolIconFilename(":/common/icons/crop.png");
    SetToolTip("Crop image.");
  }

 protected:
  Ui::vvToolCropImage ui;
  int * mReducedExtent;
  int * mInitialExtent;
  int mExtentSize;
  void UpdateExtent();
  virtual void closeEvent(QCloseEvent *event);

  std::vector<QSlider*> mSliders;
  vtkSmartPointer<vtkMatrix4x4> mConcatenedTransform;
}; // end class vvToolCropImage
//------------------------------------------------------------------------------

#endif

