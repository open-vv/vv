
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
#include "vtkMatrix4x4.h"
#include "clitkAffineRegistration_ggo.h"

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
    SetToolExperimental(true);
  }
  virtual void GetArgsInfoFromGUI();
  void SetOverlay(vvImage::Pointer Image);
  void RemoveOverlay();
  void SetXvalue();
  void SetYvalue();
  void SetZvalue();
  void Render();
  void UpdateTextEditor(vtkMatrix4x4 *matrix,QTextEdit *textEdit);
  void UpdateTransform_sliders();
  void UpdateTransform_sb();
  void UpdateTransform(bool slider_enabled);  
  void AutoRegister();
  void SaveFile();
  void ReadFile(bool matrix_given);
  void LoadFile();
  void ResetTransform();
  void SetRotationCenter();
  void SetSliderRanges();
  void InitializeSliders(double xtrans,double ytrans, double ztrans, double xrot, double yrot, double zrot,bool sliders);
  void TransformSelect();
  void OptimizerSelect();
  void InterpolatorSelect();
  void MetricSelect();
  void OutputSelect();
  void SaveTextEdit();
  void CmdlineParser(int override, int initialize);
  void Presets();
  void UpdateTextEditor2();
  protected:
  Ui::vvToolRigidReg ui;
  vvSlicerManager * mInput1;
  vvSlicerManager * mInput2;
  vvMainWindow * mWindow;
  std::string mConfigFile;
  bool mTwoInputs;
  std::vector<int> mImageSize;
  args_info_clitkAffineRegistration mArgsInfo;
  double mInitialMatrix[16];
  void SetTransform(double tX, double tY, double tZ, double aX, double aY, double aZ,bool update);
  void SetTransform(vtkMatrix4x4 * matrix);
}; // end class vvToolRigidReg
//------------------------------------------------------------------------------

#endif
  
