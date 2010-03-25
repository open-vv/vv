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
#include "vvSlicerManager.h"

#include "vvSlicer.h"
#include "vvImage.h"
#include "vvSlicerManagerCommand.h"
#include "vvInteractorStyleNavigator.h"
#include "vvLandmarks.h"
#include "vvImageReader.h"
#include "vvImageReader.h"
#include "vvMesh.h"
#include "vvImageMapToWLColors.h"

#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkColorTransferFunction.h"
#include "vtkImageClip.h"
#include <vtkLODActor.h>
#include <vtkPointData.h>

#include <vtksys/SystemTools.hxx>
//----------------------------------------------------------------------------

vvSlicerManager::vvSlicerManager(int numberOfSlicers)
{
    mFileName = "";
    mId = "";
    mVFName = "";
    mOverlayName = "";
    mFusionName = "";
    mVFId = "";
    mLastError = "";
    mType = UNDEFINEDIMAGETYPE;
    mColorMap = 0;
    mPreset = 0;
    mOverlayColor = 130;

    mFusionOpacity = 70;
    mFusionColorMap = 3;
    mFusionWindow = 1000;
    mFusionLevel = 1000;

    mReader = NULL;
    mImage = NULL;
    mVF=NULL;
    mVectorReader = NULL;
    mOverlayReader = NULL;
    mFusionReader = NULL;
    mLandmarks = NULL;
    mLinkedId.resize(0);

    for ( int i = 0; i < numberOfSlicers; i++)
    {
        vvSlicer *slicer = vvSlicer::New();
        mSlicers.push_back(slicer);
    }
}

vvSlicerManager::~vvSlicerManager()
{
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        if (mSlicers[i] != NULL)
            mSlicers[i]->Delete();
    }
    if (mReader)
    {
        delete mReader;
    }
    if (mVectorReader)
    {
        delete mVectorReader;
    }
    if (mOverlayReader)
    {
        delete mOverlayReader;
    }
    if (mFusionReader)
    {
        delete mFusionReader;
    }
    if (mLandmarks)
        delete mLandmarks;
}

void vvSlicerManager::AddContour(vvMesh::Pointer contour,bool propagate)
{
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->AddContour(contour,propagate);
    }
}

void vvSlicerManager::ToggleContourSuperposition()
{
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
        mSlicers[i]->ToggleContourSuperposition();
}

bool vvSlicerManager::SetImage(std::string filename,LoadedImageType type)
{
    mFileName = filename;
    mType = type;
    if (mReader == NULL)
        mReader = new vvImageReader;
    std::vector<std::string> filenames;
    filenames.push_back(filename);
    mReader->SetInputFilenames(filenames);
    mReader->Update(type);
    if (mReader->GetLastError().size() == 0)
    {
        mImage=mReader->GetOutput();
        for ( unsigned int i = 0; i < mSlicers.size(); i++)
        {
            mSlicers[i]->SetFileName(vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
            mSlicers[i]->SetImage(mReader->GetOutput());
        }
    }
    else
    {
        mLastError = mReader->GetLastError();
        return false;
    }
    return true;
}

void vvSlicerManager::SetImage(vvImage::Pointer image)
{
    mImage=image;
    for (unsigned int i = 0; i < mSlicers.size();i++)
    {
        mSlicers[i]->SetImage(image);
    }
}

bool vvSlicerManager::SetImages(std::vector<std::string> filenames,LoadedImageType type)
{
    mType = type;
    std::string fileWithoutExtension = vtksys::SystemTools::GetFilenameWithoutExtension(filenames[0]);
    if (type == DICOM)
        fileWithoutExtension += "_dicom";
    else if (type == MERGED)
        fileWithoutExtension += "_merged";
    else if (type == MERGEDWITHTIME)
        fileWithoutExtension += "_merged_wt";

    mFileName = fileWithoutExtension + vtksys::SystemTools::GetFilenameExtension(filenames[0]);
    if (mReader == NULL)
        mReader = new vvImageReader;
    mReader->SetInputFilenames(filenames);
    mReader->Update(type);


    if (mReader->GetLastError().size() == 0)
    {
        mImage=mReader->GetOutput();
        for ( unsigned int i = 0; i < mSlicers.size(); i++)
        {
            mSlicers[i]->SetFileName(fileWithoutExtension);
            mSlicers[i]->SetImage(mReader->GetOutput());
        }
    }
    else
    {
        mLastError = mReader->GetLastError();
        return false;
    }
    return true;
}

bool vvSlicerManager::SetOverlay(std::string filename,int dim, std::string component)
{
    mOverlayName = filename;
    mOverlayComponent = component;
    if (dim > mImage->GetNumberOfDimensions())
    {
        mLastError = " Overlay dimension cannot be greater then reference image!";
        return false;
    }
    if (mOverlayReader == NULL)
        mOverlayReader = new vvImageReader;
    std::vector<std::string> filenames;
    filenames.push_back(filename);
    mOverlayReader->SetInputFilenames(filenames);
    mOverlayReader->Update(mImage->GetNumberOfDimensions(),component.c_str(),mType);
    if (mOverlayReader->GetLastError().size() == 0)
    {
        for ( unsigned int i = 0; i < mSlicers.size(); i++)
        {
            mSlicers[i]->SetOverlay(mOverlayReader->GetOutput());
        }
    }
    else
    {
        mLastError = mOverlayReader->GetLastError();
        return false;
    }
    return true;
}

bool vvSlicerManager::SetFusion(std::string filename,int dim, std::string component)
{
    mFusionName = filename;
    mFusionComponent = component;
    if (dim > mImage->GetNumberOfDimensions())
    {
        mLastError = " Overlay dimension cannot be greater then reference image!";
        return false;
    }
    if (mFusionReader == NULL)
        mFusionReader = new vvImageReader;
    std::vector<std::string> filenames;
    filenames.push_back(filename);
    mFusionReader->SetInputFilenames(filenames);
    mFusionReader->Update(mImage->GetNumberOfDimensions(),component.c_str(),mType);
    if (mFusionReader->GetLastError().size() == 0)
    {
        for ( unsigned int i = 0; i < mSlicers.size(); i++)
        {
            mSlicers[i]->SetFusion(mFusionReader->GetOutput());
        }
    }
    else
    {
        mLastError = mFusionReader->GetLastError();
        return false;
    }
    double *fusRange = mFusionReader->GetOutput()->GetVTKImages()[0]->GetScalarRange();
    mFusionLevel = (fusRange[0]+fusRange[1])/2;
    mFusionWindow = fusRange[1]-fusRange[0];
    return true;
}

bool vvSlicerManager::SetVF(std::string filename)
{
    if (mVectorReader == NULL)
        mVectorReader = new vvImageReader;
    mVectorReader->SetInputFilename(filename);
    mVectorReader->Update(VECTORFIELD);
    if (mVectorReader->GetLastError().size() != 0)
    {
        mLastError = mVectorReader->GetLastError();
        return false;
    }
    else
        return SetVF(mVectorReader->GetOutput(),filename);
}

bool vvSlicerManager::SetVF(vvImage::Pointer vf,std::string filename)
{
    if (vf->GetNumberOfDimensions() > mImage->GetNumberOfDimensions())
    {
        mLastError = " Vector field dimension cannot be greater then reference image!";
        return false;
    }
    mVF=vf;
    mVFName = filename;
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetVF(vf);
    }
    return true;
}

void vvSlicerManager::SetExtractedImage(std::string filename,vvImage::Pointer image, int slice)
{
    mFileName = filename;
    mImage = vvImage::New();
    if (image->GetNumberOfDimensions() == 4)
    {
        mImage->AddImage(image->GetVTKImages()[slice]);
        for ( unsigned int i = 0; i < mSlicers.size(); i++)
        {
            mSlicers[i]->SetFileName(vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
            mSlicers[i]->SetImage(mImage);
        }
    }
    else
    {
        vtkImageClip* clipper = vtkImageClip::New();
        int extent[6];
        image->GetVTKImages()[0]->GetWholeExtent(extent);
        clipper->SetInput(image->GetVTKImages()[0]);
        clipper->SetOutputWholeExtent(extent[0],extent[1],extent[2],extent[3],slice,slice);
        clipper->Update();
        mImage->AddImage(clipper->GetOutput());
        for ( unsigned int i = 0; i < mSlicers.size(); i++)
        {
            mSlicers[i]->SetFileName(vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
            mSlicers[i]->SetImage(mImage);
        }
        clipper->Delete();
    }
}

vvSlicer* vvSlicerManager::GetSlicer(int i)
{
    return mSlicers[i];
}

void vvSlicerManager::UpdateSlicer(int num, bool state)
{
    if (mSlicers[num]->GetImage())
            mSlicers[num]->SetDisplayMode(state);
}

void vvSlicerManager::SetSlicerWindow(int i, vtkRenderWindow* RW)
{
    mSlicers[i]->SetRenderWindow(i,RW);
}

void vvSlicerManager::SetInteractorStyleNavigator(int i, vtkInteractorStyle* style)
{
    vvSlicerManagerCommand *smc = vvSlicerManagerCommand::New();
    smc->SM = this;
    smc->SetSlicerNumber(i);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::KeyPressEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::WindowLevelEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::EndWindowLevelEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::StartWindowLevelEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::PickEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::StartPickEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::LeaveEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::UserEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::MouseWheelForwardEvent, smc);
    mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::MouseWheelBackwardEvent, smc);
    smc->Delete();
}

void vvSlicerManager::SetTSlice(int slice)
{
    if (slice < 0)
        slice = 0;
    else if (slice > mSlicers[0]->GetTMax())
        slice = mSlicers[0]->GetTMax();
    if (mLandmarks)
        mLandmarks->SetTime(slice);
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetTSlice(slice);
        if (mSlicers[i]->GetImageActor()->GetVisibility())
            UpdateTSlice(i);
    }
}

void vvSlicerManager::SetNextTSlice(int originating_slicer)
{
    int t = mSlicers[0]->GetTSlice();
    t++;
    if (t > mSlicers[0]->GetTMax())
        t = 0;
    emit UpdateTSlice(originating_slicer,t);
}

void vvSlicerManager::SetPreviousTSlice(int originating_slicer)
{
    int t = mSlicers[0]->GetTSlice();
    t--;
    if (t < 0)
        t = mSlicers[0]->GetTMax();
    emit UpdateTSlice(originating_slicer,t);
}

void vvSlicerManager::ToggleInterpolation()
{
    bool interpolate=!(mSlicers[0]->GetImageActor()->GetInterpolate());
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->GetImageActor()->SetInterpolate(interpolate);
    }
}


void vvSlicerManager::SetTSliceInSlicer(int tslice, int slicer)
{
    if (tslice < 0)
        tslice = 0;
    else if (tslice > mSlicers[slicer]->GetTMax())
        tslice = mSlicers[slicer]->GetTMax();
    if (mLandmarks)
        mLandmarks->SetTime(tslice);
    mSlicers[slicer]->SetTSlice(tslice);
    if (mSlicers[slicer]->GetImageActor()->GetVisibility())
        UpdateTSlice(slicer);
}

void vvSlicerManager::SetColorWindow(double s)
{
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetColorWindow(s);
    }
}

void vvSlicerManager::SetColorLevel(double s)
{
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetColorLevel(s);
    }
}

void vvSlicerManager::SetCursorVisibility(int s)
{
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetCursorVisibility(s);
    }
}

void vvSlicerManager::SetOpacity(int i, double factor)
{
    mSlicers[i]->SetOpacity(1/factor);
}

void vvSlicerManager::UpdateViews(int current,int slicer)
{
    double x = (mSlicers[slicer]->GetCurrentPosition()[0] - mSlicers[slicer]->GetInput()->GetOrigin()[0])
               /mSlicers[slicer]->GetInput()->GetSpacing()[0];
    double y = (mSlicers[slicer]->GetCurrentPosition()[1] - mSlicers[slicer]->GetInput()->GetOrigin()[1])
               /mSlicers[slicer]->GetInput()->GetSpacing()[1];
    double z = (mSlicers[slicer]->GetCurrentPosition()[2] - mSlicers[slicer]->GetInput()->GetOrigin()[2])
               /mSlicers[slicer]->GetInput()->GetSpacing()[2];

    if (x >= mSlicers[slicer]->GetInput()->GetWholeExtent()[0] &&
            x <= mSlicers[slicer]->GetInput()->GetWholeExtent()[1] &&
            y >= mSlicers[slicer]->GetInput()->GetWholeExtent()[2] &&
            y <= mSlicers[slicer]->GetInput()->GetWholeExtent()[3] &&
            z >= mSlicers[slicer]->GetInput()->GetWholeExtent()[4] &&
            z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5])
    {
        mSlicers[slicer]->UpdateCursorPosition();
        mSlicers[slicer]->SetCursorColor(10,212,255);

        switch (mSlicers[slicer]->GetSliceOrientation())
        {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
            if (mSlicers[slicer]->GetSlice() == (int)floor(z))
                mSlicers[slicer]->Render();
            else
                mSlicers[slicer]->SetSlice((int)floor(z));
            break;

        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
            if (mSlicers[slicer]->GetSlice() == (int)floor(y))
                mSlicers[slicer]->Render();
            else
                mSlicers[slicer]->SetSlice((int)floor(y));
            break;

        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
            if (mSlicers[slicer]->GetSlice() == (int)floor(x))
                mSlicers[slicer]->Render();
            else
                mSlicers[slicer]->SetSlice((int)floor(x));
            break;
        }

        for ( unsigned int i = 0; i < mSlicers.size(); i++)
        {
            if (i != (unsigned int)slicer && mSlicers[i]->GetImageActor()->GetVisibility()
                    && mSlicers[i]->GetRenderWindow()->GetSize()[0] > 2
                    && mSlicers[i]->GetRenderWindow()->GetSize()[1] > 2)
            {
                mSlicers[i]->SetCurrentPosition(mSlicers[slicer]->GetCurrentPosition()[0],
                                                mSlicers[slicer]->GetCurrentPosition()[1],
                                                mSlicers[slicer]->GetCurrentPosition()[2],
                                                mSlicers[slicer]->GetTSlice());
                mSlicers[i]->UpdateCursorPosition();
                if (current) //do not display corner annotation if image is the one picked
                {
                    mSlicers[i]->SetCurrentPosition(-VTK_DOUBLE_MAX,-VTK_DOUBLE_MAX,
                                                    -VTK_DOUBLE_MAX, mSlicers[slicer]->GetTSlice());
                    mSlicers[i]->SetCursorColor(255,10,212);
                }
                else
                {
                    mSlicers[i]->SetCursorColor(150,10,282);
                }
                switch (mSlicers[i]->GetSliceOrientation())
                {
                case vtkImageViewer2::SLICE_ORIENTATION_XY:
                    if (mSlicers[i]->GetSlice() == (int)floor(z))
                        mSlicers[i]->Render();
                    else
                        mSlicers[i]->SetSlice((int)floor(z));
                    break;

                case vtkImageViewer2::SLICE_ORIENTATION_XZ:
                    if (mSlicers[i]->GetSlice() == (int)floor(y))
                        mSlicers[i]->Render();
                    else
                        mSlicers[i]->SetSlice((int)floor(y));
                    break;

                case vtkImageViewer2::SLICE_ORIENTATION_YZ:
                    if (mSlicers[i]->GetSlice() == (int)floor(x))
                        mSlicers[i]->Render();
                    else
                        mSlicers[i]->SetSlice((int)floor(x));
                    break;
                }
                UpdateSlice(i);
                UpdateTSlice(i);
            }
        }
    }
}

void vvSlicerManager::UpdateLinked(int slicer)
{
    double x = (mSlicers[slicer]->GetCurrentPosition()[0] - mSlicers[slicer]->GetInput()->GetOrigin()[0])
               /mSlicers[slicer]->GetInput()->GetSpacing()[0];
    double y = (mSlicers[slicer]->GetCurrentPosition()[1] - mSlicers[slicer]->GetInput()->GetOrigin()[1])
               /mSlicers[slicer]->GetInput()->GetSpacing()[1];
    double z = (mSlicers[slicer]->GetCurrentPosition()[2] - mSlicers[slicer]->GetInput()->GetOrigin()[2])
               /mSlicers[slicer]->GetInput()->GetSpacing()[2];

    if (x >= mSlicers[slicer]->GetInput()->GetWholeExtent()[0] &&
            x <= mSlicers[slicer]->GetInput()->GetWholeExtent()[1] &&
            y >= mSlicers[slicer]->GetInput()->GetWholeExtent()[2] &&
            y <= mSlicers[slicer]->GetInput()->GetWholeExtent()[3] &&
            z >= mSlicers[slicer]->GetInput()->GetWholeExtent()[4] &&
            z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5])
    {
        for (std::list<std::string>::const_iterator i = mLinkedId.begin(); i != mLinkedId.end(); i++)
        {
            emit UpdateLinkManager(*i, slicer,mSlicers[slicer]->GetCurrentPosition()[0],
                                   mSlicers[slicer]->GetCurrentPosition()[1],
                                   mSlicers[slicer]->GetCurrentPosition()[2],mSlicers[slicer]->GetTSlice());
        }
    }
}
double vvSlicerManager::GetColorWindow()
{
    if (mSlicers.size())
        return mSlicers[0]->GetColorWindow();
    return -1;
}

double vvSlicerManager::GetColorLevel()
{
    if (mSlicers.size())
        return mSlicers[0]->GetColorLevel();
    return -1;
}

void vvSlicerManager::Render()
{
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->Render();
    }
}

void vvSlicerManager::GenerateDefaultLookupTable()
{
    SetPreset(mPreset);
    SetColorMap(mColorMap);
}

void vvSlicerManager::Reload()
{
    mReader->Update(mType);
    mImage=mReader->GetOutput();
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetImage(mImage);
    }
}

void vvSlicerManager::ReloadFusion()
{
    mFusionReader->Update();
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetFusion(mFusionReader->GetOutput());
        mSlicers[i]->Render();
    }
}

void vvSlicerManager::ReloadOverlay()
{
    mOverlayReader->Update();
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetOverlay(mOverlayReader->GetOutput());
        mSlicers[i]->Render();
    }
}

void vvSlicerManager::ReloadVF()
{
    mVectorReader->Update(VECTORFIELD); //deletes the old images through the VF::Init() function
    mVF=mVectorReader->GetOutput();
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetVF(mVF);
        mSlicers[i]->Render();
    }
}

void vvSlicerManager::RemoveActor(const std::string& actor_type, int overlay_index)
{
    for (unsigned int i = 0; i < mSlicers.size();i++)
    {
        mSlicers[i]->RemoveActor(actor_type,overlay_index);
    }
    if (actor_type=="vector")
    {
        mVF=NULL;
        if (mVectorReader) {
            delete mVectorReader;
            mVectorReader=NULL;
        }
    }
}

void vvSlicerManager::RemoveActors()
{
    ///This method leaks a few objects. See RemoveActor for what a correct implementation would look like
    for ( unsigned int i = 0; i < mSlicers.size(); i++)
    {
        mSlicers[i]->SetDisplayMode(0);
        mSlicers[i]->GetRenderer()->RemoveActor(mSlicers[i]->GetImageActor());
    }
}

void vvSlicerManager::UpdateInfoOnCursorPosition(int slicer)
{
//  int view = mSlicers[slicer]->GetSliceOrientation();
//  int slice = mSlicers[slicer]->GetSlice();
    double x = mSlicers[slicer]->GetCursorPosition()[0];
    double y = mSlicers[slicer]->GetCursorPosition()[1];
    double z = mSlicers[slicer]->GetCursorPosition()[2];
    double X = (x - mSlicers[slicer]->GetInput()->GetOrigin()[0])/
        mSlicers[slicer]->GetInput()->GetSpacing()[0];
    double Y = (y - mSlicers[slicer]->GetInput()->GetOrigin()[1])/
        mSlicers[slicer]->GetInput()->GetSpacing()[1];
    double Z = (z - mSlicers[slicer]->GetInput()->GetOrigin()[2])/
        mSlicers[slicer]->GetInput()->GetSpacing()[2];
    double value = -VTK_DOUBLE_MAX;
    int displayVec = 0;
    double xVec=0, yVec=0, zVec=0, valueVec=0;
    int displayOver = 0;
    int displayFus = 0;
    double valueOver=0, valueFus=0;
    if (X >= mSlicers[slicer]->GetInput()->GetWholeExtent()[0] &&
            X <= mSlicers[slicer]->GetInput()->GetWholeExtent()[1] &&
            Y >= mSlicers[slicer]->GetInput()->GetWholeExtent()[2] &&
            Y <= mSlicers[slicer]->GetInput()->GetWholeExtent()[3] &&
            Z >= mSlicers[slicer]->GetInput()->GetWholeExtent()[4] &&
            Z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5])
    {
        value = mSlicers[slicer]->GetInput()->GetScalarComponentAsDouble(
                (int)floor(X),
                (int)floor(Y),
                (int)floor(Z),0);
        if (mSlicers[slicer]->GetVFActor() && mSlicers[slicer]->GetVFActor()->GetVisibility())
        {
            displayVec = 1;
            unsigned int currentTime = mSlicers[slicer]->GetTSlice();
            vtkImageData *vf = NULL;

            if (mSlicers[slicer]->GetVF()->GetVTKImages().size() > currentTime)
                vf = mSlicers[slicer]->GetVF()->GetVTKImages()[currentTime];
            else
                vf = mSlicers[slicer]->GetVF()->GetVTKImages()[0];

            if (vf)
            {
                double Xvf = (x - vf->GetOrigin()[0])/ vf->GetSpacing()[0];
                double Yvf = (y - vf->GetOrigin()[1])/ vf->GetSpacing()[1];
                double Zvf = (z - vf->GetOrigin()[2])/ vf->GetSpacing()[2];
                xVec = vf->GetScalarComponentAsDouble( (int)floor(Xvf), (int)floor(Yvf), (int)floor(Zvf),0);
                yVec = vf->GetScalarComponentAsDouble( (int)floor(Xvf), (int)floor(Yvf), (int)floor(Zvf),1);
                zVec = vf->GetScalarComponentAsDouble( (int)floor(Xvf), (int)floor(Yvf), (int)floor(Zvf),2);
                valueVec = sqrt(xVec*xVec + yVec*yVec + zVec*zVec);
            }
        }
        if (mSlicers[slicer]->GetOverlayActor() && mSlicers[slicer]->GetOverlayActor()->GetVisibility())
        {
            displayOver = 1;
            double Xover = (x - mSlicers[slicer]->GetOverlay()->GetOrigin()[0])
                /mSlicers[slicer]->GetOverlay()->GetSpacing()[0];
            double Yover = (y - mSlicers[slicer]->GetOverlay()->GetOrigin()[1])
                /mSlicers[slicer]->GetOverlay()->GetSpacing()[1];
            double Zover = (z - mSlicers[slicer]->GetOverlay()->GetOrigin()[2])
                /mSlicers[slicer]->GetOverlay()->GetSpacing()[2];
            if (Xover >= mSlicers[slicer]->GetOverlayMapper()->GetInput()->GetWholeExtent()[0] &&
                    Xover <= mSlicers[slicer]->GetOverlayMapper()->GetInput()->GetWholeExtent()[1] &&
                    Yover >= mSlicers[slicer]->GetOverlayMapper()->GetInput()->GetWholeExtent()[2] &&
                    Yover <= mSlicers[slicer]->GetOverlayMapper()->GetInput()->GetWholeExtent()[3] &&
                    Zover >= mSlicers[slicer]->GetOverlayMapper()->GetInput()->GetWholeExtent()[4] &&
                    Zover <= mSlicers[slicer]->GetOverlayMapper()->GetInput()->GetWholeExtent()[5])
            {
                valueOver = static_cast<vtkImageData*>(mSlicers[slicer]->GetOverlayMapper()->GetInput())->
                    GetScalarComponentAsDouble(
                            (int)floor(Xover),
                            (int)floor(Yover),
                            (int)floor(Zover),0);
            }
        }
        if (mSlicers[slicer]->GetFusionActor() && mSlicers[slicer]->GetFusionActor()->GetVisibility())
        {
            displayFus = 1;
            double Xfus = (x - mSlicers[slicer]->GetFusion()->GetOrigin()[0])
                /mSlicers[slicer]->GetFusion()->GetSpacing()[0];
            double Yfus = (y - mSlicers[slicer]->GetFusion()->GetOrigin()[1])
                /mSlicers[slicer]->GetFusion()->GetSpacing()[1];
            double Zfus = (z - mSlicers[slicer]->GetFusion()->GetOrigin()[2])
                /mSlicers[slicer]->GetFusion()->GetSpacing()[2];
            if (Xfus >= mSlicers[slicer]->GetFusionMapper()->GetInput()->GetWholeExtent()[0] &&
                    Xfus <= mSlicers[slicer]->GetFusionMapper()->GetInput()->GetWholeExtent()[1] &&
                    Yfus >= mSlicers[slicer]->GetFusionMapper()->GetInput()->GetWholeExtent()[2] &&
                    Yfus <= mSlicers[slicer]->GetFusionMapper()->GetInput()->GetWholeExtent()[3] &&
                    Zfus >= mSlicers[slicer]->GetFusionMapper()->GetInput()->GetWholeExtent()[4] &&
                    Zfus <= mSlicers[slicer]->GetFusionMapper()->GetInput()->GetWholeExtent()[5])
            {
                valueFus = static_cast<vtkImageData*>(mSlicers[slicer]->GetFusionMapper()->GetInput())->
                    GetScalarComponentAsDouble(
                            (int)floor(Xfus),
                            (int)floor(Yfus),
                            (int)floor(Zfus),0);
            }
        }
        emit UpdatePosition(mSlicers[slicer]->GetCursorVisibility(),
                x,y,z,X,Y,Z,value);
        emit UpdateVector(displayVec,xVec, yVec, zVec, valueVec);
        emit UpdateOverlay(displayOver,valueOver,value);
        emit UpdateFusion(displayFus,valueFus);
        for (unsigned int i = 0; i < mSlicers.size(); i++)
        {
            if (mSlicers[i]->GetImageActor()->GetVisibility() == 1)
                emit UpdateWindows(i,mSlicers[i]->GetSliceOrientation(),mSlicers[i]->GetSlice());
            else
                emit UpdateWindows(i,-1,-1);
        }
    }
}

void vvSlicerManager::Activated()
{
    emit currentImageChanged(mId);
}

void vvSlicerManager::UpdateWindowLevel()
{
    emit WindowLevelChanged(mSlicers[0]->GetColorWindow(),mSlicers[0]->GetColorLevel(),mPreset,mColorMap);
}

void vvSlicerManager::UpdateSlice(int slicer)
{
    emit UpdateSlice(slicer, mSlicers[slicer]->GetSlice());
}

void vvSlicerManager::UpdateTSlice(int slicer)
{
    emit UpdateTSlice(slicer,mSlicers[0]->GetTSlice());
}

void vvSlicerManager::UpdateSliceRange(int slicer)
{
    emit UpdateSliceRange(slicer,
                          mSlicers[slicer]->GetSliceRange()[0], mSlicers[slicer]->GetSliceRange()[1],
                          0,mSlicers[slicer]->GetTMax());
}

void vvSlicerManager::SetPreset(int preset)
{
    //vtkLookupTable* LUT = static_cast<vtkLookupTable*>(mSlicers[0]->GetWindowLevel()->GetLookupTable());
    double window = mSlicers[0]->GetColorWindow();
    double level = mSlicers[0]->GetColorLevel();

    std::string component_type=mImage->GetScalarTypeAsString();
    switch (preset)
    {
    case 0:
        if (component_type == "unsigned_char")
        {
            window = 255;
            level = 127;
        }
        else if (component_type == "short")
        {
            window = 2000;
            level = 0;
        }
        else
        {
            double range[2];
            mImage->GetScalarRange(range);
            window = range[1] - range[0];
            level = (range[1] + range[0])* 0.5;
        }
        break;
    case 1:
        window = 2000;
        level = 0;
        break;
    case 2:
        window = 350;
        level = 60;
        break;
    case 3:
        window = 1500;
        level = -500;
        break;
    case 4:
        window = 1000;
        level = 500;
        break;
    case 5:
        window = 1;
        level = 0.5;
        break;
    case 6:
        break;
    case 7:
        window=1.;
        level=0.;
        break;
    }
    mPreset = preset;
    this->SetColorWindow(window);
    this->SetColorLevel(level);

    //if (LUT)
    //{
    //    SetColorMap(-1);
    //}
}

void vvSlicerManager::SetLocalColorWindowing(const int slicer)
{
    double min, max;
	this->mSlicers[slicer]->GetExtremasAroundMousePointer(min, max);
    this->SetColorWindow(max-min);
    this->SetColorLevel(0.5*(min+max));
	this->Render();
}

void vvSlicerManager::SetColorMap()
{
    SetColorMap(mColorMap);
}

void vvSlicerManager::SetColorMap(int colormap)
{
    double range[2];
    range[0] = mSlicers[0]->GetInput()->GetScalarRange()[0];
    range[1] = mSlicers[0]->GetInput()->GetScalarRange()[1];

    double window = mSlicers[0]->GetWindowLevel()->GetWindow();
    double level = mSlicers[0]->GetWindowLevel()->GetLevel();

    vtkLookupTable* LUT = static_cast<vtkLookupTable*>(mSlicers[0]->GetWindowLevel()->GetLookupTable());
    switch (colormap)
    {
    case -1:
        break;
    case 0:
        LUT = NULL;
        break;
    case 1:
        if (LUT == NULL)
            LUT = vtkLookupTable::New();
        LUT->SetValueRange(0,1);
        LUT->SetSaturationRange(1,1);
        LUT->SetHueRange(0,0.18);
        break;
    case 2:
        if (LUT == NULL)
            LUT = vtkLookupTable::New();
        LUT->SetValueRange(0,1);
        LUT->SetSaturationRange(1,1);
        LUT->SetHueRange(0.4,0.80);
        break;
    case 3:
        if (LUT == NULL)
            LUT = vtkLookupTable::New();
        LUT->SetValueRange(0,1);
        LUT->SetSaturationRange(1,1);
        LUT->SetHueRange(0,1);
        break;
    case 5:
        if (LUT == NULL)
            LUT = vtkLookupTable::New();
        LUT->SetValueRange(0.,1);
        LUT->SetSaturationRange(1,1);
        LUT->SetHueRange(1,0.1);
        //LUT->SetRampToLinear();
        break;
    }
    if (LUT)
    {
        LUT->SetTableRange(level-fabs(window)/4,level+fabs(window)/4);
        LUT->Build();
    }
    vtkLookupTable* fusLUT = NULL;
    if (mSlicers[0]->GetFusion())
    {
        fusLUT = vtkLookupTable::New();
        double fusRange [2];
        fusRange[0] = mFusionLevel - mFusionWindow/2;
        fusRange[1] = mFusionLevel + mFusionWindow/2;
        fusLUT->SetTableRange(fusRange[0],fusRange[1]);
        fusLUT->SetValueRange(1,1);
        fusLUT->SetSaturationRange(1,1);
        if (mFusionColorMap == 1)
            fusLUT->SetHueRange(0,0.18);
        else if (mFusionColorMap == 2)
            fusLUT->SetHueRange(0.4,0.80);
        else if (mFusionColorMap == 3)
            fusLUT->SetHueRange(0,1);
        fusLUT->Build();
        if (mFusionColorMap == 0)
            fusLUT = NULL;
    }
    for ( unsigned int i = 0; i < mSlicers.size(); i++) {
        if (mSlicers[i]->GetOverlay() && mSlicers[i]->GetOverlayActor()->GetVisibility()) {
            vtkLookupTable* supLUT = vtkLookupTable::New();
            supLUT->SetTableRange(range[0],range[1]);
            supLUT->SetValueRange(1,1);
            supLUT->SetSaturationRange(1,1);
            supLUT->SetHueRange(double(mOverlayColor)/360,double(mOverlayColor)/360);
            supLUT->Build();
            vtkLookupTable* invLUT = vtkLookupTable::New();
            invLUT->SetTableRange(range[0],range[1]);
            invLUT->SetValueRange(1,1);
            invLUT->SetSaturationRange(1,1);
            invLUT->SetHueRange(double((mOverlayColor+180)%360)/360,double((mOverlayColor+180)%360)/360);
            invLUT->Build();
            dynamic_cast<vvImageMapToWLColors*>(mSlicers[i]->GetWindowLevel())
                ->SetWindowLevelMode(true);
            mSlicers[i]->GetWindowLevel()->SetLookupTable(supLUT);
            mSlicers[i]->GetOverlayMapper()->SetLookupTable(invLUT);
            invLUT->Delete();
            supLUT->Delete();
        }
        else if (mSlicers[i]->GetOverlay())
        {
            //dynamic_cast<vvImageMapToWLColors*>(mSlicers[i]->GetWindowLevel())
                //->SetWindowLevelMode(false);
            mSlicers[i]->GetWindowLevel()->SetLookupTable(LUT);
        }
        else
        {
            mSlicers[i]->GetWindowLevel()->SetLookupTable(LUT);
        }
        if (mSlicers[i]->GetFusion() && mSlicers[i]->GetFusionActor()->GetVisibility())
        {
            mSlicers[i]->GetFusionActor()->SetOpacity(double(mFusionOpacity)/100);
            mSlicers[i]->GetFusionMapper()->SetLookupTable(fusLUT);
        }
    }
    if (fusLUT)
        fusLUT->Delete();
    if (colormap >= 0)
        mColorMap = colormap;
}

vvLandmarks* vvSlicerManager::GetLandmarks()
{
    if (mLandmarks == NULL)
    {
        mLandmarks = new vvLandmarks(mSlicers[0]->GetTMax()+1);
        for (unsigned int i = 0; i < mSlicers.size(); i++)
            mSlicers[i]->SetLandmarks(mLandmarks);
    }
    return mLandmarks;
}

void vvSlicerManager::AddLandmark(float x,float y,float z,float t)
{
    double x_index = (x - mSlicers[0]->GetInput()->GetOrigin()[0])/mSlicers[0]->GetInput()->GetSpacing()[0];
    double y_index = (y - mSlicers[0]->GetInput()->GetOrigin()[1])/mSlicers[0]->GetInput()->GetSpacing()[1];
    double z_index = (z - mSlicers[0]->GetInput()->GetOrigin()[2])/mSlicers[0]->GetInput()->GetSpacing()[2];
    double value = mSlicers[0]->GetInput()->GetScalarComponentAsDouble(
            (int)x_index,
            (int)y_index,
            (int)z_index,0);
    this->GetLandmarks()->AddLandmark(x,y,z,t,value);
    emit LandmarkAdded();
}
