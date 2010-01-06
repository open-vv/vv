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
#ifndef vvMeshReader_h
#define vvMeshReader_h

#include <QThread>

#include <string>

#include "vvImage.h"
#include "vvMesh.h"

class vvMeshReader : public QThread
{
public:
    ///Returns the contour names present in a dc struct file
    std::vector<std::pair<int,std::string> > GetROINames();

    vvMeshReader();
    void SetFilename(const std::string f) { filename=f; }
    void SetModeToVTK() {vtk_mode=true;}
    void SetSelectedItems(const std::vector<int> & items) {selected_contours=items;}
    void SetImage(vvImage::Pointer im) {image=im;}
    void SetPropagationVF(vvImage::Pointer vf) {this->vf=vf;}
    std::vector<vvMesh::Pointer> GetOutput() {return result;}
    ///Called from the main thread, runs the reader and displays the progress bar
    void Update();

protected:
    void run();
    std::string filename;
    ///Indicates if the reader should expect a vtk polydata file instead of a dicom-struct
    bool vtk_mode;
    ///The list of indexes of contours the reader should read
    std::vector<int> selected_contours;
    ///Image the mesh will be displayed over, for binarization
    vvImage::Pointer image;
    std::vector<vvMesh::Pointer> result;

    ///Read a DC-struct file and return an extruded version of the contours
    std::vector<vvMesh::Pointer> readSelectedContours();
    ///Vector field used to propagate the contour
    vvImage::Pointer vf;
    ///Binarize the output of readSelectedContours() and mesh it with a Marching Cubes
    void Remesh(vvMesh::Pointer roi);
};

#endif
