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

//std 
#include <sstream>
#include <cassert>
#include <vector>

// clitk
#include "clitkCommon.h"

// vv
#include "vvMesh.h"

// itk
#include <itksys/SystemTools.hxx>

// vtk
#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkOBJReader.h>
#include <vtkImageData.h>
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkMarchingCubes.h>
#include <vtkMetaImageWriter.h>

vvMesh::vvMesh() :
  r(1),g(0),b(0),
  slice_spacing(-1)
{}

void vvMesh::AddMesh(vtkPolyData* p)
{
  vtkPolyData * mesh=vtkPolyData::New();
  mesh->ShallowCopy(p);
  meshes.push_back(mesh);
}

void vvMesh::ReadFromVTK(const char * filename)
{
  std::string extension=itksys::SystemTools::GetFilenameLastExtension(std::string(filename));
  if (extension == ".vtk" || extension== ".VTK") {
    assert(GetNumberOfMeshes() == 0); ///We assume the object is empty
    vtkSmartPointer<vtkPolyDataReader> r=vtkSmartPointer<vtkPolyDataReader>::New();
    r->SetFileName(filename);
    r->Update();
    assert(r->GetOutput());
    AddMesh(r->GetOutput());
  } else if (extension == ".obj" || extension== ".OBJ") {
    assert(GetNumberOfMeshes() == 0); ///We assume the object is empty
    vtkSmartPointer<vtkOBJReader> r=vtkSmartPointer<vtkOBJReader>::New();
    r->SetFileName(filename);
    r->Update();
    assert(r->GetOutput());
    AddMesh(r->GetOutput());
  } else
    assert (false) ; //shouldn't happen

  assert(GetNumberOfMeshes() != 0); ///We assume the object is empty
  structure_name=filename;
}

void vvMesh::RemoveMeshes()
{
  for (std::vector<vtkPolyData*>::const_iterator i=meshes.begin(); i!=meshes.end(); i++)
    (*i)->Delete();
  meshes.erase(meshes.begin(),meshes.end());
}

void vvMesh::AddMask(vtkImageData* im)
{
  assert(im->GetScalarType() == VTK_UNSIGNED_CHAR);
  vtkImageData* image=vtkImageData::New();
  image->ShallowCopy(im);
  masks.push_back(image);
}

void vvMesh::RemoveMasks()
{
  for (std::vector<vtkImageData*>::const_iterator i=masks.begin(); i!=masks.end(); i++)
    (*i)->Delete();
  masks.erase(masks.begin(),masks.end());
}

vvMesh::~vvMesh()
{
  RemoveMeshes();
  RemoveMasks();
}

void vvMesh::CopyInformation(vvMesh::Pointer input)
{
  r=input->r;
  g=input->g;
  b=input->b;
  structure_name=input->structure_name;
  slice_spacing=input->slice_spacing;
}

void vvMesh::Print() const
{
  std::cout << this << " : " << structure_name << std::endl << "RGB: " << r << "," << g << "," << b << std::endl;
  for (std::vector<vtkPolyData*>::const_iterator i=meshes.begin(); i!=meshes.end(); i++) {
    std::cout << (*i)->GetNumberOfPoints() << " points, " << (*i)->GetNumberOfCells() << " cells." << std::endl;
    DDV((*i)->GetBounds(),6);
  }
  std::cout << "-------------------------" << std::endl << std::endl;
}

void vvMesh::ComputeMasks(vtkImageData* sample,bool extrude)
{
  this->RemoveMasks();
  for (std::vector<vtkPolyData*>::iterator i=meshes.begin(); i!=meshes.end(); i++) {
    vtkPolyData* mesh=*i;
    double *bounds=mesh->GetBounds();

    vtkSmartPointer<vtkImageData> binary_image=vtkSmartPointer<vtkImageData>::New();
#if VTK_MAJOR_VERSION <= 5
    binary_image->SetScalarTypeToUnsignedChar();
#endif
    ///Use the smallest mask in which the mesh fits
    // Add two voxels on each side to make sure the mesh fits
    double * samp_origin=sample->GetOrigin();
    double * spacing=sample->GetSpacing();
    binary_image->SetSpacing(spacing);

    /// Put the origin on a voxel to avoid small skips
    binary_image->SetOrigin(floor((bounds[0]-samp_origin[0])/spacing[0]-2)*spacing[0]+samp_origin[0],
                            floor((bounds[2]-samp_origin[1])/spacing[1]-2)*spacing[1]+samp_origin[1],
                            floor((bounds[4]-samp_origin[2])/spacing[2]-2)*spacing[2]+samp_origin[2]);
    double * origin=binary_image->GetOrigin();
    binary_image->SetExtent(0,ceil((bounds[1]-origin[0])/spacing[0]+4),
                            0,ceil((bounds[3]-origin[1])/spacing[1]+4),
                            0,ceil((bounds[5]-origin[2])/spacing[2])+4);
#if VTK_MAJOR_VERSION <= 5
    binary_image->AllocateScalars();
#else
    binary_image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
#endif
    memset(binary_image->GetScalarPointer(),0,binary_image->GetDimensions()[0]*binary_image->GetDimensions()[1]*binary_image->GetDimensions()[2]*sizeof(unsigned char));


    vtkSmartPointer<vtkPolyDataToImageStencil> sts=vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    //The following line is extremely important
    //http://www.nabble.com/Bug-in-vtkPolyDataToImageStencil--td23368312.html#a23370933
    sts->SetTolerance(0);
    sts->SetInformationInput(binary_image);

    if (extrude) {
      vtkSmartPointer<vtkLinearExtrusionFilter> extrude=vtkSmartPointer<vtkLinearExtrusionFilter>::New();
#if VTK_MAJOR_VERSION <= 5
      extrude->SetInput(mesh);
#else
      extrude->SetInputData(mesh);
#endif
      ///We extrude in the -slice_spacing direction to respect the FOCAL convention
      extrude->SetVector(0, 0, -slice_spacing);
#if VTK_MAJOR_VERSION <= 5
      sts->SetInput(extrude->GetOutput());
#else
      sts->SetInputConnection(extrude->GetOutputPort());
#endif
    } else {
#if VTK_MAJOR_VERSION <= 5
      sts->SetInput(mesh);
#else
      sts->SetInputData(mesh);
#endif
   }

    vtkSmartPointer<vtkImageStencil> stencil=vtkSmartPointer<vtkImageStencil>::New();
#if VTK_MAJOR_VERSION <= 5
    stencil->SetStencil(sts->GetOutput());
    stencil->SetInput(binary_image);
#else
    stencil->SetStencilConnection(sts->GetOutputPort());
    stencil->SetInputData(binary_image);
#endif
    stencil->Update();
    this->AddMask(stencil->GetOutput());

    /*
      vtkSmartPointer<vtkMetaImageWriter> w = vtkSmartPointer<vtkMetaImageWriter>::New();
      w->SetInput(stencil->GetOutput());
      w->SetFileName("binary.mhd");
      w->Write();
    */
  }
}

void vvMesh::ComputeMeshes()
{
  this->RemoveMeshes();
  for (std::vector<vtkImageData*>::iterator i=masks.begin(); i!=masks.end(); i++) {
    vtkSmartPointer<vtkMarchingCubes> marching = vtkSmartPointer<vtkMarchingCubes>::New();
#if VTK_MAJOR_VERSION <= 5
    marching->SetInput(*i);
#else
    marching->SetInputData(*i);
#endif
    marching->SetValue(0,0.5);
    marching->Update();
    this->AddMesh(marching->GetOutput());
  }
}

void vvMesh::propagateContour(vvImage::Pointer vf)
{
  assert(this->GetNumberOfMeshes() == 1);
  std::vector<vtkImageData*> sgrids=vf->GetVTKImages();
  vtkSmartPointer<vtkPolyData> reference_mesh = vtkSmartPointer<vtkPolyData>::New();
  reference_mesh->ShallowCopy(this->GetMesh(0));
  this->RemoveMeshes();

  for (std::vector<vtkImageData*>::iterator i=sgrids.begin();
       i!=sgrids.end(); i++) {
    vtkPolyData* new_mesh=vtkPolyData::New();
    new_mesh->DeepCopy(reference_mesh);
    double Ox=vf->GetOrigin()[0];
    double Oy=vf->GetOrigin()[1];
    double Oz=vf->GetOrigin()[2];
    double Sx=vf->GetSpacing()[0];
    double Sy=vf->GetSpacing()[1];
    double Sz=vf->GetSpacing()[2];
    int *dims=vf->GetVTKImages()[0]->GetDimensions();
    assert((*i)->GetScalarType() == VTK_FLOAT); //vfs are assumed to be of float type
    assert((*i)->GetNumberOfScalarComponents() == 3);
    float * vector_data=reinterpret_cast<float*>((*i)->GetScalarPointer());
    for (int j=0; j<new_mesh->GetNumberOfPoints(); j++) {
      double* old=new_mesh->GetPoint(j);
      int ix=(old[0]-Ox)/Sx;
      int iy=(old[1]-Oy)/Sy;
      int iz=(old[2]-Oz)/Sz;
      float* vector=vector_data+(ix+iy*vf->GetSize()[0]+iz*vf->GetSize()[0]*vf->GetSize()[1])*3;
      if (ix>=0 && ix < dims[0]
          && iy>=0 && iy < dims[1]
          && iz>=0 && iz < dims[2])
        new_mesh->GetPoints()->SetPoint(j,old[0]+vector[0],old[1]+vector[1],old[2]+vector[2]);
    }
    this->AddMesh(new_mesh);
  }
  if (GetNumberOfMasks()) { //If the input mesh has a mask, use it to compute the warped meshes' masks
    vtkSmartPointer<vtkImageData> ref_mask = vtkSmartPointer<vtkImageData>::New();
    ref_mask->ShallowCopy(GetMask(0));
    this->ComputeMasks(ref_mask);
  }
}
