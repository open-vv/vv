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

#ifndef VVTOOLINPUTSELECTORWIDGET_CXX
#define VVTOOLINPUTSELECTORWIDGET_CXX

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
  DD("OpenImage");

  // Browse for file
  QString Extensions = "Images files ( *.mhd *.hdr *.his)";
  Extensions += ";;All Files (*)";
  QString filename =
    QFileDialog::getOpenFileName(this,tr("Open mask image"),
                                 "",Extensions); //mMainWindow->GetInputPathName()
  if (filename == "") return; // nothing to do
  
  // Open Image
  vvImageReader * mReader = new vvImageReader;
  mReader->SetInputFilename(filename.toStdString());
  mReader->Update(IMAGE);
  if (mReader->GetLastError().size() != 0) {
    std::cerr << "Error while reading " << filename.toStdString() << std::endl;
    QString error = "Cannot open file \n";
    error += mReader->GetLastError().c_str();
    QMessageBox::information(this,tr("Reading problem"),error);
    delete mReader;
    return;
  }
  
  // Create output pointer
  m_Output = vvImage::New();

  // Check type and convert if needed
  vvImage::Pointer temp = mReader->GetOutput();

  if (temp->GetNumberOfDimensions() != 3) {
    std::cerr << "Error while reading " << filename.toStdString() << std::endl;
    QString error;
    error = QString("Cannot open file %1 because it is not 3D\n").arg(filename);
    QMessageBox::information(this,tr("Reading problem"),error);
    delete mReader;
    return;
  }

  DD(temp->GetScalarTypeAsITKString());
  if (temp->GetScalarTypeAsITKString() != "unsigned_char") {
    DD("Cast");
    vtkImageData * p = vtkImageData::New();
    p->SetExtent(temp->GetFirstVTKImageData()->GetExtent ()); // Only first ! could not be 4D
    p->SetScalarTypeToUnsignedChar();
    p->AllocateScalars ();
    p->CopyAndCastFrom(temp->GetFirstVTKImageData(), temp->GetFirstVTKImageData()->GetExtent ());
    m_Output->AddImage(p);
    vvImageWriter * writer = new vvImageWriter;
    writer->SetOutputFileName("a.mhd");
    writer->SetInput(m_Output);
    writer->Update();
  }
  else {
    m_Output = temp;
  }
  
  // Set GUI
  mLabelInputInfo->setText(vtksys::SystemTools::GetFilenameName(filename.toStdString()).c_str());
  emit accepted();
}
//------------------------------------------------------------------------------


#endif

