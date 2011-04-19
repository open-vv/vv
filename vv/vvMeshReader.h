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
