#ifndef vvCropDialog_h
#define vvCropDialog_h
#include <vector>
#include <QDialog>
class vvSlicerManager;
#include "vvImage.h"
#include "ui_vvCropDialog.h"

///Allows the user to create a new image by cropping an existing one
class vvCropDialog : public QDialog,private Ui::vvCropDialog
{
    Q_OBJECT
public:
    vvCropDialog(std::vector<vvSlicerManager*> sms,int current=0);
    vvImage::Pointer GetOutput() {return mResult;}

protected:
    std::vector<vvSlicerManager*> mSlicerManagers;
    vvImage::Pointer mResult;
    
private slots:
    void ImageChanged(int newindex);
    void ComputeCroppedImage();
};

#endif
