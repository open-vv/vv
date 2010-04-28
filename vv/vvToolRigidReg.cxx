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
#include "vvToolRigidReg.h"
#include <QComboBox>
#include <QCursor>
#include <QApplication>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include "vvImage.h"
#include "vvSlicer.h"
#include <QString>
#include <QMessageBox>
#include <vvMainWindow.h>
//------------------------------------------------------------------------------
// Create the tool and automagically (I like this word) insert it in
// the main window menu.
ADD_TOOL(vvToolRigidReg);
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolRigidReg::vvToolRigidReg(vvMainWindowBase * parent, Qt::WindowFlags f):
    vvToolWidgetBase(parent, f),
    vvToolBase<vvToolRigidReg>(parent),
    Ui::vvToolRigidReg()
{
  // GUI Initialization
  Ui_vvToolRigidReg::setupUi(mToolWidget);

  // Set how many inputs are needed for this tool
  AddInputSelector("Select the Reference Image");
  AddInputSelector("Select the Target Image");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolRigidReg::~vvToolRigidReg()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolRigidReg::close()
{
  return vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolRigidReg::reject()
{
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolRigidReg::InputIsSelected(std::vector<vvSlicerManager *> & l)
{
  //inputs
  mInput1 = l[0];
  mInput2 = l[1];
  mTwoInputs = true;


  SetOverlay();
  for (int j = 1; j <= 4; j++) {
    mCurrentSlicerManager->GetSlicer(j-1)->SetActorVisibility("overlay",0,true);
  }
  mCurrentSlicerManager->Render();

  std::vector<int> imagesize;
  imagesize=mCurrentSlicerManager->GetImage()->GetSize();
  std::vector<double> imageorigin;
  imageorigin=mCurrentSlicerManager->GetImage()->GetOrigin();
  QString xcord,ycord,zcord;

  //default image rotation center is the center of the image
  xcord=xcord.setNum(imageorigin[0]+imagesize[0]*mCurrentSlicerManager->GetImage()->GetSpacing()[0]/2, 'g', 3);
  ycord=ycord.setNum(imageorigin[1]+imagesize[1]*mCurrentSlicerManager->GetImage()->GetSpacing()[1]/2, 'g', 3);
  zcord=zcord.setNum(imageorigin[2]+imagesize[2]*mCurrentSlicerManager->GetImage()->GetSpacing()[2]/2, 'g', 3);

  Xval->setText(xcord);
  Yval->setText(ycord);
  Zval->setText(zcord);

  xtrans_slider->setMaximum(2*mCurrentSlicerManager->GetImage()->GetSize()[0]);
  xtrans_slider->setMinimum(-2*mCurrentSlicerManager->GetImage()->GetSize()[0]);
  ytrans_slider->setMaximum(2*mCurrentSlicerManager->GetImage()->GetSize()[1]);
  ytrans_slider->setMinimum(-2*mCurrentSlicerManager->GetImage()->GetSize()[1]);
  ztrans_slider->setMaximum(2*mCurrentSlicerManager->GetImage()->GetSize()[2]);
  ztrans_slider->setMinimum(-2*mCurrentSlicerManager->GetImage()->GetSize()[2]);


  xtrans_sb->setMaximum(2*mCurrentSlicerManager->GetImage()->GetSize()[0]);
  xtrans_sb->setMinimum(-2*mCurrentSlicerManager->GetImage()->GetSize()[0]);
  ytrans_sb->setMaximum(2*mCurrentSlicerManager->GetImage()->GetSize()[1]);
  ytrans_sb->setMinimum(-2*mCurrentSlicerManager->GetImage()->GetSize()[1]);
  ztrans_sb->setMaximum(2*mCurrentSlicerManager->GetImage()->GetSize()[2]);
  ztrans_sb->setMinimum(-2*mCurrentSlicerManager->GetImage()->GetSize()[2]);

  xrot_slider->setMaximum(360);
  xrot_slider->setMinimum(-360);
  yrot_slider->setMaximum(360);
  yrot_slider->setMinimum(-360);
  zrot_slider->setMaximum(360);
  zrot_slider->setMinimum(-360);


  xrot_sb->setMaximum(360);
  xrot_sb->setMinimum(-360);
  yrot_sb->setMaximum(360);
  yrot_sb->setMinimum(-360);
  zrot_sb->setMaximum(360);
  zrot_sb->setMinimum(-360);

  //connect all sigs to slots
  connect(Xval, SIGNAL(editingFinished()), this, SLOT(SetXvalue()));
  connect(Yval, SIGNAL(editingFinished()), this, SLOT(SetYvalue()));
  connect(Zval, SIGNAL(editingFinished()), this, SLOT(SetZvalue()));
  connect(xtrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateXtranslider()));
  connect(ytrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateYtranslider()));
  connect(ztrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateZtranslider()));
  connect(xrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateXrotslider()));
  connect(yrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateYrotslider()));
  connect(zrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateZrotslider()));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::apply()
{

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOverlay()
{
  for (int i =0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetSlicer(i)->SetOverlay(mInput2->GetImage());
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetXvalue()
{
  QString xstr = Xval->text();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetYvalue()
{
  QString ystr = Yval->text();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetZvalue()
{
  QString zstr = Zval->text();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetTransform(double tX, double tY, double tZ, double aX, double aY, double aZ)
{

  vtkSmartPointer<vtkTransform> transform = mCurrentSlicerManager->GetImage()->GetTransform();
  transform->PostMultiply();

  //Rotations
  if (aX!=0 || aY!=0 || aZ!=0) {
    double x, y ,z;
    x= Xval->text().toDouble();
    y= Yval->text().toDouble();
    z= Zval->text().toDouble();
    transform->Translate(-x,-y,-z);
    if (aX!=0) transform->RotateX(aX);
    if (aY!=0) transform->RotateY(aY);
    if (aZ!=0) transform->RotateZ(aZ);
    transform->Translate(x,y,z);
  }

  //Translations
  if (tX!=0 || tY!=0 || tZ!=0)
    transform->Translate(tX,tY,tZ);

  transform->Update();

  for (int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetImage()->UpdateReslice();
    mCurrentSlicerManager->GetSlicer(i)->Render();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXtranslider()
{

  int diff=0;
  diff=xtrans_slider->value()-xtrans_sb->value();
  this->SetTransform(diff, 0, 0, 0, 0, 0);
  xtrans_sb->setValue(xtrans_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYtranslider()
{
  int diff=0;
  diff=ytrans_slider->value()-ytrans_sb->value();
  this->SetTransform(0, diff, 0, 0, 0, 0);
  ytrans_sb->setValue(ytrans_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZtranslider()
{
  int diff=0;
  diff=ztrans_slider->value()-ztrans_sb->value();
  this->SetTransform(0, 0, diff, 0, 0, 0);
  ztrans_sb->setValue(ztrans_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXrotslider()
{
  int diff=0;
  diff=xrot_slider->value()-xrot_sb->value();
  this->SetTransform(0, 0, 0, diff, 0, 0);
  xrot_sb->setValue(xrot_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYrotslider()
{
  int diff=0;
  diff=yrot_slider->value()-yrot_sb->value();
  this->SetTransform(0, 0, 0, 0, diff, 0);
  yrot_sb->setValue(yrot_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZrotslider()
{
  int diff=0;
  diff=zrot_slider->value()-zrot_sb->value();
  this->SetTransform(0, 0, 0, 0, 0, diff);
  zrot_sb->setValue(zrot_slider->value());
}
