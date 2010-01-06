#ifndef VV_DEFORMATION_DIALOG
#define VV_DEFORMATION_DIALOG

#include <vector>
#include <QDialog>
#include <QString>
class vvSlicerManager;
#include <vvImage.h>
#include "ui_vvDeformationDialog.h"

class vvDeformationDialog : public QDialog, private Ui::vvDeformationDialog
{
    Q_OBJECT

public:
    vvDeformationDialog(int initialSlicer,const std::vector<vvSlicerManager*>& slicerManagers);
    int GetInputFileIndex() const {
        return inputSequenceBox->currentIndex();
    }
    QString getFieldFile() const {
        return outputLineEdit->text();
    }
    vvImage::Pointer GetOutput() {
        return mOutput;
    }
    vvSlicerManager * GetSelectedSlicer() const;
    int GetReferenceFrameIndex() const;

private slots:
    void computeDeformationField();
    void updateSliderLabel(int refimage);
    void resetSlider(int slicer_index);
    void selectOutputFile();
protected:
    template<unsigned int Dim> void Update_WithDim();
    template<unsigned int Dim,class PixelType> void Update_WithDimAndPixelType();
    std::vector<vvSlicerManager*> mSlicerManagers;
    vvImage::Pointer mOutput;
};



#endif
