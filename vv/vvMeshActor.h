/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)
 Program:   vv

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
#ifndef vvMeshActor_h
#define vvMeshActor_h

#include "vvMesh.h"
#include "vvImage.h"

class vtkActor;
class vtkPolyDataMapper;
class vtkMarchingSquares;
class vtkImageData;

/** An actor that takes a vvMesh as input, and displays it
 ** sliced along the given slice plane */
class vvMeshActor
{
public:
    vvMeshActor();
    ///Sets slicing orientation
    void SetSlicingOrientation(unsigned int d);
    ///Changes the cut plane value in real world coordinates
    void SetCutSlice(double slice);
    /**Initialize the actor and set the inputs. If vf is not null, will use
     **it to propagate the contour on all time frames */ 
    void Init(vvMesh::Pointer mesh,int time_slice, vvImage::Pointer vf=NULL);
    ///Set the time slice (only useful when using a 4D contour)
    void SetTimeSlice(int time);
    ///Toggles between normal 4D mode and superposition mode
    void ToggleSuperposition();
    vtkActor* GetActor() {return mActor;}
    ~vvMeshActor();

protected:
    ///0 for x, 1 for y, 2 for z
    unsigned int mCutDimension;
    vvMesh::Pointer mMesh;
    vtkMarchingSquares * mMarching;
    vtkPolyDataMapper* mMapper;
    vtkActor* mActor;
    double mCutPlaneValue;
    bool mSuperpostionMode;
    int mTimeSlice;
    double mCutSlice;
};

#endif
