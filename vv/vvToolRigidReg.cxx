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

  // Set how many inputs are needed for this tool
  AddInputSelector("Select the Reference Image");
  AddInputSelector("Select the Target Image");
    mOrder=QString("Rotation Order: ");
 
   
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
  mInput2->Reload();	
  for(int i=0;i<mCurrentSlicerManager->NumberOfSlicers();i++)
      { 
	mInput1->GetSlicer(i)->RemoveActor("overlay",0);
	mInput1->SetColorMap(0);
	mInput1->Render();
      }
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
  this->mImageSize=mCurrentSlicerManager->GetImage()->GetSize();
  
  SetRotationCenter();
  SetSliderRanges();
  
    //connect all sigs to slots	
  connect(resetbutton, SIGNAL(pressed()), this, SLOT(ResetTransform()));
  
  connect(Xval, SIGNAL(editingFinished()), this, SLOT(SetXvalue()));
  connect(Yval, SIGNAL(editingFinished()), this, SLOT(SetYvalue()));
  connect(Zval, SIGNAL(editingFinished()), this, SLOT(SetZvalue()));
  
  connect(xtrans_slider, SIGNAL(sliderReleased()), this, SLOT(SetOrderXtrans()));
  connect(ytrans_slider, SIGNAL(sliderReleased()), this, SLOT(SetOrderYtrans()));
  connect(ztrans_slider, SIGNAL(sliderReleased()), this, SLOT(SetOrderZtrans()));
  connect(xrot_slider, SIGNAL(sliderReleased()), this, SLOT(SetOrderXrot()));
  connect(yrot_slider, SIGNAL(sliderReleased()), this, SLOT(SetOrderYrot()));
  connect(zrot_slider, SIGNAL(sliderReleased()), this, SLOT(SetOrderZrot()));
  
  
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
  std::ostringstream osstream;
  osstream << 	"Transformed_"  
           << mInput2->GetSlicer(0)->GetFileName() << ".mhd";
  AddImage(mInput2->GetImage(),osstream.str()); 
  QApplication::restoreOverrideCursor();
  close();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOverlay()
{
  mCurrentSlicerManager=mInput2;
  for (int i =0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mInput1->GetSlicer(i)->SetOverlay(mInput2->GetImage());
    mInput1->GetSlicer(i)->SetActorVisibility("overlay",0,true);
    mInput1->SetColorMap();
    mInput1->Render();
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
  if (tX!=0||tY!=0||tZ!=0)
    transform->Translate(tX,tY,tZ);

    transform->Update();

  for (int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
    mCurrentSlicerManager->GetImage()->UpdateReslice();
    mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();
    mCurrentSlicerManager->GetSlicer(i)->Render();
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetTransform(vtkMatrix4x4 *matrix)
{
    vtkSmartPointer<vtkTransform> transform =mCurrentSlicerManager->GetImage()->GetTransform();
    transform->SetMatrix(matrix);
   // mCurrentSlicerManager->GetImage()->SetTransform(transform);
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
  void vvToolRigidReg::UpdateTextEditor(vtkMatrix4x4 *matrix,QString SetOrder)
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
    str2.append("Transformation Matrix:\n");
    textEdit->setText(str2);

    
    for(int i=0;i<4;i++)
    {
    for(int j=0;j<4;j++)
      {
    str2=textEdit->toPlainText();
    str2.append("\t"+str1.setNum(matrix->Element[i][j]));
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
    mOrder+=SetOrder+QString(" ");
    str2.append(mOrder);
    textEdit->setText(str2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOrderXtrans()
{
  if(mOrder.size()-mOrder.lastIndexOf("Tx")<10) //see if the same Transformation Parameter has been used back to back
  mOrder.chop(mOrder.size()-mOrder.lastIndexOf("Tx"));
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Tx: ")+xtrans_sb->text());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOrderYtrans()
{
  if(mOrder.size()-mOrder.lastIndexOf("Ty")<10) 
  mOrder.chop(mOrder.size()-mOrder.lastIndexOf("Ty"));
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Ty: ")+ytrans_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOrderZtrans()
{
  if(mOrder.size()-mOrder.lastIndexOf("Tz")<10) 
  mOrder.chop(mOrder.size()-mOrder.lastIndexOf("Tz"));
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Tz: ")+ztrans_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOrderXrot()
{
  if(mOrder.size()-mOrder.lastIndexOf("Rx")<10) 
  mOrder.chop(mOrder.size()-mOrder.lastIndexOf("Rx"));
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Rx: ")+xrot_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOrderYrot()
{
 if(mOrder.size()-mOrder.lastIndexOf("Ry")<10) //see if the same Transformation Parameter has been used back to back
  mOrder.chop(mOrder.size()-mOrder.lastIndexOf("Ry"));
 UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Ry: ")+yrot_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOrderZrot()
{
  if(mOrder.size()-mOrder.lastIndexOf("Rz")<10) //see if the same Transformation Parameter has been used back to back
  mOrder.chop(mOrder.size()-mOrder.lastIndexOf("Rz"));
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Rz: ")+zrot_sb->text());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXtransb()
{
  int diff=0;
  diff=xtrans_sb->value()-xtrans_slider->value();
  this->SetTransform(diff, 0, 0, 0, 0, 0);
  xtrans_slider->setValue(xtrans_sb->value());
 // UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Tx:")+xtrans_sb->text());
    
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXtranslider()
{
  int diff=0;
  diff=xtrans_slider->value()-xtrans_sb->value();
  this->SetTransform(diff, 0, 0, 0, 0, 0);
  xtrans_sb->setValue(xtrans_slider->value());
  //UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Tx:")+xtrans_sb->text());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYtransb()
{

  int diff=0;
  diff=ytrans_sb->value()-ytrans_slider->value();
  this->SetTransform(0, diff, 0, 0, 0, 0);
  ytrans_slider->setValue(ytrans_sb->value());
//  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Ty:")+ytrans_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYtranslider()
{
  int diff=0;
  diff=ytrans_slider->value()-ytrans_sb->value();
  this->SetTransform(0, diff, 0, 0, 0, 0);
  ytrans_sb->setValue(ytrans_slider->value());
// UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Ty:")+ytrans_sb->text());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZtransb()
{

  int diff=0;
  diff=ztrans_sb->value()-ztrans_slider->value();
  this->SetTransform(0, 0, diff, 0, 0, 0);
  ztrans_slider->setValue(ztrans_sb->value());
 // UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Tz:")+ztrans_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZtranslider()
{
  int diff=0;
  diff=ztrans_slider->value()-ztrans_sb->value();
  this->SetTransform(0, 0, diff, 0, 0, 0);
  ztrans_sb->setValue(ztrans_slider->value());
// UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Tz:")+ztrans_sb->text());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXrotsb()
{

  int diff=0;
  diff=xrot_sb->value()-xrot_slider->value();
  this->SetTransform(0, 0, 0, diff, 0, 0);
  xrot_slider->setValue(xrot_sb->value());
 // UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Rx:")+xrot_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateXrotslider()
{
  int diff=0;
  diff=xrot_slider->value()-xrot_sb->value();
  this->SetTransform(0, 0, 0, diff, 0, 0);
  xrot_sb->setValue(xrot_slider->value());
 // UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Rx:")+xrot_sb->text());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYrotsb()
{

  int diff=0;
  diff=yrot_sb->value()-yrot_slider->value();
  this->SetTransform(0, 0, 0, 0, diff, 0);
  yrot_slider->setValue(yrot_sb->value());
 // UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Ry:")+yrot_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateYrotslider()
{
  int diff=0;
  diff=yrot_slider->value()-yrot_sb->value();
  this->SetTransform(0, 0, 0, 0, diff, 0);
  yrot_sb->setValue(yrot_slider->value());
//  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Ry:")+yrot_sb->text());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZrotsb()
{

  int diff=0;
  diff=zrot_sb->value()-zrot_slider->value();
  this->SetTransform(0, 0, 0, 0, 0,diff);
  zrot_slider->setValue(zrot_sb->value());
 // UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Rz:")+zrot_sb->text());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateZrotslider()
{
  int diff=0;
  diff=zrot_slider->value()-zrot_sb->value();
  this->SetTransform(0, 0, 0, 0, 0, diff);
  zrot_sb->setValue(zrot_slider->value());
//  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),QString("Rz:")+zrot_sb->text());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SaveFile()
{	
   QString s = QFileDialog::getSaveFileName(this, tr("Save File"),
                            "/home/bharath/clitk3_27Apr/clitk3/build/bin/text.txt",
                            tr("Text (*.txt *.doc *.rtf)"));
    QFile file(s);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out(&file);
    out << textEdit->toPlainText() ;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ReadFile()
{	
  QString parameters=QString("Tx: Ty: Tz:");
   vtkSmartPointer<vtkTransform> transform = mCurrentSlicerManager->GetImage()->GetTransform();
   
   
   QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a file",
                    "/home/bharath/clitk3_27Apr/clitk3/build/bin/",
                    "Text (*.txt *.rtf *.doc)");
   QFile file(s);
   ifstream readfile;
   std::string x;
   std::vector<QString> Qstr;
   
   readfile.open("text.txt");
   if (readfile.is_open()) 
   {
    while (!readfile.eof())
      {
      readfile >> x;
      Qstr.push_back(QString(x.c_str()));
      }
      readfile.close();
   }
   else cout << "Unable to open file";

   //Obtain the Rotation Center 
    Xval->setText(Qstr.at(2));
    Yval->setText(Qstr.at(3));
    Zval->setText(Qstr.at(4));

//    InitializeSliders();
    double * orientations=new double[4];
    double * translations=new double[3];
    transform->PostMultiply();
  //Works currently wit the ROtation order Y Z X 
    transform->GetPosition(translations);
    transform->GetOrientation(orientations);
   /* xtrans_slider->setValue(Qstr.at(10).toDouble());
    UpdateXtranslider();
    ytrans_slider->setValue(Qstr.at(14).toDouble());
    UpdateYtranslider();
    ztrans_slider->setValue(Qstr.at(18).toDouble());
    UpdateZtranslider();
*/
    DD(translations[0]);
    DD(translations[1]);
    DD(translations[2]);
    DD(orientations[0]);
    DD(orientations[1]);
    DD(orientations[2]);
    DD(orientations[3]);
    DD(*transform->GetMatrix());
    //Obtain the Transformation Matrix
    /*QString str;
   str=QString(value.at(10).c_str());
   DD(str.toStdString());
   */
// DD(value[6]);
	    // DD(value[10]);	
   //put the streamed values in matrix array
  /* for(int i=0;i<4;i++)
   {
     for(int j=0;j<4;j++)
     {
       matrix->Element[i][j]=value[i*4+j];
     }
   }
   SetTransform( matrix);*/

 /*xtrans_slider->setValue(value.at(10));
   ytrans_slider->setValue(value.at(14));
   ztrans_slider->setValue(value.at(18));
   UpdateXtranslider();
   UpdateYtranslider();
   UpdateZtranslider();*/
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ResetTransform()
{
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
   matrix->Identity();
   
   SetRotationCenter();
   SetSliderRanges();
   SetTransform(matrix);
   mOrder.clear();
   mOrder=QString("Rotation Order: ");
   UpdateTextEditor(matrix,mOrder);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetRotationCenter()
{
    //default image rotation center is the center of the image
    QString xcord,ycord,zcord;
    std::vector<double> imageorigin;
    imageorigin=mCurrentSlicerManager->GetImage()->GetOrigin();
    
    xcord=xcord.setNum(imageorigin[0]+mImageSize[0]*mCurrentSlicerManager->GetImage()->GetSpacing()[0]/2, 'g', 3);
    ycord=ycord.setNum(imageorigin[1]+mImageSize[1]*mCurrentSlicerManager->GetImage()->GetSpacing()[1]/2, 'g', 3);
    zcord=zcord.setNum(imageorigin[2]+mImageSize[2]*mCurrentSlicerManager->GetImage()->GetSpacing()[2]/2, 'g', 3);

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
