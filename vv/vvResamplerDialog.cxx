#ifndef _vvResamplerDialog_CXX
#define _vvResamplerDialog_CXX
#include "vvResamplerDialog.h"
#include "clitkImageResampleGenericFilter.h"
#include "vvSlicer.h"

#include <QFileInfo>
#include <QMessageBox>

#define COLUMN_IMAGE_NAME 7

//====================================================================
vvResamplerDialog::vvResamplerDialog(QWidget * parent, Qt::WindowFlags f)
        :QDialog(parent,f), Ui::vvResamplerDialog() {

    // initialization
    setupUi(this);
    Init();

    // Connect signals & slots
    connect(this, SIGNAL(accepted()), this, SLOT(Resample()));
    connect(mImagesComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(UpdateCurrentInputImage()));

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
//====================================================================

//====================================================================
void vvResamplerDialog::Init() {
    mLastError ="";

    mInputFileFormat = "";
    ComponentType = "";
    mPixelType = "";

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

    UpdateCurrentInputImage();
}
//====================================================================

//====================================================================
void vvResamplerDialog::UpdateCurrentInputImage() {
    // Set current image & index
    mCurrentIndex = mImagesComboBox->currentIndex();
    if (mCurrentIndex == -1) {
        mCurrentImage = NULL;
        return ;
    }
    mCurrentImage = mSlicerManagers[mCurrentIndex]->GetSlicer(0)->GetImage();
    if (mCurrentImage.IsNull()) return;
    mInputFileName = mSlicerManagers[mCurrentIndex]->GetFileName().c_str();

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
//====================================================================

//====================================================================
void vvResamplerDialog::UpdateOutputInfo() {
    mOutputSizeLabel->setText(GetVectorIntAsString(mOutputSize));
    mOutputSpacingLabel->setText(GetVectorDoubleAsString(mOutputSpacing));
    mOutputMemoryLabel->setText(GetSizeInBytes(mOutputSize));
}
//====================================================================

//====================================================================
QString vvResamplerDialog::GetSizeInBytes(std::vector<int> & size) {
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
    return result;
}
//====================================================================

//====================================================================
QString vvResamplerDialog::GetVectorDoubleAsString(std::vector<double> vectorDouble) {
    QString result;
    for (unsigned int i= 0; i<vectorDouble.size(); i++) {
        if (i != 0)
            result += " x ";
        result += QString::number(vectorDouble[i]);
    }
    return result;
}
//====================================================================

//====================================================================
QString vvResamplerDialog::GetVectorIntAsString(std::vector<int> vectorInt) {
    QString result;
    for (unsigned int i= 0; i<vectorInt.size(); i++) {
        if (i != 0)
            result += " x ";
        result += QString::number(vectorInt[i]);
    }
    return result;
}
//====================================================================

//====================================================================
void vvResamplerDialog::FillSizeEdit(std::vector<int> size) {
    xSizeLineEdit->setText(QString::number(size[0]));
    ySizeLineEdit->setText(QString::number(size[1]));
    if (size.size() > 2)
        zSizeLineEdit->setText(QString::number(size[2]));
}
//====================================================================

//====================================================================
void vvResamplerDialog::FillSpacingEdit(std::vector<double> spacing) {
    xSpacingLineEdit->setText(QString::number(spacing[0]));
    ySpacingLineEdit->setText(QString::number(spacing[1]));
    if (spacing.size() > 2)
        zSpacingLineEdit->setText(QString::number(spacing[2]));
}
//====================================================================

//====================================================================
void vvResamplerDialog::UpdateOutputSizeAndSpacing() {
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
//====================================================================

//====================================================================
void vvResamplerDialog::UpdateControlSizeAndSpacing() {
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
//====================================================================

//====================================================================
void vvResamplerDialog::ComputeNewSpacingFromSize() {
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
//====================================================================

//====================================================================
void vvResamplerDialog::ComputeNewSizeFromSpacing() {
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
//====================================================================

//====================================================================
void vvResamplerDialog::ComputeNewSpacingFromScale() {
    xSpacingLineEdit->setText(QString::number(mInputSpacing[0]*scaleSpacingLineEdit->text().toDouble()/100));
    ySpacingLineEdit->setText(QString::number(mInputSpacing[1]*scaleSpacingLineEdit->text().toDouble()/100));
    if (mDimension > 2)
        zSpacingLineEdit->setText(QString::number(mInputSpacing[2]*scaleSpacingLineEdit->text().toDouble()/100));
    ComputeNewSizeFromSpacing();
}
//====================================================================

//====================================================================
void vvResamplerDialog::ComputeNewSizeFromScale() {
    xSizeLineEdit->setText(QString::number(mInputSize[0]*scaleSizeLineEdit->text().toDouble()/100));
    ySizeLineEdit->setText(QString::number(mInputSize[1]*scaleSizeLineEdit->text().toDouble()/100));
    if (mDimension > 2)
        zSizeLineEdit->setText(QString::number(mInputSize[2]*scaleSizeLineEdit->text().toDouble()/100));
    ComputeNewSpacingFromSize();
}
//====================================================================

//====================================================================
void vvResamplerDialog::ComputeNewSpacingFromIso() {
    xSpacingLineEdit->setText(QString::number(isoSpacingLineEdit->text().toDouble()));
    ySpacingLineEdit->setText(QString::number(isoSpacingLineEdit->text().toDouble()));
    if (mDimension > 2)
        zSpacingLineEdit->setText(QString::number(isoSpacingLineEdit->text().toDouble()));
    ComputeNewSizeFromSpacing();
}
//====================================================================

//====================================================================
void vvResamplerDialog::ComputeNewSizeFromIso() {
    xSizeLineEdit->setText(QString::number(isoSizeLineEdit->text().toDouble()));
    ySizeLineEdit->setText(QString::number(isoSizeLineEdit->text().toDouble()));
    if (mDimension > 2)
        zSizeLineEdit->setText(QString::number(isoSizeLineEdit->text().toDouble()));
    ComputeNewSpacingFromSize();
}
//====================================================================

//====================================================================
void vvResamplerDialog::UpdateInterpolation() {
    if (interpolationComboBox->currentText() == "BSpline") {
        bSplineLabel->show();
        bSplineOrderSpinBox->show();
        bLUTFactorLabel->hide();
        bLUTSpinBox->hide();
    }
    else if (interpolationComboBox->currentText() == "Blut (faster BSpline)")   {
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
//====================================================================

//====================================================================
void vvResamplerDialog::UpdateGaussianFilter() {
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
//====================================================================

//====================================================================
void vvResamplerDialog::SetSlicerManagers(std::vector<vvSlicerManager*> & m,int current_image_index) {
    mSlicerManagers = m;
    for (unsigned int i = 0; i < mSlicerManagers.size(); i++) {
        mImagesComboBox->addItem(mSlicerManagers[i]->GetFileName().c_str());
    }
    mImagesComboBox->setCurrentIndex(current_image_index);
}
//====================================================================

//====================================================================
void vvResamplerDialog::Resample() {

    // Get resampler options
    std::vector<double> sigma;
    sigma.push_back(xGaussianLineEdit->text().toDouble());
    sigma.push_back(yGaussianLineEdit->text().toDouble());
    if (mDimension > 2) sigma.push_back(zGaussianLineEdit->text().toDouble());


    // Create resampler filter
    clitk::ImageResampleGenericFilter::Pointer filter = clitk::ImageResampleGenericFilter::New();
    filter->SetOutputSize(mOutputSize);
    filter->SetOutputSpacing(mOutputSpacing);
    filter->SetInterpolationName(interpolationComboBox->currentText().toLower().toStdString());

    if (interpolationComboBox->currentText() == "BSpline")
        filter->SetBSplineOrder(bSplineOrderSpinBox->value());
    else if (interpolationComboBox->currentText() == "Blut (faster BSpline)") {
        filter->SetInterpolationName("blut");
        filter->SetBSplineOrder(bSplineOrderSpinBox->value());
        filter->SetBLUTSampling(bLUTSpinBox->value());
    }
    if (gaussianFilterCheckBox->isChecked())
        filter->SetGaussianSigma(sigma);
    //  filter->SetOutputFileName(OutputFileName.toStdString());
    filter->SetDefaultPixelValue(defaultPixelValueLineEdit->text().toDouble());
    filter->SetInputVVImage(mCurrentImage);

    // Go !
    filter->Update();
    mOutput = filter->GetOutputVVImage();
}
//====================================================================
std::string vvResamplerDialog::GetOutputFileName()
{
    QFileInfo info(mImagesComboBox->currentText());
    return (info.path().toStdString() + "/resampled_" + info.fileName().toStdString());
}

#endif /* end #define _vvResamplerDialog_CXX */

