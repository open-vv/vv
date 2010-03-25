#ifndef _VVRESAMPLERDIALOG_H
#define _VVRESAMPLERDIALOG_H
#include "ui_vvResamplerDialog.h"
#include "clitkCommon.h"
#include "vvImage.h"
#include "vvSlicerManager.h"

#include <QtDesigner/QDesignerExportWidget>
#include <QTreeWidget>

//====================================================================
class vvResamplerDialog :  public QDialog, private Ui::vvResamplerDialog {

    Q_OBJECT

public:
    // constructor - destructor
    vvResamplerDialog(QWidget * parent=0, Qt::WindowFlags f=0);
    void SetSlicerManagers(std::vector<vvSlicerManager*> & m,int current_image_index);

    // Get output result
    vvImage::Pointer GetOutput() {
        return mOutput;
    }
    bool GetDisplayResult() {
        return display_result->checkState() > 0;
    }
    std::string GetOutputFileName();

public slots:
//  void SetImagesList(QTreeWidget * tree);
    void Resample();
    void UpdateControlSizeAndSpacing();
    void ComputeNewSizeFromSpacing();
    void ComputeNewSizeFromScale();
    void ComputeNewSizeFromIso();
    void ComputeNewSpacingFromSize();
    void ComputeNewSpacingFromScale();
    void ComputeNewSpacingFromIso();
    void UpdateInterpolation();
    void UpdateGaussianFilter();
    void UpdateCurrentInputImage();

protected:
    Ui::vvResamplerDialog ui;
    std::vector<vvSlicerManager*> mSlicerManagers;
    vvImage::Pointer mOutput;

    vvImage::Pointer mCurrentImage;
    int mCurrentIndex;

    std::vector<int> mInputOrigin;
    std::vector<int> mInputSize;
    std::vector<double> mInputSpacing;
    std::vector<int> mOutputSize;
    std::vector<double> mOutputSpacing;
    int mDimension;

    QString mLastError;

    QString mInputFileName;

    QString mInputFileFormat;
    QString mPixelType;
    QString ComponentType;

    QStringList OutputListFormat;

    void Init();
    void UpdateInputInfo();
    void UpdateOutputInfo();
    void UpdateOutputFormat();
    void FillSizeEdit(std::vector<int> size);
    void FillSpacingEdit(std::vector<double> spacing);
    void UpdateOutputSizeAndSpacing();

    QString GetSizeInBytes(std::vector<int> & size);
    QString GetVectorDoubleAsString(std::vector<double> vectorDouble);
    QString GetVectorIntAsString(std::vector<int> vectorInt);

}; // end class vvResamplerDialog
//====================================================================

#endif /* end #define _VVRESAMPLERDIALOG_H */

