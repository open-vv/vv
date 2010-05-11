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

#include "vvToolResample.h"
#include "vvSlicer.h"
#include "vvToolCreator.h"

#include <QFileInfo>
#include <QMessageBox>

#define COLUMN_IMAGE_NAME 7

ADD_TOOL(vvToolResample);

//------------------------------------------------------------------------------
vvToolResample::vvToolResample(vvMainWindowBase * parent, Qt::WindowFlags f):
  vvToolWidgetBase(parent,f),
  vvToolBase<vvToolResample>(parent),
  Ui::vvToolResample() {
  Ui_vvToolResample::setupUi(mToolWidget);

  mFilter = clitk::ImageResampleGenericFilter::New();
  mLastError ="";

  mInputFileFormat = "";
  ComponentType = "";
  mPixelType = "";
  mCurrentSlicerManager=NULL;

  mInputSize.resize(0);
  mInputSpacing.resize(0);
  mInputOrigin.resize(0);
  mOutputSize.resize(0);
  mOutputSpacing.resize(0);

  xSizeLineEdit->setText("");
  ySizeLineEdit->setText("");
  zSizeLineEdit->setText("");
  xSpacingLineEdit->setText("");
  ySpacingLineEdit->setText("");
  zSpacingLineEdit->setText("");
  scaleSizeLineEdit->setText("");
  scaleSpacingLineEdit->setText("");

  sizeRadioButton->setChecked(0);
  scaleSizeRadioButton->setChecked(0);
  isoSizeRadioButton->setChecked(0);
  spacingRadioButton->setChecked(0);
  scaleSpacingRadioButton->setChecked(0);
  isoSpacingRadioButton->setChecked(0);

  gaussianFilterCheckBox->setCheckState(Qt::Unchecked);

  xGaussianLineEdit->hide();
  yGaussianLineEdit->hide();
  zGaussianLineEdit->hide();
  gaussianFilterLabel->hide();

  bSplineLabel->hide();
  bSplineOrderSpinBox->hide();
  bLUTFactorLabel->hide();
  bLUTSpinBox->hide();
  mDimension = -1;

  QPalette qPalette;
  qPalette.setColor(QPalette::Foreground, QColor(Qt::blue));
  mInputFormatLabel->setPalette(qPalette);
  mInputDimLabel->setPalette(qPalette);
  mInputPixelTypeLabel->setPalette(qPalette);
  mInputSizeLabel->setPalette(qPalette);
  mInputSpacingLabel->setPalette(qPalette);

  // Set how many inputs are needed for this tool
  AddInputSelector("Select an image to resample", mFilter);

  // Connect signals & slots

  connect(sizeRadioButton, SIGNAL(clicked()), this, SLOT(UpdateControlSizeAndSpacing()));
  connect(scaleSizeRadioButton, SIGNAL(clicked()), this, SLOT(UpdateControlSizeAndSpacing()));
  connect(isoSizeRadioButton, SIGNAL(clicked()), this, SLOT(UpdateControlSizeAndSpacing()));
  connect(spacingRadioButton, SIGNAL(clicked()), this, SLOT(UpdateControlSizeAndSpacing()));
  connect(scaleSpacingRadioButton, SIGNAL(clicked()), this, SLOT(UpdateControlSizeAndSpacing()));
  connect(isoSpacingRadioButton, SIGNAL(clicked()), this, SLOT(UpdateControlSizeAndSpacing()));

  connect(xSizeLineEdit, SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSpacingFromSize()));
  connect(ySizeLineEdit, SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSpacingFromSize()));
  connect(zSizeLineEdit, SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSpacingFromSize()));
  connect(xSpacingLineEdit, SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSizeFromSpacing()));
  connect(ySpacingLineEdit, SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSizeFromSpacing()));
  connect(zSpacingLineEdit, SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSizeFromSpacing()));
  connect(scaleSizeLineEdit,SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSizeFromScale()));
  connect(scaleSpacingLineEdit,SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSpacingFromScale()));
  connect(isoSizeLineEdit,SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSizeFromIso()));
  connect(isoSpacingLineEdit,SIGNAL(textEdited(QString)),this,SLOT(ComputeNewSpacingFromIso()));

  connect(gaussianFilterCheckBox,SIGNAL(stateChanged(int)),this,SLOT(UpdateGaussianFilter()));
  connect(interpolationComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(UpdateInterpolation()));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::Initialize() {
  SetToolName("Resample Image");
  SetToolMenuName("Resample");
  SetToolIconFilename(":/common/icons/resample.png");
  SetToolTip("Resample image with various interpolation methods.");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::InputIsSelected(vvSlicerManager* m) {

  mCurrentSlicerManager = m;
  mCurrentImage = mCurrentSlicerManager->GetSlicer(0)->GetImage();
  if (mCurrentImage.IsNull()) return;
  mInputFileName = mCurrentSlicerManager->GetFileName().c_str();

  // Set current information
  mPixelType = mCurrentImage->GetScalarTypeAsString().c_str();
  //ds      ComponentType = mCurrentImageGetNumberOfScalarComponents();
  mDimension = mCurrentImage->GetNumberOfDimensions();

  // Copy size, spacing ...
  mInputOrigin.resize(mDimension);
  mInputSpacing.resize(mDimension);
  mInputSize.resize(mDimension);
  for (int i = 0; i < mDimension;i++) {
    mInputOrigin[i] = mCurrentImage->GetOrigin()[i];
    mInputSpacing[i] = mCurrentImage->GetSpacing()[i];
    mInputSize[i] = mCurrentImage->GetSize()[i];
  }

  // Get file format
  mInputFileFormat = itksys::SystemTools::GetFilenameLastExtension(mInputFileName.toStdString()).c_str();

  // Display infos
  mInputFormatLabel->setText(mInputFileFormat);
  mInputSizeLabel->setText(GetVectorIntAsString(mInputSize));
  mInputDimLabel->setText(QString::number(mDimension)+"D");
  mInputSpacingLabel->setText(GetVectorDoubleAsString(mInputSpacing));
  mInputPixelTypeLabel->setText(mPixelType);
  mInputMemoryLabel->setText(GetSizeInBytes(mInputSize));

  // Set current size
  scaleSizeRadioButton->setChecked(true);
  UpdateControlSizeAndSpacing();
  scaleSizeLineEdit->setText("100");
  ComputeNewSizeFromScale();

  // Update output
  UpdateOutputInfo();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::UpdateOutputInfo() {
  mOutputSizeLabel->setText(GetVectorIntAsString(mOutputSize));
  mOutputSpacingLabel->setText(GetVectorDoubleAsString(mOutputSpacing));
  mOutputMemoryLabel->setText(GetSizeInBytes(mOutputSize));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
QString vvToolResample::GetSizeInBytes(std::vector<int> & size) {
  int t = 1;
  for (unsigned int i=0; i<size.size(); i++) t *= size[i];
  t *= mCurrentImage->GetScalarSize()*mCurrentImage->GetNumberOfScalarComponents();
  QString result = QString::number(t);
  result += " bytes (";
  if (t > 1000000000) {
    t /= 1000000000;
    result += QString::number(t);
    result += " GB)";
  }
  else if (t > 1000000) {
    t /= 1000000;
    result += QString::number(t);
    result += " MB)";
  }
  else if (t > 1000) {
    t /= 1000;
    result += QString::number(t);
    result += " KB)";
  }
  else result += ")";
  return result;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
QString vvToolResample::GetVectorDoubleAsString(std::vector<double> vectorDouble) {
  QString result;
  for (unsigned int i= 0; i<vectorDouble.size(); i++) {
    if (i != 0)
      result += " x ";
    result += QString::number(vectorDouble[i]);
  }
  return result;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
QString vvToolResample::GetVectorIntAsString(std::vector<int> vectorInt) {
  QString result;
  for (unsigned int i= 0; i<vectorInt.size(); i++) {
    if (i != 0)
      result += " x ";
    result += QString::number(vectorInt[i]);
  }
  return result;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::FillSizeEdit(std::vector<int> size) {
  xSizeLineEdit->setText(QString::number(size[0]));
  ySizeLineEdit->setText(QString::number(size[1]));
  if (size.size() > 2)
    zSizeLineEdit->setText(QString::number(size[2]));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::FillSpacingEdit(std::vector<double> spacing) {
  xSpacingLineEdit->setText(QString::number(spacing[0]));
  ySpacingLineEdit->setText(QString::number(spacing[1]));
  if (spacing.size() > 2)
    zSpacingLineEdit->setText(QString::number(spacing[2]));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::UpdateOutputSizeAndSpacing() {
  mOutputSize.resize(mDimension);
  mOutputSize = mInputSize;
  mOutputSpacing.resize(mDimension);
  mOutputSpacing = mInputSpacing;
  mOutputSize[0] = (int)xSizeLineEdit->text().toDouble();
  mOutputSize[1] = (int)ySizeLineEdit->text().toDouble();
  if (mDimension > 2)
    mOutputSize[2] = (int)zSizeLineEdit->text().toDouble();

  mOutputSpacing[0] = xSpacingLineEdit->text().toDouble();
  mOutputSpacing[1] = ySpacingLineEdit->text().toDouble();
  if (mDimension > 2)
    mOutputSpacing[2] = zSpacingLineEdit->text().toDouble();

  UpdateOutputInfo();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::UpdateControlSizeAndSpacing() {
  scaleSizeLineEdit->setText("");
  scaleSpacingLineEdit->setText("");
  isoSizeLineEdit->setText("");
  isoSpacingLineEdit->setText("");

  xSizeLineEdit->setReadOnly(1);
  ySizeLineEdit->setReadOnly(1);
  zSizeLineEdit->setReadOnly(1);
  scaleSizeLineEdit->setReadOnly(1);
  isoSizeLineEdit->setReadOnly(1);

  xSpacingLineEdit->setReadOnly(1);
  ySpacingLineEdit->setReadOnly(1);
  zSpacingLineEdit->setReadOnly(1);
  scaleSpacingLineEdit->setReadOnly(1);
  isoSpacingLineEdit->setReadOnly(1);

  if (sizeRadioButton->isChecked()) {
    xSizeLineEdit->setReadOnly(0);
    ySizeLineEdit->setReadOnly(0);
    if (mDimension > 2)
      zSizeLineEdit->setReadOnly(0);
  }
  else {
    if (spacingRadioButton->isChecked()) {
      xSpacingLineEdit->setReadOnly(0);
      ySpacingLineEdit->setReadOnly(0);
      if (mDimension > 2)
        zSpacingLineEdit->setReadOnly(0);
    }
    else if (scaleSizeRadioButton->isChecked())
      scaleSizeLineEdit->setReadOnly(0);
    else if (scaleSpacingRadioButton->isChecked())
      scaleSpacingLineEdit->setReadOnly(0);
    else if (isoSizeRadioButton->isChecked())
      isoSizeLineEdit->setReadOnly(0);
    else if (isoSpacingRadioButton->isChecked())
      isoSpacingLineEdit->setReadOnly(0);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::ComputeNewSpacingFromSize() {
  double newSpacing = mInputSpacing[0]*mInputSize[0];
  xSpacingLineEdit->setText(QString::number(newSpacing/xSizeLineEdit->text().toDouble()));
  newSpacing = mInputSpacing[1]*mInputSize[1];
  ySpacingLineEdit->setText(QString::number(newSpacing/ySizeLineEdit->text().toDouble()));
  if (mDimension > 2)
    {
      newSpacing = mInputSpacing[2]*mInputSize[2];
      zSpacingLineEdit->setText(QString::number(newSpacing/zSizeLineEdit->text().toDouble()));
    }
  UpdateOutputSizeAndSpacing();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::ComputeNewSizeFromSpacing() {
  double newSize = mInputSpacing[0]*mInputSize[0];
  xSizeLineEdit->setText(QString::number(newSize/xSpacingLineEdit->text().toDouble()));
  newSize = mInputSpacing[1]*mInputSize[1];
  ySizeLineEdit->setText(QString::number(newSize/ySpacingLineEdit->text().toDouble()));
  if (mDimension > 2)
    {
      newSize = mInputSpacing[2]*mInputSize[2];
      zSizeLineEdit->setText(QString::number(newSize/zSpacingLineEdit->text().toDouble()));
    }
  UpdateOutputSizeAndSpacing();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::ComputeNewSpacingFromScale() {
  xSpacingLineEdit->setText(QString::number(mInputSpacing[0]*scaleSpacingLineEdit->text().toDouble()/100));
  ySpacingLineEdit->setText(QString::number(mInputSpacing[1]*scaleSpacingLineEdit->text().toDouble()/100));
  if (mDimension > 2)
    zSpacingLineEdit->setText(QString::number(mInputSpacing[2]*scaleSpacingLineEdit->text().toDouble()/100));
  ComputeNewSizeFromSpacing();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::ComputeNewSizeFromScale() {
  xSizeLineEdit->setText(QString::number(mInputSize[0]*scaleSizeLineEdit->text().toDouble()/100));
  ySizeLineEdit->setText(QString::number(mInputSize[1]*scaleSizeLineEdit->text().toDouble()/100));
  if (mDimension > 2)
    zSizeLineEdit->setText(QString::number(mInputSize[2]*scaleSizeLineEdit->text().toDouble()/100));
  ComputeNewSpacingFromSize();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::ComputeNewSpacingFromIso() {
  xSpacingLineEdit->setText(QString::number(isoSpacingLineEdit->text().toDouble()));
  ySpacingLineEdit->setText(QString::number(isoSpacingLineEdit->text().toDouble()));
  if (mDimension > 2)
    zSpacingLineEdit->setText(QString::number(isoSpacingLineEdit->text().toDouble()));
  ComputeNewSizeFromSpacing();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::ComputeNewSizeFromIso() {
  xSizeLineEdit->setText(QString::number(isoSizeLineEdit->text().toDouble()));
  ySizeLineEdit->setText(QString::number(isoSizeLineEdit->text().toDouble()));
  if (mDimension > 2)
    zSizeLineEdit->setText(QString::number(isoSizeLineEdit->text().toDouble()));
  ComputeNewSpacingFromSize();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::UpdateInterpolation() {
  if (interpolationComboBox->currentText() == "BSpline") {
    bSplineLabel->show();
    bSplineOrderSpinBox->show();
    bLUTFactorLabel->hide();
    bLUTSpinBox->hide();
  }
  else if (interpolationComboBox->currentText() == "B-LUT (faster BSpline)")   {
    bSplineLabel->show();
    bSplineOrderSpinBox->show();
    bLUTFactorLabel->show();
    bLUTSpinBox->show();
  }
  else {
    bSplineLabel->hide();
    bSplineOrderSpinBox->hide();
    bLUTFactorLabel->hide();
    bLUTSpinBox->hide();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::UpdateGaussianFilter() {
  if (gaussianFilterCheckBox->isChecked()) {
    gaussianFilterLabel->show();
    xGaussianLineEdit->show();
    yGaussianLineEdit->show();
    if (mDimension > 2)
      zGaussianLineEdit->show();
  }
  else {
    gaussianFilterLabel->hide();
    xGaussianLineEdit->hide();
    yGaussianLineEdit->hide();
    zGaussianLineEdit->hide();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvToolResample::apply() {

  // Get resampler options
  std::vector<double> sigma;
  sigma.push_back(xGaussianLineEdit->text().toDouble());
  sigma.push_back(yGaussianLineEdit->text().toDouble());
  if (mDimension > 2) sigma.push_back(zGaussianLineEdit->text().toDouble());
  if (mDimension == 4) sigma.push_back(0.01); //FIXME Don't filter along the temporal direction

  mFilter->SetOutputSize(mOutputSize);
  mFilter->SetOutputSpacing(mOutputSpacing);
  mFilter->SetInterpolationName(interpolationComboBox->currentText().toLower().toStdString());

  if (interpolationComboBox->currentText() == "BSpline")
    mFilter->SetBSplineOrder(bSplineOrderSpinBox->value());
  else if (interpolationComboBox->currentText() == "B-LUT (faster BSpline)") {
    mFilter->SetInterpolationName("blut");
    mFilter->SetBSplineOrder(bSplineOrderSpinBox->value());
    mFilter->SetBLUTSampling(bLUTSpinBox->value());
  }
  if (gaussianFilterCheckBox->isChecked())
    mFilter->SetGaussianSigma(sigma);
  //  mFilter->SetOutputFileName(OutputFileName.toStdString());
  mFilter->SetDefaultPixelValue(defaultPixelValueLineEdit->text().toDouble());
  mFilter->SetInputVVImage(mCurrentImage);

  // Go !
  mFilter->Update();
  mOutput = mFilter->GetOutputVVImage();
  AddImage(mOutput,GetOutputFileName());
  close();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::string vvToolResample::GetOutputFileName() {
  QFileInfo info(QString(mCurrentSlicerManager->GetFileName().c_str()));
  return (info.path().toStdString() + "/resampled_" + info.fileName().toStdString());
}
//------------------------------------------------------------------------------

