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

// clitk
#include "clitkDicomRT_ROI_ConvertToImageFilter.h"
#include "clitkImageCommon.h"

// vtk
#include <vtkPolyDataToImageStencil.h>
#include <vtkSmartPointer.h>
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkMetaImageWriter.h>


//--------------------------------------------------------------------
clitk::DicomRT_ROI_ConvertToImageFilter::DicomRT_ROI_ConvertToImageFilter()
{
  mROI = NULL;
  mWriteOutput = false;
  mCropMask = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::DicomRT_ROI_ConvertToImageFilter::~DicomRT_ROI_ConvertToImageFilter()
{

}
//--------------------------------------------------------------------

bool clitk::DicomRT_ROI_ConvertToImageFilter::ImageInfoIsSet() const
{
  return mSize.size() && mSpacing.size() && mOrigin.size();
}

//--------------------------------------------------------------------
void clitk::DicomRT_ROI_ConvertToImageFilter::SetROI(clitk::DicomRT_ROI * roi)
{
  mROI = roi;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI_ConvertToImageFilter::SetCropMaskEnabled(bool b)
{
  mCropMask = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI_ConvertToImageFilter::SetOutputImageFilename(std::string s)
{
  mOutputFilename = s;
  mWriteOutput = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRT_ROI_ConvertToImageFilter::SetImageFilename(std::string f)
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
  for(unsigned int i=0; i<3; i++) {
    mSpacing[i] = header->GetSpacing(i);
    mOrigin[i] = header->GetOrigin(i);
    mSize[i] = header->GetDimensions(i);
  }
}
//--------------------------------------------------------------------

void clitk::DicomRT_ROI_ConvertToImageFilter::SetOutputOrigin(const double* origin)
{
  std::copy(origin,origin+3,std::back_inserter(mOrigin));
}
//--------------------------------------------------------------------
void clitk::DicomRT_ROI_ConvertToImageFilter::SetOutputSpacing(const double* spacing)
{
  std::copy(spacing,spacing+3,std::back_inserter(mSpacing));
}
//--------------------------------------------------------------------
void clitk::DicomRT_ROI_ConvertToImageFilter::SetOutputSize(const unsigned long* size)
{
  std::copy(size,size+3,std::back_inserter(mSize));
}

//--------------------------------------------------------------------
void clitk::DicomRT_ROI_ConvertToImageFilter::Update()
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

  // Get bounds
  double *bounds=mesh->GetBounds();
  // for(int i=0; i<6; i++){
//     DD(bounds[i]);
//   }

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
  mBinaryImage->SetScalarTypeToUnsignedChar();
  mBinaryImage->SetOrigin(&origin[0]);
  mBinaryImage->SetSpacing(&mSpacing[0]);
  mBinaryImage->SetExtent(0, extend[0],
                          0, extend[1],
                          0, extend[2]);
  mBinaryImage->AllocateScalars();

  // for(int i=0; i<3; i++){
  //     DD(origin[i]);
  //     DD(extend[i]);
  //     DD(mBinaryImage->GetDimensions()[i]);
  //   }
  memset(mBinaryImage->GetScalarPointer(), 0,
         mBinaryImage->GetDimensions()[0]*mBinaryImage->GetDimensions()[1]*mBinaryImage->GetDimensions()[2]*sizeof(unsigned char));

  // Extrude
  vtkSmartPointer<vtkLinearExtrusionFilter> extrude=vtkSmartPointer<vtkLinearExtrusionFilter>::New();
  extrude->SetInput(mesh);
  ///We extrude in the -slice_spacing direction to respect the FOCAL convention (NEEDED !)
  extrude->SetVector(0, 0, -mSpacing[2]);

  // Binarization
  vtkSmartPointer<vtkPolyDataToImageStencil> sts=vtkSmartPointer<vtkPolyDataToImageStencil>::New();
  //The following line is extremely important
  //http://www.nabble.com/Bug-in-vtkPolyDataToImageStencil--td23368312.html#a23370933
  sts->SetTolerance(0);
  sts->SetInformationInput(mBinaryImage);
  sts->SetInput(extrude->GetOutput());
  //sts->SetInput(mesh);

  vtkSmartPointer<vtkImageStencil> stencil=vtkSmartPointer<vtkImageStencil>::New();
  stencil->SetStencil(sts->GetOutput());
  stencil->SetInput(mBinaryImage);
  stencil->ReverseStencilOn();
  stencil->Update();

  /*
  vtkSmartPointer<vtkMetaImageWriter> w = vtkSmartPointer<vtkMetaImageWriter>::New();
  w->SetInput(stencil->GetOutput());
  w->SetFileName("binary2.mhd");
  w->Write();
  */

  mBinaryImage->ShallowCopy(stencil->GetOutput());

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
vtkImageData * clitk::DicomRT_ROI_ConvertToImageFilter::GetOutput()
{
  assert(mBinaryImage);
  return mBinaryImage;
}
//--------------------------------------------------------------------

