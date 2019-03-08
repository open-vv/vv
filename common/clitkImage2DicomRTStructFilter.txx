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

// std
#include <iterator>
#include <algorithm>

// clitk
#include "clitkImage2DicomRTStructFilter.h"
#include "clitkBinaryImageToMeshFilter.h"
#include "clitkImageCommon.h"
#include "vvFromITK.h"

// vtk
#include <vtkPolyDataToImageStencil.h>
#include <vtkSmartPointer.h>
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkMetaImageWriter.h>
#include <vtkImageData.h>

// FIXME to remove
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataReader.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include <vtksys/SystemTools.hxx>

// itk
#include <itkImage.h>
#include <itkVTKImageToImageFilter.h>

// gdcm
#include <vtkRTStructSetProperties.h>
#include <vtkGDCMPolyDataReader.h>
#include <vtkGDCMPolyDataWriter.h>

//--------------------------------------------------------------------
template<class PixelType>
clitk::Image2DicomRTStructFilter<PixelType>::Image2DicomRTStructFilter()
{
  m_StructureSetFilename = "";
  m_DicomFolder = "";
  m_OutputFilename = "default-output.dcm";
  m_ThresholdValue = 0.5;
  m_SkipInitialStructuresFlag = false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
clitk::Image2DicomRTStructFilter<PixelType>::~Image2DicomRTStructFilter()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
void
clitk::Image2DicomRTStructFilter<PixelType>::SetROIType(std::string type)
{
  m_ROIType = type;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
void clitk::Image2DicomRTStructFilter<PixelType>::Update() 
{
  // Check this is a RT-Struct
  gdcm::Reader gdcm_reader;
  gdcm_reader.SetFileName(m_StructureSetFilename.c_str());
  if (!gdcm_reader.Read()) {
    clitkExceptionMacro("Error could not open the file '" << m_StructureSetFilename << std::endl);
  }
  gdcm::MediaStorage ms;
  ms.SetFromFile(gdcm_reader.GetFile());
  if (ms != gdcm::MediaStorage::RTStructureSetStorage) {
    clitkExceptionMacro("File '" << m_StructureSetFilename << "' is not a DICOM-RT-Struct file." << std::endl);
  }

  // Read rt struct
  vtkSmartPointer<vtkGDCMPolyDataReader> reader = vtkGDCMPolyDataReader::New();
  reader->SetFileName(m_StructureSetFilename.c_str());
  reader->Update();  

  // Get properties
  vtkRTStructSetProperties * p = reader->GetRTStructSetProperties();
  if (GetVerboseFlag()) {
    std::cout << "Number of structures in the dicom-rt-struct : " 
              << p->GetNumberOfStructureSetROIs() << std::endl;
  }

  // number of additional contours
  int m = m_InputFilenames.size();

  // Init writer
  vtkGDCMPolyDataWriter * writer = vtkGDCMPolyDataWriter::New();
  int numMasks = reader->GetNumberOfOutputPorts() + m;

  if (m_SkipInitialStructuresFlag) {
    numMasks = m;
  }

  writer->SetNumberOfInputPorts(numMasks);    
  writer->SetFileName(m_OutputFilename.c_str());
  writer->SetMedicalImageProperties(reader->GetMedicalImageProperties());

  // List of already present rois
  vtkStringArray* roiNames = vtkStringArray::New();
  vtkStringArray* roiAlgorithms = vtkStringArray::New();
  vtkStringArray* roiTypes = vtkStringArray::New();
  roiNames->SetNumberOfValues(numMasks);
  roiAlgorithms->SetNumberOfValues(numMasks);
  roiTypes->SetNumberOfValues(numMasks);

  // Convert the image into a mesh
  std::vector<vtkSmartPointer<vtkPolyData> > meshes;
  std::vector<std::string> m_ROINames;
  meshes.resize(m);
  m_ROINames.resize(m);
  for(unsigned int i=0; i<m; i++) {

    // read image
    //    typedef float PixelType;
    //typedef itk::Image<PixelType, 3> ImageType;
    ImagePointer input = clitk::readImage<ImageType>(m_InputFilenames[i], false);

    std::ostringstream oss;
    oss << vtksys::SystemTools::
      GetFilenameName(vtksys::SystemTools::GetFilenameWithoutLastExtension(m_InputFilenames[i]));
    std::string name = oss.str();
    m_ROINames[i] = name;

    // convert to mesh
    typedef clitk::BinaryImageToMeshFilter<ImageType> BinaryImageToMeshFilterType;
    typename BinaryImageToMeshFilterType::Pointer convert = BinaryImageToMeshFilterType::New();
    convert->SetThresholdValue(m_ThresholdValue);
    convert->SetInput(input);
    convert->Update();
    meshes[i] = convert->GetOutputMesh();
    if (GetVerboseFlag()) {
      std::cout << "Mesh has " << meshes[i]->GetNumberOfLines() << " lines." << std::endl;
    }

    /*
    // debug mesh write  FIXME
    vtkSmartPointer<vtkPolyDataWriter> wr = vtkSmartPointer<vtkPolyDataWriter>::New();
    wr->SetInputConnection(convert->GetOutputPort()); //psurface->GetOutputPort()
    wr->SetFileName("bidon.obj");
    wr->Update();
    wr->Write();
    */
  }

  // Copy previous contours
  for (unsigned int i = 0; i < numMasks-m; ++i) {
#if VTK_MAJOR_VERSION <= 5
    writer->SetInput(i, reader->GetOutput(i));
#else
    writer->SetInputData(i, reader->GetOutput(i));
#endif
    std::string theString = reader->GetRTStructSetProperties()->GetStructureSetROIName(i);
    roiNames->InsertValue(i, theString);
    theString = reader->GetRTStructSetProperties()->GetStructureSetROIGenerationAlgorithm(i);
    roiAlgorithms->InsertValue(i, theString);
    theString = reader->GetRTStructSetProperties()->GetStructureSetRTROIInterpretedType(i);
    roiTypes->InsertValue(i, theString);
  }

  // Add new ones
  for (unsigned int i = numMasks-m; i < numMasks; ++i) {
#if VTK_MAJOR_VERSION <= 5
    writer->SetInput(i, meshes[i-numMasks+m]);
#else
    writer->SetInputData(i, meshes[i-numMasks+m]);
#endif
    roiNames->InsertValue(i, m_ROIType);
    roiAlgorithms->InsertValue(i, "CLITK_CREATED");
    roiTypes->InsertValue(i, m_ROIType);
  }

  /*
  //  Visu DEBUG
  vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
  cubeMapper->SetInput( mesh );
  cubeMapper->SetScalarRange(0,7);
  vtkActor *cubeActor = vtkActor::New();
  cubeActor->SetMapper(cubeMapper);
  vtkProperty * property = cubeActor->GetProperty();
  property->SetRepresentationToWireframe();

  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  renderer->AddActor(cubeActor);
  renderer->ResetCamera();
  renderer->SetBackground(1,1,1);

  renWin->SetSize(300,300);

  renWin->Render();
  iren->Start();
  */
  // End visu

  // Write (need to read dicom for slice id)
  vtkRTStructSetProperties* theProperties = vtkRTStructSetProperties::New();
  writer->SetRTStructSetProperties(theProperties);
  if (GetVerboseFlag()) {
    std::cout << "Looking for dicom info, study instance "
              << p->GetStudyInstanceUID() << std::endl;
  }
  writer->InitializeRTStructSet(m_DicomFolder,
                                reader->GetRTStructSetProperties()->GetStructureSetLabel(),
                                reader->GetRTStructSetProperties()->GetStructureSetName(),
                                roiNames, roiAlgorithms, roiTypes);
  writer->Write();
  reader->Delete();
  roiNames->Delete();
  roiTypes->Delete();
  roiAlgorithms->Delete();
  writer->Delete();
}
//--------------------------------------------------------------------




