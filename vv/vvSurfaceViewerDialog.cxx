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
#ifndef _vvSurfaceViewerDialog_CXX
#define _vvSurfaceViewerDialog_CXX
#include <QtGui>
#include <Qt>

#include "vvSurfaceViewerDialog.h"
#include "vtkOBJReader.h"
#include "vtkInteractorStyle.h"

#include <vtkVersion.h>
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"

#include <QMessageBox>
#include <QFileDialog>

//----------------------------------------------------------------------------
class vvManagerCallback : public vtkCommand
{
public:
  static vvManagerCallback *New() {
    return new vvManagerCallback;
  }

  void Execute(vtkObject *caller,
               unsigned long event,
               void *vtkNotUsed(callData)) {
    vtkInteractorStyle *isi = static_cast<vtkInteractorStyle *>(caller);
    if (isi) {
      std::string KeyPress = isi->GetInteractor()->GetKeySym();
      if (KeyPress == "Left") {
        this->IV->PreviousTime();
      }
      if (KeyPress == "Right") {
        this->IV->NextTime();
      }
    }
  }
  vvSurfaceViewerDialog *IV;
};


//====================================================================
vvSurfaceViewerDialog::vvSurfaceViewerDialog(QWidget * parent, Qt::WindowFlags f)
  :QDialog(parent,f), Ui::vvSurfaceViewerDialog()
{

  // initialization
  setupUi(this);

  mReaders.clear();
  mMapper = NULL;
  mActor = NULL;
  mRenderer = NULL;
  mCurrentTime = 0;

  connect(loadButton,SIGNAL(clicked()),this,SLOT(LoadSurface()));
}

vvSurfaceViewerDialog::~vvSurfaceViewerDialog()
{
  for (unsigned int i = 0; i < mReaders.size(); i++)
    mReaders[i]->Delete();

  if (mMapper)
    mMapper->Delete();

  if (mActor)
    mActor->Delete();

  if (mRenderer)
    mRenderer->Delete();
}

void vvSurfaceViewerDialog::LoadSurface()
{
  QString Extensions = "All Files (*)";
  Extensions += ";;Surface ( *.vtk)";
  QStringList files = QFileDialog::getOpenFileNames(this,tr("Load Surfaces"),tr(""),Extensions);
  if (files.isEmpty())
    return;

  if (!mMapper) {
    mMapper = vtkPolyDataMapper::New();
  }

  for (int i = 0; i < files.size(); i++) {
    vtkOBJReader* reader = vtkOBJReader::New();
    reader->SetFileName(files[i].toStdString().c_str());
    reader->Update();
    mReaders.push_back(reader);
  }
#if VTK_MAJOR_VERSION <= 5
  mMapper->SetInput(mReaders[mCurrentTime]->GetOutput());
#else
  mMapper->SetInputData(mReaders[mCurrentTime]->GetOutput());
#endif

  if (!mActor) {
    mActor = vtkActor::New();
    mActor->SetMapper(mMapper);
  }
  if (!mRenderer) {
    mRenderer = vtkRenderer::New();
    mRenderer->AddActor(mActor);
    renderWidget->GetRenderWindow()->AddRenderer(mRenderer);
  }

  mRenderer->ResetCamera();
  renderWidget->GetRenderWindow()->Render();

  vvManagerCallback *smc = vvManagerCallback::New();
  smc->IV = this;

  if (renderWidget->GetRenderWindow()->GetInteractor())
    renderWidget->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::KeyPressEvent, smc);
  //readHeader->Delete();
}

void vvSurfaceViewerDialog::NextTime()
{
  mCurrentTime++;
  if (mCurrentTime >= mReaders.size())
    mCurrentTime = 0;
#if VTK_MAJOR_VERSION <= 5
  mMapper->SetInput(mReaders[mCurrentTime]->GetOutput());
#else
  mMapper->SetInputData(mReaders[mCurrentTime]->GetOutput());
#endif
  mMapper->Modified();
  renderWidget->GetRenderWindow()->Render();
}

void vvSurfaceViewerDialog::PreviousTime()
{
  mCurrentTime--;
  if (mCurrentTime < 0)
    mCurrentTime = (unsigned int) mReaders.size() - 1;
#if VTK_MAJOR_VERSION <= 5
  mMapper->SetInput(mReaders[mCurrentTime]->GetOutput());
#else
  mMapper->SetInputData(mReaders[mCurrentTime]->GetOutput());
#endif
  mMapper->Modified();
  renderWidget->GetRenderWindow()->Render();
}

#endif /* end #define _vvSurfaceViewerDialog_CXX */
