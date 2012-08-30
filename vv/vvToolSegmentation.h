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
#ifndef VVTOOLSEGMENTATION_H
#define VVTOOLSEGMENTATION_H

#include <QtDesigner/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvROIActor.h"
#include "ui_vvToolSegmentation.h"

//------------------------------------------------------------------------------
class vvToolSegmentation:
  public vvToolWidgetBase,
  public vvToolBase<vvToolSegmentation>, 
  private Ui::vvToolSegmentation 
{
  Q_OBJECT
    public:
  vvToolSegmentation(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolSegmentation();

  //-----------------------------------------------------
  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager * m);
  void OpenBinaryImage();
  void Erode();
  void Dilate();
  void UpdateAndRender();

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  virtual void keyPressEvent(QKeyEvent * event);
  virtual bool close();
  // virtual void reject();

 protected:
  // virtual void closeEvent(QCloseEvent *event);
  Ui::vvToolSegmentation ui;
  QSharedPointer<vvROIActor> mRefMaskActor;
  vvImage::Pointer mMaskImage;
  int mKernelValue;

}; // end class vvToolSegmentation
//------------------------------------------------------------------------------

#endif
