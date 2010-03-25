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
