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
  cb_transform->hide();
  cb_interpolator->hide();
  cb_optimizer->hide();
  cb_metric->hide();
  cb_selectoutput->hide();
  cb_presets->hide();
  translabel->hide();
  metriclabel->hide();
  outputlabel->hide();
  optimlabel->hide();
  interpollabel->hide();
  presetlabel->hide();
  
  
  // Set how many inputs are needed for this tool
  AddInputSelector("Select moving image");
  AddInputSelector("Select fixed image");
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
  QFont font=QFont("Courier",10);
  tab2textedit->setTextColor(QColor(255,0,0));
  tab2textedit->setCurrentFont(font);
  tab2textedit->update();
   
  QString file = QFileDialog::getOpenFileName(
                    this,
		    "Locate the Config File",
                    mMainWindow->GetInputPathName(),
                    "Text (*.conf *.txt *.rtf *.doc)");
  
  if (file.isEmpty())
  return;
  
  QFile Qfile1(file);
  mConfigFile= file.toStdString();
  CmdlineParser(1, 1);//1,1 - override, initialize
  cb_transform->setCurrentIndex(mArgsInfo.transform_arg);
  cb_interpolator->setCurrentIndex(mArgsInfo.interp_arg);
  cb_optimizer->setCurrentIndex(mArgsInfo.optimizer_arg);
  cb_metric->setCurrentIndex(mArgsInfo.metric_arg);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::Presets()
{
  mConfigFile="Presets";
  std::string matrixfilename;
if(cb_presets->currentIndex()==0)
{
    mArgsInfo.reference_arg=new char; 	
    mArgsInfo.reference_given=0;
    mArgsInfo.target_arg=new char; 	 
    mArgsInfo.target_given=0;
    mArgsInfo.output_arg=new char; 	 
    mArgsInfo.referenceMask_arg=new char; 	 
    mArgsInfo.targetMask_arg=new char; 	 
    mArgsInfo.initMatrix_arg=new char; 	 
    mArgsInfo.matrix_arg=new char;
    mArgsInfo.referenceMask_given=0; 	 
    mArgsInfo.reference_given=0; 	 
    mArgsInfo.reference_arg=new char; 	 
    mArgsInfo.target_given=0; 	 
    mArgsInfo.target_arg=new char; 	 
    mArgsInfo.output_given=0; 	 
    mArgsInfo.output_arg=new char; 	 
    mArgsInfo.checker_after_given=0; 	 
    mArgsInfo.checker_before_given=0; 	 
    mArgsInfo.after_given=0; 	 
    mArgsInfo.before_given=0; 	 
    mArgsInfo.threads_given=1; 	 
    mArgsInfo.threads_arg=3; 	 
    mArgsInfo.normalize_flag=0; 	 
    mArgsInfo.blur_arg=0.0; 	 
    mArgsInfo.referenceMask_arg=new char; 	 
    mArgsInfo.targetMask_arg=new char; 	 
    mArgsInfo.targetMask_given=0; 	 
    mArgsInfo.levels_given=1; 	 
    mArgsInfo.levels_arg=2; 	 
    mArgsInfo.moment_flag=1; 	 
    mArgsInfo.intThreshold_given=0; 	 
    mArgsInfo.intThreshold_arg=0.0; 	 
    mArgsInfo.transX_arg=0.0; 	 
    mArgsInfo.transY_arg=0.0; 	 
    mArgsInfo.transZ_arg=0.0; 	 
    mArgsInfo.transform_arg=2; 	 
    mArgsInfo.gradient_flag=1; 	 
    mArgsInfo.interp_given=1; 	 
    mArgsInfo.interp_arg=1; 	 
    mArgsInfo.interpOrder_given=1; 	 
    mArgsInfo.interpOrder_arg=3; 	 
    mArgsInfo.interpSF_given=1; 	 
    mArgsInfo.interpSF_arg=20;//default 	 
    mArgsInfo.metric_given=1; 	 
    mArgsInfo.metric_arg=0; 	 
    mArgsInfo.samples_arg=1;//default 	 
    mArgsInfo.stdDev_arg=0.4; 	 
    mArgsInfo.step_arg=2.0; 	 
    mArgsInfo.relax_arg=0.7; 	 
    mArgsInfo.valueTol_arg=0.01; 	 
    mArgsInfo.stepTol_arg=0.1; 	 
    mArgsInfo.gradTol_arg=1e-5; 	 
    mArgsInfo.lineAcc_arg=0.9; 	 
    mArgsInfo.convFactor_arg=1e+12; 	 
    mArgsInfo.maxIt_arg=500; 	 
    mArgsInfo.maxLineIt_arg=50; 	 
    mArgsInfo.maxEval_arg=500; 	 
    mArgsInfo.maxCorr_arg=5; 	 
    mArgsInfo.selectBound_arg=0; 	 
    mArgsInfo.inc_arg=1.2; 	 
    mArgsInfo.dec_arg=4; 	 
    mArgsInfo.optimizer_arg=1; 	 
    mArgsInfo.initMatrix_given=0; 	 
    mArgsInfo.initMatrix_arg=new char; 	 
    mArgsInfo.tWeight_given=1; 	 
    mArgsInfo.tWeight_arg=1.0; 	 
    mArgsInfo.rWeight_given=1.0; 	 
    mArgsInfo.rWeight_arg=50.0; 	 
    mArgsInfo.matrix_given=1;
    matrixfilename="/home/bharath/bin/writematrix.txt";//put ur path here for retreiving your matrix
    mArgsInfo.matrix_arg=const_cast<char*>(matrixfilename.c_str()); 
    UpdateTextEditor2();
}
else {
  QMessageBox::information(this,"Sorry", "Other Presets are not available for the moment!");
  return;
 }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateTextEditor2()
{
  
    QString str1,str2,str3;
    QColor color;
    tab2textedit->clear();
    tab2textedit->setAcceptRichText(true);
    str2=tab2textedit->toPlainText();
    tab2textedit->setTextColor(QColor(255,0,0));
    str2.append(str3.append("threads="+str1.setNum(mArgsInfo.threads_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);

    str2=tab2textedit->toPlainText();
    str2.append(str3.append("reference="+str1.append(mArgsInfo.reference_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("target="+str1.append(mArgsInfo.target_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("matrix="+str1.append(mArgsInfo.matrix_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("interp="+str1.setNum(mArgsInfo.interp_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("transform="+str1.setNum(mArgsInfo.transform_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("transX="+str1.setNum(mArgsInfo.transX_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("transY="+str1.setNum(mArgsInfo.transY_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("transZ="+str1.setNum(mArgsInfo.transZ_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("metric="+str1.setNum(mArgsInfo.metric_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("samples="+str1.setNum(mArgsInfo.samples_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("intThreshold="+str1.setNum(mArgsInfo.intThreshold_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("stdDev="+str1.setNum(mArgsInfo.stdDev_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("blur="+str1.setNum(mArgsInfo.blur_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("optimizer="+str1.setNum(mArgsInfo.optimizer_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("step="+str1.setNum(mArgsInfo.step_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("relax="+str1.setNum(mArgsInfo.relax_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("valueTol="+str1.setNum(mArgsInfo.valueTol_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();
    
    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("stepTol="+str1.setNum(mArgsInfo.stepTol_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("gradTol="+str1.setNum(mArgsInfo.gradTol_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("lineAcc="+str1.setNum(mArgsInfo.lineAcc_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("convFactor="+str1.setNum(mArgsInfo.convFactor_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("maxIt="+str1.setNum(mArgsInfo.maxIt_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();
 
    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("maxLineIt="+str1.setNum(mArgsInfo.maxLineIt_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
    str2=tab2textedit->toPlainText();
    str2.append(str3.append("maxEval="+str1.setNum(mArgsInfo.maxEval_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
      str2=tab2textedit->toPlainText();
    str2.append(str3.append("maxCorr="+str1.setNum(mArgsInfo.maxCorr_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
      str2=tab2textedit->toPlainText();
    str2.append(str3.append("selectBound="+str1.setNum(mArgsInfo.selectBound_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
      str2=tab2textedit->toPlainText();
    str2.append(str3.append("rWeight="+str1.setNum(mArgsInfo.rWeight_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
      str2=tab2textedit->toPlainText();
    str2.append(str3.append("tWeight="+str1.setNum(mArgsInfo.tWeight_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
      str2=tab2textedit->toPlainText();
    str2.append(str3.append("levels="+str1.setNum(mArgsInfo.levels_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
      str2=tab2textedit->toPlainText();
    str2.append(str3.append("inc="+str1.setNum(mArgsInfo.inc_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
    
      str2=tab2textedit->toPlainText();
    str2.append(str3.append("dec="+str1.setNum(mArgsInfo.dec_arg)));
    tab2textedit->setText(str2);
    str3.clear();
    str1.clear();

    str2=tab2textedit->toPlainText();
    str2.append("\n");
    tab2textedit->setText(str2);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::TransformSelect()
{
  if(!mConfigFile.empty()){
   mArgsInfo.transform_arg=cb_transform->currentIndex();
   UpdateTextEditor2();
  }
  else{
    QMessageBox::information(this,"Warning","Load the Config File First!..");
  }
}
//------------------------------------------------------------------------------
  
//------------------------------------------------------------------------------
void vvToolRigidReg::CmdlineParser(int override, int initialize)
{
   //0 opened, 1 not opened fine
   int opened=cmdline_parser_clitkAffineRegistration_configfile(const_cast<char*>(mConfigFile.c_str()),&mArgsInfo,override,initialize,1);
   DD(opened);
   mArgsInfo.gradient_flag=1;
   QString str;
   //Read the transformation parameters from the  path in the config file(mArgsInfo.matrix_arg) and display it on the TextBox 2
   ifstream readfile;
   std::vector<QString> Qstr;
   
   if(!opened){
   readfile.open(mConfigFile.c_str());
   }
   else{
   QMessageBox::information(this,"Warning","Load the Config File First..");
   }
   if (readfile.is_open()) 
   { 
    while (!readfile.eof())
      {
      readfile >> mConfigFile;
      Qstr.push_back(QString(mConfigFile.c_str()));
      }
      readfile.close();
   }
    else {
      QMessageBox::information(this,"Warning","Cannot Open File!");
      return;
    }
     for(unsigned int i=0;i<Qstr.size()-1;i++)
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
void vvToolRigidReg::OptimizerSelect()
{
  if(!mConfigFile.empty()){
  mArgsInfo.optimizer_arg=cb_optimizer->currentIndex();
  UpdateTextEditor2();
   }
  else{
    QMessageBox::information(this,"Warning","Load the Config File First!..");
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::InterpolatorSelect()
{
  if(!mConfigFile.empty()){
  mArgsInfo.interp_arg=cb_interpolator->currentIndex();
  UpdateTextEditor2();
   }
  else{
    QMessageBox::information(this,"Warning","Load the Config File First!..");
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::MetricSelect()
{
  if(!mConfigFile.empty()){
 mArgsInfo.metric_arg=cb_metric->currentIndex();
 UpdateTextEditor2();
   }
  else{
    QMessageBox::information(this,"Warning","Load the Config File First!..");
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::OutputSelect()
{
  std::ostringstream osstream;
  if(!mConfigFile.empty()){
    if(cb_selectoutput->currentIndex()==0){
      mArgsInfo.output_given=0;
      mArgsInfo.checker_after_given=0;
      mArgsInfo.checker_before_given=0;
      mArgsInfo.after_given=0;
      mArgsInfo.before_given=0;
    }//get transformed output image
    if(cb_selectoutput->currentIndex()==1){ 
      mArgsInfo.output_given=0;
      mArgsInfo.checker_after_given=1;
      mArgsInfo.checker_before_given=0;
      mArgsInfo.after_given=0;
      mArgsInfo.before_given=0;
    }//get checkered image after reg
    if(cb_selectoutput->currentIndex()==2){  
      mArgsInfo.output_given=0;
      mArgsInfo.checker_after_given=0;
      mArgsInfo.checker_before_given=1;
      mArgsInfo.after_given=0;
      mArgsInfo.before_given=0;
    }//get checkered  image before reg
    if(cb_selectoutput->currentIndex()==3){
      mArgsInfo.output_given=0;
      mArgsInfo.checker_after_given=0;
      mArgsInfo.checker_before_given=0;
      mArgsInfo.after_given=1;
      mArgsInfo.before_given=0;
    }//get difference image after reg
    if(cb_selectoutput->currentIndex()==4){  
      mArgsInfo.output_given=0;
      mArgsInfo.checker_after_given=0;
      mArgsInfo.checker_before_given=0;
      mArgsInfo.after_given=0;
      mArgsInfo.before_given=1;
    }//get difference image before reg
  }
  else{
    QMessageBox::information(this,"Warning","Load the Config File First!..");
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SaveTextEdit()
{
     DD(mArgsInfo.transform_arg);
   QString f1 = QFileDialog::getSaveFileName(this, tr("Save Config File"),
                                              mMainWindow->GetInputPathName(),
                                              tr("Text (*.mat *.txt *.doc *.rtf)"));
   QFile file(f1);				      
   if(file.open(QFile::WriteOnly | QFile::Truncate) & !mConfigFile.empty() ){
   QTextStream stream( &file );
   stream << tab2textedit->toPlainText();
     }
    else
     {
      QMessageBox::information(this,"Warning","Nothing to Save!");
      return;
     }
   tab2textedit->clear();
   DD(mArgsInfo.transform_arg);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::InputIsSelected(std::vector<vvSlicerManager *> & l)
{
  //inputs
  mInput1 = l[0];
  mInput2 = l[1];
  UpdateTextEditor(mCurrentSlicerManager->GetImage()->GetTransform()->GetMatrix(),textEdit_2);

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
   
   connect(loadbutton, SIGNAL(pressed()), this, SLOT(LoadFile()));
   connect(savebutton, SIGNAL(pressed()), this, SLOT(SaveFile()));
   
   connect(checkBox_rigid, SIGNAL(clicked(bool)), this, SLOT(CheckRigidReg()));
   connect(checkBox_deformable, SIGNAL(clicked(bool)), this, SLOT(CheckDeformableReg()));
   
   connect(cb_presets, SIGNAL(activated(int)), this, SLOT(Presets()));
   connect(cb_transform, SIGNAL(activated(int)), this, SLOT(TransformSelect()));
   connect(cb_optimizer, SIGNAL(activated(int)), this, SLOT(OptimizerSelect()));
   connect(cb_interpolator, SIGNAL(activated(int)), this, SLOT(InterpolatorSelect()));
   connect(cb_metric, SIGNAL(activated(int)), this, SLOT(MetricSelect()));
   connect(cb_selectoutput, SIGNAL(activated(int)), this, SLOT(OutputSelect()));
   connect(tab2savebutton, SIGNAL(pressed()), this, SLOT(SaveTextEdit()));
   
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::apply()
{
  RemoveOverlay();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::CheckRigidReg()
{
  checkBox_deformable->setChecked(false);
  cb_transform->show();
  cb_presets->show();
  cb_metric->show();
  cb_interpolator->show();
  cb_optimizer->show();
  cb_selectoutput->show();
  translabel->show();
  metriclabel->show();
  outputlabel->show();
  optimlabel->show();
  interpollabel->show();
  presetlabel->show();
  
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::CheckDeformableReg()
{
  checkBox_rigid->setChecked(false);
  presetlabel->hide();
  cb_transform->show();
  cb_metric->show();
  cb_interpolator->show();
  cb_optimizer->show();
  cb_selectoutput->show();
  cb_presets->hide();
  translabel->show();
  metriclabel->show();
  outputlabel->show();
  optimlabel->show();
  interpollabel->show();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetOverlay(vvImage::Pointer Image)
{
    for (int i =0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
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
   for(int i=0;i<mCurrentSlicerManager->GetNumberOfSlicers();i++)
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
    transform->Translate(tX*mInput1->GetImage()->GetSpacing()[0],
                         tY*mInput1->GetImage()->GetSpacing()[1],
                         tZ*mInput1->GetImage()->GetSpacing()[2]);
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
void vvToolRigidReg::LoadFile()
{
  ReadFile(false);
}
//------------------------------------------------------------------------------
  
//------------------------------------------------------------------------------
void vvToolRigidReg::Render()
{
  for (int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++) {
       mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();
       mCurrentSlicerManager->GetSlicer(i)->Render();
      }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
  void vvToolRigidReg::UpdateTextEditor(vtkMatrix4x4 *matrix,QTextEdit* textEdit)
{
    QFont font=QFont("Courier",11);
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
    
    if(!mConfigFile.empty()){
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
    filter->EnableReadOnDisk(false);
    filter->Update();
    std::ostringstream osstream;
    vvImage::Pointer output;
    output = filter->GetOutputVVImage();
    SetOverlay(output);
    
    if(!cb_selectoutput->currentIndex()==0){
     std::string outputstring;
     output = filter->GetOutputVVImages()[1];
     if(cb_selectoutput->currentIndex()==1){outputstring="Checkered_after_reg";}
     if(cb_selectoutput->currentIndex()==2){outputstring="Checkered_before_reg";}
     if(cb_selectoutput->currentIndex()==3){outputstring="Diff_after_reg";}
     if(cb_selectoutput->currentIndex()==4){outputstring="Diff_before_reg";}
    osstream << outputstring << mCurrentSlicerManager->GetSlicer(0)->GetFileName() << ".mhd";
    AddImage(output,osstream.str());
    filter->DeleteLastOutputImage();
    }
    DD(filter->GetOutputVVImages().capacity());
    QApplication::restoreOverrideCursor();
    ReadFile(true);
    }
    else
    {
      QMessageBox::information(this, "Warning","Load the Config File First!...");
      return;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::UpdateTransform(bool slider_enabled)
{
     vtkSmartPointer<vtkTransform> transform_final=mInput1->GetImage()->GetTransform();
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
    transform_final->Update();
    Render();
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
void vvToolRigidReg::ReadFile(bool matrix_given)
{
   std::string x;
   QString center;
   double * orientations=new double[3];
   double * translations=new double[3];
   vtkMatrix4x4 *matrix=vtkMatrix4x4::New();
   std::string transfile;
   vtkSmartPointer<vtkTransform> transform = mCurrentSlicerManager->GetImage()->GetTransform();
   if(!matrix_given)
   {
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
   transfile= file1.toStdString();
   }
   else
   {
    transfile=mArgsInfo.matrix_arg;
    DD(transfile);
   }
   std::string filename1(transfile);
   std::ifstream f1(filename1.c_str());
   if(f1.is_open())
   {
   f1.close();
   itk::Matrix<double, 4, 4> itkMat = clitk::ReadMatrix3D(transfile);
   for(int j=0; j<4; j++)
      for(int i=0; i<4; i++)
    matrix->SetElement(i,j,rint(itkMat[i][j]));
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
  vtkSmartPointer<vtkTransform> transform = mCurrentSlicerManager->GetImage()->GetTransform();
  transform->SetMatrix(mInitialMatrix);
  transform->Update();
  
   Render();
   SetRotationCenter();
   SetSliderRanges();
   UpdateTextEditor(transform->GetMatrix(),textEdit);
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
