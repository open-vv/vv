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
#include "vvMeshActor.h"
#include "clitkCommon.h"
#include <vtkVersion.h>
#include <vtkPolyDataMapper.h>
#include <vtkMarchingSquares.h>
#include <vtkImageData.h>
#include <vtkPlane.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <cassert>

vvMeshActor::vvMeshActor() :
  mCutDimension(0), mMesh(NULL),
  mMarching(NULL), mMapper(NULL),
  mActor(NULL),
  mSuperpostionMode(false), mTimeSlice(0)
{}

vvMeshActor::~vvMeshActor()
{
  mMarching->Delete();
  mMapper->Delete();
  mActor->Delete();
}

void vvMeshActor::Init(vvMesh::Pointer mesh,int time_slice,vvImage::Pointer vf)
{
  mMesh=mesh;

  mMarching=vtkMarchingSquares::New();
  mTimeSlice=time_slice;
  if (static_cast<unsigned int>(time_slice)<mMesh->GetNumberOfMeshes()) {
#if VTK_MAJOR_VERSION <= 5
    mMarching->SetInput(mMesh->GetMask(time_slice));
#else
    mMarching->SetInputData(mMesh->GetMask(time_slice));
#endif
  } else {
#if VTK_MAJOR_VERSION <= 5
    mMarching->SetInput(mMesh->GetMask(0));
#else
    mMarching->SetInputData(mMesh->GetMask(0));
#endif
  }
  mMarching->SetValue(0,0.5);
  //mMarching->Update();

  mMapper=vtkPolyDataMapper::New();
#if VTK_MAJOR_VERSION <= 5
  mMapper->SetInput(mMarching->GetOutput());
#else
  mMapper->SetInputData(mMarching->GetOutput());
#endif
  //The following line allows to display the contour over the image
  //(http://www.nabble.com/What-happens-when-two-actors-are-at-the-same-depth--td23175458.html)
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
  mActor=vtkActor::New();
  mActor->SetMapper(mMapper);
  mActor->SetPickable(false);
  mActor->GetProperty()->EdgeVisibilityOn();
  mActor->GetProperty()->SetEdgeColor(mMesh->r,mMesh->g,mMesh->b);
  mActor->GetProperty()->SetLineWidth(2.);
}

void vvMeshActor::SetCutSlice(double slice)
{
  //DD("SetCutSlice");
  //DD(slice);
  mCutSlice=slice;
  vtkImageData* mask;
  if (static_cast<unsigned int>(mTimeSlice)<mMesh->GetNumberOfMasks())
    mask=mMesh->GetMask(mTimeSlice);
  else
    mask=mMesh->GetMask(0);
  int* dims=mask->GetDimensions();
  int mask_slice=(slice-mask->GetOrigin()[mCutDimension])/mask->GetSpacing()[mCutDimension];
  switch (mCutDimension) {
  case 0:
    mMarching->SetImageRange(mask_slice,mask_slice,0,dims[1],0,dims[2]);
    break;
  case 1:
    mMarching->SetImageRange(0,dims[0],mask_slice,mask_slice,0,dims[2]);
    break;
  case 2:
    mMarching->SetImageRange(0,dims[0],0,dims[1],mask_slice,mask_slice);
    break;
  default:
    assert(false);
  }
  mMarching->Update();
}

void vvMeshActor::SetTimeSlice(int time)
{
  mTimeSlice=time;
  if (static_cast<unsigned int>(time)<mMesh->GetNumberOfMasks()) {
#if VTK_MAJOR_VERSION <= 5
    mMarching->SetInput(mMesh->GetMask(time));
#else
    mMarching->SetInputData(mMesh->GetMask(time));
#endif
  } else {
#if VTK_MAJOR_VERSION <= 5
    mMarching->SetInput(mMesh->GetMask(0));
#else
    mMarching->SetInputData(mMesh->GetMask(0));
#endif
 }
  SetCutSlice(mCutSlice); //We need to find the new mask cut slice,
  //since masks do not all have the same origin
}

void vvMeshActor::SetSlicingOrientation(unsigned int d)
{
  mCutDimension=d;
}

void vvMeshActor::ToggleSuperposition()
{
  DD("Warning: superposition not implemented");
  // std::cout << "vvMeshActor::ToggleSuperposition size = " << mMeshes.size() << std::endl;
  if (! mSuperpostionMode && mMesh->GetNumberOfMeshes() > 1) {
    mSuperpostionMode=true;
  } else {
    mSuperpostionMode=false;
  }
}
