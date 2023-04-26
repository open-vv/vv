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
#ifndef _vvSegmentationDialog_CXX
#define _vvSegmentationDialog_CXX
#include <QtGui>
#include <Qt>

#include "vvSegmentationDialog.h"
#include "vvProgressDialog.h"
#include "vvImageWriter.h"
#include "vvLandmarks.h"
#include "vvInteractorStyleNavigator.h"
#include "vvSlicer.h"

#include <vtkVersion.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include "vtkMarchingCubes.h"
#include "vtkMarchingSquares.h"
#include "vtkImageClip.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"
#include "vtkLookupTable.h"
#include "vtkClipPolyData.h"
#include "vtkImageToPolyDataFilter.h"
#include "vtkLookupTable.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkImageLogic.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkImageSeedConnectivity.h"
#include "vtkConnectivityFilter.h"
#include "vtkPolyData.h"
#include <vtkPolyDataMapper.h>
#include <vtkImageData.h>
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include <vtkPolyDataWriter.h>

#include <QMessageBox>
#include <QFileDialog>

#ifdef Q_OS_OSX
# include "vvOSXHelper.h"
#endif

//====================================================================
vvSegmentationDialog::vvSegmentationDialog(QWidget * parent, Qt::WindowFlags f)
  :QDialog(parent,f), Ui::vvSegmentationDialog()
{

  // initialization
  setupUi(this);
  mManager = new vvSlicerManager(1);

  mClipper = vtkImageClip::New();
  mSquares1 = vtkMarchingSquares::New();
  mSquaresMapper1 = vtkPolyDataMapper::New();
  mSquaresActor1 = vtkActor::New();

  mSquares2 = vtkMarchingSquares::New();
  mSquaresMapper2 = vtkPolyDataMapper::New();
  mSquaresActor2 = vtkActor::New();

  //m3DMapper = vtkPolyDataMapper::New();
  //m3DActor = vtkActor::New();
  m3DExtractor = vtkMarchingCubes::New();
  m3DExtractor->ComputeScalarsOff();
  m3DMappers.clear();
  m3DActors.clear();

  mBinaireImages.clear();
  mKernelValue = 2;

  connect(clipping1Slider,SIGNAL(valueChanged(int)),this,SLOT(clippingvaluechanged(int)));
  connect(clipping2Slider,SIGNAL(valueChanged(int)),this,SLOT(clippingvaluechanged(int)));
  connect(binaryButton,SIGNAL(clicked()),this,SLOT(BinariseSurface()));
  connect(saveButton,SIGNAL(clicked()),this,SLOT(Save()));
  connect(erodeButton,SIGNAL(clicked()),this,SLOT(Erode()));
  connect(dilateButton,SIGNAL(clicked()),this,SLOT(Dilate()));
  connect(dimButton,SIGNAL(clicked()),this,SLOT(ChangeDimRendering()));
  connect(kernelSpinBox,SIGNAL(valueChanged(int)),this,SLOT(KernelValueChanged(int)));

  binaryButton->setEnabled(0);
  erodeButton->setEnabled(0);
  dilateButton->setEnabled(0);
  infoLabel->setText("Select Up and Down threshold before clicking binarise !");

#ifdef Q_OS_OSX
  disableGLHiDPI(viewWidget->winId());
#endif
}

vvSegmentationDialog::~vvSegmentationDialog()
{
  mClipper->Delete();

  mSquaresActor1->Delete();
  mSquaresMapper1->Delete();
  mSquares1->Delete();

  mSquaresActor2->Delete();
  mSquaresMapper2->Delete();
  mSquares2->Delete();

  //m3DMapper->Delete();
  //m3DActor->Delete();
  m3DExtractor->Delete();

  for (unsigned int i = 0; i < mBinaireImages.size(); i++)
    mBinaireImages[i]->Delete();

  for (unsigned int i = 0; i < m3DActors.size(); i++)
    m3DActors[i]->Delete();

  for (unsigned int i = 0; i < m3DMappers.size(); i++)
    m3DMappers[i]->Delete();

  delete mManager;
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
// Handles the one input operations
template <class T>
void vvImageBinarize(vtkImageData *in1Data, T *in1Ptr,
                     int outExt[6],int clampMin, int clampMax)
{
  int idxR, idxY, idxZ;
  int maxY, maxZ;
  vtkIdType inIncX, inIncY, inIncZ;
  int rowLength;

  // find the region to loop over
  rowLength =
    (outExt[1] - outExt[0]+1)*in1Data->GetNumberOfScalarComponents();
  // What a pain. Maybe I should just make another filter.

  maxY = outExt[3] - outExt[2];
  maxZ = outExt[5] - outExt[4];

  // Get increments to march through data
  in1Data->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);

  for (idxZ = 0; idxZ <= maxZ; idxZ++) {
    for (idxY = 0; idxY <= maxY; idxY++) {
      for (idxR = 0; idxR < rowLength; idxR++) {
        if (static_cast<double>(*in1Ptr) > clampMin && static_cast<double>(*in1Ptr) <= clampMax)
          *in1Ptr = static_cast<T>(1);
        else
          *in1Ptr = static_cast<T>(0);
        in1Ptr++;
      }
      in1Ptr += inIncY;
    }
    in1Ptr += inIncZ;
  }
}

void vvSegmentationDialog::SetImage(vvImage::Pointer image)
{

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  mManager->SetImage(image);
#if VTK_MAJOR_VERSION >= 9
  mManager->SetSlicerWindow(0,viewWidget->renderWindow());
#else
  mManager->SetSlicerWindow(0,viewWidget->GetRenderWindow());
#endif
  vvInteractorStyleNavigator* style = vvInteractorStyleNavigator::New();
  mManager->SetInteractorStyleNavigator(0,style);
  style->Delete();

  double range[2];
  mManager->GetImage()->GetScalarRange(range);
  mManager->GetSlicer(0)->SetColorWindow(range[1]-range[0]);
  mManager->GetSlicer(0)->SetColorLevel((range[1]+range[0])/2);

  clipping1Slider->setMinimum(range[0]);
  clipping1Slider->setMaximum(range[1]);
  clipping2Slider->setMinimum(range[0]);
  clipping2Slider->setMaximum(range[1]);
  clipping1Slider->setValue(range[0]);
  clipping2Slider->setValue(range[1]);

#if VTK_MAJOR_VERSION <= 5
  mClipper->SetInput(mManager->GetSlicer(0)->GetInput());
#else
  mClipper->SetInputData(mManager->GetSlicer(0)->GetInput());
#endif
  mSquares1->SetValue(0,clipping1Slider->value());
  mSquares2->SetValue(0,clipping2Slider->value());

#if VTK_MAJOR_VERSION <= 5
  mSquares1->SetInput(mClipper->GetOutput());
  mSquares2->SetInput(mClipper->GetOutput());

  mSquaresMapper1->SetInput(mSquares1->GetOutput());
  mSquaresMapper2->SetInput(mSquares2->GetOutput());
#else
  mSquares1->SetInputData(mClipper->GetOutput());
  mSquares2->SetInputData(mClipper->GetOutput());

  mSquaresMapper1->SetInputData(mSquares1->GetOutput());
  mSquaresMapper2->SetInputData(mSquares2->GetOutput());
#endif
  mSquaresMapper1->ScalarVisibilityOff();
  mSquaresMapper2->ScalarVisibilityOff();

  mSquaresActor1->SetMapper(mSquaresMapper1);
  mSquaresActor2->SetMapper(mSquaresMapper2);
  mSquaresActor1->GetProperty()->SetColor(1.0,0,0);
  mSquaresActor2->GetProperty()->SetColor(0,0,1.0);
  mSquaresActor1->SetPickable(0);
  mSquaresActor2->SetPickable(0);

  mManager->GetSlicer(0)->GetRenderer()->AddActor(mSquaresActor1);
  mManager->GetSlicer(0)->GetRenderer()->AddActor(mSquaresActor2);

  mSquares1->Update();
  mSquares2->Update();

  UpdateSlice(0,mManager->GetSlicer(0)->GetSlice());

  connect(mManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  connect(mManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
  connect(mManager,SIGNAL(UpdateSliceRange(int,int,int,int,int)),this,SLOT(UpdateSlice(int, int)));
  connect(mManager,SIGNAL(LandmarkAdded()),this,SLOT(InsertSeed()));
  QApplication::restoreOverrideCursor();
}

void vvSegmentationDialog::UpdateSlice(int slicer,int slices)
{
  int slice = mManager->GetSlicer(0)->GetSlice();
  int tslice = mManager->GetSlicer(0)->GetTSlice();
#if VTK_MAJOR_VERSION <= 5
  mClipper->SetInput(mManager->GetSlicer(0)->GetInput());
#else
  mClipper->SetInputData(mManager->GetSlicer(0)->GetInput());
#endif
  int* extent = mManager->GetSlicer(0)->GetImageActor()->GetDisplayExtent();
  mClipper->SetOutputWholeExtent(extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);
  int i;
  for (i = 0; i < 6; i = i+2) {
    if (extent[i] == extent[i+1]) {
      break;
    }
  }

  switch (i) {
  case 0:
    if (mManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[0] > slice) {
      mSquaresActor1->SetPosition(1,0,0);
      mSquaresActor2->SetPosition(1,0,0);
    } else {
      mSquaresActor1->SetPosition(-1,0,0);
      mSquaresActor2->SetPosition(-1,0,0);
    }
    break;
  case 2:
    if (mManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[1] > slice) {
      mSquaresActor1->SetPosition(0,1,0);
      mSquaresActor2->SetPosition(0,1,0);
    } else {
      mSquaresActor1->SetPosition(0,-1,0);
      mSquaresActor2->SetPosition(0,-1,0);
    }
    break;
  case 4:
    if (mManager->GetSlicer(0)->GetRenderer()->GetActiveCamera()->GetPosition()[2] > slice) {
      mSquaresActor1->SetPosition(0,0,1);
      mSquaresActor2->SetPosition(0,0,1);
    } else {
      mSquaresActor1->SetPosition(0,0,-1);
      mSquaresActor2->SetPosition(0,0,-1);
    }
    break;
  }
  mSquares1->Update();
  mSquares2->Update();

  if (m3DActors.size()) {
    for (unsigned int i =0; i < m3DActors.size(); i++) {
      if (m3DActors[i]->GetVisibility()) {
        m3DActors[i]->VisibilityOff();
      }
    }
    std::cout << "display " << tslice << " on " << m3DActors.size()  << std::endl;
    m3DActors[tslice]->VisibilityOn();
  }

  mManager->Render();
}


void vvSegmentationDialog::clippingvaluechanged(int value)
{
  binaryButton->setEnabled(1);
  int min = (clipping1Slider->value() < clipping2Slider->value() ) ?
            clipping1Slider->value():clipping2Slider->value();
  int max = (clipping1Slider->value() > clipping2Slider->value() ) ?
            clipping1Slider->value():clipping2Slider->value();
  mSquares1->SetValue(0,min);
  mSquares2->SetValue(0,max);

  QString textMin = "<b> Min : </b>";
  textMin += QString::number(min);
  QString textMax = "\n <b> Max : </b>";
  textMax += QString::number(max);
  minLabel->setText(textMin);
  maxLabel->setText(textMax);

  if (mSquares1->GetInput()) {
    mSquares1->Update();
    mSquares2->Update();
    mManager->Render();
  }
}


void vvSegmentationDialog::BinariseSurface()
{
  infoLabel->setText("Click erode then space on desired organ !");

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  int clampMin = (clipping1Slider->value() < clipping2Slider->value() ) ?
                 clipping1Slider->value():clipping2Slider->value();
  int clampMax = (clipping1Slider->value() > clipping2Slider->value() ) ?
                 clipping1Slider->value():clipping2Slider->value();
  vtkImageData* outputImage = vtkImageData::New();

  for (unsigned int numImage = 0; numImage < mManager->GetSlicer(0)->GetImage()->GetVTKImages().size(); numImage++) {
    vtkImageData* image = mManager->GetSlicer(0)->GetImage()->GetVTKImages()[numImage];
    int ext[6];
#if VTK_MAJOR_VERSION <= 5
    image->GetWholeExtent(ext);
#else
    image->GetExtent(ext);
#endif
    void *in1Ptr;
    in1Ptr = image->GetScalarPointerForExtent(ext);

    switch (image->GetScalarType()) {
      vtkTemplateMacro(
        vvImageBinarize(image, static_cast<VTK_TT *>(in1Ptr),
                        ext,clampMin,clampMax));
    default:
      std::cerr << "Error, unknown pixel format : " << image->GetScalarTypeAsString() << std::endl;
      return;
    }

    outputImage->Initialize();
    outputImage->SetExtent(ext);
    outputImage->SetOrigin(image->GetOrigin());
    outputImage->SetSpacing(image->GetSpacing());
#if VTK_MAJOR_VERSION <= 5
    outputImage->SetScalarTypeToUnsignedChar();
#else
    outputImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
#endif
    outputImage->CopyAndCastFrom(image,ext);
#if VTK_MAJOR_VERSION <= 5
    outputImage->Update();
#else
    //outputImage->Update();
#endif

    image->DeepCopy(outputImage);
#if VTK_MAJOR_VERSION <= 5
    image->UpdateInformation();
    image->PropagateUpdateExtent();
#else
    //image->UpdateInformation();
    //image->PropagateUpdateExtent();
#endif

    vtkImageData* imageBin = vtkImageData::New();
    imageBin->DeepCopy(image);
#if VTK_MAJOR_VERSION <= 5
    imageBin->Update();
#else
    //imageBin->Update();
#endif
    mBinaireImages.push_back(imageBin);
  }

  outputImage->Delete();
  erodeButton->setEnabled(1);
  QApplication::restoreOverrideCursor();
  mManager->SetColorWindow(2);
  mManager->SetColorLevel(0.5);
  mManager->Render();
}

void vvSegmentationDialog::Erode()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vtkImageContinuousErode3D* erode = vtkImageContinuousErode3D::New();
  erode->SetKernelSize(mKernelValue,mKernelValue,mKernelValue);
  for (unsigned int numImage = 0; numImage < mManager->GetSlicer(0)->GetImage()->GetVTKImages().size(); numImage++) {
    vtkImageData* image = mManager->GetSlicer(0)->GetImage()->GetVTKImages()[numImage];
#if VTK_MAJOR_VERSION <= 5
    erode->SetInput(image);
    erode->Update();
#else
    erode->SetInputData(image);
    //erode->Update();
#endif
    image->DeepCopy(erode->GetOutput());
#if VTK_MAJOR_VERSION <= 5
    image->Update();
#else
    //image->Update();
#endif
  }
  erode->Delete();
  dilateButton->setEnabled(1);
  mManager->Render();
  QApplication::restoreOverrideCursor();
}

void vvSegmentationDialog::Dilate()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  vtkImageContinuousDilate3D* dilate = vtkImageContinuousDilate3D::New();
  vtkImageLogic* And = vtkImageLogic::New();
  And->SetOperationToAnd();
  dilate->SetKernelSize(mKernelValue,mKernelValue,mKernelValue);
  for (unsigned int numImage = 0; numImage < mManager->GetSlicer(0)->GetImage()->GetVTKImages().size(); numImage++) {
    vtkImageData* image = mManager->GetSlicer(0)->GetImage()->GetVTKImages()[numImage];
#if VTK_MAJOR_VERSION <= 5
    dilate->SetInput(image);
#else
    dilate->SetInputData(image);
#endif
    vtkImageData* mask = mBinaireImages[numImage];
#if VTK_MAJOR_VERSION <= 5
    And->SetInput1(dilate->GetOutput());
    And->SetInput2(mask);
#else
    And->SetInput1Data(dilate->GetOutput());
    And->SetInput2Data(mask);
#endif
    And->Update();
    image->DeepCopy(And->GetOutput());
#if VTK_MAJOR_VERSION <= 5
    image->Update();
#else
    //image->Update();
#endif
  }
  And->Delete();
  dilate->Delete();
  mManager->Render();
  QApplication::restoreOverrideCursor();
}

void vvSegmentationDialog::InsertSeed()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  int point4D[4];
  point4D[0] = mManager->GetLandmarks()->GetCoordinates(
                 mManager->GetLandmarks()->GetNumberOfPoints()-1)[0];
  point4D[1] = mManager->GetLandmarks()->GetCoordinates(
                 mManager->GetLandmarks()->GetNumberOfPoints()-1)[1];
  point4D[2] = mManager->GetLandmarks()->GetCoordinates(
                 mManager->GetLandmarks()->GetNumberOfPoints()-1)[2];
  point4D[3] = mManager->GetLandmarks()->GetCoordinates(
                 mManager->GetLandmarks()->GetNumberOfPoints()-1)[3];

  point4D[0] = (point4D[0]-mManager->GetSlicer(0)->GetImage()->GetVTKImages()[0]->GetOrigin()[0])/mManager->GetSlicer(0)->GetImage()->GetVTKImages()[0]->GetSpacing()[0];
  point4D[1] = (point4D[1]-mManager->GetSlicer(0)->GetImage()->GetVTKImages()[0]->GetOrigin()[1])/mManager->GetSlicer(0)->GetImage()->GetVTKImages()[0]->GetSpacing()[1];
  point4D[2] = (point4D[2]-mManager->GetSlicer(0)->GetImage()->GetVTKImages()[0]->GetOrigin()[2])/mManager->GetSlicer(0)->GetImage()->GetVTKImages()[0]->GetSpacing()[2];

  vtkImageSeedConnectivity* seed = vtkImageSeedConnectivity::New();
  seed->SetInputConnectValue(1);
  seed->SetOutputConnectedValue(1);
  seed->SetOutputUnconnectedValue(0);
  seed->AddSeed(point4D[0],point4D[1],point4D[2]);

  for (unsigned int numImage = 0; numImage < mManager->GetSlicer(0)->GetImage()->GetVTKImages().size(); numImage++) {
    vtkImageData* image = mManager->GetSlicer(0)->GetImage()->GetVTKImages()[numImage];
#if VTK_MAJOR_VERSION <= 5
    seed->SetInput(image);
#else
    seed->SetInputData(image);
#endif
    seed->Update();
    image->DeepCopy(seed->GetOutput());
#if VTK_MAJOR_VERSION <= 5
    image->Update();
#else
    //image->Update();
#endif
  }

  seed->Delete();
  QApplication::restoreOverrideCursor();
}

void vvSegmentationDialog::ChangeDimRendering()
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if (dimButton->text() == "3D") {
    if (m3DActors.size() == 0) {
      m3DExtractor->SetValue(0,0.5);
      for (unsigned int numImage = 0; numImage < mManager->GetSlicer(0)->GetImage()->GetVTKImages().size(); numImage++) {
        vtkActor* actor = vtkActor::New();
#if VTK_MAJOR_VERSION <= 5
        m3DExtractor->SetInput(mManager->GetSlicer(0)->GetImage()->GetVTKImages()[numImage]);
#else
        m3DExtractor->SetInputData(mManager->GetSlicer(0)->GetImage()->GetVTKImages()[numImage]);
#endif
        m3DExtractor->Update();

        vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
#if VTK_MAJOR_VERSION <= 5
        mapper->SetInput(m3DExtractor->GetOutput());
#else
        mapper->SetInputData(m3DExtractor->GetOutput());
#endif
        m3DMappers.push_back(mapper);

        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1.0,0.7,0.2);
        actor->VisibilityOff();

        mManager->GetSlicer(0)->GetRenderer()->AddActor(actor);
        m3DActors.push_back(actor);
      }
    }

    mManager->GetSlicer(0)->GetRenderer()->SetBackground(0.5,0.6,0.9);
    m3DActors[0]->VisibilityOn();

    vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New();
    mManager->SetInteractorStyleNavigator(0,style);
    style->Delete();

    mManager->GetSlicer(0)->GetImageActor()->VisibilityOff();
    mSquaresActor1->VisibilityOff();
    mSquaresActor2->VisibilityOff();
    mManager->Render();
    dimButton->setText("2D");
  } else {
    mManager->GetSlicer(0)->GetRenderer()->SetBackground(0.0,0.0,0.0);
    vvInteractorStyleNavigator* style = vvInteractorStyleNavigator::New();
    mManager->SetInteractorStyleNavigator(0,style);
    style->Delete();

    mManager->GetSlicer(0)->SetSliceOrientation(2);
    m3DActors[mManager->GetSlicer(0)->GetTSlice()]->VisibilityOff();

    mManager->GetSlicer(0)->GetImageActor()->VisibilityOn();
    mSquaresActor1->VisibilityOn();
    mSquaresActor2->VisibilityOn();
    dimButton->setText("3D");
  }
  QApplication::restoreOverrideCursor();
}

void vvSegmentationDialog::KernelValueChanged(int kernel)
{
  mKernelValue = kernel;
}

void vvSegmentationDialog::Save()
{
  if (dimButton->text() == "2D") { //If in *3D* mode, save the mesh
    QString fileName = QFileDialog::getSaveFileName(this,
                       tr("Save Mesh As"),
                       QDir::home().dirName(),
                       "Mesh Files (*.vtk *.vtp)");
    if (!fileName.isEmpty()) {
      vtkSmartPointer<vtkPolyDataWriter> w = vtkSmartPointer<vtkPolyDataWriter>::New();
#if VTK_MAJOR_VERSION <= 5
      w->SetInput(m3DExtractor->GetOutput());
#else
      w->SetInputData(m3DExtractor->GetOutput());
#endif
      w->SetFileName(fileName.toStdString().c_str());
      w->Write();
    }
  } else {
    QStringList OutputListeFormat;
    OutputListeFormat.clear();
    int dimension = mManager->GetDimension();
    if (dimension == 1) {
      OutputListeFormat.push_back(".mhd");
      OutputListeFormat.push_back(".mha");
    }
    if (dimension == 2) {
      OutputListeFormat.push_back(".bmp");
      OutputListeFormat.push_back(".png");
      OutputListeFormat.push_back(".jpeg");
      OutputListeFormat.push_back(".tif");
      OutputListeFormat.push_back(".mha");
      OutputListeFormat.push_back(".mhd");
      OutputListeFormat.push_back(".hdr");
      OutputListeFormat.push_back(".vox");
    } else if (dimension == 3) {
      OutputListeFormat.push_back(".mha");
      OutputListeFormat.push_back(".mhd");
      OutputListeFormat.push_back(".hdr");
      OutputListeFormat.push_back(".vox");
    } else if (dimension == 4) {
      OutputListeFormat.push_back(".mha");
      OutputListeFormat.push_back(".mhd");
    }
    QString Extensions = "AllFiles(*.*)";
    for (int i = 0; i < OutputListeFormat.count(); i++) {
      Extensions += ";;Images ( *";
      Extensions += OutputListeFormat[i];
      Extensions += ")";
    }
    QString fileName = QFileDialog::getSaveFileName(this,
                       tr("Save As"),
                       QDir::home().dirName(),
                       Extensions);
    if (!fileName.isEmpty()) {
      std::string fileformat = vtksys::SystemTools::GetFilenameLastExtension(fileName.toStdString());
      if (OutputListeFormat.contains(
            fileformat.c_str())) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        vvProgressDialog progress("Saving "+fileName.toStdString());
        qApp->processEvents();
        vvImageWriter::Pointer writer = vvImageWriter::New();
        writer->SetOutputFileName(fileName.toStdString());
        writer->SetInput(mManager->GetSlicer(0)->GetImage());
        writer->Update(dimension,"unsigned_char");
        QApplication::restoreOverrideCursor();
        if (writer->GetLastError().size()) {
          QString error = "Saving did not succeed\n";
          error += writer->GetLastError().c_str();
          QMessageBox::information(this,tr("Saving Problem"),error);
          Save();
        }
      } else {
        QString error = fileformat.c_str();
        if (error.isEmpty())
          error += "no file format specified !";
        else
          error += " format unknown !!!\n";
        QMessageBox::information(this,tr("Saving Problem"),error);
        Save();
      }
    }
  }
}

#endif /* end #define _vvSegmentationDialog_CXX */

