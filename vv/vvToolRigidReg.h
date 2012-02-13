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
#ifndef VVTOOLRIGIDREG_H
#define VVTOOLRIGIDREG_H

#include "vvToolBase.h"
#include "vvToolWidgetBase.h"
#include "vvMainWindow.h"
#include "ui_vvToolRigidReg.h"
#include "vtkMatrix4x4.h"

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
  static void Initialize();
  virtual void InputIsSelected(vvSlicerManager *input);

public slots:
  virtual void apply();
  virtual bool close();
  virtual void reject();
  void SetTranslationStep(double v);
  void SetRotationStep(double v);
  void SliderChange(int newVal);
  void SpinBoxChange(double newVal);
  void ToggleSpinBoxAnglesUnit();
  void SaveFile();
  void LoadFile();
  void ChangeOfRotationCenter();
  void ResetTransform();

protected:
  Ui::vvToolRigidReg ui;
  vvSlicerManager * mInput;
  vtkSmartPointer<vtkMatrix4x4> mInitialMatrix;
  void SetTransform(vtkMatrix4x4 * matrix);
  void GetSlidersAndSpinBoxes(std::vector<QSlider *>&transSliders, std::vector<QSlider *>&rotSliders,
                              std::vector<QDoubleSpinBox *>&transSBs, std::vector<QDoubleSpinBox *>&rotSBs);
  void Render();
}; // end class vvToolRigidReg
//------------------------------------------------------------------------------

#endif
  
