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

#include "vvBinaryImageOverlayActor.h"
#include "vvImage.h"
#include <vtkVersion.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageMapToRGBA.h>
#include <vtkLookupTable.h>
#include <vtkImageMapper3D.h>

//------------------------------------------------------------------------------
vvBinaryImageOverlayActor::vvBinaryImageOverlayActor()
{ cout << __func__ << endl;
  mTSlice = -1;
  mSlice = 0;
  mColor.resize(3);
  mAlpha = 0.6;
  mImage = 0;
  mSlicer = 0;
  mColorLUT = vtkSmartPointer<vtkLookupTable>::New();
  mForegroundValue = 1;
  mBackgroundValue = 0;
  m_modeBG = true;
  mDepth = 1.0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvBinaryImageOverlayActor::~vvBinaryImageOverlayActor()
{ cout << __func__ << endl;
  mImageActorList.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::RemoveActors()
{ cout << __func__ << endl;
  for (unsigned int i = 0; i < mImageActorList.size(); i++) {
    if (mSlicer != 0) {
      if (mSlicer != NULL) {
        if (mSlicer->GetRenderer() != 0) {
          if (mImageActorList[i] != 0)  {
            mSlicer->GetRenderer()->RemoveActor(mImageActorList[i]);
          }
        }
      }
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::SetColor(double r, double g, double b)
{ cout << __func__ << endl;
  mColor[0] = r;
  mColor[1] = g;
  mColor[2] = b;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::SetSlicer(vvSlicer * slicer)
{ cout << __func__ << endl;
  mSlicer = slicer;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::Initialize(bool IsVisible)
{ cout << __func__ << endl;
  if (!mSlicer) {
    std::cerr << "ERROR. Please use setSlicer before setSlicer in vvBinaryImageOverlayActor." << std::endl;
    exit(0);
  }

  if (!mImage) {
    std::cerr << "ERROR. Please use setImage before setSlicer in vvBinaryImageOverlayActor." << std::endl;
    exit(0);
  }
  // Create an actor for each time slice
  for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
    // how many intensity ?
    vtkSmartPointer<vtkImageMapToRGBA> mOverlayMapper = vtkSmartPointer<vtkImageMapToRGBA>::New();
#if VTK_MAJOR_VERSION <= 5
    mOverlayMapper->SetInput(mImage->GetVTKImages()[0]); // DS TODO : to change if it is 4D !!!
#else
    mOverlayMapper->SetInputData(mImage->GetVTKImages()[0]); // DS TODO : to change if it is 4D !!!
#endif

    double range[2];
    mImage->GetVTKImages()[0]->GetScalarRange(range);
    int n = range[1]-range[0]+1;
    mColorLUT->SetRange(range[0],range[1]);
    mColorLUT->SetNumberOfTableValues(n);

    // Mode BG -> all is color except BG
    if (m_modeBG) {
      for(int i=0; i<n; i++) {
	mColorLUT->SetTableValue(i, mColor[0], mColor[1], mColor[2], mAlpha);
      }
      mColorLUT->SetTableValue(mBackgroundValue, 0, 0, 0, 0.0);
    }
    else {
      // Mode FG -> all is BG, except FG which is color
      for(int i=0; i<n; i++) {
	mColorLUT->SetTableValue(i, 0, 0, 0, 0.0);
      }
      mColorLUT->SetTableValue(mForegroundValue, mColor[0], mColor[1], mColor[2], mAlpha);
    }

    mOverlayMapper->SetLookupTable(mColorLUT);

    vtkSmartPointer<vtkImageActor> mOverlayActor = vtkSmartPointer<vtkImageActor>::New();
#if VTK_MAJOR_VERSION <= 5
    mOverlayActor->SetInput(mOverlayMapper->GetOutput());
#else
    mOverlayActor->GetMapper()->SetInputConnection(mOverlayMapper->GetOutputPort());
#endif
    mOverlayActor->SetPickable(0);
    mOverlayActor->SetVisibility(IsVisible);
    //mOverlayActor->SetOpacity(1.0);	

    // FIXME : by default overlay is not interpolated.
    // mOverlayActor->SetInterpolate(mSlicer->GetImageActor()->GetInterpolate());
    mOverlayActor->InterpolateOff();

    mMapperList.push_back(mOverlayMapper);
    mImageActorList.push_back(mOverlayActor);
    mSlicer->GetRenderer()->AddActor(mOverlayActor);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::SetOpacity(double d)
{ cout << __func__ << endl;
  mAlpha = d;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvBinaryImageOverlayActor::UpdateOpacity(double d) {
//   mAlpha = d;
//   mColorLUT->SetTableValue(1, mColor[0], mColor[1], mColor[2], mAlpha); // FG
//   for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
//     // how many intensity ?
//     vtkImageMapToRGBA * mOverlayMapper = mMapperList[numImage];
//     mOverlayMapper->SetLookupTable(mColorLUT);

//     vtkImageActor * mOverlayActor = mImageActorList[numImage];
//     mOverlayActor->SetInput(mOverlayMapper->GetOutput());
//   }
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void vvBinaryImageOverlayActor::SetColor(double r, double v, double b) {
//   mColor[0] = r;
//   mColor[1] = v;
//   mColor[2] = b;
//   // mColorLUT->SetTableValue(1, mColor[0], mColor[1], mColor[2], mAlpha); // FG
//   // for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
//   //   // how many intensity ?
//   //   vtkImageMapToRGBA * mOverlayMapper = mMapperList[numImage];
//   //   mOverlayMapper->SetLookupTable(mColorLUT);

//   //   vtkImageActor * mOverlayActor = mImageActorList[numImage];
//   //   mOverlayActor->SetInput(mOverlayMapper->GetOutput());
//   // }
// }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::SetImage(vvImage::Pointer image, double bg, bool modeBG)
{ cout << __func__ << endl;
  mImage = image;
  if (modeBG) {
    mBackgroundValue = bg;
    m_modeBG = true;
  }
  else {
    mForegroundValue = bg;
    m_modeBG = false;
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::HideActors()
{ cout << __func__ << endl;
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  for(unsigned int i=0; i<mImageActorList.size(); i++) {
    mImageActorList[i]->VisibilityOff();
  }
  // Caller MUST call Render
  //  mSlicer->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::ShowActors()
{ cout << __func__ << endl;
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  mTSlice = mSlicer->GetTSlice();
  //  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
  mImageActorList[mTSlice]->VisibilityOn();
  UpdateSlice(0, mSlice);
  //}
  // Caller MUST call Render
  //mSlicer->Render();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::UpdateColor()
{ cout << __func__ << endl;
  mColorLUT->SetTableValue(1, mColor[0], mColor[1], mColor[2], mAlpha); // FG
  for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
    // how many intensity ?
    vtkImageMapToRGBA * mOverlayMapper = mMapperList[numImage];
    mOverlayMapper->SetLookupTable(mColorLUT);

    vtkImageActor * mOverlayActor = mImageActorList[numImage];
#if VTK_MAJOR_VERSION <= 5
    mOverlayActor->SetInput(mOverlayMapper->GetOutput());
#else
    mOverlayActor->SetInputData(mOverlayMapper->GetOutput());
#endif
  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::UpdateSlice(int slicer, int slice, bool force)
{ cout << __func__ << endl;
  if (!mSlicer) return;

  if (!force) {
    if (mPreviousSlice == mSlicer->GetSlice()) {
      if (mPreviousTSlice == mSlicer->GetTSlice()) {
        return; // Nothing to do
      }
    }
  }

  // Get current slice
  mSlice = mSlicer->GetSlice();
  mTSlice = mSlicer->GetTSlice();

  // Update extent
  int * imageExtent = mSlicer->GetExtent();
  int orientation = mSlicer->GetOrientation();
  int maskExtent[6];
  ComputeExtent(orientation, mSlice, imageExtent, maskExtent);
  ComputeExtent(maskExtent, maskExtent, mSlicer->GetImage()->GetFirstVTKImageData(), mImage->GetFirstVTKImageData());
#if VTK_MAJOR_VERSION <= 5
  mSlicer->ClipDisplayedExtent(maskExtent, mMapperList[mTSlice]->GetInput()->GetWholeExtent());
#else
  mSlicer->ClipDisplayedExtent(maskExtent, mMapperList[mTSlice]->GetInput()->GetInformation()->Get(vtkDataObject::DATA_EXTENT()));
#endif
mSlicer->Print(cout);
  SetDisplayExtentAndCameraPosition(orientation, mSlice, maskExtent, mImageActorList[mTSlice], mDepth);

  // set previous slice
  mPreviousTSlice = mSlicer->GetTSlice();
  mPreviousSlice  = mSlicer->GetSlice();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::ComputeExtent(int orientation,
					      int slice,
					      int * inExtent,
					      int * outExtent)
{ cout << __func__ << endl;
  switch (orientation) {
  case vtkImageViewer2::SLICE_ORIENTATION_XY:
    for(int i=0; i<4; i++) outExtent[i] = inExtent[i];
    outExtent[4] = outExtent[5] = slice;
    break;
  case vtkImageViewer2::SLICE_ORIENTATION_XZ:
    for(int i=0; i<2; i++) outExtent[i] = inExtent[i];
    for(int i=4; i<6; i++) outExtent[i] = inExtent[i];
    outExtent[2] = outExtent[3] = slice;
    break;
  case vtkImageViewer2::SLICE_ORIENTATION_YZ:
    for(int i=2; i<6; i++) outExtent[i] = inExtent[i];
    outExtent[0] = outExtent[1] = slice;
    break;
  }
}
//------------------------------------------------------------------------------


//----------------------------------------------------------------------------
void vvBinaryImageOverlayActor::ComputeExtent(int * inExtent, int * outExtent, vtkImageData * image, vtkImageData * overlay)
{ cout << __func__ << endl;
  for(int i=0; i<3; i++) {
    double a = (image->GetOrigin()[i] + inExtent[i*2]*image->GetSpacing()[i] - 
                overlay->GetOrigin()[i]) / overlay->GetSpacing()[i];
    double b = (image->GetOrigin()[i] + inExtent[i*2+1]*image->GetSpacing()[i] - 
                overlay->GetOrigin()[i]) / overlay->GetSpacing()[i];
    outExtent[i*2] = lrint(a);
    outExtent[i*2+1] = lrint(b);
  }

  /* // FIXME (original)
  outExtent[0] = (int)lrint(((image->GetOrigin()[0] + inExtent[0]*image->GetSpacing()[0]) - overlay->GetOrigin()[0]) / overlay->GetSpacing()[0]);
  outExtent[1] = (int)lrint(((image->GetOrigin()[0] + inExtent[1]*image->GetSpacing()[0]) - overlay->GetOrigin()[0]) / overlay->GetSpacing()[0]);
  outExtent[2] = (int)lrint(((image->GetOrigin()[1] + inExtent[2]*image->GetSpacing()[1]) - overlay->GetOrigin()[1]) / overlay->GetSpacing()[1]);
  outExtent[3] = (int)lrint(((image->GetOrigin()[1] + inExtent[3]*image->GetSpacing()[1]) - overlay->GetOrigin()[1]) / overlay->GetSpacing()[1]);
  outExtent[4] = (int)lrint(((image->GetOrigin()[2] + inExtent[4]*image->GetSpacing()[2]) - overlay->GetOrigin()[2]) / overlay->GetSpacing()[2]);
  outExtent[5] = (int)lrint(((image->GetOrigin()[2] + inExtent[5]*image->GetSpacing()[2]) - overlay->GetOrigin()[2]) / overlay->GetSpacing()[2]);
  */
}
//----------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvBinaryImageOverlayActor::SetDisplayExtentAndCameraPosition(int orientation,
								  int slice,
								  int * extent,
								  vtkImageActor * actor,
								  double position)
{ cout << __func__ << endl;
  /* FIXME
     Error according to camera orientation
   */

  // Set position
  if (orientation == vtkImageViewer2::SLICE_ORIENTATION_XY) {
    //if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[2] > slice)
      actor->SetPosition(0,0, -position);
      //else
      //actor->SetPosition(0,0, position);
  }
  if (orientation == vtkImageViewer2::SLICE_ORIENTATION_XZ) {
    //if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[1] > slice)
    // actor->SetPosition(0,position,0);
    //else
      actor->SetPosition(0,-position,0);
  }
  if (orientation == vtkImageViewer2::SLICE_ORIENTATION_YZ) {
    //if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[0] > slice)
    //  actor->SetPosition(position,0, 0);
    //else
      actor->SetPosition(-position,0, 0);
  }
  actor->SetDisplayExtent(extent);
}
//------------------------------------------------------------------------------

