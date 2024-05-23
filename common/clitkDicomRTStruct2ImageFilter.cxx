/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX
  Extended by  :   Petros K. Iosifidis (ipetroskon@cgfl.fr)

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
#include "clitkDicomRTStruct2ImageFilter.h"
#include "clitkImageCommon.h"
#include "vvImageWriter.h"

// vtk
#include <vtkVersion.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkSmartPointer.h>
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkMetaImageWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkTransformPolyDataFilter.h>

//visualization imports
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkImageViewer.h>
#include <vtkImageActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

//--------------------------------------------------------------------
clitk::DicomRTStruct2ImageFilter::DicomRTStruct2ImageFilter()
{
  mROI = NULL;
  jsonMesh = NULL;
  mWriteOutput = false;
  mWriteMesh = false;
  mCropMask = true;
  verbose = false;
  mode = false;
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
//what is the difference between this and SetImageFilename? dep?
void clitk::DicomRTStruct2ImageFilter::SetImage(vvImage::Pointer image)
{
  if (image->GetNumberOfDimensions() != 3) {
    std::cerr << "Error. Please provide a 3D image." << std::endl;
    exit(EXIT_FAILURE);
  }
  mSpacing.resize(3);
  mOrigin.resize(3);
  mSize.resize(3);
  mDirection.resize(3);
  //mTransformMatrix = image->GetTransform()[0]->GetMatrix();
  mTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  for(unsigned int i=0;i<4;i++) {
      for(unsigned int j=0;j<4;j++) {
          mTransformMatrix->SetElement(i,j,image->GetTransform()[0]->GetMatrix()->GetElement(i,j));
      }
  }
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
    exit(EXIT_FAILURE);
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
  //cf. AddItkImage function in vvImage.txx
  mTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  mTransformMatrix->Identity();
  for(unsigned int i=0; i<3; i++) {
      double tmp = 0;
      for(unsigned int j=0; j<3; j++) {
          mTransformMatrix->SetElement(i,j,mDirection[i][j]);
          tmp -= mDirection[i][j] * mOrigin[j];
      }
      tmp += mOrigin[i];
      mTransformMatrix->SetElement(i,3,tmp);
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
  if (!ImageInfoIsSet()) {
    std::cerr << "Error. Please provide image info (spacing/origin) with SetImageFilename" << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();
  if (mode == false) { //RTStruct 
      if (!mROI) {
          std::cerr << "Error. No ROI set, please use SetROI." << std::endl;
          exit(EXIT_FAILURE);
      }
      // Get Mesh
      mesh = mROI->GetMesh();
  }
  else { //JSON
      mesh = jsonMesh;
  }
  
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
  std::vector<double> origin; origin.resize(3);
  std::vector<double> extend; extend.resize(3);
    
  // sets origin & extend with respects to crop flag
  if (mCropMask == false) { //original 
      for (int i = 0; i < 3; i++) {
          origin[i] = mOrigin[i];
          extend[i] = mSize[i] - 1;
      }
  }
  else { //croped
      // Compute origin
      origin[0] = floor((bounds[0] - mOrigin[0]) / mSpacing[0] - 2) * mSpacing[0] + mOrigin[0];
      origin[1] = floor((bounds[2] - mOrigin[1]) / mSpacing[1] - 2) * mSpacing[1] + mOrigin[1];
      origin[2] = floor((bounds[4] - mOrigin[2]) / mSpacing[2] - 2) * mSpacing[2] + mOrigin[2];
      // Compute extend
      extend[0] = ceil((bounds[1] - origin[0]) / mSpacing[0] + 4);
      extend[1] = ceil((bounds[3] - origin[1]) / mSpacing[1] + 4);
      extend[2] = ceil((bounds[5] - origin[2]) / mSpacing[2] + 4);
  }
  
  if (verbose) {
      //  good old C printf
      printf("MESH bounds: %5.5f\t%5.5f\t%5.5f\n", bounds[0], bounds[1], bounds[2]);
      printf("OUTPUT origin: %5.5f\t%5.5f\t%5.5f\n", origin[0], origin[1], origin[2]);
      printf("OUTPUT extend: %5.5f\t%5.5f\t%5.5f\n", extend[0], extend[1], extend[2]);
      std::cout << "\n--Geometry transformation matrix--" << std::endl;
      mTransformMatrix->Print(std::cout);
      visualize(mesh);
  }

  //Apply the transform to the mesh
  vtkSmartPointer<vtkTransform> outputLabelmapGeometryTransform = vtkSmartPointer<vtkTransform>::New();
  outputLabelmapGeometryTransform->SetMatrix(mTransformMatrix);
  // Apparently the inverse is wrong...
  //outputLabelmapGeometryTransform->Inverse();
  vtkSmartPointer<vtkTransformPolyDataFilter> transformPolyDataFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
#if VTK_MAJOR_VERSION <= 5
  transformPolyDataFilter->SetInput(mesh);
#else
  transformPolyDataFilter->SetInputData(mesh);
#endif
  transformPolyDataFilter->SetTransform(outputLabelmapGeometryTransform);
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

  // Extrude
  vtkSmartPointer<vtkLinearExtrusionFilter> extrude=vtkSmartPointer<vtkLinearExtrusionFilter>::New();
  extrude->SetInputConnection(transformPolyDataFilter->GetOutputPort());
  ///We extrude in the -slice_spacing direction to respect the FOCAL convention (NEEDED !)
  extrude->SetVector(0, 0, -mSpacing[2]);

  // Binarization
  vtkSmartPointer<vtkPolyDataToImageStencil> sts=vtkSmartPointer<vtkPolyDataToImageStencil>::New();
  //The following line is extremely important
  //http://www.nabble.com/Bug-in-vtkPolyDataToImageStencil--td23368312.html#a23370933
  sts->SetTolerance(0);
  sts->SetInformationInput(mBinaryImage);
  sts->SetInputConnection(extrude->GetOutputPort(0));
  //sts->SetInput(mesh);

  vtkSmartPointer<vtkImageStencil> stencil=vtkSmartPointer<vtkImageStencil>::New();
#if VTK_MAJOR_VERSION <= 5
  stencil->SetStencil(sts->GetOutput());
#else
  stencil->SetStencilConnection(sts->GetOutputPort(0));
#endif
#if VTK_MAJOR_VERSION <= 5
  stencil->SetInput(mBinaryImage);
#else
  stencil->SetInputData(mBinaryImage);
#endif
  stencil->ReverseStencilOn();
  stencil->Update();

  mBinaryImage->ShallowCopy(stencil->GetOutput());

  vvImage::Pointer vvBinaryImage = vvImage::New();
  vtkSmartPointer<vtkTransform> vvBinaryImageT = vtkSmartPointer<vtkTransform>::New();
  vvBinaryImageT->SetMatrix(mTransformMatrix);
  vvBinaryImage->AddVtkImage(mBinaryImage, vvBinaryImageT);

  if (mWriteOutput) {
    //typedef itk::Image<unsigned char, 3> ImageType;
    //typedef itk::VTKImageToImageFilter<ImageType> ConnectorType;
    //ConnectorType::Pointer connector = ConnectorType::New();
    //connector->SetInput(GetOutput());
    //clitk::writeImage<ImageType>(connector->GetOutput(), mOutputFilename);
    vvImageWriter::Pointer writer = vvImageWriter::New();
    writer->SetInput(vvBinaryImage);
    if (!vvBinaryImage->GetTransform().empty())
        writer->SetSaveTransform(true);
    writer->SetOutputFileName(mOutputFilename);
    writer->Update();
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkImageData * clitk::DicomRTStruct2ImageFilter::GetOutput()
{
  //assert(mBinaryImage);
  if (mBinaryImage == NULL) {
      FATAL("The binary RTStruct image is NULL");
  }
  return mBinaryImage;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetVerbose(bool b)
{
    verbose = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetInputData(vtkSmartPointer<vtkPolyData>& data)
{
    //init & grab
    jsonMesh = vtkSmartPointer<vtkPolyData>::New();
    jsonMesh->DeepCopy(data);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::visualize(vtkSmartPointer<vtkPolyData> polyData)
{
    //vars , not copy paste, surely c': 
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    vtkNew<vtkActor> actor;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

    // Create a mapper and actor
    mapper->SetInputData(polyData);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("White").GetData());

    // Visualize
    renderWindow->SetWindowName("visual feedback");
    //renderWindow->SetSize(255, 255);
    renderWindow->AddRenderer(renderer);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("Gray").GetData());
    renderWindow->Render();
    renderWindowInteractor->Start();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::DicomRTStruct2ImageFilter::SetMode(bool b){
    mode = b;
}
//--------------------------------------------------------------------