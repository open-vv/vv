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
#ifndef vvMesh_h
#define vvMesh_h
#include <string>
#include <utility>
#include <vector>

#include <itkObjectFactory.h>

#include "vvImage.h"

class vtkPolyData;
class vtkImageData;

/** A vvMesh is assumed to be either a 3D closed surface 
 *  or a series of surfaces 
 *  It uses a dual representation: both binary mask and mesh**/
class vvMesh : public itk::LightObject
{
public:
    typedef vvMesh Self;
    typedef itk::SmartPointer<Self> Pointer;
    itkNewMacro(Self);

    void ReadFromVTK(const char * filename);
    std::string structure_name;
    ///Contour color, [0,1]
    double r,g,b;

    vtkPolyData* GetMesh(unsigned int i) const {return meshes[i];}
    void AddMesh(vtkPolyData* p);
    ///Removes all meshes in the object
    void RemoveMeshes();
    unsigned int GetNumberOfMeshes() { return (unsigned int)meshes.size(); }

    vtkImageData* GetMask(unsigned int i) const {return masks[i];}
    void AddMask(vtkImageData* im);
    void RemoveMasks();
    unsigned int GetNumberOfMasks() { return (unsigned int)masks.size(); }

    ///Pretty-print information about the mesh
    void Print() const;
    ///Copies the meta-informations from another mesh
    void CopyInformation(vvMesh::Pointer input);
    void SetSpacing(double spacing) {slice_spacing=spacing;}
    double GetSpacing() {return slice_spacing;}
    ///Recompute the meshes from the masks
    void ComputeMeshes();
    /**Recompute the masks from the meshes.
     * extrude means that the mesh must be extruded before binarizing,
     * which is useful when creating a mesh from a stack of slices */
    void ComputeMasks(vtkImageData* sample,bool extrude=false);
    ///Create a new vvMesh by propagating the mesh with a displacement VF
    void propagateContour(vvImage::Pointer vf);
protected:
    ///The spacing between the planar contour, assumed to be constant
    double slice_spacing;
    std::vector<vtkPolyData *> meshes;
    std::vector<vtkImageData *> masks;

    vvMesh();
    ~vvMesh();
};

///Propagate a contour using a vector of motion fields, returns a 4D contour

#endif
