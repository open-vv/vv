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

// vv
#include "vvToolRigidReg.h"
#include "vvSlicer.h"

// vtk
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkInformation.h>
#include <vtkTransform.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkOpenGLImageSliceMapper.h>

// itk
#include <itkEuler3DTransform.h>

// clitk
#include "clitkTransformUtilities.h"
#include "clitkMatrix.h"

// qt
#include <QMessageBox>
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
  AddInputSelector("Select moving image");

  QFont font = transformationLabel->font();
  font.setStyleHint(QFont::TypeWriter);
  transformationLabel->setFont(font);

  mInitialMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

  // Set slider ranges, assume degrees, will not be changed for radians
  std::vector<QSlider *> transSliders, rotSliders;
  std::vector<QDoubleSpinBox *> transSBs, rotSBs;
  GetSlidersAndSpinBoxes(transSliders, rotSliders, transSBs, rotSBs);
  for(int i=0; i<3; i++) {
    transSliders[i]->setRange(-2000,2000);
    rotSliders[i]->setRange(-360,360);
    transSBs[i]->setRange(-2000,2000);
    transSBs[i]->setDecimals(3);
    rotSBs[i]->setRange(-360,360);
    rotSBs[i]->setDecimals(3);
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vvToolRigidReg::~vvToolRigidReg()
{
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::Initialize()
{
  SetToolName("Register");
  SetToolMenuName("Register manually");
  SetToolIconFilename(":/common/icons/register.png");
  SetToolTip("Register manually.");
  SetToolExperimental(false);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::InputIsSelected(vvSlicerManager *input)
{
  mInput = input;
  HideInputSelector();
  QTabWidget * tab = dynamic_cast<vvMainWindow*>(mMainWindow)->GetTab();
  move(tab->mapToGlobal(tab->pos()));
  resize(tab->width(), 0);

  //default image rotation center is the center of the image
  QString xcord,ycord,zcord;
  std::vector<double> imageorigin;
  imageorigin=mInput->GetImage()->GetOrigin();
  std::vector<int> imageSize = mInput->GetImage()->GetSize();
  std::vector<double> imageSpacing = mInput->GetImage()->GetSpacing();
  xcord=xcord.setNum(imageorigin[0]+(imageSize[0]-1)*imageSpacing[0]*0.5, 'g', 3);
  ycord=ycord.setNum(imageorigin[1]+(imageSize[1]-1)*imageSpacing[1]*0.5, 'g', 3);
  zcord=zcord.setNum(imageorigin[2]+(imageSize[2]-1)*imageSpacing[2]*0.5, 'g', 3);
  Xval->setText(xcord);
  Yval->setText(ycord);
  Zval->setText(zcord);

  //backup original matrix
  for(int j=0; j<4; j++)
    for(int i=0; i<4; i++)
      // TODO SR and BP: check on the list of transforms and not the first only
      mInitialMatrix->SetElement(i,j, mCurrentSlicerManager->GetImage()->GetTransform()[0]->GetMatrix()->GetElement(i,j));
  QString origTransformString(clitk::Get4x4MatrixDoubleAsString(mInitialMatrix).c_str());
  transformationLabel->setText(origTransformString);
  SetTransform(mInitialMatrix);

  //connect all sigs to slots
  connect(resetbutton, SIGNAL(pressed()), this, SLOT(ResetTransform()));
  connect(loadbutton, SIGNAL(pressed()), this, SLOT(LoadFile()));
  connect(savebutton, SIGNAL(pressed()), this, SLOT(SaveFile()));

  connect(xtrans_slider, SIGNAL(valueChanged(int)), this, SLOT(SliderChange(int)));
  connect(ytrans_slider, SIGNAL(valueChanged(int)), this, SLOT(SliderChange(int)));
  connect(ztrans_slider, SIGNAL(valueChanged(int)), this, SLOT(SliderChange(int)));
  connect(xrot_slider, SIGNAL(valueChanged(int)), this, SLOT(SliderChange(int)));
  connect(yrot_slider, SIGNAL(valueChanged(int)), this, SLOT(SliderChange(int)));
  connect(zrot_slider, SIGNAL(valueChanged(int)), this, SLOT(SliderChange(int)));
  connect(xtrans_sb, SIGNAL(valueChanged(double)), this, SLOT(SpinBoxChange(double)));
  connect(ytrans_sb, SIGNAL(valueChanged(double)), this, SLOT(SpinBoxChange(double)));
  connect(ztrans_sb, SIGNAL(valueChanged(double)), this, SLOT(SpinBoxChange(double)));
  connect(xrot_sb,   SIGNAL(valueChanged(double)), this, SLOT(SpinBoxChange(double)));
  connect(yrot_sb,   SIGNAL(valueChanged(double)), this, SLOT(SpinBoxChange(double)));
  connect(zrot_sb,   SIGNAL(valueChanged(double)), this, SLOT(SpinBoxChange(double)));

  connect(stepTransSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SetTranslationStep(double)));
  connect(stepRotSpinBox, SIGNAL(valueChanged(double)), this, SLOT(SetRotationStep(double)));

  connect(checkBoxDegrees, SIGNAL(stateChanged(int)), this, SLOT(ToggleSpinBoxAnglesUnit()));

  connect(Xval, SIGNAL(editingFinished()), this, SLOT(ChangeOfRotationCenter()));
  connect(Yval, SIGNAL(editingFinished()), this, SLOT(ChangeOfRotationCenter()));
  connect(Zval, SIGNAL(editingFinished()), this, SLOT(ChangeOfRotationCenter()));

  // Init step modifiers
  stepTransSpinBox->setValue(1.);
  stepRotSpinBox->setValue(1.);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::apply()
{
  vvToolWidgetBase::close();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool vvToolRigidReg::close()
{
  QString warning = "Are you sure you want to reset the original transform?";
  QMessageBox msgBox(QMessageBox::Warning, tr("Reset transform"),warning, 0, this);
  msgBox.addButton(tr("Yes"), QMessageBox::AcceptRole);
  msgBox.addButton(tr("No"), QMessageBox::RejectRole);
  if (msgBox.exec() == QMessageBox::AcceptRole) {
    SetTransform(mInitialMatrix);
    return vvToolWidgetBase::close();
  }
  return false;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::reject()
{
  return vvToolWidgetBase::reject();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetTranslationStep(double v)
{
  xtrans_sb->setSingleStep(v);
  ytrans_sb->setSingleStep(v);
  ztrans_sb->setSingleStep(v);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetRotationStep(double v)
{
  xrot_sb->setSingleStep(v);
  yrot_sb->setSingleStep(v);
  zrot_sb->setSingleStep(v);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SliderChange(int newVal)
{
  std::vector<QSlider *> transSliders, rotSliders;
  std::vector<QDoubleSpinBox *> transSBs, rotSBs;
  GetSlidersAndSpinBoxes(transSliders, rotSliders, transSBs, rotSBs);
  for(int i=0; i<3; i++) {
    if(transSliders[i] == QObject::sender()) {
      transSBs[i]->setValue(newVal);
    }
    if(rotSliders[i] == QObject::sender()) {
      double rad = (checkBoxDegrees->checkState()==Qt::Unchecked)?itk::Math::pi/180.:1.;
      rotSBs[i]->setValue(newVal*rad);
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SpinBoxChange(double newVal)
{
  std::vector<QSlider *> transSliders, rotSliders;
  std::vector<QDoubleSpinBox *> transSBs, rotSBs;
  GetSlidersAndSpinBoxes(transSliders, rotSliders, transSBs, rotSBs);
  for(int i=0; i<3; i++) {
    if(transSBs[i] == QObject::sender()) {
      transSliders[i]->blockSignals(true);
      transSliders[i]->setValue(itk::Math::Round<double,double>(newVal));
      transSliders[i]->blockSignals(false);
    }
    if(rotSBs[i] == QObject::sender()) {
      double rad = (checkBoxDegrees->checkState()==Qt::Unchecked)?180./itk::Math::pi:1.;
      rotSliders[i]->blockSignals(true);
      rotSliders[i]->setValue(itk::Math::Round<double,double>(newVal*rad));
      rotSliders[i]->blockSignals(false);
    }
  }

  // Compute transform and set
  // TODO SR and BP: check on the list of transforms and not the first only
  vtkSmartPointer<vtkTransform> transform_final=mInput->GetImage()->GetTransform()[0];
  transform_final->Identity();
  transform_final->PostMultiply();

  // Rotations
  double x=0, y=0 ,z=0;
  x= Xval->text().toDouble();
  y= Yval->text().toDouble();
  z= Zval->text().toDouble();
  double rad = (checkBoxDegrees->checkState()==Qt::Unchecked)?180./itk::Math::pi:1.;
  transform_final->Translate(-x,-y,-z);
  transform_final->RotateY(yrot_sb->value()*rad);
  transform_final->RotateX(xrot_sb->value()*rad);
  transform_final->RotateZ(zrot_sb->value()*rad);
  transform_final->Translate(x,y,z);

  // Translation
  transform_final->Translate(xtrans_sb->value(),
                             ytrans_sb->value(),
                             ztrans_sb->value());
  transform_final->Update();
  SetTransform(transform_final->GetMatrix());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ToggleSpinBoxAnglesUnit()
{
  double rad = (checkBoxDegrees->checkState()==Qt::Unchecked)?itk::Math::pi/180.:180./itk::Math::pi;
  std::vector<QSlider *> transSliders, rotSliders;
  std::vector<QDoubleSpinBox *> transSBs, rotSBs;
  GetSlidersAndSpinBoxes(transSliders, rotSliders, transSBs, rotSBs);
  for(int i=0; i<3; i++) {
    rotSBs[i]->blockSignals(true);
    rotSBs[i]->setValue(rotSBs[i]->value()*rad);
    rotSBs[i]->blockSignals(false);
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SaveFile()
{
  //Write the Transformation Matrix
  std::string absPath = mCurrentSlicerManager->GetFileName();
  absPath = itksys::SystemTools::GetFilenameWithoutExtension(absPath) + std::string(".mat");
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Transformation Matrix File"),
                                            absPath.c_str(),
                                            tr("Text (*.mat *.txt *.doc *.rtf)"));

  QFile file(filename);
  if (file.open(QFile::WriteOnly | QFile::Truncate)) {
    // TODO SR and BP: check on the list of transforms and not the first only
    vtkMatrix4x4* matrix = mCurrentSlicerManager->GetImage()->GetTransform()[0]->GetMatrix();
    QString matrixStr = clitk::Get4x4MatrixDoubleAsString(matrix,16).c_str();
    QTextStream out(&file);
    out << matrixStr;
  }
  else
  {
    QMessageBox::information(this,"Error","Unable to open file for writing");
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::LoadFile()
{
  //Open File to read the transformation parameters
  QString file = QFileDialog::getOpenFileName(
                   this,
                   "Choose the filename for the transformation matrix",
                   vtksys::SystemTools::GetFilenamePath(mCurrentSlicerManager->GetFileName()).c_str(),
                   "Text (*.mat *.txt *.rtf *.doc)");
   if (file.isEmpty())
     return;


  itk::Matrix<double, 4, 4> itkMat = clitk::ReadMatrix3D(file.toStdString());
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  matrix->Identity();
  for(int j=0; j<4; j++)
    for(int i=0; i<4; i++)
      matrix->SetElement(j,i,itkMat[j][i]);
  SetTransform(matrix);
}
//------------------------------------------------------------------------------
  
//------------------------------------------------------------------------------
void vvToolRigidReg::ChangeOfRotationCenter()
{
  // TODO SR and BP: check on the list of transforms and not the first only
  SetTransform(mCurrentSlicerManager->GetImage()->GetTransform()[0]->GetMatrix());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ResetTransform()
{
  SetTransform(mInitialMatrix);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::SetTransform(vtkMatrix4x4 *matrix)
{
  vtkSmartPointer<vtkTransform> transform=vtkSmartPointer<vtkTransform>::New();
  // TODO SR and BP: check on the list of transforms and not the first only
  mCurrentSlicerManager->GetImage()->GetTransform()[0]->SetMatrix(matrix);
  transform->Update();
  Render();
  dynamic_cast<vvMainWindow*>(mMainWindow)->ImageInfoChanged();

  // Compute parameters from transfer using itk Euler transform
  itk::Euler3DTransform<double>::CenterType center;
  center[0] = Xval->text().toDouble();
  center[1] = Yval->text().toDouble();
  center[2] = Zval->text().toDouble();
  itk::Euler3DTransform<double>::MatrixType rotMat;
  itk::Euler3DTransform<double>::OutputVectorType transVec;
  for(int i=0; i<3; i++) {
    transVec[i] = matrix->GetElement(i,3);
    for(int j=0; j<3; j++)
      rotMat[i][j] = matrix->GetElement(i,j);
  }
  itk::Euler3DTransform<double>::Pointer euler;
  euler = itk::Euler3DTransform<double>::New();
  euler->SetCenter(center);
  euler->SetMatrix(rotMat);
  euler->SetOffset(transVec);

  // Modify GUI according to the new parameters
  std::vector<QSlider *> transSliders, rotSliders;
  std::vector<QDoubleSpinBox *> transSBs, rotSBs;
  GetSlidersAndSpinBoxes(transSliders, rotSliders, transSBs, rotSBs);
  for(int i=0; i<3; i++) {
    // Translations
    transSBs[i]->blockSignals(true);
    transSBs[i]->setValue( euler->GetParameters()[i+3] );
    transSBs[i]->blockSignals(false);
    transSliders[i]->blockSignals(true);
    transSliders[i]->setValue( itk::Math::Round<double,double>(euler->GetParameters()[i+3]) );
    transSliders[i]->blockSignals(false);

    // Rotations
    double rad = (checkBoxDegrees->checkState()==Qt::Checked)?180./itk::Math::pi:1.;
    double angleDiff = euler->GetParameters()[i]-rotSBs[i]->value()/rad+2*itk::Math::pi;
    angleDiff = angleDiff - 2*itk::Math::pi*itk::Math::Round<double,double>(angleDiff/(2*itk::Math::pi));
    if(angleDiff>1.e-4) {
      rotSBs[i]->blockSignals(true);
      rotSBs[i]->setValue( euler->GetParameters()[i]*rad );
      rotSBs[i]->blockSignals(false);
    }
    int iAngle = itk::Math::Round<int,double>(euler->GetParameters()[i]*180./itk::Math::pi);
    if((iAngle-rotSliders[i]->value()+360)%360!=0) {
      rotSliders[i]->blockSignals(true);
      rotSliders[i]->setValue(iAngle);
      rotSliders[i]->blockSignals(false);
    }
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Just an helper function to shorten the code with loops on sliders and spinboxes
void vvToolRigidReg::GetSlidersAndSpinBoxes(std::vector<QSlider *>&transSliders, std::vector<QSlider *>&rotSliders,
                                            std::vector<QDoubleSpinBox *>&transSBs, std::vector<QDoubleSpinBox *>&rotSBs)
{
  transSliders.push_back(xtrans_slider);
  transSliders.push_back(ytrans_slider);
  transSliders.push_back(ztrans_slider);

  rotSliders.push_back(xrot_slider);
  rotSliders.push_back(yrot_slider);
  rotSliders.push_back(zrot_slider);

  transSBs.push_back(xtrans_sb);
  transSBs.push_back(ytrans_sb);
  transSBs.push_back(ztrans_sb);

  rotSBs.push_back(xrot_sb);
  rotSBs.push_back(yrot_sb);
  rotSBs.push_back(zrot_sb);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::ExtentMax(const double pointExtent[8][4], double maxExtent[2][3])
{
    double max, min;
    for (int i=0; i<3; ++i) {
        max = pointExtent[0][i];
        min = pointExtent[0][i];
        for (int j=1; j<8; ++j) {
            if (pointExtent[j][i] > max) {
                max = pointExtent[j][i];
            }
            if (pointExtent[j][i] < min) {
                min = pointExtent[j][i];
            }
        }
        maxExtent[0][i] = min;
        maxExtent[1][i] = max;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvToolRigidReg::Render()
{ //out << __func__ << endl;
#if VTK_MAJOR_VERSION > 5
vtkMatrix4x4* matrix = mCurrentSlicerManager->GetImage()->GetTransform()[0]->GetMatrix();
vtkMatrix4x4* matrixTranspose = matrix->NewInstance();
for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j)
    {
        matrixTranspose->SetElement(i,j,matrix->GetElement(j,i));
    }
}
for (int i=0; i<4; ++i) {
   matrixTranspose->SetElement(3,i,matrix->GetElement(3,i));
}
for (int i=0; i<4; ++i) {
   matrixTranspose->SetElement(i,3,matrix->GetElement(i,3));
}
#endif
  for (int i=0; i<mCurrentSlicerManager->GetNumberOfSlicers(); i++)
    {
#if VTK_MAJOR_VERSION > 5 
    double pointExtent[8][4], pointExtentUpdate[8][4];
    std::vector<int> w_ext;
    w_ext=mCurrentSlicerManager->GetImage()->GetSize();
    pointExtent[0][0] = 0.0;
	pointExtent[0][1] = 0.0;
	pointExtent[0][2] = 0.0;
	pointExtent[0][3] = 1.0;
    pointExtent[1][0] = w_ext[0]-1;
	pointExtent[1][1] = w_ext[1]-1;
	pointExtent[1][2] = w_ext[2]-1;
	pointExtent[1][3] = 1.0;
	pointExtent[2][0] = 0.0;
	pointExtent[2][1] = w_ext[1]-1;
	pointExtent[2][2] = w_ext[2]-1;
	pointExtent[2][3] = 1.0;
	pointExtent[3][0] = w_ext[0]-1;
	pointExtent[3][1] = 0.0;
	pointExtent[3][2] = w_ext[2]-1;
	pointExtent[3][3] = 1.0;
	pointExtent[4][0] = w_ext[0]-1;
	pointExtent[4][1] = w_ext[1]-1;
	pointExtent[4][2] = 0.0;
	pointExtent[4][3] = 1.0;
	pointExtent[5][0] = 0.0;
	pointExtent[5][1] = 0.0;
	pointExtent[5][2] = w_ext[2]-1;
	pointExtent[5][3] = 1.0;
	pointExtent[6][0] = 0.0;
	pointExtent[6][1] = w_ext[1]-1;
	pointExtent[6][2] = 0.0;
	pointExtent[6][3] = 1.0;
	pointExtent[7][0] = w_ext[0]-1;
	pointExtent[7][1] = 0.0;
	pointExtent[7][2] = 0.0;
	pointExtent[7][3] = 1.0;
	
	for (int k=0; k<8; ++k) {
	    for (int j=0; j<3; ++j)
	    {
	        pointExtent[k][j] = mCurrentSlicerManager->GetImage()->GetOrigin()[j] + mCurrentSlicerManager->GetImage()->GetSpacing()[j] * pointExtent[k][j];
	    }
	    matrixTranspose->MultiplyPoint(pointExtent[k], pointExtentUpdate[k]);
		for (int j=0; j<3; ++j)
	    {
	        pointExtentUpdate[k][j] = (pointExtentUpdate[k][j] - mCurrentSlicerManager->GetImage()->GetOrigin()[j])/mCurrentSlicerManager->GetImage()->GetSpacing()[j];
	    }
    }
double extUpdateTemp[2][3];
int extUpdate[6];
ExtentMax(pointExtentUpdate, extUpdateTemp);
for (int j=0; j<3; ++j) {
    extUpdate[2*j] = 0;
    extUpdate[2*j+1] = itk::Math::Round<double>(extUpdateTemp[1][j] - extUpdateTemp[0][j]);
}
    mCurrentSlicerManager->GetSlicer(i)->SetRegisterExtent(extUpdate);
    extUpdate[2*mCurrentSlicerManager->GetSlicer(i)->GetOrientation()] = mCurrentSlicerManager->GetSlicer(i)->GetSlice();
    extUpdate[2*mCurrentSlicerManager->GetSlicer(i)->GetOrientation()+1] = mCurrentSlicerManager->GetSlicer(i)->GetSlice();
    
    vtkSmartPointer<vtkOpenGLImageSliceMapper> mapperOpenGL= vtkSmartPointer<vtkOpenGLImageSliceMapper>::New();
    try {
        mapperOpenGL = dynamic_cast<vtkOpenGLImageSliceMapper*>(mCurrentSlicerManager->GetSlicer(i)->GetImageActor()->GetMapper());
    } catch (const std::bad_cast& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Conversion error" << std::endl;
		return;
	}
	mapperOpenGL->SetCroppingRegion(extUpdate);
#endif
    mCurrentSlicerManager->GetSlicer(i)->ForceUpdateDisplayExtent();
    mCurrentSlicerManager->GetSlicer(i)->Render();
    }
}
//------------------------------------------------------------------------------

