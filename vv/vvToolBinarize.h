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
#ifndef VVTOOLBINARIZE_H
#define VVTOOLBINARIZE_H

#include <QtGlobal>
#include <QtDesigner/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvImageContour.h"
#include "ui_vvToolBinarize.h"

#include "clitkBinarizeImage_ggo.h"

//------------------------------------------------------------------------------
class vvToolBinarize:
  public vvToolWidgetBase,
  public vvToolBase<vvToolBinarize>, 
  private Ui::vvToolBinarize 
{
  Q_OBJECT
    public:
  vvToolBinarize(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolBinarize();

  //-----------------------------------------------------
  static void Initialize();
  void GetArgsInfoFromGUI();
  virtual void InputIsSelected(vvSlicerManager * m);

  //-----------------------------------------------------
  public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  void valueChangedT1(double v);
  void valueChangedT2(double v);
  void UpdateOrientation(int slicer, int orientation);
  void UpdateSlice(int slicer,int slices, int code=0);
  void enableLowerThan(bool b);
  void useFGBGtoggled(bool);
  void InteractiveDisplayToggled(bool b);
  //  void LeftButtonReleaseEvent(int slicer);

 protected:
  void RemoveVTKObjects();
  virtual void closeEvent(QCloseEvent *event);
  Ui::vvToolBinarize ui;
  args_info_clitkBinarizeImage mArgsInfo;
  std::vector<vvImageContour::Pointer> mImageContour;
  std::vector<vvImageContour::Pointer> mImageContourLower;
  bool mInteractiveDisplayIsEnabled;
  
  void Update(int slicer);

}; // end class vvToolBinarize
//------------------------------------------------------------------------------

#endif

