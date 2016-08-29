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

// vv
#include "vvWipeImage.h"
#include "vvSlicer.h"
#include "vvMainWindow.h"

// clitk
#include "clitkCropImage_ggo.h"
#include "clitkCropImageGenericFilter.h"
#include "clitkExceptionObject.h"

// qt
#include <QComboBox>
#include <QCursor>
#include <QApplication>
#include <QMessageBox>
#include <QSignalMapper>
#include <QFileDialog>

// vtk
#include <vtkVersion.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>


#include <extensions.h>

#define COLUMN_TREE 0
#define COLUMN_UL_VIEW 1
#define COLUMN_UR_VIEW 2
#define COLUMN_DL_VIEW 3
#define COLUMN_DR_VIEW 4
#define COLUMN_CLOSE_IMAGE 5
#define COLUMN_RELOAD_IMAGE 6
#define COLUMN_IMAGE_NAME 7

//------------------------------------------------------------------------------
vvWipeImage::vvWipeImage():
  Ui::vvWipeImage()
{
    vtkSmartPointer<vtkImageRectilinearWipe> mWipe = vtkSmartPointer<vtkImageRectilinearWipe>::New();
    mWipe->SetWipe(0);
    mWipe->SetPosition(256,256);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvWipeImage::~vvWipeImage()
{
}
//------------------------------------------------------------------------------

void vvWipeImage::Initialize(QString inputPathName, std::vector<vvSlicerManager*> slicerManagers)
{
  mInputPathName = inputPathName;
  mSlicerManagers = slicerManagers;
}

//------------------------------------------------------------------------------
void vvWipeImage::setInputImage(int number, vvImage::Pointer image)
{
  if (image->GetVTKImages().size()) {
    mImage = image;
    mWipe->SetInputData(number, mImage->GetVTKImages()[0]); //[0] pour du 4D ?
  }
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void vvWipeImage::crossPointerChanged()
{
  mWipe->SetPosition(256,256);
  UpdateWipe();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvWipeImage::UpdateWipe()
{

}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvWipeImage::selectWipeImage(int index, QTreeWidget* DataTree)
{ 

  QString Extensions = EXTENSIONS;
  Extensions += ";;All Files (*)";
  QStringList files = QFileDialog::getOpenFileNames(NULL,tr("Load Wipe image"),mInputPathName,Extensions);
  if (files.isEmpty())
    return;

  std::vector<std::string> vecFileNames;
  for (int i = 0; i < files.size(); i++) {
    vecFileNames.push_back(files[i].toStdString());
  }

  AddWipeImage(index,vecFileNames,vvImageReader::IMAGE, DataTree);
}
//------------------------------------------------------------------------------

void vvWipeImage::AddWipeImage(int index, std::vector<std::string> fileNames, vvImageReader::LoadedImageType type, QTreeWidget* DataTree)
{ 
  QString file(fileNames[0].c_str());
  if (QFile::exists(file))
  {
    mInputPathName = itksys::SystemTools::GetFilenamePath(file.toStdString()).c_str();
    itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(
      file.toStdString().c_str(), itk::ImageIOFactory::ReadMode);
    reader->SetFileName(fileNames[0].c_str());
    reader->ReadImageInformation();
    std::string component = reader->GetComponentTypeAsString(reader->GetComponentType());
    int dimension = reader->GetNumberOfDimensions();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    vvProgressDialog progress("Opening " + file.toStdString());
    qApp->processEvents();

    std::string filename = itksys::SystemTools::GetFilenameWithoutExtension(file.toStdString()).c_str();
//    if (mSlicerManagers[index]->SetOverlay(fileNames,dimension, component,type)) {
      //create an item in the tree with good settings
      QTreeWidgetItem *item = new QTreeWidgetItem();
      item->setData(0,Qt::UserRole,file.toStdString().c_str());
      item->setData(1,Qt::UserRole,tr("wipe"));
      QFileInfo fileinfo(file); //Do not show the path
      item->setData(COLUMN_IMAGE_NAME,Qt::DisplayRole,fileinfo.fileName());
      item->setToolTip(COLUMN_IMAGE_NAME, mSlicerManagers[index]->GetListOfAbsoluteFilePathInOneString("wipe").c_str());
      qApp->processEvents();
#if VTK_MAJOR_VERSION > 5
      for ( unsigned int i = 0; i < mSlicerManagers[index]->GetNumberOfSlicers(); i++)
        mSlicerManagers[index]->GetSlicer(i)->ForceUpdateDisplayExtent();
#endif

      for (int j = 1; j <= 4; j++) {
        item->setData(j,Qt::CheckStateRole,DataTree->topLevelItem(index)->data(j,Qt::CheckStateRole));
      }

      //Create the buttons for reload and close
      qApp->processEvents();
      QTreePushButton* cButton = new QTreePushButton;
      cButton->setItem(item);
      cButton->setColumn(COLUMN_CLOSE_IMAGE);
      cButton->setToolTip(tr("close image"));
      cButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/exit.png")));
      connect(cButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
        vvMainWindow::Instance(),SLOT(CloseImage(QTreeWidgetItem*, int)));

      QTreePushButton* rButton = new QTreePushButton;
      rButton->setItem(item);
      rButton->setColumn(COLUMN_RELOAD_IMAGE);
      rButton->setToolTip(tr("reload image"));
      rButton->setIcon(QIcon(QString::fromUtf8(":/common/icons/rotateright.png")));
      //connect(rButton,SIGNAL(clickedInto(QTreeWidgetItem*, int)),
      //  this,SLOT(ReloadImage(QTreeWidgetItem*, int)));

      DataTree->topLevelItem(index)->setExpanded(1);
      DataTree->topLevelItem(index)->addChild(item);
      DataTree->setItemWidget(item, COLUMN_CLOSE_IMAGE, cButton);
      DataTree->setItemWidget(item, COLUMN_RELOAD_IMAGE, rButton);

      //set the id of the image
      QString id = DataTree->topLevelItem(index)->data(COLUMN_IMAGE_NAME,Qt::UserRole).toString();
      item->setData(COLUMN_IMAGE_NAME,Qt::UserRole,id.toStdString().c_str());
      //UpdateTree(DataTree);
      qApp->processEvents();
      //ImageInfoChanged();
      QApplication::restoreOverrideCursor();

      // Update the display to update, e.g., the sliders
//      for(int i=0; i<4; i++)
//        DisplaySliders(index, i);
/*    } else {
      QApplication::restoreOverrideCursor();
      QString error = "Cannot import the new image.\n";
      error += mSlicerManagers[index]->GetLastError().c_str();
      QMessageBox::information(this,tr("Problem reading image !"),error);
    } */
    //WindowLevelChanged();
  }
  else
    QMessageBox::information(NULL,tr("Problem reading wipe Image !"),"File doesn't exist!");
}
//------------------------------------------------------------------------------


