#ifndef _vvSurfaceViewerDialog_CXX
#define _vvSurfaceViewerDialog_CXX

/*=========================================================================

Program:   vv
Module:    $RCSfile: vvSurfaceViewerDialog.cxx,v $
Language:  C++
Date:      $Date: 2010/01/06 13:31:57 $
Version:   $Revision: 1.1 $
Author :   David Sarrut (david.sarrut@gmail.com)

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
#include <QtGui>
#include <Qt>

#include "vvSurfaceViewerDialog.h"
#include "vtkOBJReader.h"
#include "vtkInteractorStyle.h"


#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"

#include <QMessageBox>

//----------------------------------------------------------------------------
class vvManagerCallback : public vtkCommand
{
public:
    static vvManagerCallback *New() {
        return new vvManagerCallback;
    }

    void Execute(vtkObject *caller,
                 unsigned long event,
                 void *vtkNotUsed(callData))
    {
        vtkInteractorStyle *isi = static_cast<vtkInteractorStyle *>(caller);
        if (isi)
        {
            std::string KeyPress = isi->GetInteractor()->GetKeySym();
            if (KeyPress == "Left")
            {
                this->IV->PreviousTime();
            }
            if (KeyPress == "Right")
            {
                this->IV->NextTime();
            }
        }
    }
    vvSurfaceViewerDialog *IV;
};


//====================================================================
vvSurfaceViewerDialog::vvSurfaceViewerDialog(QWidget * parent, Qt::WindowFlags f)
        :QDialog(parent,f), Ui::vvSurfaceViewerDialog() {

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
    for (unsigned int i = 0; i < mReaders.size();i++)
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

    if (!mMapper)
    {
        mMapper = vtkPolyDataMapper::New();
    }

    for (int i = 0; i < files.size(); i++)
    {
        vtkOBJReader* reader = vtkOBJReader::New();
        reader->SetFileName(files[i].toStdString().c_str());
        reader->Update();
        mReaders.push_back(reader);
    }

    mMapper->SetInput(mReaders[mCurrentTime]->GetOutput());

    if (!mActor)
    {
        mActor = vtkActor::New();
        mActor->SetMapper(mMapper);
    }
    if (!mRenderer)
    {
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
    mMapper->SetInput(mReaders[mCurrentTime]->GetOutput());
    mMapper->Modified();
    renderWidget->GetRenderWindow()->Render();
}

void vvSurfaceViewerDialog::PreviousTime()
{
    mCurrentTime--;
    if (mCurrentTime < 0)
        mCurrentTime = mReaders.size() - 1;
    mMapper->SetInput(mReaders[mCurrentTime]->GetOutput());
    mMapper->Modified();
    renderWidget->GetRenderWindow()->Render();
}

#endif /* end #define _vvSurfaceViewerDialog_CXX */
