/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)

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

#include "vvMeshActor.h"
#include "clitkCommon.h"
#include <vtkPolyDataMapper.h>
#include <vtkMarchingSquares.h>
#include <vtkImageData.h>
#include <vtkPlane.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <cassert>

vvMeshActor::vvMeshActor() :
    mCutDimension(NULL), mMesh(NULL),
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
    if (static_cast<unsigned int>(time_slice)<mMesh->GetNumberOfMeshes())
        mMarching->SetInput(mMesh->GetMask(time_slice));
    else
        mMarching->SetInput(mMesh->GetMask(0));
    mMarching->SetValue(0,0.5);
    //mMarching->Update();

    mMapper=vtkPolyDataMapper::New();
    mMapper->SetInput(mMarching->GetOutput());
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
    mCutSlice=slice;
    vtkImageData* mask;
    if (static_cast<unsigned int>(mTimeSlice)<mMesh->GetNumberOfMasks())
        mask=mMesh->GetMask(mTimeSlice);
    else
        mask=mMesh->GetMask(0);
    int* dims=mask->GetDimensions(); 
    int mask_slice=(slice-mask->GetOrigin()[mCutDimension])/mask->GetSpacing()[mCutDimension];
    switch (mCutDimension)
    {
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
    if (static_cast<unsigned int>(time)<mMesh->GetNumberOfMasks())
        mMarching->SetInput(mMesh->GetMask(time));
    else
        mMarching->SetInput(mMesh->GetMask(0));
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
    if (! mSuperpostionMode && mMesh->GetNumberOfMeshes() > 1)
    {
        mSuperpostionMode=true;
    }
    else
    {
        mSuperpostionMode=false;
    }
}
