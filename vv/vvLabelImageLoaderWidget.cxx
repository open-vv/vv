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

#ifndef VVTOOLINPUTSELECTORWIDGET_CXX
#define VVTOOLINPUTSELECTORWIDGET_CXX

// clitk
#include <clitkImageCommon.h>

// vv
#include "vvLabelImageLoaderWidget.h"
#include "vvSlicerManager.h"
#include "vvImageReader.h"
#include "vvImageWriter.h"

// vtk
#include <vtkImageData.h>

// qt
#include <QAbstractButton>
#include <QFileDialog>
#include <QMessageBox>

//------------------------------------------------------------------------------
vvLabelImageLoaderWidget::vvLabelImageLoaderWidget(QWidget * parent, Qt::WindowFlags f):
  QWidget(parent, f)
{
  setupUi(this);
  setEnabled(true);
  connect(mOpenButton, SIGNAL(clicked()), this, SLOT(OpenImage()));
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvImage::Pointer vvLabelImageLoaderWidget::GetImage()
{
  return m_Output;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
double vvLabelImageLoaderWidget::GetBackgroundValue()
{
  return mBackgroundValueSpinBox->value();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvLabelImageLoaderWidget::SetText(QString t)
{
  mOpenLabel->setText(t);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvLabelImageLoaderWidget::OpenImage()
{
  // Browse for file
  QString Extensions = "Images files ( *.mhd *.hdr *.his)";
  Extensions += ";;All Files (*)";
  QString filename =
    QFileDialog::getOpenFileName(this,tr("Open mask image"),
                                 "",Extensions); //mMainWindow->GetInputPathName()
  if (filename == "") return; // nothing to do
  
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(filename.toStdString());

  // Open Image
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  if (!header) {
    std::cerr << "Error while reading " << filename.toStdString() << std::endl;
    QString error = QString("Cannot open file %1\n").arg(filename);
    QMessageBox::information(this,tr("Reading problem"),error);
    return;
  }
  
  // Create output pointer
  if (header->GetNumberOfDimensions() != 3) {
    std::cerr << "Error while reading " << filename.toStdString() << std::endl;
    QString error;
    error = QString("Cannot open file %1 because it is not 3D\n").arg(filename);
    QMessageBox::information(this,tr("Reading problem"),error);
    return;
  }

  // Convert to unsigned char while reading (if not already)
  typedef itk::ImageFileReader< itk::Image< unsigned char, 3 > > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.toStdString());
  reader->Update();

  // Create vv image
  m_Output = vvImage::New();
  try {
    m_Output->AddItkImage( reader->GetOutput() );
  }
  catch ( itk::ExceptionObject & err ) {
    std::cerr << "Error while reading " << filename.toStdString() << std::endl
              << "The error is " << err << std::endl;
    QString error;
    error = QString("Cannot open file %1\n").arg(filename);
    QMessageBox::information(this,tr("Reading problem"),error);
    return;
  }

  // Set GUI
  mLabelInputInfo->setText(vtksys::SystemTools::GetFilenameName(filename.toStdString()).c_str());
  QApplication::restoreOverrideCursor();
  emit accepted();
}
//------------------------------------------------------------------------------


#endif

