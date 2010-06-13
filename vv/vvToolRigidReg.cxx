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
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <vvMainWindow.h>
#include <QFileDialog>
#include <QTextStream>
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
   QSize qsize;
   qsize.setHeight(170);
   qsize.setWidth(650);
   textEdit->setFixedSize(qsize);

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
    ResetTransform();
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::InputIsSelected(std::vector<vvSlicerManager *> & l)
{
  //inputs
  mInput1 = l[0];
  mInput2 = l[1];
  if(mInput1->GetFileName()==mInput2->GetFileName())
  {  
    QMessageBox::information(this, "Warning","Your Reference and Target Images are the same");
  } 
  mTwoInputs = true;
  SetOverlay();
  mImageSize=mInput1->GetImage()->GetSize();
  
  SetRotationCenter();
  SetSliderRanges();
  
    //connect all sigs to slots	
  connect(resetbutton, SIGNAL(pressed()), this, SLOT(ResetTransform()));
  
  connect(Xval, SIGNAL(editingFinished()), this, SLOT(SetXvalue()));
  connect(Yval, SIGNAL(editingFinished()), this, SLOT(SetYvalue()));
  connect(Zval, SIGNAL(editingFinished()), this, SLOT(SetZvalue()));
  connect(xtrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateXtranslider()));
    
  connect(ytrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateYtranslider()));
  connect(ztrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateZtranslider()));
  
  
  connect(xrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateXrotslider()));
  connect(yrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateYrotslider()));
  connect(zrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateZrotslider()));
  
	
   connect(xtrans_sb, SIGNAL(valueChanged(int)), this, SLOT(UpdateXtransb()));
   connect(ytrans_sb, SIGNAL(valueChanged(int)), this, SLOT(UpdateYtransb()));
   connect(ztrans_sb, SIGNAL(valueChanged(int)), this, SLOT(UpdateZtransb()));
   connect(xrot_sb, SIGNAL(valueChanged(int)), this, SLOT(UpdateXrotsb()));
   connect(yrot_sb, SIGNAL(valueChanged(int)), this, SLOT(UpdateYrotsb()));
   connect(zrot_sb, SIGNAL(valueChanged(int)), this, SLOT(UpdateZrotsb()));
   
  
  connect(loadbutton, SIGNAL(pressed()), this, SLOT(ReadFile()));
  connect(savebutton, SIGNAL(pressed()), this, SLOT(SaveFile()));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::apply()
{
 QMessageBox mb( "Keep Overlay",
                    "Do you want to keep the overlay on?\n"
                    "You can switch it off anytime by pressing 'x'",
                    QMessageBox::Information,
                    QMessageBox::Yes | QMessageBox::Default,
                    QMessageBox::No,
                    QMessageBox::Cancel | QMessageBox::Escape );
    mb.setButtonText( QMessageBox::Yes, "Yes" );
    mb.setButtonText( QMessageBox::No, "No" );
    switch( mb.exec() ) {
        case QMessageBox::Yes:
          hide();
	  break;
        case QMessageBox::No:
         mInput2->Reload();
	 mInput2->Render();
	 mInput1->Reload(); 
	 mInput1->Render();
	 for(int i=0;i<mCurrentSlicerManager->NumberOfSlicers();i++)
	 {
	   mInput1->RemoveActor("overlay",0);
	   mInput1->SetColorMap(0);
	   mInput1->Render();
	   hide();
	 }
	 break;
        case QMessageBox::Cancel:
            break;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOverlay()
{
    for (int i =0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
   mCurrentSlicerManager->GetSlicer(i)->SetOverlay(mInput2->GetImage());
   mCurrentSlicerManager->GetSlicer(i)->SetActorVisibility("overlay",0,true);	
   mCurrentSlicerManager->SetColorMap();
   mCurrentSlicerManager->Render();
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
void vvToolRigidReg::SetTransform(double tX, double tY, double tZ, double aX, double aY, double aZ,bool update)
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
  if (tX!=0||tY!=0||tZ!=0)
    transform->Translate(tX*mInput1->GetImage()->GetSpacing()[0],tY*mInput1->GetImage()->GetSpacing()[1],tZ*mInput1->GetImage()->GetSpacing()[2]);

    transform->Update();
  if(update)
  {
    for (int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
      mCurrentSlicerManager->GetImage()->UpdateReslice();
      mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();
      mCurrentSlicerManager->GetSlicer(i)->Render();
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetTransform(vtkMatrix4x4 *matrix)
{
    vtkSmartPointer<vtkTransform> transform =mCurrentSlicerManager->GetImage()->GetTransform();
    transform->SetMatrix(matrix);
    transform->PostMultiply();
    transform->Concatenate(matrix);
    transform->Update();
    mCurrentSlicerManager->GetImage()->SetTransform(transform);
   
    for (int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
      mCurrentSlicerManager->GetImage()->UpdateReslice();
      mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();
      mCurrentSlicerManager->GetSlicer(i)->Render();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
  void vvToolRigidReg::UpdateTextEditor(vtkMatrix4x4 *matrix)
{
   QString str1,str2,str3;
    QColor color;
    textEdit->clear();
    textEdit->setAcceptRichText(true);

    str2=textEdit->toPlainText();
    str2.append("Rotation Center(mm): \n");
    textEdit->setText(str2);
    
    str2=textEdit->toPlainText();
    textEdit->setTextColor(QColor(255,0,0));
    str2.append(str3.append(Xval->text()));
    textEdit->setText(str2);
    str3.clear();

    str2=textEdit->toPlainText();
    str2.append("\t");
    textEdit->setText(str2);

    str2=textEdit->toPlainText();
    str2.append(str3.append(Yval->text()));
    textEdit->setText(str2);
    str3.clear();
    
    str2=textEdit->toPlainText();
    str2.append("\t");
    textEdit->setText(str2);
    

    str2=textEdit->toPlainText();
    str2.append(str3.append(Zval->text()));
    textEdit->setText(str2);


    str2=textEdit->toPlainText();
    str2.append("\n");
    textEdit->setText(str2);
    

    str2=textEdit->toPlainText();
    str2.append("Transformation Matrix(mm):\n");
    textEdit->setText(str2);

    for(int i=0;i<4;i++)
    {
    for(int j=0;j<4;j++)
      {
    str2=textEdit->toPlainText();
   // str2.append("\t"+str1.setNum(matrix->Element[i][j]));
    str2.append(QString("%1\t").arg(str1.setNum(matrix->Element[i][j]),20));  
    textEdit->setText(str2);
      }
    str2=textEdit->toPlainText();
    str2.append("\n");  
    textEdit->setText(str2);
    }
    //QString str = QFileDialog::getOpenFileName();
    textEdit->setTextColor(QColor(255,0,0));
    textEdit->setFont(QFont("courrier new",12,4,true));
    textEdit->toPlainText().toAscii();
    
    str2=textEdit->toPlainText();
    textEdit->setText(str2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXtransb()
{
  int diff=0;
  diff=xtrans_sb->value()-xtrans_slider->value();
  this->SetTransform(diff, 0, 0, 0, 0, 0,true);
  xtrans_slider->setValue(xtrans_sb->value());
   UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
    
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXtranslider()
{
  int diff=0;
  diff=xtrans_slider->value()-xtrans_sb->value();
  this->SetTransform(diff, 0, 0, 0, 0, 0,true);
  xtrans_sb->setValue(xtrans_slider->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYtransb()
{

  int diff=0;
  diff=ytrans_sb->value()-ytrans_slider->value();
  this->SetTransform(0, diff, 0, 0, 0, 0,true);
  ytrans_slider->setValue(ytrans_sb->value());
 UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYtranslider()
{
  int diff=0;
  diff=ytrans_slider->value()-ytrans_sb->value();
  this->SetTransform(0, diff, 0, 0, 0, 0,true);
  ytrans_sb->setValue(ytrans_slider->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZtransb()
{

  int diff=0;
  diff=ztrans_sb->value()-ztrans_slider->value();
  this->SetTransform(0, 0, diff, 0, 0, 0,true);
  ztrans_slider->setValue(ztrans_sb->value());
   UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZtranslider()
{
  int diff=0;
  diff=ztrans_slider->value()-ztrans_sb->value();
  this->SetTransform(0, 0, diff, 0, 0, 0,true);
  ztrans_sb->setValue(ztrans_slider->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXrotsb()
{

  int diff=0;
  diff=xrot_sb->value()-xrot_slider->value();
  this->SetTransform(0, 0, 0, diff, 0, 0,true);
  xrot_slider->setValue(xrot_sb->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXrotslider()
{
  int diff=0;
  diff=xrot_slider->value()-xrot_sb->value();
  this->SetTransform(0, 0, 0, diff, 0, 0,true);
  xrot_sb->setValue(xrot_slider->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYrotsb()
{

  int diff=0;
  diff=yrot_sb->value()-yrot_slider->value();
  this->SetTransform(0, 0, 0, 0, diff, 0,true);
  yrot_slider->setValue(yrot_sb->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYrotslider()
{
  int diff=0;
  diff=yrot_slider->value()-yrot_sb->value();
  this->SetTransform(0, 0, 0, 0, diff, 0,true);
  yrot_sb->setValue(yrot_slider->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZrotsb()
{

  int diff=0;
  diff=zrot_sb->value()-zrot_slider->value();
  this->SetTransform(0, 0, 0, 0, 0,diff,true);
  zrot_slider->setValue(zrot_sb->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZrotslider()
{
  int diff=0;
  diff=zrot_slider->value()-zrot_sb->value();
  this->SetTransform(0, 0, 0, 0, 0, diff,true);
  zrot_sb->setValue(zrot_slider->value());
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SaveFile()
{	
   QString s = QFileDialog::getSaveFileName(this, tr("Save File"),
                            "/home",
                            tr("Text (*.txt *.doc *.rtf)"));
    QFile file(s);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&file);
    out<<textEdit->toPlainText() ;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ReadFile()
{
  vtkMatrix4x4 *matrix=vtkMatrix4x4::New();
   vtkSmartPointer<vtkTransform> transform = mCurrentSlicerManager->GetImage()->GetTransform();
   QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    "/home",
                    "Text (*.txt *.rtf *.doc)");
    if (s.isEmpty())
    return;
    
   QFile file(s);
   ifstream readfile;
   std::string x= s.toStdString();
   std::vector<QString> Qstr;
   readfile.open(x.c_str());
   if (readfile.is_open()) 
   {
    while (!readfile.eof())
      {
      readfile >> x;
      Qstr.push_back(QString(x.c_str()));
      }
      readfile.close();
   }
    else {
      cout << "Unable to open file";
    }
    //Set the transformation matrix
    int index =7;
   for(int i=0;i<4;i++)
   {
     for(int j=0;j<4;j++)
     {
       matrix->Element[i][j]=Qstr.at(index).toDouble();
       index++;
     }
   }
   transform->SetMatrix(matrix);
   // SetTransform(matrix);
    double * orientations=new double[3];
    double * translations=new double[3];
    transform->GetOrientation(orientations);
    transform->PostMultiply();
    
       //Obtain the Rotation Center 
    Xval->setText(Qstr.at(2));
    Yval->setText(Qstr.at(3));
    Zval->setText(Qstr.at(4));
 
    //In the Order or Y X Z //
    //now  postmultiply for the rotations 
     SetTransform(0,0,0,0,0,-orientations[2],false);
     SetTransform(0,0,0,-orientations[0],0,0,false);
     SetTransform(0,0,0,0,-orientations[1],0,false);
      
    transform->GetPosition(translations);
    transform->Identity();	
 
  /*  DD(translations[0]/mInput1->GetImage()->GetSpacing()[0]);
    DD(translations[1]/mInput1->GetImage()->GetSpacing()[1]);
    DD(translations[2]/mInput1->GetImage()->GetSpacing()[2]);
    DD(orientations[0]);
    DD(orientations[1]);
    DD(orientations[2]);*/
  //set the sliders  and spin box values 
    xtrans_slider->setValue(rint(translations[0]/mInput1->GetImage()->GetSpacing()[0]));
    UpdateXtranslider();
    ytrans_slider->setValue(rint(translations[1]/mInput1->GetImage()->GetSpacing()[1]));
    UpdateYtranslider();	
    ztrans_slider->setValue(rint(translations[2]/mInput1->GetImage()->GetSpacing()[2]));
    UpdateZtranslider();
    yrot_slider->setValue(rint(orientations[1]));
    UpdateYrotslider();
    xrot_slider->setValue(rint(orientations[0]));
    UpdateXrotslider();	
    zrot_slider->setValue(rint(orientations[2]));
    UpdateZrotslider();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ResetTransform()
{
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
   matrix->Identity();
   mInput1->Reload();
   mInput1->Render();
   mInput2->Reload();
   mInput2->Render(); 
   SetRotationCenter();
   SetSliderRanges();
   SetTransform(matrix);
   UpdateTextEditor(matrix);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetRotationCenter()
{
    //default image rotation center is the center of the image
    QString xcord,ycord,zcord;
    std::vector<double> imageorigin;
    imageorigin=mInput1->GetImage()->GetOrigin();
    
    xcord=xcord.setNum(imageorigin[0]+mImageSize[0]*mInput1->GetImage()->GetSpacing()[0]/2, 'g', 3);
    ycord=ycord.setNum(imageorigin[1]+mImageSize[1]*mInput1->GetImage()->GetSpacing()[1]/2, 'g', 3);
    zcord=zcord.setNum(imageorigin[2]+mImageSize[2]*mInput1->GetImage()->GetSpacing()[2]/2, 'g', 3);

    Xval->setText(xcord);
    Yval->setText(ycord);
    Zval->setText(zcord);
    InitializeSliders();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::InitializeSliders()
{
     xtrans_slider->signalsBlocked();
     ytrans_slider->signalsBlocked();
     ztrans_slider->signalsBlocked();
     xtrans_sb->signalsBlocked();
     ytrans_sb->signalsBlocked();
     ztrans_sb->signalsBlocked();
     xrot_slider->signalsBlocked();
     yrot_slider->signalsBlocked();
     zrot_slider->signalsBlocked();
     xrot_sb->signalsBlocked();
     yrot_sb->signalsBlocked();
     zrot_sb->signalsBlocked();
     
    xtrans_sb->setValue(0);
    ytrans_sb->setValue(0);
    ztrans_sb->setValue(0);
    
    xtrans_slider->setValue(0);
    ytrans_slider->setValue(0);
    ztrans_slider->setValue(0);
    
    xrot_sb->setValue(0);
    yrot_sb->setValue(0);
    zrot_sb->setValue(0);

    xrot_slider->setValue(0);
    yrot_slider->setValue(0);
    zrot_slider->setValue(0);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetSliderRanges()
{
  xtrans_slider->setMaximum(1000);
  xtrans_slider->setMinimum(-1000);
  ytrans_slider->setMaximum(1000);
  ytrans_slider->setMinimum(-1000);
  ztrans_slider->setMaximum(1000);
  ztrans_slider->setMinimum(-1000);

  xtrans_sb->setMaximum(1000);
  xtrans_sb->setMinimum(-1000);
  ytrans_sb->setMaximum(1000);
  ytrans_sb->setMinimum(-1000);
  ztrans_sb->setMaximum(1000);
  ztrans_sb->setMinimum(-1000);

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
}
