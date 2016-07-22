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
#ifndef VVWIPEIMAGE_H
#define VVWIPEIMAGE_H

//qt
#include <QtGlobal>
#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>
#include <QSlider>
// vv
#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvMainWindowBase.h"
//#include "ui_vvWipeImage.h"
//vtk
#include <vtkTransform.h>

//------------------------------------------------------------------------------
class vvWipeImage:
  public vvToolWidgetBase,
  public vvToolBase<vvWipeImage>, 
  private Ui::vvWipeImage 
{
  Q_OBJECT
    public:
  vvWipeImage(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvWipeImage();

  virtual void InputIsSelected(vvSlicerManager *m);

public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  void crossPointerChanged();


  //-----------------------------------------------------
  static void Initialize() {
    SetToolName("Wipe");
    SetToolMenuName("Wipe");
    SetToolIconFilename(":/common/icons/crop.png");
    SetToolTip("Wipe 2 images.");
  }

 protected:
  vtkSmartPointer<vtkImageRectilinearWipe> mWipe;
  void UpdateWipe();
  virtual void closeEvent(QCloseEvent *event);

}; // end class vvWipeImage
//------------------------------------------------------------------------------

#endif

