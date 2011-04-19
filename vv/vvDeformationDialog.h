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
