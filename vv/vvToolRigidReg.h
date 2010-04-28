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
#ifndef VVTOOLRIGIDREG_H
#define VVTOOLRIGIDREG_H
#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvMainWindowBase.h"
#include "vvMainWindow.h"
#include "ui_vvToolRigidReg.h"

//------------------------------------------------------------------------------
class vvToolRigidReg:
    public vvToolWidgetBase,
    public vvToolBase<vvToolRigidReg>,
    private Ui::vvToolRigidReg
{
  Q_OBJECT
public:
  vvToolRigidReg(vvMainWindowBase * parent=0, Qt::WindowFlags f=0);
  ~vvToolRigidReg();
  virtual void InputIsSelected(std::vector<vvSlicerManager *> & m);
public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  //-----------------------------------------------------
  static void Initialize() {
    SetToolName("Register");
    SetToolMenuName("Register");
    SetToolIconFilename(":/common/icons/register.png");
    SetToolTip("Register Image.");
  }
  void SetXvalue();
  void SetYvalue();
  void SetZvalue();
  void UpdateXtranslider();
  void UpdateYtranslider();
  void UpdateZtranslider();
  void UpdateXrotslider();
  void UpdateYrotslider();
  void UpdateZrotslider();
  void SetOverlay();
protected:
  Ui::vvToolRigidReg ui;
  vvSlicerManager * mInput1;
  vvSlicerManager * mInput2;
  vvMainWindow * mWindow;
  bool mTwoInputs;
  void SetTransform(double tX, double tY, double tZ, double aX, double aY, double aZ);
}; // end class vvToolRigidReg
//------------------------------------------------------------------------------

#endif

