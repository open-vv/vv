/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX

  Authors belongs to:
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://www.centreleonberard.fr
  - CREATIS CNRS laboratory      http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence
  - BSD       http://www.opensource.org/licenses/bsd-license.php
  - CeCILL-B  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html

  =========================================================================*/

#include <iterator>
#include <algorithm>
#include <map>
#include <cmath>

// clitk
#include "clitkDicomRTStruct2ImageFilter.h"
#include "clitkDicomRT_Contour.h"
#include "clitkDicomRT_ROI.h"
#include "clitkImageCommon.h"

// vtk
#include <vtkVersion.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkSmartPointer.h>
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkMetaImageWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>


//--------------------------------------------------------------------
clitk::DicomRTStruct2ImageFilter::DicomRTStruct2ImageFilter()
{
  mROI = NULL;
  mWriteOutput = false;
  mWriteMesh = false;
  mCropMask = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRTStruct2ImageFilter::~DicomRTStruct2ImageFilter()
{

}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool clitk::DicomRTStruct2ImageFilter::ImageInfoIsSet() const
{
  return mSize.size() && mSpacing.size() && mOrigin.size();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetWriteOutputFlag(bool b)
{
  mWriteOutput = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetROI(clitk::DicomRT_ROI * roi)
{
  mROI = roi;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetCropMaskEnabled(bool b)
{
  mCropMask = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetWriteMesh(bool b)
{
  mWriteMesh = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetOutputImageFilename(std::string s)
{
  mOutputFilename = s;
  mWriteOutput = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetImage(vvImage::Pointer image)
{
  if (image->GetNumberOfDimensions() != 3) {
    std::cerr << "Error. Please provide a 3D image." << std::endl;
    exit(0);
  }
  mSpacing.resize(3);
  mOrigin.resize(3);
  mSize.resize(3);
  mDirection.resize(3);
  mTransformMatrix = image->GetTransform()[0]->GetMatrix();
  for(unsigned int i=0; i<3; i++) {
    mSpacing[i] = image->GetSpacing()[i];
    mOrigin[i] = image->GetOrigin()[i];
    mSize[i] = image->GetSize()[i];
    mDirection[i].resize(3);
    for(unsigned int j=0; j<3; j++)
      mDirection[i][j] = image->GetDirection()[i][j];
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetImageFilename(std::string f)
{
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(f);
  if (header->GetNumberOfDimensions() < 3) {
    std::cerr << "Error. Please provide a 3D image instead of " << f << std::endl;
    exit(0);
  }
  if (header->GetNumberOfDimensions() > 3) {
    std::cerr << "Warning dimension > 3 are ignored" << std::endl;
  }
  mSpacing.resize(3);
  mOrigin.resize(3);
  mSize.resize(3);
  mDirection.resize(3);
  for(unsigned int i=0; i<3; i++) {
    mSpacing[i] = header->GetSpacing(i);
    mOrigin[i] = header->GetOrigin(i);
    mSize[i] = header->GetDimensions(i);
    mDirection[i].resize(3);
    for(unsigned int j=0; j<3; j++)
      mDirection[i][j] = header->GetDirection(i)[j];
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetOutputOrigin(const double* origin)
{
  std::copy(origin,origin+3,std::back_inserter(mOrigin));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetOutputSpacing(const double* spacing)
{
  std::copy(spacing,spacing+3,std::back_inserter(mSpacing));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetOutputSize(const unsigned long* size)
{
  std::copy(size,size+3,std::back_inserter(mSize));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::Update()
{
  if (!mROI) {
    std::cerr << "Error. No ROI set, please use SetROI." << std::endl;
    exit(0);
  }
  if (!ImageInfoIsSet()) {
    std::cerr << "Error. Please provide image info (spacing/origin) with SetImageFilename" << std::endl;
    exit(0);
  }

  // Get Mesh
  vtkPolyData * mesh = mROI->GetMesh();
  if (mWriteMesh) {
    vtkSmartPointer<vtkXMLPolyDataWriter> meshWriter = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    std::string vtkName = mOutputFilename;
    vtkName += ".vtk";
    meshWriter->SetFileName(vtkName.c_str());
#if VTK_MAJOR_VERSION <= 5
    meshWriter->SetInput(mesh);
#else
    meshWriter->SetInputData(mesh);
#endif
    meshWriter->Write();
  }

  // Get bounds
  double *bounds=mesh->GetBounds();

  //Change mOrigin, mSize and mSpacing with respect to the directions
  // Spacing is influenced by input direction
  std::vector<double> tempSpacing;
  tempSpacing.resize(3);
  for(int i=0; i<3; i++) {
    tempSpacing[i] = 0.0;
    for(int j=0; j<3; j++) {
      tempSpacing[i] += mDirection[i][j] * mSpacing[j];
    }
  }
  for(int i=0; i<3; i++)
    mSpacing[i] = tempSpacing[i];

  // Size is influenced by affine transform matrix and input direction
  // Size is converted to double, transformed and converted back to size type.
  std::vector<double> tempSize;
  tempSize.resize(3);
  for(int i=0; i<3; i++) {
    tempSize[i] = 0.0;
    for(int j=0; j<3; j++) {
      tempSize[i] += mDirection[i][j] * mSize[j];
    }
  }
  for(int i=0; i<3; i++) {
    if (tempSize[i] < 0.0) {
      tempSize[i] *= -1;
      mOrigin[i] += mSpacing[i]*(tempSize[i] -1);
      mSpacing[i] *= -1;
    }
    mSize[i] = lrint(tempSize[i]);
  }

  // Compute origin
  std::vector<double> origin;
  origin.resize(3);
  origin[0] = floor((bounds[0]-mOrigin[0])/mSpacing[0]-2)*mSpacing[0]+mOrigin[0];
  origin[1] = floor((bounds[2]-mOrigin[1])/mSpacing[1]-2)*mSpacing[1]+mOrigin[1];
  origin[2] = floor((bounds[4]-mOrigin[2])/mSpacing[2]-2)*mSpacing[2]+mOrigin[2];

  // Compute extend
  std::vector<double> extend;
  extend.resize(3);
  extend[0] = ceil((bounds[1]-origin[0])/mSpacing[0]+4);
  extend[1] = ceil((bounds[3]-origin[1])/mSpacing[1]+4);
  extend[2] = ceil((bounds[5]-origin[2])/mSpacing[2]+4);
  // If no crop, set initial image size/origin
  if (!mCropMask) {
    for(int i=0; i<3; i++) {
      origin[i] = mOrigin[i];
      extend[i] = mSize[i]-1;
    }
  }

  // Create new output image
  mBinaryImage = vtkSmartPointer<vtkImageData>::New();
#if VTK_MAJOR_VERSION <= 5
  mBinaryImage->SetScalarTypeToUnsignedChar();
#endif
  mBinaryImage->SetOrigin(&origin[0]);
  mBinaryImage->SetSpacing(&mSpacing[0]);
  mBinaryImage->SetExtent(0, extend[0],
                          0, extend[1],
                          0, extend[2]);
#if VTK_MAJOR_VERSION <= 5
  mBinaryImage->AllocateScalars();
#else
  mBinaryImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
#endif

  memset(mBinaryImage->GetScalarPointer(), 0,
         mBinaryImage->GetDimensions()[0]*mBinaryImage->GetDimensions()[1]*mBinaryImage->GetDimensions()[2]*sizeof(unsigned char));

  // Per-slice rasterisation.
  //
  // Each Z slice's closed contours are rasterised independently so that
  // sub-voxel / degenerate contours on one slice cannot corrupt the parity
  // count for neighbouring slices (the original implementation appended
  // every contour into one polydata, extruded it globally, and ran the
  // stencil with tolerance 0 -- which dropped entire slices when a tiny
  // sub-pixel contour was present, e.g. LUNG1-002 slice 88 of NSCLC-Radiomics).
  //
  // For each slice we extrude the contours symmetrically around their Z
  // (-spacing/2, +spacing/2), so the destination voxel centre sits exactly
  // in the middle of the slab.  The even-odd rule inside vtkPolyDataToImageStencil
  // still produces holes for nested contours on the same slice.
  const std::vector<clitk::DicomRT_Contour::Pointer> & contours = mROI->GetListOfContours();

  // Group contour indices by destination mask slice index in Z.
  std::map<int, std::vector<size_t> > sliceGroups;
  for (size_t ci = 0; ci < contours.size(); ++ci) {
    double cz = contours[ci]->GetZ();
    int sliceIdx = static_cast<int>(lrint((cz - origin[2]) / mSpacing[2]));
    if (sliceIdx < 0 || sliceIdx > static_cast<int>(extend[2])) continue;
    sliceGroups[sliceIdx].push_back(ci);
  }

  for (std::map<int, std::vector<size_t> >::const_iterator git = sliceGroups.begin();
       git != sliceGroups.end(); ++git) {
    int sliceIdx = git->first;
    const std::vector<size_t> & idxs = git->second;
    if (idxs.empty()) continue;

    // Combine all contour polylines for this slice.
    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    for (size_t k = 0; k < idxs.size(); ++k) {
#if VTK_MAJOR_VERSION <= 5
      append->AddInput(contours[idxs[k]]->GetMesh());
#else
      append->AddInputData(contours[idxs[k]]->GetMesh());
#endif
    }
    append->Update();

    // Centre the future extruded slab on the destination voxel centre.
    // Shift = (voxelCenterZ - sourceZ) - mSpacing[2]/2, then extrude by +mSpacing[2].
    double voxelCenterZ = origin[2] + sliceIdx * mSpacing[2];
    double sourceZ = contours[idxs[0]]->GetZ();
    double shiftZ = (voxelCenterZ - sourceZ) - mSpacing[2] / 2.0;

    vtkSmartPointer<vtkTransform> tf = vtkSmartPointer<vtkTransform>::New();
    tf->Translate(0.0, 0.0, shiftZ);
    vtkSmartPointer<vtkTransformPolyDataFilter> tfFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    tfFilter->SetTransform(tf);
    tfFilter->SetInputConnection(append->GetOutputPort(0));

    vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
    extrude->SetInputConnection(tfFilter->GetOutputPort(0));
    extrude->SetVector(0.0, 0.0, mSpacing[2]);

    vtkSmartPointer<vtkPolyDataToImageStencil> sts = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    // Keep VTK's default tolerance (~1e-3) so boundary voxels are decided
    // deterministically.  The old SetTolerance(0) was the trigger for the
    // missing-slice bug when combined with degenerate sub-voxel input.
    sts->SetInformationInput(mBinaryImage);
    sts->SetInputConnection(extrude->GetOutputPort(0));

    vtkSmartPointer<vtkImageStencil> stencil = vtkSmartPointer<vtkImageStencil>::New();
    stencil->SetStencilConnection(sts->GetOutputPort(0));
#if VTK_MAJOR_VERSION <= 5
    stencil->SetInput(mBinaryImage);
#else
    stencil->SetInputData(mBinaryImage);
#endif
    stencil->ReverseStencilOn();
    stencil->Update();

    // Accumulate into mBinaryImage so the next slice's pass sees previous fills.
    mBinaryImage->ShallowCopy(stencil->GetOutput());
  }

  if (mWriteOutput) {
    typedef itk::Image<unsigned char, 3> ImageType;
    typedef itk::VTKImageToImageFilter<ImageType> ConnectorType;
    ConnectorType::Pointer connector = ConnectorType::New();
    connector->SetInput(GetOutput());
    connector->Update();
    clitk::writeImage<ImageType>(connector->GetOutput(), mOutputFilename);
  }
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
vtkImageData * clitk::DicomRTStruct2ImageFilter::GetOutput()
{
  assert(mBinaryImage);
  return mBinaryImage;
}
//--------------------------------------------------------------------

