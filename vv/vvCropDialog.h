/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)
 Program:   vv

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
