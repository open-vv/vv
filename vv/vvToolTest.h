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
#ifndef VVTOOLTEST_H
#define VVTOOLTEST_H

#include <QtUiPlugin/QDesignerExportWidget>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvImageContour.h"
#include "ui_vvToolTest.h"


//------------------------------------------------------------------------------
class vvToolTest:
  public vvToolWidgetBase,
  public vvToolBase<vvToolTest>,
  private Ui::vvToolTest 
{
  Q_OBJECT
    public:
  vvToolTest(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolTest();

  //-----------------------------------------------------
  static void Initialize();
  //void GetArgsInfoFromGUI();
  virtual void InputIsSelected(vvSlicerManager * m);

  //-----------------------------------------------------
  public slots:
  virtual bool close();
  virtual void reject();
  virtual void apply();
  void UpdateSlice(int slicer,int slices);
  void valueChangedT1();

 protected:
  Ui::vvToolTest ui;
  
  void Update(int slicer);

}; // end class vvToolTest
//------------------------------------------------------------------------------

#endif

