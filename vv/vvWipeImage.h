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
#ifndef VVWIPEIMAGE_H
#define VVWIPEIMAGE_H

//qt
#include <QtGlobal>
#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>
#include <QSlider>
#include <QMessageBox>
#include "QTreePushButton.h"
// vv
#include "vvMainWindowBase.h"
#include "vvImage.h"
#include "vvImageReader.h"
#include "vvProgressDialog.h"
#include "vvSlicerManager.h"
#include "ui_vvWipeImage.h"
//vtk
#include <vtkTransform.h>
#include <vtkImageRectilinearWipe.h>
//itk
#include <itkLightObject.h>

//------------------------------------------------------------------------------
class vvWipeImage:
  public QObject,
  public itk::LightObject,
  private Ui::vvWipeImage
{
  Q_OBJECT
    public:
        
  typedef vvWipeImage Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::ProcessObject::Pointer ConverterPointer;
  
  itkNewMacro(Self);
  vvWipeImage();
  ~vvWipeImage();

  void Initialize(QString inputPathName, std::vector<vvSlicerManager*> slicerManagers);
  void selectWipeImage(int index);
  void setInputImage(int number, vvImage::Pointer image);
  void AddWipeImage(int index, std::vector<std::string> fileNames, vvImageReader::LoadedImageType type);

public slots:
  void crossPointerChanged();

 protected:
  vtkSmartPointer<vtkImageRectilinearWipe> mWipe;
  void UpdateWipe();
  vtkSmartPointer<vtkMatrix4x4> mConcatenedTransform;
  vvImage::Pointer mImage;
  QString mInputPathName;
  std::vector<vvSlicerManager*> mSlicerManagers;

}; // end class vvWipeImage
//------------------------------------------------------------------------------

#endif

