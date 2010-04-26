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
  Ui::vvToolRigidReg() {
  // GUI Initialization
  Ui_vvToolRigidReg::setupUi(mToolWidget);
  
  // Set how many inputs are needed for this tool
  AddInputSelector("Select the Reference Image");
  AddInputSelector("Select the Target Image");
  }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvToolRigidReg::~vvToolRigidReg() {
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvToolRigidReg::close() { 
  return vvToolWidgetBase::close(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolRigidReg::reject() { 
  return vvToolWidgetBase::reject(); 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolRigidReg::InputIsSelected(std::vector<vvSlicerManager *> & l) {
  //inputs
  mInput1 = l[0];
  mInput2 = l[1];  
  mTwoInputs = true;
  
  
  SetOverlay();
  for (int j = 1; j <= 4; j++)
    {
     mCurrentSlicerManager->GetSlicer(j-1)->SetActorVisibility("overlay",0,true);
    }
  mCurrentSlicerManager->Render();

  origin = new double[3];
  std::vector<int> imagesize;
  imagesize=mCurrentSlicerManager->GetImage()->GetSize();
  QString xcord,ycord,zcord;

  //default image origin is its centroid 
  xcord=xcord.setNum(imagesize[0]*mCurrentSlicerManager->GetImage()->GetSpacing()[0]/2);
  ycord=ycord.setNum(imagesize[1]*mCurrentSlicerManager->GetImage()->GetSpacing()[1]/2);
  zcord=zcord.setNum(imagesize[2]*mCurrentSlicerManager->GetImage()->GetSpacing()[2]/2);
  
  Xval->setText(xcord);Yval->setText(ycord);Zval->setText(zcord);
  origin[0]=xcord.toDouble();origin[1]=ycord.toDouble();origin[2]=zcord.toDouble();
  this->mCurrentSlicerManager->GetImage()->SetOrigin(origin);
 
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
void vvToolRigidReg::apply() {
  
}
//------------------------------------------------------------------------------
  
//------------------------------------------------------------------------------
void vvToolRigidReg::SetOverlay()
{
  for(int i =0;i<mCurrentSlicerManager->NumberOfSlicers();i++)
  {
    mCurrentSlicerManager->GetSlicer(i)->SetOverlay(mInput2->GetImage());
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetXvalue() {
  QString xstr = Xval->text();
  origin[0]= xstr.toDouble();
  this->mCurrentSlicerManager->GetImage()->SetOrigin(origin);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetYvalue() {
  QString ystr = Yval->text();
  origin[1]= ystr.toDouble();
  this->mCurrentSlicerManager->GetImage()->SetOrigin(origin);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetZvalue() {
  QString zstr = Zval->text();
  origin[2]= zstr.toDouble();
  this->mCurrentSlicerManager->GetImage()->SetOrigin(origin);

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXtranslider(){
    
    int diff=0; 
    diff=xtrans_slider->value()-xtrans_sb->value();
    mCurrentSlicerManager->GetImage()->SetTranslationX(diff);//perform the translation difference  
    for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) 
     {
      mCurrentSlicerManager->GetSlicer(i)->Render();    
     }
   xtrans_sb->setValue(xtrans_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYtranslider()
{  
    int diff=0; 
    diff=ytrans_slider->value()-ytrans_sb->value();
    mCurrentSlicerManager->GetImage()->SetTranslationY(diff);//perform the translation difference
    for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) 
     {
      mCurrentSlicerManager->GetSlicer(i)->Render();    
     }
   ytrans_sb->setValue(ytrans_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZtranslider()
{
    int diff=0; 
    diff=ztrans_slider->value()-ztrans_sb->value();
    mCurrentSlicerManager->GetImage()->SetTranslationZ(diff);
    for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) 
     {
      mCurrentSlicerManager->GetSlicer(i)->Render();    
     }
   ztrans_sb->setValue(ztrans_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXrotslider()
{
    int diff=0; 
    diff=xrot_slider->value()-xrot_sb->value();
    mCurrentSlicerManager->GetImage()->SetRotateX(diff);
    for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) 
     {
      mCurrentSlicerManager->GetSlicer(i)->Render();    
     }
   xrot_sb->setValue(xrot_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYrotslider()
{
  int diff=0; 
    diff=yrot_slider->value()-yrot_sb->value();
      mCurrentSlicerManager->GetImage()->SetRotateY(diff);
    for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) 
     {
      mCurrentSlicerManager->GetSlicer(i)->Render();    
     }
   yrot_sb->setValue(yrot_slider->value());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZrotslider()
{
    int diff=0; 
    diff=zrot_slider->value()-zrot_sb->value();
    mCurrentSlicerManager->GetImage()->SetRotateZ(diff);
    for(int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) 
     {
      mCurrentSlicerManager->GetSlicer(i)->Render();    
     }
   zrot_sb->setValue(zrot_slider->value());
}
