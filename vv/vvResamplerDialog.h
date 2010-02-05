#ifndef _VVRESAMPLERDIALOG_H
#define _VVRESAMPLERDIALOG_H

/*=========================================================================

Program:   vv
Language:  C++
Author :   David Sarrut (david.sarrut@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/


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

