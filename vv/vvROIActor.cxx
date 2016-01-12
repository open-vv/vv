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
#include "vvROIActor.h"
#include "vvSlicerManager.h"

// vtk
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//------------------------------------------------------------------------------
vvROIActor::vvROIActor()
{ 
  mIsVisible = true;
  mIsContourVisible = false;
  mOpacity = 0.5;
  mIsSelected = false;
  mContourWidth = 1;
  mContourColor.resize(3);
  m_modeBG = true;
  mDepth = 1.0;
  mROI = 0;
  mSlicerManager = 0;
  mContourColor[0] = 1;
  mContourColor[1] = 1;
  mContourColor[2] = 1;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvROIActor::~vvROIActor()
{ 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::RemoveActors()
{ 
  for(unsigned int i= 0; i<mOverlayActors.size(); i++) {
    mOverlayActors[i]->RemoveActors();
  }

  for(unsigned int i= 0; i<mImageContour.size(); i++) {
    mImageContour[i]->RemoveActors();
  }  
  
  Update(true);
  //mImageContour.clear();
  //mOverlayActors.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetROI(clitk::DicomRT_ROI * s)
{ 
  mROI = s;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetContourWidth(int n) 
{ 
  mContourWidth = n;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetSlicerManager(vvSlicerManager * s) 
{ 
  mSlicerManager = s;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::UpdateImage()
{ 
  mOverlayActors.clear();
  mImageContour.clear();
  Initialize(mDepth, mIsVisible);
  Update(); // No Render
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetVisible(bool b)
{ 
  mIsVisible = b;
  if (!b) { // remove actor
    for(unsigned int i= 0; i<mOverlayActors.size(); i++)
      mOverlayActors[i]->HideActors();
  } else {
    for(unsigned int i= 0; i<mOverlayActors.size(); i++)
      mOverlayActors[i]->ShowActors();
  }
  Update(); // No Render
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetContourVisible(bool b) 
{ 
  mIsContourVisible = b;
  if (!b) { // remove actor
    for(unsigned int i= 0; i<mImageContour.size(); i++) {
      mImageContour[i]->HideActors();
    }
  }
  else {
    for(unsigned int i= 0; i<mImageContour.size(); i++) {
      mImageContour[i]->ShowActors();
    }
  }
  Update(); // No Render
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvROIActor::IsVisible() 
{ 
  return mIsVisible;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
bool vvROIActor::IsContourVisible() 
{ 
  return mIsContourVisible;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::Initialize(double depth, bool IsVisible) 
{ 
  if (mROI->GetImage()) {
    mImageContour.clear();
    mOverlayActors.clear();
    mDepth = depth;
    for(int i=0; i<mSlicerManager->GetNumberOfSlicers(); i++) {
      mImageContour.push_back(vvImageContour::New());
      mImageContour[i]->SetSlicer(mSlicerManager->GetSlicer(i));
      mImageContour[i]->SetImage(mROI->GetImage());
      // Color of the contour is same of roi color
      mContourColor[0] = mROI->GetDisplayColor()[0];
      mContourColor[1] = mROI->GetDisplayColor()[1];
      mContourColor[2] = mROI->GetDisplayColor()[2];
      mImageContour[i]->SetColor(mContourColor[0], mContourColor[1], mContourColor[2]);
      mImageContour[i]->SetLineWidth(mContourWidth);
      mImageContour[i]->SetPreserveMemoryModeEnabled(true);
      mImageContour[i]->SetDepth(mDepth+0.5);
      mImageContour[i]->ShowActors();
      
      mOverlayActors.push_back(vvBinaryImageOverlayActor::New());

      // BG or FG
      if (m_modeBG) {
	mOverlayActors[i]->SetImage(mROI->GetImage(), mROI->GetBackgroundValueLabelImage());
      }
      else {
	mOverlayActors[i]->SetImage(mROI->GetImage(), mROI->GetForegroundValueLabelImage(), false);
      }


      mOverlayActors[i]->SetColor(mROI->GetDisplayColor()[0],
                                  mROI->GetDisplayColor()[1],
                                  mROI->GetDisplayColor()[2]);
      mOverlayActors[i]->SetOpacity(mOpacity);
      mOverlayActors[i]->SetSlicer(mSlicerManager->GetSlicer(i));
      mOverlayActors[i]->Initialize(IsVisible);      
      mOverlayActors[i]->SetDepth(mDepth);
    }
    connect(mSlicerManager,SIGNAL(UpdateSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
    //connect(mSlicerManager,SIGNAL(UpdateTSlice(int,int)),this,SLOT(UpdateSlice(int, int)));
    connect(mSlicerManager, SIGNAL(AVerticalSliderHasChanged(int, int)), SLOT(UpdateSlice(int, int)));
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetDepth(double d)
{ 
  mDepth = d;
  if (!mSlicerManager) return;
  for(int i=0; i<mSlicerManager->GetNumberOfSlicers(); i++) {  
    mOverlayActors[i]->SetDepth(d);
    mImageContour[i]->SetDepth(d+0.5);
  }
  Update(true);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::Update(bool force)
{ 
  if (!mSlicerManager) return;
  for(int i=0; i<mSlicerManager->GetNumberOfSlicers(); i++) {
    UpdateSlice(i, mSlicerManager->GetSlicer(i)->GetSlice(), force);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::UpdateSlice(int slicer, int slices, bool force)
{ 
  if (!mROI->GetImage())  return;
  if ((!mIsVisible) && (!mIsContourVisible)) return; 
  if (!mSlicerManager) {
    std::cerr << "Error. No mSlicerManager ?" << std::endl;
    exit(0);
  }

  if (mIsContourVisible) {
    mImageContour[slicer]->Update(mROI->GetBackgroundValueLabelImage()+1); 
  }

  // Refresh overlays
  mOverlayActors[slicer]->UpdateSlice(slicer, slices, force);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetOpacity(double d)
{ 
  mOpacity = d;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetContourColor(double r, double v, double b) 
{ 
  mContourColor[0] = r;
  mContourColor[1] = v;
  mContourColor[2] = b;  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetOverlayColor(double r, double v, double b) 
{ 
  if (mROI)
    mROI->SetDisplayColor(r,v,b);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::vector<double> & vvROIActor::GetContourColor() 
{ 
  return mContourColor; 
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::vector<double> & vvROIActor::GetOverlayColor() 
{ 
  return mROI->GetDisplayColor();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::UpdateColor() 
{ 
  for(unsigned int i=0; i<mOverlayActors.size(); i++) {
    mOverlayActors[i]->SetOpacity(mOpacity);
    mOverlayActors[i]->SetColor(mROI->GetDisplayColor()[0],
                                mROI->GetDisplayColor()[1],
                                mROI->GetDisplayColor()[2]);
    mOverlayActors[i]->UpdateColor();
  }
  for(unsigned int i=0; i<mImageContour.size(); i++) {
    mImageContour[i]->SetLineWidth(mContourWidth);
    mImageContour[i]->SetColor(mContourColor[0], mContourColor[1], mContourColor[2]);
    if (mIsContourVisible)
      mImageContour[i]->Update(mROI->GetBackgroundValueLabelImage()+1);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
double vvROIActor::GetOpacity()
{ 
  return mOpacity;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::SetSelected(bool b)
{ 
  mIsSelected = b;
  if (b) {
    for(int i=0; i<mSlicerManager->GetNumberOfSlicers(); i++) {
      mImageContour[i]->SetLineWidth(3.0);
      mImageContour[i]->ShowActors();
    }
  } else {
    for(int i=0; i<mSlicerManager->GetNumberOfSlicers(); i++) {
      mImageContour[i]->HideActors();
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvROIActor::CopyParameters(QSharedPointer<vvROIActor> roi)
{ 
  // Overlay
  SetVisible(roi->IsVisible());
  SetOpacity(roi->GetOpacity());
  SetOverlayColor(roi->GetOverlayColor()[0], roi->GetOverlayColor()[1], roi->GetOverlayColor()[2]);

  // Contour
  SetContourVisible(roi->IsContourVisible());
  SetContourWidth(roi->GetContourWidth());
  SetContourColor(roi->GetContourColor()[0], roi->GetContourColor()[1], roi->GetContourColor()[2]);
  
  // Global
  SetDepth(roi->GetDepth());
}
//------------------------------------------------------------------------------
