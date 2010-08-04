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

// vv
#include "vvToolRigidReg.h"
#include "vvImageReader.h"
#include "vvSlicer.h"

// vtk
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

// clitk
#include "clitkTransformUtilities.h"
#include "clitkAffineRegistrationGenericFilter.h"	
// qt
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QComboBox>
#include <QCursor>


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
   // QSize qsize;
//    qsize.setHeight(470);
//    qsize.setWidth(850);
//    mToolWidget->setFixedSize(qsize);
  // Set how many inputs are needed for this tool
  mFilter = new clitk::AffineRegistrationGenericFilter;
  
  // Set how many inputs are needed for this tool
 AddInputSelector("Select moving image",mFilter);
 AddInputSelector("Select fixed image",mFilter);
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
  ResetTransform();
  RemoveOverlay();
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
void vvToolRigidReg::GetArgsInfoFromGUI()
{   
  
  QFont font=QFont("Times New Roman",10);
  tab2textedit->setTextColor(QColor(255,0,0));
  tab2textedit->setCurrentFont(font);
  tab2textedit->update();
  QString str;
  QString file = QFileDialog::getOpenFileName(
                    this,
		    "Choose the Transformation Parameters file",
                    mMainWindow->GetInputPathName(),
                    "Text (*.conf *.txt *.rtf *.doc)");
    if (file.isEmpty())
    return;
   QFile Qfile1(file);
  // ifstream readfile;
   std::string configfile= file.toStdString();
   cmdline_parser_clitkAffineRegistration_configfile(const_cast<char*>(configfile.c_str()),&mArgsInfo,1,1,1);
   mArgsInfo.gradient_flag=1;
   DD(mArgsInfo.matrix_arg);
   
   //Read from File and display it on the TextBox 2
   ifstream readfile;
   std::vector<QString> Qstr;
   readfile.open(configfile.c_str());
   if (readfile.is_open()) 
   {
    while (!readfile.eof())
      {
      readfile >> configfile;
      Qstr.push_back(QString(configfile.c_str()));
      }
      readfile.close();
   }
    else {
      cout << "Unable to open file";
    }
    for(unsigned int i=0;i<Qstr.size();i++)
      {
    str=tab2textedit->toPlainText();
    str.append(Qstr.at(i));
    tab2textedit->setText(str);
    str.append("\n");
    tab2textedit->setText(str);
      }
      
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::InitializeComboBox()
{
 
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::InputIsSelected(std::vector<vvSlicerManager *> & l)
{
  //inputs
  mInput1 = l[0];
  mInput2 = l[1];

  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),textEdit_2);

  DD(*mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix());
  for(int i =0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      mInitialMatrix[i*4+j]=mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix()->GetElement(i,j);
    }
  }

  if(mInput1->GetFileName()==mInput2->GetFileName())
  {
    QMessageBox::information(this, "Warning","Your Reference and Target Images are the same");
  }
  mTwoInputs = true;
  SetOverlay(mInput2->GetImage());
  mImageSize=mInput1->GetImage()->GetSize();
  SetRotationCenter();
  SetSliderRanges();

    //connect all sigs to slots
   connect(resetbutton, SIGNAL(pressed()), this, SLOT(ResetTransform()));
   connect(tab2loadbutton, SIGNAL(pressed()), this, SLOT(GetArgsInfoFromGUI()));
   connect(tab2applybutton, SIGNAL(pressed()), this, SLOT(AutoRegister()));


   connect(Xval, SIGNAL(editingFinished()), this, SLOT(SetXvalue()));
   connect(Yval, SIGNAL(editingFinished()), this, SLOT(SetYvalue()));
   connect(Zval, SIGNAL(editingFinished()), this, SLOT(SetZvalue()));

   connect(xtrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateTransform_sliders()));
   connect(ytrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateTransform_sliders()));
   connect(ztrans_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateTransform_sliders()));


   connect(xrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateTransform_sliders()));
   connect(yrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateTransform_sliders()));
   connect(zrot_slider, SIGNAL(valueChanged(int)), this, SLOT(UpdateTransform_sliders()));

   connect(xtrans_sb, SIGNAL(valueChanged(double)), this, SLOT(UpdateTransform_sb()));
   connect(ytrans_sb, SIGNAL(valueChanged(double)), this, SLOT(UpdateTransform_sb()));
   connect(ztrans_sb, SIGNAL(valueChanged(double)), this, SLOT(UpdateTransform_sb()));
   connect(xrot_sb, SIGNAL(valueChanged(double)), this, SLOT(UpdateTransform_sb()));
   connect(yrot_sb, SIGNAL(valueChanged(double)), this, SLOT(UpdateTransform_sb()));
   connect(zrot_sb, SIGNAL(valueChanged(double)), this, SLOT(UpdateTransform_sb()));
   
   connect(loadbutton, SIGNAL(pressed()), this, SLOT(ReadFile()));
   connect(savebutton, SIGNAL(pressed()), this, SLOT(SaveFile()));
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::apply()
{
  RemoveOverlay();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOverlay(vvImage::Pointer Image)
{
    for (int i =0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
   mCurrentSlicerManager->GetSlicer(i)->SetOverlay(Image);
   mCurrentSlicerManager->GetSlicer(i)->SetActorVisibility("overlay",0,true);
   mCurrentSlicerManager->SetColorMap();
   mCurrentSlicerManager->Render();
   }
  }
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::RemoveOverlay()
{
   for(int i=0;i<mCurrentSlicerManager->NumberOfSlicers();i++)
	 {
	   mInput1->RemoveActor("overlay",0);
	   mInput1->SetColorMap(0);
	   mInput1->Render();
	   hide();
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
  vtkSmartPointer<vtkTransform> transform = mInput1->GetImage()->GetTransform();
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
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetTransform(vtkMatrix4x4 *matrix)
{
    vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();
    for(int i=0; i<4;i++)
      for(int j=0;j<4;j++)
    mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix()->SetElement(i,j,matrix->GetElement(i,j));
    
    Render();
}
//------------------------------------------------------------------------------
  
//------------------------------------------------------------------------------
void vvToolRigidReg::Render()
{
    for (int i=0; i<mCurrentSlicerManager->NumberOfSlicers(); i++) {
       mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();
      mCurrentSlicerManager->GetSlicer(i)->Render();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
  void vvToolRigidReg::UpdateTextEditor(vtkMatrix4x4 *matrix,QTextEdit* textEdit)
{
    QFont font=QFont("Times New Roman",11);
    textEdit->setCurrentFont(font);
    textEdit->update();

    QString str1,str2,str3;
    QColor color;
    textEdit->clear();
    textEdit->setAcceptRichText(true);
    str2=textEdit->toPlainText();
    str2.append("#Rotation Center(mm): \n#");
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
    str2.append("#Transformation Matrix(mm):\n");
    textEdit->setText(str2);
    for(int i=0;i<4;i++)
    {
    for(int j=0;j<4;j++)
      {
    str2=textEdit->toPlainText();
   // str2.append("\t"+str1.setNum(matrix->Element[i][j]));
    str2.append(QString("%1\t").arg(str1.setNum(matrix->Element[i][j]),2));
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
void vvToolRigidReg::UpdateTransform_sliders()
{
       InitializeSliders(xtrans_slider->value()*mInput1->GetImage()->GetSpacing()[0],
			 ytrans_slider->value()*mInput1->GetImage()->GetSpacing()[1],
			 ztrans_slider->value()*mInput1->GetImage()->GetSpacing()[2],
			xrot_slider->value(),yrot_slider->value(),zrot_slider->value(),false);
        UpdateTransform(true);
        Render();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateTransform_sb()
{
   InitializeSliders(xtrans_sb->value(),
      ytrans_sb->value(),
      ztrans_sb->value(),
			xrot_sb->value(),yrot_sb->value(),zrot_sb->value(),false);
      UpdateTransform(false);
      Render();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::AutoRegister()
{ 
    if (!mCurrentSlicerManager) close();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    std::vector<vvImage::Pointer> inputs;
    // Input
    inputs.push_back(mInput1->GetImage());
    inputs.push_back(mInput2->GetImage());
    // Check input type
    // Main filter
    clitk::AffineRegistrationGenericFilter::Pointer filter =
    clitk::AffineRegistrationGenericFilter::New();
    filter->SetInputVVImages(inputs);
    filter->SetArgsInfo(mArgsInfo);
    DD("ArgsInfo given in");
    filter->EnableReadOnDisk(false);
    filter->Update();
    DD("I am done...! Updated");
    vvImage::Pointer output = filter->GetOutputVVImage();
    DD("filter getoutput done...");
    //osstream << "Registered" << "_ "
      //     << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
    //AddImage(output,osstream.str());
    QApplication::restoreOverrideCursor();
  // ReadFile();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateTransform(bool slider_enabled)
{
     vtkSmartPointer<vtkTransform> transform_final=vtkSmartPointer<vtkTransform>::New();
     transform_final->SetMatrix(mInitialMatrix);
     transform_final->PostMultiply();
  //Rotations
    double x=0, y=0 ,z=0;
    x= Xval->text().toDouble();
    y= Yval->text().toDouble();
    z= Zval->text().toDouble();
    transform_final->Translate(-x,-y,-z);
    if(slider_enabled){
    transform_final->RotateY(yrot_slider->value());
    transform_final->RotateX(xrot_slider->value());
    transform_final->RotateZ(zrot_slider->value());
    }
    else{
    transform_final->RotateY(yrot_sb->value());
    transform_final->RotateX(xrot_sb->value());
    transform_final->RotateZ(zrot_sb->value());
    }
    transform_final->Translate(x,y,z);
    transform_final->PreMultiply();
    if(slider_enabled){
    transform_final->Translate(xtrans_slider->value()*mInput1->GetImage()->GetSpacing()[0],0,0);
    transform_final->Translate(0,ytrans_slider->value()*mInput1->GetImage()->GetSpacing()[1],0);
    transform_final->Translate(0,0,ztrans_slider->value()*mInput1->GetImage()->GetSpacing()[2]);
    }
    else{
    transform_final->Translate(xtrans_sb->value(),0,0);
    transform_final->Translate(0,ytrans_sb->value(),0);
    transform_final->Translate(0,0,ztrans_sb->value());
    }
    SetTransform(transform_final->GetMatrix());
    UpdateTextEditor(transform_final->GetMatrix(),textEdit);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolRigidReg::SaveFile()
{
  //Write the Transformation Matrix
    QString f1 = QFileDialog::getSaveFileName(this, tr("Save Transformation Matrix File"),
                                              mMainWindow->GetInputPathName(),
                                              tr("Text (*.mat *.txt *.doc *.rtf)"));
    QFile file1(f1);
    std::vector<QString> transparameters;
    QString line1;

    for(int i =0;i<4;i++)
      for(int j=0;j<4;j++)
    transparameters.push_back(line1.setNum(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix()->Element[i][j]));

    if (file1.open(QFile::WriteOnly | QFile::Truncate)) {
    QTextStream out1(&file1);
     for(int i =0;i<4;i++){
      for(int j=0;j<4;j++) {
      out1<<transparameters[i*4+j]+"\t";
      }
      out1<<"\n";
     }
    }
     else
     {
      QMessageBox::information(this,"Warning","Error Reading Parameters");
     }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ReadFile()
{
   std::string x;
   QString center;
   double * orientations=new double[3];
   double * translations=new double[3];
   vtkMatrix4x4 *matrix=vtkMatrix4x4::New();
   vtkSmartPointer<vtkTransform> transform = mCurrentSlicerManager->GetImage()->GetTransform();

   //Open File to read the transformation parameters
   QString file1 = QFileDialog::getOpenFileName(
                    this,
		    "Choose the Transformation Parameters file",
                    mMainWindow->GetInputPathName(),
                    "Text (*.mat *.txt *.rtf *.doc)");
    if (file1.isEmpty())
    return;
   QFile Qfile1(file1);
  // ifstream readfile;
   std::string transfile= file1.toStdString();
   std::string filename1(transfile);
   std::ifstream f1(filename1.c_str());
   if(f1.is_open())
   {
   f1.close();
   itk::Matrix<double, 4, 4> itkMat = clitk::ReadMatrix3D(transfile);
   for(int j=0; j<4; j++)
      for(int i=0; i<4; i++)
    matrix->SetElement(i,j,itkMat[i][j]);
   }
    UpdateTextEditor(matrix,textEdit);
    transform->SetMatrix(matrix);
    transform->GetOrientation(orientations);
    transform->PostMultiply();

     //Obtain the Rotation Center , set it to origin
    Xval->setText(center.setNum(0));
    Yval->setText(center.setNum(0));
    Zval->setText(center.setNum(0));

    //In the Order or Y X Z //
    //now  postmultiply for the rotations
    SetTransform(0,0,0,0,0,-rint(orientations[2]),false);
    SetTransform(0,0,0,-rint(orientations[0]),0,0,false);
    SetTransform(0,0,0,0,-rint(orientations[1]),0,false);

    transform->GetPosition(translations);
    transform->Identity();

    DD(translations[0]/mInput1->GetImage()->GetSpacing()[0]);
    DD(translations[1]/mInput1->GetImage()->GetSpacing()[1]);
    DD(translations[2]/mInput1->GetImage()->GetSpacing()[2]);
    DD(mInput1->GetImage()->GetSpacing()[0]);
    DD(mInput1->GetImage()->GetSpacing()[1]);
    DD(mInput1->GetImage()->GetSpacing()[2]);
    DD(orientations[0]);
    DD(orientations[1]);
    DD(orientations[2]);
      //set the sliders  and spin box values
    InitializeSliders(rint(translations[0]),rint(translations[1])
    ,rint(translations[2]),rint(orientations[0]),rint(orientations[1]),rint(orientations[2]),true);
    SetTransform(matrix);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ResetTransform()
{
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
          matrix->SetElement(i,j,mInitialMatrix[i*4+j]);
   SetTransform(matrix);
   SetRotationCenter();
   SetSliderRanges();
   UpdateTextEditor(matrix,textEdit);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetRotationCenter()
{
    //default image rotation center is the center of the image
    QString xcord,ycord,zcord;
    std::vector<double> imageorigin;
    imageorigin=mInput1->GetImage()->GetOrigin();
    DD("before bug");
    xcord=xcord.setNum(imageorigin[0]+mImageSize[0]*mInput1->GetImage()->GetSpacing()[0]/2, 'g', 3);
    ycord=ycord.setNum(imageorigin[1]+mImageSize[1]*mInput1->GetImage()->GetSpacing()[1]/2, 'g', 3);
    zcord=zcord.setNum(imageorigin[2]+mImageSize[2]*mInput1->GetImage()->GetSpacing()[2]/2, 'g', 3);

    Xval->setText(xcord);
    Yval->setText(ycord);
    Zval->setText(zcord);
    InitializeSliders(0,0,0,0,0,0,true);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::InitializeSliders(double xtrans,double ytrans, double ztrans, double xrot, double yrot, double zrot,bool sliders)
{

    xtrans_sb->blockSignals(true);
    xtrans_sb->setSingleStep(mInput1->GetImage()->GetSpacing()[0]);
    xtrans_sb->setValue(xtrans);
    xtrans_sb->blockSignals(false);
    ytrans_sb->blockSignals(true);
    ytrans_sb->setSingleStep(mInput1->GetImage()->GetSpacing()[1]);
    ytrans_sb->setValue(ytrans);
    ytrans_sb->blockSignals(false);
    ztrans_sb->blockSignals(true);
    ztrans_sb->setSingleStep(mInput1->GetImage()->GetSpacing()[2]);
    ztrans_sb->setValue(ztrans);
    ztrans_sb->blockSignals(false);
    DD(ytrans);

    if(sliders){
    xtrans_slider->blockSignals(true);
    xtrans_slider->setValue(rint(xtrans/mInput1->GetImage()->GetSpacing()[0]));
    xtrans_slider->blockSignals(false);
    ytrans_slider->blockSignals(true);
    ytrans_slider->setValue(rint(ytrans/mInput1->GetImage()->GetSpacing()[1]));
    ytrans_slider->blockSignals(false);
    ztrans_slider->blockSignals(true);
    ztrans_slider->setValue(rint(ztrans/mInput1->GetImage()->GetSpacing()[2]));
    ztrans_slider->blockSignals(false);
    }
    xrot_sb->blockSignals(true);
    xrot_sb->setValue(xrot);									
    xrot_sb->blockSignals(false);
    yrot_sb->blockSignals(true);
    yrot_sb->setValue(yrot);
    yrot_sb->blockSignals(false);
    zrot_sb->blockSignals(true);
    zrot_sb->setValue(zrot);
    zrot_sb->blockSignals(false);
    xrot_slider->blockSignals(true);
    xrot_slider->setValue(xrot);
    xrot_slider->blockSignals(false);
    yrot_slider->blockSignals(true);
    yrot_slider->setValue(yrot);
    yrot_slider->blockSignals(false);
    zrot_slider->blockSignals(true);
    zrot_slider->setValue(zrot);
    zrot_slider->blockSignals(false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetSliderRanges()
{
  xtrans_slider->blockSignals(true);
  xtrans_slider->setRange(-2000,2000);
  xtrans_slider->blockSignals(false);

  ytrans_slider->blockSignals(true);
  ytrans_slider->setRange(-2000,2000);
  ytrans_slider->blockSignals(false);

  ztrans_slider->blockSignals(true);
  ztrans_slider->setRange(-2000,2000);
  ztrans_slider->blockSignals(false);

  xtrans_sb->blockSignals(true);
  xtrans_sb->setRange(-2000,2000);
  xtrans_sb->setDecimals(3);
  xtrans_sb->blockSignals(false);

  ytrans_sb->blockSignals(true);
  ytrans_sb->setRange(-2000,2000);
  ytrans_sb->setDecimals(3);
  ytrans_sb->blockSignals(false);

  ztrans_sb->blockSignals(true);
  ztrans_sb->setRange(-2000,2000);
  ztrans_sb->setDecimals(3);
  ztrans_sb->blockSignals(false);

  xrot_slider->blockSignals(true);
  xrot_slider->setRange(-360,360);
  xrot_slider->blockSignals(false);

  yrot_slider->blockSignals(true);
  yrot_slider->setRange(-360,360);
  yrot_slider->blockSignals(false);

  zrot_slider->blockSignals(true);
  zrot_slider->setRange(-360,360);
  zrot_slider->blockSignals(false);

  xrot_sb->blockSignals(true);
  xrot_sb->setRange(-360,360);
  xrot_sb->blockSignals(false);

  yrot_sb->blockSignals(true);
  yrot_sb->setRange(-360,360);
  yrot_sb->blockSignals(false);

  zrot_sb->blockSignals(true);
  zrot_sb->setRange(-360,360);
  zrot_sb->blockSignals(false);
}
