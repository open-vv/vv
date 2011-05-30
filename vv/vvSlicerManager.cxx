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
#include "vvMesh.h"
#include "vvImageMapToWLColors.h"
#include "vvBlendImageActor.h"

#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageClip.h>
#include <vtkLODActor.h>
#include <vtkPointData.h>
#include <vtksys/SystemTools.hxx>
#include <vtkCamera.h>

#include <qfileinfo.h>

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
  mType = vvImageReader::UNDEFINEDIMAGETYPE;
  mColorMap = 0;
  mPreset = 0;
  mOverlayColor = 130;

  mFusionOpacity = 70;
  mFusionColorMap = 3;
  mFusionWindow = 1000;
  mFusionLevel = 1000;

  mLandmarks = NULL;
  mLinkedId.resize(0);

  for ( int i = 0; i < numberOfSlicers; i++)
    mSlicers.push_back(vtkSmartPointer<vvSlicer>::New());

  mPreviousSlice.resize(numberOfSlicers);
  mPreviousTSlice.resize(numberOfSlicers);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
vvSlicerManager::~vvSlicerManager()
{
  if (mLandmarks)
    delete mLandmarks;
}
//----------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvSlicerManager::SetFilename(std::string filename, int number)
{
  mFileName = filename;
  mFileName = vtksys::SystemTools::GetFilenameName(mFileName);
  mBaseFileName = vtksys::SystemTools::GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(mFileName));
  mBaseFileNameNumber = number;

  mFileName = mBaseFileName;
  if (number != 0) {
    mFileName.append("_"+clitk::toString(number));
  }
  mFileName.append(vtksys::SystemTools::GetFilenameLastExtension(filename));

  for(unsigned int i=0; i<mSlicers.size(); i++) {
    mSlicers[i]->SetFileName(mFileName);//vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
  }
  
}
//------------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::AddContour(vvMesh::Pointer contour,bool propagate)
{
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->AddContour(contour,propagate);
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::ToggleContourSuperposition()
{
  for ( unsigned int i = 0; i < mSlicers.size(); i++)
    mSlicers[i]->ToggleContourSuperposition();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
std::string vvSlicerManager::GetListOfAbsoluteFilePathInOneString(const std::string &actorType)
{
  vvImageReader *reader = NULL;

  if(actorType=="image")
    reader = mReader;
  else if(actorType=="overlay")
    reader = mOverlayReader;
  else if(actorType=="fusion")
    reader = mFusionReader;
  else if(actorType=="vector")
    reader = mVectorReader;

  if(!reader)
    return "";

  std::string list;
  for(unsigned int i=0; i<reader->GetInputFilenames().size(); i++){
    QFileInfo fileinfo(reader->GetInputFilenames()[i].c_str()); //Do not show the path
    if(i)
      list += '\n';
    list += fileinfo.absoluteFilePath().toStdString();
  }
  return list;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
bool vvSlicerManager::SetImage(std::string filename, vvImageReader::LoadedImageType type, int n, unsigned int slice)
{
  mType = type;
  if (mReader.IsNull())
    mReader = vvImageReader::New();
  std::vector<std::string> filenames;
  filenames.push_back(filename);
  mReader->SetInputFilenames(filenames);
  mReader->SetSlice(slice); // Only used for SLICED type
  mReader->Update(type);

  SetFilename(filename, n);
  //  mFileName = vtksys::SystemTools::GetFilenameName(mFileName);
  //mBaseFileName = vtksys::SystemTools::GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(mFileName));
  //mBaseFileNameNumber = n;

  if (mReader->GetLastError().size() == 0) {
    mImage=mReader->GetOutput();
    for ( unsigned int i = 0; i < mSlicers.size(); i++) {
      mSlicers[i]->SetFileName(vtksys::SystemTools::GetFilenameWithoutLastExtension(filename));
      mSlicers[i]->SetImage(mReader->GetOutput());
    }
  } else {
    mLastError = mReader->GetLastError();
    return false;
  }
  // if (n!=0) {
  //   mFileName.append("_"+clitk::toString(n));
  // }
  return true;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetImage(vvImage::Pointer image)
{
  mImage=image;
  for (unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetImage(image);
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
bool vvSlicerManager::SetImages(std::vector<std::string> filenames, vvImageReader::LoadedImageType type, int n)
{
  mType = type;
  std::string fileWithoutExtension = vtksys::SystemTools::GetFilenameWithoutExtension(filenames[0]);
  if (type == vvImageReader::DICOM)
    fileWithoutExtension += "_dicom";
  else if (type == vvImageReader::MERGED)
    fileWithoutExtension += "_merged";
  else if (type == vvImageReader::MERGEDWITHTIME)
    fileWithoutExtension += "_merged_wt";

  mFileName = vtksys::SystemTools::GetFilenameName(mFileName);
  mFileName = fileWithoutExtension + vtksys::SystemTools::GetFilenameExtension(filenames[0]);
  if (mReader.IsNull())
    mReader = vvImageReader::New();
  mReader->SetInputFilenames(filenames);
  mReader->Update(type);

  mBaseFileName = vtksys::SystemTools::GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(mFileName));
  mBaseFileNameNumber = n;

  if (mReader->GetLastError().size() == 0) {
    mImage=mReader->GetOutput();
    for ( unsigned int i = 0; i < mSlicers.size(); i++) {
      mSlicers[i]->SetFileName(fileWithoutExtension);
      mSlicers[i]->SetImage(mReader->GetOutput());
    }
  } else {
    mLastError = mReader->GetLastError();
    return false;
  }
  if (n!=0) {
    mFileName.append("_"+clitk::toString(n));
  }
  return true;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
bool vvSlicerManager::SetOverlay(std::string filename,int dim, std::string component)
{
  mOverlayName = filename;
  mOverlayComponent = component;
  if (dim > mImage->GetNumberOfDimensions()) {
    mLastError = " Overlay dimension cannot be greater then reference image!";
    return false;
  }
  if (mOverlayReader.IsNull())
    mOverlayReader = vvImageReader::New();
  std::vector<std::string> filenames;
  filenames.push_back(filename);
  mOverlayReader->SetInputFilenames(filenames);
  mOverlayReader->Update(mImage->GetNumberOfDimensions(),component.c_str(),mType);
  if (mOverlayReader->GetLastError().size() == 0) {
    for ( unsigned int i = 0; i < mSlicers.size(); i++) {
      mSlicers[i]->SetOverlay(mOverlayReader->GetOutput());
    }
  } else {
    mLastError = mOverlayReader->GetLastError();
    return false;
  }
  return true;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
bool vvSlicerManager::SetFusion(std::string filename,int dim, std::string component)
{
  mFusionName = filename;
  mFusionComponent = component;
  if (dim > mImage->GetNumberOfDimensions()) {
    mLastError = " Overlay dimension cannot be greater then reference image!";
    return false;
  }
  if (mFusionReader.IsNull())
    mFusionReader = vvImageReader::New();
  std::vector<std::string> filenames;
  filenames.push_back(filename);
  mFusionReader->SetInputFilenames(filenames);
  mFusionReader->Update(mImage->GetNumberOfDimensions(),component.c_str(),mType);
  if (mFusionReader->GetLastError().size() == 0) {
    for ( unsigned int i = 0; i < mSlicers.size(); i++) {
      mSlicers[i]->SetFusion(mFusionReader->GetOutput());
    }
  } else {
    mLastError = mFusionReader->GetLastError();
    return false;
  }
  double *fusRange = mFusionReader->GetOutput()->GetVTKImages()[0]->GetScalarRange();
  mFusionLevel = (fusRange[0]+fusRange[1])/2;
  mFusionWindow = fusRange[1]-fusRange[0];
  return true;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
bool vvSlicerManager::SetVF(std::string filename)
{
  if (mVectorReader.IsNull())
    mVectorReader = vvImageReader::New();
  mVectorReader->SetInputFilename(filename);
  mVectorReader->Update(vvImageReader::VECTORFIELD);
  if (mVectorReader->GetLastError().size() != 0) {
    mLastError = mVectorReader->GetLastError();
    return false;
  } else
    return SetVF(mVectorReader->GetOutput(),filename);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
bool vvSlicerManager::SetVF(vvImage::Pointer vf,std::string filename)
{
  if (vf->GetNumberOfDimensions() > mImage->GetNumberOfDimensions()) {
    mLastError = "Sorry, vector field dimension cannot be greater then reference image.";
    return false;
  }
  if (vf->GetNumberOfDimensions() == 4) {
    if (vf->GetSpacing()[3] != mImage->GetSpacing()[3]) {
      mLastError = "Sorry, vector field time spacing cannot be different from time spacing of the reference image.";
      return false;
    }
    if (vf->GetOrigin()[3] != mImage->GetOrigin()[3]) {
      mLastError = "Sorry, vector field time origin cannot be different from time origin of the reference image.";
      return false;
    }
  }
  mVF=vf;
  mVFName = filename;
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetVF(vf);
  }
  return true;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
vvSlicer* vvSlicerManager::GetSlicer(int i)
{
  return mSlicers[i];
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::UpdateSlicer(int num, bool state)
{
  if (mSlicers[num]->GetImage())
    mSlicers[num]->SetDisplayMode(state);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetSlicerWindow(int i, vtkRenderWindow* RW)
{
  mSlicers[i]->SetRenderWindow(i,RW);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
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
  // mSlicers[i]->GetRenderWindow()->GetInteractor()->
  //   GetInteractorStyle()->AddObserver(vtkCommand::LeftButtonReleaseEvent, smc);
  mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::EndPickEvent, smc);
  mSlicers[i]->GetRenderWindow()->GetInteractor()->
    GetInteractorStyle()->AddObserver(vtkCommand::EndInteractionEvent, smc);
  smc->Delete();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::LeftButtonReleaseEvent(int slicer)
{
  emit LeftButtonReleaseSignal(slicer);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::SetSliceOrientation(int slicer, int orientation)
{
  mSlicers[slicer]->SetSliceOrientation(orientation);
  emit UpdateOrientation(slicer, orientation);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::SetTSlice(int slice)
{
  if (slice < 0)
    slice = 0;
  else if (slice > mSlicers[0]->GetTMax())
    slice = mSlicers[0]->GetTMax();
  if (mLandmarks)
    mLandmarks->SetTime(slice);
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    if (slice != mSlicers[i]->GetTSlice()) {
      mSlicers[i]->SetTSlice(slice);
      if (mSlicers[i]->GetImageActor()->GetVisibility())
        UpdateTSlice(i);
    }
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetNextTSlice(int originating_slicer)
{
  int t = mSlicers[0]->GetTSlice();
  t++;
  if (t > mSlicers[0]->GetTMax())
    t = 0;
  //std::cout << "vvSlicerManager::SetNextTSlice" << std::endl;
  emit UpdateTSlice(originating_slicer,t);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetPreviousTSlice(int originating_slicer)
{
  int t = mSlicers[0]->GetTSlice();
  t--;
  if (t < 0)
    t = mSlicers[0]->GetTMax();
  //std::cout << "vvSlicerManager::SetPreviousTSlice" << std::endl;
  emit UpdateTSlice(originating_slicer,t);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::ToggleInterpolation()
{
  bool interpolate=!(mSlicers[0]->GetImageActor()->GetInterpolate());
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->GetImageActor()->SetInterpolate(interpolate);
    if (mSlicers[i]->GetOverlayActor())
      mSlicers[i]->GetOverlayActor()->SetInterpolate(interpolate);
    if (mSlicers[i]->GetFusionActor())
      mSlicers[i]->GetFusionActor()->SetInterpolate(interpolate);
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetTSliceInSlicer(int tslice, int slicer)
{
  if (tslice < 0)
    tslice = 0;
  else if (tslice > mSlicers[slicer]->GetTMax())
    tslice = mSlicers[slicer]->GetTMax();
  if (mLandmarks)
    mLandmarks->SetTime(tslice);

  if (mSlicers[slicer]->GetTSlice() == tslice) return;

  mSlicers[slicer]->SetTSlice(tslice);
  if (mSlicers[slicer]->GetImageActor()->GetVisibility())
    UpdateTSlice(slicer);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetColorWindow(double s)
{
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetColorWindow(s);
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetColorLevel(double s)
{
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetColorLevel(s);
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::SetCursorAndCornerAnnotationVisibility(int s)
{
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetCursorVisibility(s);
    mSlicers[i]->SetCornerAnnotationVisibility(s);
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::SetOpacity(int i, double factor)
{
  mSlicers[i]->SetOpacity(1/factor);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
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
      z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5]) {
    mSlicers[slicer]->UpdateCursorPosition();
    mSlicers[slicer]->SetCursorColor(10,212,255);

    switch (mSlicers[slicer]->GetSliceOrientation()) {
    case vtkImageViewer2::SLICE_ORIENTATION_XY:
      if (mSlicers[slicer]->GetSlice() != (int)floor(z))
        mSlicers[slicer]->SetSlice((int)floor(z));
      break;

    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
      if (mSlicers[slicer]->GetSlice() != (int)floor(y))
        mSlicers[slicer]->SetSlice((int)floor(y));
      break;

    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
      if (mSlicers[slicer]->GetSlice() != (int)floor(x))
        mSlicers[slicer]->SetSlice((int)floor(x));
      break;
    }
    mSlicers[slicer]->Render();

    for ( unsigned int i = 0; i < mSlicers.size(); i++) {
      if (i != (unsigned int)slicer && mSlicers[i]->GetImageActor()->GetVisibility()
          && mSlicers[i]->GetRenderWindow()->GetSize()[0] > 2
          && mSlicers[i]->GetRenderWindow()->GetSize()[1] > 2) {
        mSlicers[i]->SetCurrentPosition(mSlicers[slicer]->GetCurrentPosition()[0],
                                        mSlicers[slicer]->GetCurrentPosition()[1],
                                        mSlicers[slicer]->GetCurrentPosition()[2],
                                        mSlicers[slicer]->GetTSlice());
        mSlicers[i]->UpdateCursorPosition();
        if (current) { //do not display corner annotation if image is the one picked
          mSlicers[i]->SetCurrentPosition(-VTK_DOUBLE_MAX,-VTK_DOUBLE_MAX,
                                          -VTK_DOUBLE_MAX, mSlicers[slicer]->GetTSlice());
          mSlicers[i]->SetCursorColor(255,10,212);
        } else {
          mSlicers[i]->SetCursorColor(150,10,282);
        }
        switch (mSlicers[i]->GetSliceOrientation()) {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
          if (mSlicers[i]->GetSlice() != (int)floor(z))
            mSlicers[i]->SetSlice((int)floor(z));
          break;

        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
          if (mSlicers[i]->GetSlice() != (int)floor(y))
            mSlicers[i]->SetSlice((int)floor(y));
          break;

        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
          if (mSlicers[i]->GetSlice() != (int)floor(x))
            mSlicers[i]->SetSlice((int)floor(x));
          break;
        }
        
        mSlicers[i]->Render();
        
        UpdateSlice(i);
        UpdateTSlice(i);
      }
    }
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
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
      z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5]) {
    for (std::list<std::string>::const_iterator i = mLinkedId.begin(); i != mLinkedId.end(); i++) {
      emit UpdateLinkManager(*i, slicer,mSlicers[slicer]->GetCurrentPosition()[0],
                             mSlicers[slicer]->GetCurrentPosition()[1],
                             mSlicers[slicer]->GetCurrentPosition()[2],mSlicers[slicer]->GetTSlice());
    }
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::UpdateLinkedNavigation(vvSlicer *refSlicer, bool bPropagate)
{
  vtkCamera *refCam = refSlicer->GetRenderer()->GetActiveCamera();
  double refPosition[3], refFocal[3];
  refCam->GetPosition(refPosition);
  refCam->GetFocalPoint(refFocal);
  
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    vtkCamera *camera = mSlicers[i]->GetRenderer()->GetActiveCamera();
    camera->SetParallelScale(refCam->GetParallelScale());

    double position[3], focal[3];
    camera->GetPosition(position);
    camera->GetFocalPoint(focal);

    if(refSlicer->GetSliceOrientation()==mSlicers[i]->GetSliceOrientation()) {
      for(int i=0; i<3; i++) {
        position[i] = refPosition[i];
        focal[i]    = refFocal[i];
      }
    }

    if(refSlicer->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_XY) {
      if(mSlicers[i]->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_XZ) {
        position[0] = refPosition[0];
        focal[0]    = refFocal[0];
      }
      if(mSlicers[i]->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_YZ) {
        position[1] = refPosition[1];
        focal[1]    = refFocal[1];
      }
    }

    if(refSlicer->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_XZ) {
      if(mSlicers[i]->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_YZ) {
        position[2] = refPosition[2];
        focal[2]    = refFocal[2];
      }
      if(mSlicers[i]->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_XY) {
        position[0] = refPosition[0];
        focal[0]    = refFocal[0];
      }
    }

    if(refSlicer->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_YZ) {
      if(mSlicers[i]->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_XY) {
        position[1] = refPosition[1];
        focal[1]    = refFocal[1];
      }
      if(mSlicers[i]->GetSliceOrientation()==vtkImageViewer2::SLICE_ORIENTATION_XZ) {
        position[2] = refPosition[2];
        focal[2]    = refFocal[2];
      }
    }

    camera->SetFocalPoint(focal);
    camera->SetPosition(position);
  
    //Fix for bug #243
    mSlicers[i]->ForceUpdateDisplayExtent();
  }
  
  Render();
  if(bPropagate)
    for (std::list<std::string>::const_iterator i = mLinkedId.begin(); i != mLinkedId.end(); i++)
      emit UpdateLinkedNavigation(*i, this, refSlicer);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
double vvSlicerManager::GetColorWindow()
{
  if (mSlicers.size())
    return mSlicers[0]->GetColorWindow();
  return -1;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
double vvSlicerManager::GetColorLevel()
{
  if (mSlicers.size())
    return mSlicers[0]->GetColorLevel();
  return -1;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::Render()
{
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->Render();
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::GenerateDefaultLookupTable()
{
  SetPreset(mPreset);
  SetColorMap(mColorMap);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::Reload()
{
  mReader->Update(mType);
  mImage=mReader->GetOutput();
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetImage(mImage);
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::ReloadFusion()
{
  mFusionReader->Update();
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetFusion(mFusionReader->GetOutput());
    mSlicers[i]->Render();
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::ReloadOverlay()
{
  mOverlayReader->Update();
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetOverlay(mOverlayReader->GetOutput());
    mSlicers[i]->Render();
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::ReloadVF()
{
  mVectorReader->Update(vvImageReader::VECTORFIELD); //deletes the old images through the VF::Init() function
  mVF=mVectorReader->GetOutput();
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetVF(mVF);
    mSlicers[i]->Render();
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::RemoveActor(const std::string& actor_type, int overlay_index)
{
  if (actor_type =="overlay")
    mOverlayReader = NULL;

  if (actor_type =="fusion")
    mFusionReader = NULL;

  for (unsigned int i = 0; i < mSlicers.size(); i++)
    mSlicers[i]->RemoveActor(actor_type,overlay_index);

  if (actor_type=="vector") {
    mVF=NULL;
    mVectorReader=NULL;
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::RemoveActors()
{
  ///This method leaks a few objects. See RemoveActor for what a
  ///correct implementation would look like
  //DS -> probably due to the reader (now released in the
  //RemoveActor() function. (I hope)
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetDisplayMode(0);
    mSlicers[i]->GetRenderer()->RemoveActor(mSlicers[i]->GetImageActor());
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
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
      Z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5]) {
    value = this->GetScalarComponentAsDouble(mSlicers[slicer]->GetInput(), X, Y, Z);

    if (mSlicers[slicer]->GetVFActor() && mSlicers[slicer]->GetVFActor()->GetVisibility()) {
      displayVec = 1;
      unsigned int currentTime = mSlicers[slicer]->GetTSlice();
      vtkImageData *vf = NULL;

      if (mSlicers[slicer]->GetVF()->GetVTKImages().size() > currentTime)
        vf = mSlicers[slicer]->GetVF()->GetVTKImages()[currentTime];
      else
        vf = mSlicers[slicer]->GetVF()->GetVTKImages()[0];

      if (vf) {
        double Xvf = (x - vf->GetOrigin()[0])/ vf->GetSpacing()[0];
        double Yvf = (y - vf->GetOrigin()[1])/ vf->GetSpacing()[1];
        double Zvf = (z - vf->GetOrigin()[2])/ vf->GetSpacing()[2];
        xVec = this->GetScalarComponentAsDouble( vf, Xvf, Yvf, Zvf, 0);
        yVec = this->GetScalarComponentAsDouble( vf, Xvf, Yvf, Zvf, 1);
        zVec = this->GetScalarComponentAsDouble( vf, Xvf, Yvf, Zvf, 2);
        valueVec = sqrt(xVec*xVec + yVec*yVec + zVec*zVec);
      }
    }
    if (mSlicers[slicer]->GetOverlayActor() && mSlicers[slicer]->GetOverlayActor()->GetVisibility()) {
      displayOver = 1;
      vtkImageData *overlay = dynamic_cast<vtkImageData*>(mSlicers[slicer]->GetOverlayMapper()->GetInput());
      double Xover = (x - overlay->GetOrigin()[0]) / overlay->GetSpacing()[0];
      double Yover = (y - overlay->GetOrigin()[1]) / overlay->GetSpacing()[1];
      double Zover = (z - overlay->GetOrigin()[2]) / overlay->GetSpacing()[2];
      valueOver = this->GetScalarComponentAsDouble(overlay, Xover, Yover, Zover);
    }
    if (mSlicers[slicer]->GetFusionActor() && mSlicers[slicer]->GetFusionActor()->GetVisibility()) {
      displayFus = 1;
      vtkImageData *fusion = dynamic_cast<vtkImageData*>(mSlicers[slicer]->GetFusionMapper()->GetInput());
      double Xover = (x - fusion->GetOrigin()[0]) / fusion->GetSpacing()[0];
      double Yover = (y - fusion->GetOrigin()[1]) / fusion->GetSpacing()[1];
      double Zover = (z - fusion->GetOrigin()[2]) / fusion->GetSpacing()[2];
      valueFus = this->GetScalarComponentAsDouble(fusion, Xover, Yover, Zover);
    }
    emit UpdatePosition(mSlicers[slicer]->GetCursorVisibility(),
                        x,y,z,X,Y,Z,value);
    emit UpdateVector(displayVec,xVec, yVec, zVec, valueVec);
    emit UpdateOverlay(displayOver,valueOver,value);
    emit UpdateFusion(displayFus,valueFus);
    for (unsigned int i = 0; i < mSlicers.size(); i++) {
      if (mSlicers[i]->GetImageActor()->GetVisibility() == 1)
        emit UpdateWindows(i,mSlicers[i]->GetSliceOrientation(),mSlicers[i]->GetSlice());
      else
        emit UpdateWindows(i,-1,-1);
    }
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::Activated()
{
  emit currentImageChanged(mId);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::Picked()
{
  emit currentPickedImageChanged(mId);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::UpdateWindowLevel()
{
  emit WindowLevelChanged(mSlicers[0]->GetColorWindow(),mSlicers[0]->GetColorLevel(),mPreset,mColorMap);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::UpdateSlice(int slicer)
{
  if (mPreviousSlice[slicer] == mSlicers[slicer]->GetSlice()) {
    //DD("============= NOTHING");
    return;
  }
  //std::cout << "vvSlicerManager::UpdateSlice " << slicer << " " << mSlicers[slicer]->GetSlice() << std::endl;
  emit UpdateSlice(slicer, mSlicers[slicer]->GetSlice());
  mSlicers[slicer]->Render(); // DS <-- I add this, this could/must be the only Render ...
  mPreviousSlice[slicer] = mSlicers[slicer]->GetSlice();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::UpdateTSlice(int slicer)
{
  int slice = mSlicers[slicer]->GetSlice();
  int tslice = mSlicers[slicer]->GetTSlice();
  if (mPreviousSlice[slicer] == slice) {
    if (mPreviousTSlice[slicer] == tslice) {
      //      DD("************** NOTHING ***********");
      return;
    }
  }
  mPreviousSlice[slicer] = slice;
  mPreviousTSlice[slicer] = tslice;
  //std::cout << "vvSlicerManager::UpdateTSlice " << slicer << " " << tslice << std::endl;
  emit UpdateTSlice(slicer, tslice);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::UpdateSliceRange(int slicer)
{
  emit UpdateSliceRange(slicer,
                        mSlicers[slicer]->GetSliceRange()[0], mSlicers[slicer]->GetSliceRange()[1],
                        0,mSlicers[slicer]->GetTMax());
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetPreset(int preset)
{
  //vtkLookupTable* LUT = static_cast<vtkLookupTable*>(mSlicers[0]->GetWindowLevel()->GetLookupTable());
  double window = mSlicers[0]->GetColorWindow();
  double level = mSlicers[0]->GetColorLevel();

  std::string component_type=mImage->GetScalarTypeAsITKString();
  switch (preset) {
  case 0:
    double range[2];
    mImage->GetScalarRange(range);
    window = range[1] - range[0];
    level = (range[1] + range[0])* 0.5;
    break;
  case 1:
    window = 2000;
    level = 0;
    break;
  case 2:
    window = 400;
    level = 20;
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
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetLocalColorWindowing(const int slicer)
{
  double min, max;
  this->mSlicers[slicer]->GetExtremasAroundMousePointer(min, max);
  this->SetColorWindow(max-min);
  this->SetColorLevel(0.5*(min+max));
  this->UpdateWindowLevel();
  this->Render();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetColorMap()
{
  SetColorMap(mColorMap);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetColorMap(int colormap)
{
  double range[2];
  range[0] = mSlicers[0]->GetInput()->GetScalarRange()[0];
  range[1] = mSlicers[0]->GetInput()->GetScalarRange()[1];

  double window = mSlicers[0]->GetWindowLevel()->GetWindow();
  double level = mSlicers[0]->GetWindowLevel()->GetLevel();

  vtkLookupTable* LUT = static_cast<vtkLookupTable*>(mSlicers[0]->GetWindowLevel()->GetLookupTable());
  switch (colormap) {
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
  if (LUT) {
    LUT->SetTableRange(level-fabs(window)/4,level+fabs(window)/4);
    LUT->Build();
  }
  vtkLookupTable* fusLUT = NULL;
  if (mSlicers[0]->GetFusion()) {
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
    } else if (mSlicers[i]->GetOverlay()) {
      //dynamic_cast<vvImageMapToWLColors*>(mSlicers[i]->GetWindowLevel())
      //->SetWindowLevelMode(false);
      mSlicers[i]->GetWindowLevel()->SetLookupTable(LUT);
    } else {
      mSlicers[i]->GetWindowLevel()->SetLookupTable(LUT);
    }
    if (mSlicers[i]->GetFusion() && mSlicers[i]->GetFusionActor()->GetVisibility()) {
      mSlicers[i]->GetFusionActor()->SetOpacity(double(mFusionOpacity)/100);
      mSlicers[i]->GetFusionMapper()->SetLookupTable(fusLUT);
      mSlicers[i]->GetFusionMapper()->SetWindow(mFusionWindow);
      mSlicers[i]->GetFusionMapper()->SetLevel(mFusionLevel);
    }
  }
  if (fusLUT)
    fusLUT->Delete();
  if (colormap >= 0)
    mColorMap = colormap;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
vvLandmarks* vvSlicerManager::GetLandmarks()
{
  if (mLandmarks == NULL) {
    mLandmarks = new vvLandmarks(mSlicers[0]->GetTMax()+1);
    for (unsigned int i = 0; i < mSlicers.size(); i++)
      mSlicers[i]->SetLandmarks(mLandmarks);
  }
  return mLandmarks;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::AddLandmark(float x,float y,float z,float t)
{
  double x_index = (x - mSlicers[0]->GetInput()->GetOrigin()[0])/mSlicers[0]->GetInput()->GetSpacing()[0];
  double y_index = (y - mSlicers[0]->GetInput()->GetOrigin()[1])/mSlicers[0]->GetInput()->GetSpacing()[1];
  double z_index = (z - mSlicers[0]->GetInput()->GetOrigin()[2])/mSlicers[0]->GetInput()->GetSpacing()[2];
  if (x_index >= mSlicers[0]->GetInput()->GetWholeExtent()[0] &&
      x_index <= mSlicers[0]->GetInput()->GetWholeExtent()[1] &&
      y_index >= mSlicers[0]->GetInput()->GetWholeExtent()[2] &&
      y_index <= mSlicers[0]->GetInput()->GetWholeExtent()[3] &&
      z_index >= mSlicers[0]->GetInput()->GetWholeExtent()[4] &&
      z_index <= mSlicers[0]->GetInput()->GetWholeExtent()[5]) {
    double value = this->GetScalarComponentAsDouble(mSlicers[0]->GetInput(), x_index, y_index, z_index);
    this->GetLandmarks()->AddLandmark(x,y,z,t,value);
    emit LandmarkAdded();
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::PrevImage(int slicer)
{
  emit ChangeImageWithIndexOffset(this, slicer, -1);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::NextImage(int slicer)
{
  emit ChangeImageWithIndexOffset(this, slicer,  1);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::VerticalSliderHasChanged(int slicer, int slice)
{
  emit AVerticalSliderHasChanged(slicer, slice);
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
double vvSlicerManager::GetScalarComponentAsDouble(vtkImageData *image, double X, double Y, double Z, int component)
{
  int ix, iy, iz;
  return mSlicers[0]->GetScalarComponentAsDouble(image, X, Y, Z, ix, iy, iz, component);
}
//----------------------------------------------------------------------------
