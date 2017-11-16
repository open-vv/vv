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
#include "vvBlendImageActor.h"

#include <vtkVersion.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
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
#include <QMessageBox>
//----------------------------------------------------------------------------
vvSlicerManager::vvSlicerManager(int numberOfSlicers)
{ 

  connect(this, SIGNAL(callAddLandmark(float,float,float,float)), this, SLOT(AddLandmark(float,float,float,float)));

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

  mFusionOpacity = 30;
  mFusionThresOpacity = 1;
  mFusionColorMap = 3;
  mFusionWindow = 1000;
  mFusionLevel = 1000;
  mFusionShowLegend = true;
  
  mFusionSequenceInvolvementCode = -1;
  mFusionSequenceIndexLinkedManager = -1;
  mFusionSequenceFrameIndex = -1;
  mFusionSequenceNbFrames = 0;
  mFusionSequenceSpatialSyncFlag = false;
  mFusionSequenceTemporalSyncFlag = false;

  mLandmarks = NULL;
  mLinkedId.resize(0);

  for ( int i = 0; i < numberOfSlicers; i++) {
    mSlicers.push_back(vtkSmartPointer<vvSlicer>::New());
    mSlicers[i]->SetSlicerNumber(i);
  }
  mSelectedSlicer = -1;
  
  mPreviousSlice.resize(numberOfSlicers, 0);
  mPreviousTSlice.resize(numberOfSlicers, 0);
  mSlicingPreset = WORLD_SLICING;

  
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
  else if(actorType=="fusionSequence")
	reader = mFusionSequenceReader;

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
bool vvSlicerManager::SetImages(std::vector<std::string> filenames, vvImageReader::LoadedImageType type, int n, bool patientCoordinateSystem)
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
  if (type == vvImageReader::DICOM)
    mReader->SetPatientCoordinateSystem(patientCoordinateSystem);
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
bool vvSlicerManager::SetOverlay(std::vector<std::string> filenames,int dim, std::string component, vvImageReader::LoadedImageType type)
{ 
  mOverlayName = filenames[0];
  mOverlayComponent = component;
  if (dim > mImage->GetNumberOfDimensions()) {
    mLastError = " Overlay dimension cannot be greater than reference image!";
    return false;
  }
  if (mOverlayReader.IsNull())
    mOverlayReader = vvImageReader::New();
  mOverlayReader->SetInputFilenames(filenames);
  mOverlayReader->Update(type);
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
bool vvSlicerManager::SetFusion(std::vector<std::string> filenames,int dim, std::string component, vvImageReader::LoadedImageType type)
{ 
  mFusionName = filenames[0];
  mFusionComponent = component;
  if (dim > mImage->GetNumberOfDimensions()) {
    mLastError = " Fusion dimension cannot be greater than reference image!";
    return false;
  }
  if (mFusionReader.IsNull())
    mFusionReader = vvImageReader::New();
  mFusionReader->SetInputFilenames(filenames);
  mFusionReader->Update(type);
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
//this function is called by vvMainWindow::AddFusionSequence for the primary sequence (CT), while the given files constitute the secondary sequence.
bool vvSlicerManager::SetFusionSequence(std::vector<std::string> filenames, int dim, std::string component, vvImageReader::LoadedImageType type)
{ 
	mFusionSequenceInvolvementCode = 0;

	mFusionName = filenames[0];
	mFusionComponent = component;

	if (dim > mImage->GetNumberOfDimensions()) {
		mLastError = " Fusion Sequence dimension cannot be greater than reference image!";
		return false;
	}

	if (mFusionSequenceReader.IsNull())
		mFusionSequenceReader = vvImageReader::New();

	mFusionSequenceReader->SetInputFilenames(filenames);
	mFusionSequenceReader->Update(type);


	if (mFusionSequenceReader->GetLastError().size() == 0) {
		for ( unsigned int i = 0; i < mSlicers.size(); i++) {
			mSlicers[i]->SetFusion(mFusionSequenceReader->GetOutput(), mFusionSequenceInvolvementCode);
		}
	} else {
		mLastError = mFusionSequenceReader->GetLastError();
		return false;
	}
	double *fusRange = mFusionSequenceReader->GetOutput()->GetVTKImages()[0]->GetScalarRange();
	mFusionLevel = (fusRange[0]+fusRange[1])/2;
	mFusionWindow = fusRange[1]-fusRange[0];

	//store the initial transform matrices of each frame, and reset them to identity
	mFusionSequenceListInitialTransformMatrices.clear();
	for (unsigned i=0 ; i<mFusionSequenceReader->GetOutput()->GetTransform().size() ; i++) {
		AddFusionSequenceInitialTransformMatrices( mFusionSequenceReader->GetOutput()->GetTransform()[i]->GetMatrix() );
		mFusionSequenceReader->GetOutput()->GetTransform()[i]->Identity();
		mFusionSequenceReader->GetOutput()->GetTransform()[i]->Update();
	}

	//adjust the time slider in the overlay panel
	mFusionSequenceNbFrames = mFusionSequenceReader->GetOutput()->GetTransform().size(); 
	mFusionSequenceFrameIndex = std::max<int>( 0, std::min<int>(mFusionSequenceFrameIndex, mFusionSequenceNbFrames));

	return true;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
bool vvSlicerManager::SetVF(std::string filename)
{ 
  if (mVectorReader.IsNull())
    mVectorReader = vvImageReader::New();
  mVectorReader->SetInputFilename(filename);
  
  if (mType == vvImageReader::IMAGEWITHTIME)
    mVectorReader->Update(vvImageReader::VECTORFIELDWITHTIME);
  else
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
  if (mSlicers[num]->GetImage()) {
    mSlicers[num]->SetDisplayMode(state);
  }
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
void vvSlicerManager::EmitMousePositionUpdated(int slicer)
{ 
  emit MousePositionUpdatedSignal(slicer);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::EmitKeyPressed(std::string KeyPress)
{ 
  emit KeyPressedSignal(KeyPress);
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
int vvSlicerManager::GetTSlice()
{ 
  return mSlicers[0]->GetTSlice();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::SetTSlice(int slice, bool updateLinkedImages)
{ 
	if (!updateLinkedImages) { //for fusionSequence, TMax / MaxCurrentTSlice are irrelevant.
		for ( unsigned int i = 0; i < mSlicers.size(); i++) {
			mSlicers[i]->SetTSlice(slice, updateLinkedImages);
			UpdateTSlice(i);
		}
		return;
	}

  if (slice < 0)
    slice = 0;
  else if (slice > mSlicers[0]->GetTMax())
    slice = mSlicers[0]->GetTMax();
  if (mLandmarks)
    mLandmarks->SetTime(slice);
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    if (slice != mSlicers[i]->GetMaxCurrentTSlice()) {
      mSlicers[i]->SetTSlice(slice, updateLinkedImages);
      UpdateTSlice(i);
    }
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetFusionSequenceTSlice(int slice)
{ 
	for ( unsigned int i = 0; i < mSlicers.size(); i++) {
		mSlicers[i]->SetFusionSequenceTSlice(slice);
		UpdateTSlice(i);
	}
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetNextTSlice(int originating_slicer)
{ 
  int t = mSlicers[0]->GetMaxCurrentTSlice();
  t++;
  if (t > mSlicers[0]->GetTMax())
    t = 0;
  emit UpdateTSlice(originating_slicer,t, mFusionSequenceInvolvementCode);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::SetPreviousTSlice(int originating_slicer)
{ 
  int t = mSlicers[0]->GetMaxCurrentTSlice();
  t--;
  if (t < 0)
    t = mSlicers[0]->GetTMax();
  emit UpdateTSlice(originating_slicer,t, mFusionSequenceInvolvementCode);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::ToggleInterpolation()
{ 
  bool interpolate=!(mSlicers[0]->GetImageActor()->GetInterpolate());
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetInterpolationImageReslice(interpolate);
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

  if (mSlicers[slicer]->GetMaxCurrentTSlice() == tslice) return;

  mSlicers[slicer]->SetTSlice(tslice);

  if(mSlicingPreset==VOXELS_SLICING) {
    vtkMatrix4x4 *imageTransformInverse = vtkMatrix4x4::New();
    mImage->GetTransform()[mSlicers[slicer]->GetTSlice()]->GetInverse(imageTransformInverse);
    this->GetSlicer(slicer)->GetSlicingTransform()->SetMatrix(imageTransformInverse);
    imageTransformInverse->Delete();
  }

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
void vvSlicerManager::SetOverlayColorWindow(double s)
{ 
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetOverlayColorWindow(s);
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::SetOverlayColorLevel(double s)
{ 
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetOverlayColorLevel(s);
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::SetLinkOverlayWindowLevel(bool b)
{ 
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetLinkOverlayWindowLevel(b);
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
  double p[3], pt[3];
  p[0] = mSlicers[slicer]->GetCurrentPosition()[0];
  p[1] = mSlicers[slicer]->GetCurrentPosition()[1];
  p[2] = mSlicers[slicer]->GetCurrentPosition()[2];
  mSlicers[slicer]->GetSlicingTransform()->GetInverse()->TransformPoint(p, pt);

  double x = (pt[0] - mSlicers[slicer]->GetInput()->GetOrigin()[0])
    /mSlicers[slicer]->GetInput()->GetSpacing()[0];
  double y = (pt[1] - mSlicers[slicer]->GetInput()->GetOrigin()[1])
    /mSlicers[slicer]->GetInput()->GetSpacing()[1];
  double z = (pt[2] - mSlicers[slicer]->GetInput()->GetOrigin()[2])
    /mSlicers[slicer]->GetInput()->GetSpacing()[2];
#if VTK_MAJOR_VERSION <= 5
  if (x >= mSlicers[slicer]->GetInput()->GetWholeExtent()[0]-0.5 &&
      x <= mSlicers[slicer]->GetInput()->GetWholeExtent()[1]+0.5 &&
      y >= mSlicers[slicer]->GetInput()->GetWholeExtent()[2]-0.5 &&
      y <= mSlicers[slicer]->GetInput()->GetWholeExtent()[3]+0.5 &&
      z >= mSlicers[slicer]->GetInput()->GetWholeExtent()[4]-0.5 &&
      z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5]+0.5)
#else
int extentImageReslice[6];
mSlicers[slicer]->GetRegisterExtent(extentImageReslice);
  if (x >= extentImageReslice[0]-0.5 &&
      x <= extentImageReslice[1]+0.5 &&
      y >= extentImageReslice[2]-0.5 &&
      y <= extentImageReslice[3]+0.5 &&
      z >= extentImageReslice[4]-0.5 &&
      z <= extentImageReslice[5]+0.5)
#endif
  {
    mSlicers[slicer]->UpdateCursorPosition();
    mSlicers[slicer]->SetCursorColor(10,212,255);
    mSelectedSlicer = slicer;

    switch (mSlicers[slicer]->GetSliceOrientation()) {
    case vtkImageViewer2::SLICE_ORIENTATION_XY:
      if (mSlicers[slicer]->GetSlice() != (int)lrint(z))
        mSlicers[slicer]->SetSlice((int)lrint(z));
      break;

    case vtkImageViewer2::SLICE_ORIENTATION_XZ:
      if (mSlicers[slicer]->GetSlice() != (int)lrint(y))
        mSlicers[slicer]->SetSlice((int)lrint(y));
      break;

    case vtkImageViewer2::SLICE_ORIENTATION_YZ:
      if (mSlicers[slicer]->GetSlice() != (int)lrint(x))
        mSlicers[slicer]->SetSlice((int)lrint(x));
      break;
    }
    mSlicers[slicer]->Render();

    for ( unsigned int i = 0; i < mSlicers.size(); i++) {
      if (i != (unsigned int)slicer
          && mSlicers[i]->GetRenderer()->GetDraw()
          && mSlicers[i]->GetRenderWindow()->GetSize()[0] > 2
          && mSlicers[i]->GetRenderWindow()->GetSize()[1] > 2) {
        mSlicers[i]->SetCurrentPosition(p[0], p[1], p[2], mSlicers[slicer]->GetMaxCurrentTSlice());
        mSlicers[i]->UpdateCursorPosition();
        if (current) { //do not display corner annotation if image is the one picked
          mSlicers[i]->SetCurrentPosition(-VTK_DOUBLE_MAX,-VTK_DOUBLE_MAX,
                                          -VTK_DOUBLE_MAX, mSlicers[slicer]->GetMaxCurrentTSlice());
          mSlicers[i]->SetCursorColor(255,10,212);
        } else {
          mSlicers[i]->SetCursorColor(150,10,282);
        }
        switch (mSlicers[i]->GetSliceOrientation()) {
        case vtkImageViewer2::SLICE_ORIENTATION_XY:
          if (mSlicers[i]->GetSlice() != (int)lrint(z))
            mSlicers[i]->SetSlice((int)lrint(z));
          break;

        case vtkImageViewer2::SLICE_ORIENTATION_XZ:
          if (mSlicers[i]->GetSlice() != (int)lrint(y))
            mSlicers[i]->SetSlice((int)lrint(y));
          break;

        case vtkImageViewer2::SLICE_ORIENTATION_YZ:
          if (mSlicers[i]->GetSlice() != (int)lrint(x))
            mSlicers[i]->SetSlice((int)lrint(x));
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
  double p[3], pt[3];
  p[0] = mSlicers[slicer]->GetCurrentPosition()[0];
  p[1] = mSlicers[slicer]->GetCurrentPosition()[1];
  p[2] = mSlicers[slicer]->GetCurrentPosition()[2];
  mSlicers[slicer]->GetSlicingTransform()->GetInverse()->TransformPoint(p, pt);
  double x = (pt[0] - mSlicers[slicer]->GetInput()->GetOrigin()[0]) / mSlicers[slicer]->GetInput()->GetSpacing()[0];
  double y = (pt[1] - mSlicers[slicer]->GetInput()->GetOrigin()[1]) / mSlicers[slicer]->GetInput()->GetSpacing()[1];
  double z = (pt[2] - mSlicers[slicer]->GetInput()->GetOrigin()[2]) / mSlicers[slicer]->GetInput()->GetSpacing()[2];
#if VTK_MAJOR_VERSION <= 5
  if (x >= mSlicers[slicer]->GetInput()->GetWholeExtent()[0]-0.5 &&
      x <= mSlicers[slicer]->GetInput()->GetWholeExtent()[1]+0.5 &&
      y >= mSlicers[slicer]->GetInput()->GetWholeExtent()[2]-0.5 &&
      y <= mSlicers[slicer]->GetInput()->GetWholeExtent()[3]+0.5 &&
      z >= mSlicers[slicer]->GetInput()->GetWholeExtent()[4]-0.5 &&
      z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5]+0.5)
#else
int extentImageReslice[6];
mSlicers[slicer]->GetRegisterExtent(extentImageReslice);
  if (x >= extentImageReslice[0]-0.5 &&
      x <= extentImageReslice[1]+0.5 &&
      y >= extentImageReslice[2]-0.5 &&
      y <= extentImageReslice[3]+0.5 &&
      z >= extentImageReslice[4]-0.5 &&
      z <= extentImageReslice[5]+0.5)
#endif
  {
    for (std::list<std::string>::const_iterator i = mLinkedId.begin(); i != mLinkedId.end(); i++) {
		if (this->IsInvolvedInFusionSequence()) {
			//this SlicerManager is involved in fusionSequence => do not synchronize the times
			emit UpdateLinkManager(*i, slicer, p[0], p[1], p[2], -1);
		}
		else {
			emit UpdateLinkManager(*i, slicer, p[0], p[1], p[2], mSlicers[slicer]->GetMaxCurrentTSlice());
		}
    }
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::UpdateLinkedNavigation(vvSlicer *refSlicer, bool bPropagate)
{ 
  vtkCamera *refCam = refSlicer->GetRenderer()->GetActiveCamera();

  double refPosition[3];
  refCam->GetPosition(refPosition);
  refPosition[refSlicer->GetSliceOrientation()] = refSlicer->GetSlice() *
      refSlicer->GetInput()->GetSpacing()[refSlicer->GetSliceOrientation()] +
      refSlicer->GetInput()->GetOrigin()[refSlicer->GetSliceOrientation()];

  refSlicer->GetSlicingTransform()->TransformPoint(refPosition, refPosition);
  mSlicers[0]->GetSlicingTransform()->GetInverse()->TransformPoint(refPosition, refPosition);

  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    vtkCamera *camera = mSlicers[i]->GetRenderer()->GetActiveCamera();
    camera->SetParallelScale(refCam->GetParallelScale());

    double position[3], focal[3];
    camera->GetPosition(position);
    camera->GetFocalPoint(focal);

    for(int j=0; j<3; j++) {
      if(j!=mSlicers[i]->GetSliceOrientation()) {
        position[j] = refPosition[j];
        focal[j]    = refPosition[j];
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
double vvSlicerManager::GetColorWindow() const
{ 
  if (mSlicers.size())
    return mSlicers[0]->GetColorWindow();
  return -1;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
double vvSlicerManager::GetColorLevel() const
{ 
  if (mSlicers.size())
    return mSlicers[0]->GetColorLevel();
  return -1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
double vvSlicerManager::GetOverlayColorWindow() const
{ 
  if (mSlicers.size())
    return mSlicers[0]->GetOverlayColorWindow();
  return -1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
double vvSlicerManager::GetOverlayColorLevel() const
{ 
  if (mSlicers.size())
    return mSlicers[0]->GetOverlayColorLevel();
  return -1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
bool vvSlicerManager::GetLinkOverlayWindowLevel() const
{ 
  if (mSlicers.size())
    return mSlicers[0]->GetLinkOverlayWindowLevel();
  return -1;
}
//----------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvSlicerManager::ResetTransformationToIdentity(const std::string actorType)
{ 
  if(actorType == "image")
    for(unsigned int i=0; i<this->GetImage()->GetTransform().size(); i++)
      this->GetImage()->GetTransform()[i]->Identity();
  else if(actorType == "overlay")
    for(unsigned int i=0; i<this->GetImage()->GetTransform().size(); i++)
      this->GetSlicer(0)->GetOverlay()->GetTransform()[i]->Identity();
  else if(actorType == "fusion")
    for(unsigned int i=0; i<this->GetImage()->GetTransform().size(); i++)
      this->GetSlicer(0)->GetFusion()->GetTransform()[i]->Identity();
  else if(actorType == "fusionSequence") //TODO: Check what should really be done here
    for(unsigned int i=0; i<this->GetImage()->GetTransform().size(); i++)
      this->GetSlicer(0)->GetFusion()->GetTransform()[i]->Identity();
  else if(actorType == "vf")
    for(unsigned int i=0; i<this->GetImage()->GetTransform().size(); i++)
      this->GetVF()->GetTransform()[i]->Identity();
  else
    return;

  for(int i=0; i< this->GetNumberOfSlicers(); i++){
    this->GetSlicer(i)->ForceUpdateDisplayExtent();
    this->GetSlicer(i)->ResetCamera();
    this->GetSlicer(i)->Render();
  }
}
//------------------------------------------------------------------------------

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
  
  //if this image is the primary sequence of a fusion sequence, then the main transform matrix should be updated.
  if (this->IsMainSequenceOfFusionSequence()) {
	  SetFusionSequenceMainTransformMatrix( mImage->GetTransform()[0]->GetMatrix() );
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::ReloadFusion()
{ 
  mFusionReader->Update(mImage->GetNumberOfDimensions(),mFusionComponent.c_str(),mType);

  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetFusion(mFusionReader->GetOutput());
    mSlicers[i]->Render();
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//the secondary sequence is being reloaded.
void vvSlicerManager::ReloadFusionSequence()
{ 
  //  this is to keep the slice thickness, which needs to be artificially increased for visualization
  double sp_x, sp_y, sp_z;
  this->GetImage()->GetVTKImages()[0]->GetSpacing(sp_x, sp_y, sp_z);

  mFusionSequenceReader->Update(mImage->GetNumberOfDimensions(),mFusionComponent.c_str(),vvImageReader::MERGEDWITHTIME);

  for ( unsigned int i = 0; i < mSlicers.size(); i++) {
    mSlicers[i]->SetFusion(mFusionSequenceReader->GetOutput(), 1);
    mSlicers[i]->Render();
  }

  //Update the slider
  mFusionSequenceNbFrames = mFusionSequenceReader->GetOutput()->GetTransform().size();
  if (mFusionSequenceFrameIndex>=mFusionSequenceNbFrames) {
	  mFusionSequenceFrameIndex=0;
  }

  //Update the list of initial transforms
  mFusionSequenceListInitialTransformMatrices.clear();
  for (unsigned i=0 ; i<mFusionSequenceNbFrames ; i++) {
	  this->AddFusionSequenceInitialTransformMatrices( mFusionSequenceReader->GetOutput()->GetTransform()[i]->GetMatrix() );
  }

  //  also update the slice thickness
  for (unsigned i=0 ; i<this->GetImage()->GetTransform().size() ; i++) {
    sp_x = this->GetImage()->GetVTKImages()[i]->GetSpacing()[0];
    sp_y = this->GetImage()->GetVTKImages()[i]->GetSpacing()[1];
    this->GetImage()->GetVTKImages()[i]->SetSpacing( sp_x, sp_y, sp_z);
  }

}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::ReloadOverlay()
{ 
  mOverlayReader->Update(mImage->GetNumberOfDimensions(),mOverlayComponent.c_str(),mType);
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

  if (actor_type =="fusionSequence") {
    mFusionSequenceReader = NULL;
  }

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
  double x = mSlicers[slicer]->GetCurrentPosition()[0];
  double y = mSlicers[slicer]->GetCurrentPosition()[1];
  double z = mSlicers[slicer]->GetCurrentPosition()[2];
  double xyz[3], xyzTransform[3];
  xyz[0] = x;
  xyz[1] = y;
  xyz[2] = z;
  mSlicers[slicer]->GetSlicingTransform()->GetInverse()->TransformPoint(xyz, xyzTransform);
  double X = (xyzTransform[0] - mSlicers[slicer]->GetInput()->GetOrigin()[0])/mSlicers[slicer]->GetInput()->GetSpacing()[0];
  double Y = (xyzTransform[1] - mSlicers[slicer]->GetInput()->GetOrigin()[1])/mSlicers[slicer]->GetInput()->GetSpacing()[1];
  double Z = (xyzTransform[2] - mSlicers[slicer]->GetInput()->GetOrigin()[2])/mSlicers[slicer]->GetInput()->GetSpacing()[2];
  double value = -VTK_DOUBLE_MAX;
  int displayVec = 0;
  double xVec=0, yVec=0, zVec=0, valueVec=0;
  int displayOver = 0;
  int displayFus = 0;
  double valueOver=0, valueFus=0;
#if VTK_MAJOR_VERSION <= 5
    if (X >= mSlicers[slicer]->GetInput()->GetWholeExtent()[0] -0.5 &&
      X <= mSlicers[slicer]->GetInput()->GetWholeExtent()[1] +0.5 &&
      Y >= mSlicers[slicer]->GetInput()->GetWholeExtent()[2] -0.5 &&
      Y <= mSlicers[slicer]->GetInput()->GetWholeExtent()[3] +0.5 &&
      Z >= mSlicers[slicer]->GetInput()->GetWholeExtent()[4] -0.5 &&
      Z <= mSlicers[slicer]->GetInput()->GetWholeExtent()[5] +0.5)
#else
int extentImageReslice[6];
mSlicers[slicer]->GetRegisterExtent(extentImageReslice);
    if (X >= extentImageReslice[0] -0.5 &&
      X <= extentImageReslice[1] +0.5 &&
      Y >= extentImageReslice[2] -0.5 &&
      Y <= extentImageReslice[3] +0.5 &&
      Z >= extentImageReslice[4] -0.5 &&
      Z <= extentImageReslice[5] +0.5)
#endif
    {
    value = this->GetScalarComponentAsDouble(mSlicers[slicer]->GetInput(), X, Y, Z);

    if (mSlicers[slicer]->GetVFActor() ) {
      displayVec = 1;
      unsigned int currentTime = mSlicers[slicer]->GetMaxCurrentTSlice();
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
    if (mSlicers[slicer]->GetOverlayActor() ) {
      displayOver = 1;
      vtkImageData *overlay = dynamic_cast<vtkImageData*>(mSlicers[slicer]->GetOverlayMapper()->GetInput());
      double Xover = (x - overlay->GetOrigin()[0]) / overlay->GetSpacing()[0];
      double Yover = (y - overlay->GetOrigin()[1]) / overlay->GetSpacing()[1];
      double Zover = (z - overlay->GetOrigin()[2]) / overlay->GetSpacing()[2];
      valueOver = this->GetScalarComponentAsDouble(overlay, Xover, Yover, Zover);
    }

	if ( mSlicers[slicer]->GetFusionActor() ) {
		displayFus = 1;
		vtkImageData *fusion = dynamic_cast<vtkImageData*>(mSlicers[slicer]->GetFusionMapper()->GetInput());
		double Xover = (x - fusion->GetOrigin()[0]) / fusion->GetSpacing()[0];
		double Yover = (y - fusion->GetOrigin()[1]) / fusion->GetSpacing()[1];
		double Zover = (z - fusion->GetOrigin()[2]) / fusion->GetSpacing()[2];
		valueFus = this->GetScalarComponentAsDouble(fusion, Xover, Yover, Zover);
	}
	else if (this->IsInvolvedInFusionSequence()) { 
    //if the cursor moves over the 'independent' version of the secondary sequence
    //do not update the panel, just keep it as it is.
		displayFus = 1;
		valueFus = std::numeric_limits<double>::quiet_NaN();
	}

    emit UpdatePosition(mSlicers[slicer]->GetCursorVisibility(),
                        x,y,z,X,Y,Z,value);
    emit UpdateVector(displayVec,xVec, yVec, zVec, valueVec);
    emit UpdateOverlay(displayOver,valueOver,value);
    emit UpdateFusion(displayFus,valueFus);
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
  emit WindowLevelChanged();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::UpdateSlice(int slicer)
{ 
  if (mPreviousSlice[slicer] == mSlicers[slicer]->GetSlice()) {
    //DD("============= NOTHING");
    return;
  }
  emit UpdateSlice(slicer, mSlicers[slicer]->GetSlice());
  mSlicers[slicer]->Render(); // DS <-- I add this, this could/must be the only Render ...
  mPreviousSlice[slicer] = mSlicers[slicer]->GetSlice();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvSlicerManager::UpdateTSlice(int slicer)
{ 
  int slice = mSlicers[slicer]->GetSlice();

  int tslice = mSlicers[slicer]->GetMaxCurrentTSlice();
  //if (this->IsInvolvedInFusionSequence()) tslice = mSlicers[slicer]->GetTSlice(); //actually, this is handled by the Slicer

  if (mPreviousSlice[slicer] == slice) {
    if (mPreviousTSlice[slicer] == tslice) {
      //      DD("************** NOTHING ***********");
      return;
    }
  }
  mPreviousSlice[slicer] = slice;
  mPreviousTSlice[slicer] = tslice;

  emit UpdateTSlice(slicer, tslice, mFusionSequenceInvolvementCode);
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
void vvSlicerManager::SetSlicingPreset(SlicingPresetType preset)
{ 
  if(mSlicingPreset==preset)
    return;

  vtkMatrix4x4 *imageTransformInverse = vtkMatrix4x4::New();
  mImage->GetTransform()[this->GetTSlice()]->GetInverse(imageTransformInverse);

  for(int i=0; i< this->GetNumberOfSlicers(); i++){
    vvSlicer *s = this->GetSlicer(i);
    switch(preset)
    {
    case WORLD_SLICING:
      s->GetSlicingTransform()->Identity();
      break;
    case VOXELS_SLICING:
      s->GetSlicingTransform()->SetMatrix(imageTransformInverse);
      break;
    default:
      imageTransformInverse->Delete();
      return;
    }
    s->ForceUpdateDisplayExtent();
#if VTK_MAJOR_VERSION <= 5
    s->SetSlice((s->GetInput()->GetWholeExtent()[s->GetSliceOrientation()*2+1]
                +s->GetInput()->GetWholeExtent()[s->GetSliceOrientation()*2])/2.0);
#else
    int extentImageReslice[6];
    s->GetRegisterExtent(extentImageReslice);
    s->SetSlice((extentImageReslice[s->GetSliceOrientation()*2+1]
                +extentImageReslice[s->GetSliceOrientation()*2])/2.0);
#endif
    s->ResetCamera();
    s->Render();
  }

  imageTransformInverse->Delete();
  mSlicingPreset = preset;
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
  case WL_AUTO:
    double range[2];
    mImage->GetScalarRange(range);
    window = range[1] - range[0];
    level = (range[1] + range[0])* 0.5;
    break;
  case WL_HOUNSFIELD:
    window = 2000;
    level = 0;
    break;
  case WL_SOFTTISSUE:
    window = 400;
    level = 20;
    break;
  case WL_LUNGS: // lungs (same as FOCAL)
    window = 1700;
    level = -300;
    break;
  case WL_BONES:
    window = 1000;
    level = 500;
    break;
  case WL_HEAD:
    window = 200;
    level = 70;
    break;
  case WL_BINARY:
    window = 1;
    level = 0.5;
    break;
  case WL_USER:
    break;
  case WL_VENTILATION:
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
void vvSlicerManager::SetLocalColorWindowing(const int slicer, const bool bCtrlKey)
{ 
  double min, max;
  if(bCtrlKey && this->mSlicers[slicer]->GetFusion()) {
    int t = mSlicers[slicer]->GetFusionTSlice();
    this->mSlicers[slicer]->GetExtremasAroundMousePointer(min, max,
                                                          this->mSlicers[slicer]->GetFusion()->GetVTKImages()[t],
                                                          this->mSlicers[slicer]->GetConcatenatedFusionTransform());
    this->SetFusionWindow(max-min);
    this->SetFusionLevel(0.5*(min+max));
    this->SetColorMap(mColorMap);
  }
  else if(bCtrlKey && this->mSlicers[slicer]->GetOverlay()) {
    int t = mSlicers[slicer]->GetOverlayTSlice();
    this->mSlicers[slicer]->GetExtremasAroundMousePointer(min, max,
                                                          this->mSlicers[slicer]->GetOverlay()->GetVTKImages()[t],
                                                          this->mSlicers[slicer]->GetConcatenatedOverlayTransform());
    if(this->mSlicers[slicer]->GetLinkOverlayWindowLevel()){
      this->SetColorWindow(max-min);
      this->SetColorLevel(0.5*(min+max));
    } else {
      this->SetOverlayColorWindow(max-min);
      this->SetOverlayColorLevel(0.5*(min+max));
    }
  }
  else {
    int t = this->GetTSlice();
    this->mSlicers[slicer]->GetExtremasAroundMousePointer(min, max,
                                                          this->mSlicers[slicer]->GetImage()->GetVTKImages()[t],
                                                          this->mSlicers[slicer]->GetConcatenatedTransform());
    this->SetColorWindow(max-min);
    this->SetColorLevel(0.5*(min+max));
    this->SetPreset(WL_USER);
  }
  this->Render();
  this->UpdateWindowLevel();
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
    LUT->SetValueRange(0.5,1);
    LUT->SetSaturationRange(1,1);
    LUT->SetHueRange(0.666,0);
    break;
  case 4:
    if (LUT == NULL)
      LUT = vtkLookupTable::New();
    LUT->SetValueRange(1,1);
    LUT->SetSaturationRange(1,1);
    LUT->SetHueRange(0,1);
    LUT->SetAlphaRange(1, 1);
    break;
  case 5:
    if (LUT == NULL)
      LUT = vtkLookupTable::New();
    LUT->SetValueRange(1,1);
    LUT->SetSaturationRange(1,1);
    LUT->SetHueRange(1,0.1);
    //LUT->SetRampToLinear();
    break;
  }
  if (LUT) {
    LUT->SetTableRange(level-fabs(window)/2,level+fabs(window)/2);
    LUT->Build();
  }
  vtkWindowLevelLookupTable* fusLUT = NULL;

  //FUSION / FUSION SEQUENCE
  if (mSlicers[0]->GetFusion()) { // && mFusionColorMap >= 0) {
    fusLUT = vtkWindowLevelLookupTable::New();
    double fusRange [2];
    fusRange[0] = mFusionLevel - mFusionWindow/2;
    fusRange[1] = mFusionLevel + mFusionWindow/2;

    //check whether it is actually a fusionSequence or a fusion, before invoking mFusionReader...
    double* frange;
    if (this->IsInvolvedInFusionSequence()) 
      frange = mFusionSequenceReader->GetOutput()->GetVTKImages()[0]->GetScalarRange();
    else
      frange = mFusionReader->GetOutput()->GetVTKImages()[0]->GetScalarRange();

    fusLUT->SetTableRange(frange);
    fusLUT->SetValueRange(1,1);
    fusLUT->SetSaturationRange(1,1);
    fusLUT->SetAlphaRange(1, 1);
    fusLUT->SetWindow(mFusionWindow);
    fusLUT->SetLevel(mFusionLevel);
    if (mFusionColorMap == 1)
      fusLUT->SetHueRange(0,0.18);
    else if (mFusionColorMap == 2)
      fusLUT->SetHueRange(0.4,0.80);
    else if (mFusionColorMap == 3)
    {
      fusLUT->SetHueRange(0.666, 0);
      fusLUT->SetValueRange(0.5, 1);
    }
    else if (mFusionColorMap == 4)
      fusLUT->SetHueRange(0,1);
    else if (mFusionColorMap <= 0)
    {
      fusLUT->SetValueRange(0,1);
      fusLUT->SetSaturationRange(0,0);
    }

    fusLUT->ForceBuild();
    double v[4];
    // set color table transparency
    //double alpha_range=(double)mFusionThresOpacity/10;
    double range_end = fusRange[0] + (double)mFusionThresOpacity*(fusRange[1] - fusRange[0])/100;
    double curr_value = fusRange[0];
    int nvalues = fusLUT->GetNumberOfTableValues();
    for (double i = 0; curr_value < range_end; i++) {  
      fusLUT->GetTableValue(i, v);
      v[3] = 0;
      //if (curr_value >= -alpha_range && curr_value <= alpha_range) v[3] = pow(fabs(curr_value/alpha_range),2);
      //else v[3] = 1;
      fusLUT->SetTableValue(i, v);
      curr_value += (fusRange[1] - fusRange[0])/nvalues;
    }
  }
  for ( unsigned int i = 0; i < mSlicers.size(); i++) {

    if (mSlicers[i]->GetOverlay()) {
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
      mSlicers[i]->GetWindowLevel()->SetLookupTable(supLUT);
      mSlicers[i]->GetOverlayMapper()->SetLookupTable(invLUT);
      invLUT->Delete();
      supLUT->Delete();
    } else if (mSlicers[i]->GetOverlay()) {
      mSlicers[i]->GetWindowLevel()->SetLookupTable(LUT);
    } else {
      mSlicers[i]->GetWindowLevel()->SetLookupTable(LUT);
    }

    if (mSlicers[i]->GetFusion()) {
      mSlicers[i]->ShowFusionLegend(mFusionShowLegend);
      mSlicers[i]->GetFusionMapper()->SetLookupTable(fusLUT);
      mSlicers[i]->GetFusionActor()->SetOpacity(double(mFusionOpacity)/100);
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
void vvSlicerManager::AddNewLandmark(float x,float y,float z,float t)
{ 
    emit callAddLandmark(x,y,z,t);
}

//----------------------------------------------------------------------------
void vvSlicerManager::AddLandmark(float x,float y,float z,float t)
{ 
  double x_index = (x - mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetOrigin()[0])/mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetSpacing()[0];
  double y_index = (y - mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetOrigin()[1])/mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetSpacing()[1];
  double z_index = (z - mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetOrigin()[2])/mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetSpacing()[2];
#if VTK_MAJOR_VERSION <= 5
    if (x_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[0]-0.5 &&
        x_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[1]+0.5 &&
        y_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[2]-0.5 &&
        y_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[3]+0.5 &&
        z_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[4]-0.5 &&
        z_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[5]+0.5)
#else
    if (x_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[0]-0.5 &&
        x_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[1]+0.5 &&
        y_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[2]-0.5 &&
        y_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[3]+0.5 &&
        z_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[4]-0.5 &&
        z_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[5]+0.5)
#endif
  {
    double value = this->GetScalarComponentAsDouble(mSlicers[mSelectedSlicer]->GetImage()->GetVTKImages()[mSlicers[mSelectedSlicer]->GetTSlice()], x_index, y_index, z_index); //Value in selected Slicer (not 0): bug #2848
    this->GetLandmarks()->AddLandmark(x,y,z,t,value);
    emit LandmarkAdded();
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::AddLandmarkProfile(float x,float y,float z,float t)
{ 
  double x_index = (x - mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetOrigin()[0])/mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetSpacing()[0];
  double y_index = (y - mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetOrigin()[1])/mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetSpacing()[1];
  double z_index = (z - mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetOrigin()[2])/mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetSpacing()[2];
#if VTK_MAJOR_VERSION <= 5
    if (x_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[0]-0.5 &&
        x_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[1]+0.5 &&
        y_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[2]-0.5 &&
        y_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[3]+0.5 &&
        z_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[4]-0.5 &&
        z_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetWholeExtent()[5]+0.5)
#else
    if (x_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[0]-0.5 &&
        x_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[1]+0.5 &&
        y_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[2]-0.5 &&
        y_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[3]+0.5 &&
        z_index >= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[4]-0.5 &&
        z_index <= mSlicers[0]->GetImage()->GetVTKImages()[mSlicers[0]->GetTSlice()]->GetInformation()->Get(vtkDataObject::DATA_EXTENT())[5]+0.5)
#endif
  {
    double value = this->GetScalarComponentAsDouble(mSlicers[mSelectedSlicer]->GetImage()->GetVTKImages()[mSlicers[mSelectedSlicer]->GetTSlice()], x_index, y_index, z_index); //Value in selected Slicer (not 0): bug #2848
    this->GetLandmarks()->AddLandmark(x,y,z,t,value);
  }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void vvSlicerManager::UpdateLandmark()
{
    this->GetLandmarks()->TransformUpdate(mSlicers[mSelectedSlicer]->GetConcatenatedTransform()->GetInverse());
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
