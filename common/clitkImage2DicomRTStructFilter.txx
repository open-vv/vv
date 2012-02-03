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

// itk
#include <itkImage.h>
#include <itkVTKImageToImageFilter.h>

// gdcm
#include <vtkGDCMPolyDataWriter.h>

//--------------------------------------------------------------------
template<class PixelType>
clitk::Image2DicomRTStructFilter<PixelType>::Image2DicomRTStructFilter()
{
  DD("Image2DicomRTStructFilter Const");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
clitk::Image2DicomRTStructFilter<PixelType>::~Image2DicomRTStructFilter()
{
  DD("Image2DicomRTStructFilter Destructor");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
void clitk::Image2DicomRTStructFilter<PixelType>::Update() 
{
  DD("Image2DicomRTStructFilter::GenerateData");

  // Read rt struct
  vtkSmartPointer<vtkGDCMPolyDataReader> reader = vtkGDCMPolyDataReader::New();
  reader->SetFileName(m_StructureSetFilename.c_str());
  reader->Update();
  DD("reader done");

  // Get properties
  vtkRTStructSetProperties * p = reader->GetRTStructSetProperties();
  DD(p->GetNumberOfStructureSetROIs());
  DD(p->GetStructureSetROIName(0));
  DD(p->GetStructureSetROINumber(0));  
  
  // Init writer
  vtkGDCMPolyDataWriter * writer = vtkGDCMPolyDataWriter::New();
  int numMasks = reader->GetNumberOfOutputPorts() + 1;//add one more
  DD(numMasks);
  
  // numMasks = 3; //FIXME temporary

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
  
  typedef clitk::BinaryImageToMeshFilter<ImageType> BinaryImageToMeshFilterType;
  typename BinaryImageToMeshFilterType::Pointer convert = BinaryImageToMeshFilterType::New();
  convert->SetInput(m_Input);
  DD("Update");
  convert->Update();
  DD("here");
  DD("end update");
  vtkPolyData* mesh = convert->GetOutputMesh();
  DD(mesh->GetNumberOfVerts());
  DD(mesh->GetNumberOfLines());
  DD(mesh->GetNumberOfPolys());
  DD(mesh->GetNumberOfStrips());

  // Add on (FIXME) to replace with binary image
  // vtkPolyData* blank = vtkPolyData::New();
  // writer->SetInput(0, blank);
  writer->SetInput(0, mesh);
  roiNames->InsertValue(0, "blank");
  roiAlgorithms->InsertValue(0, "blank");
  roiTypes->InsertValue(0, "ORGAN");

  for (unsigned int i = 1; i < numMasks; ++i) {
    // DD(i);
    writer->SetInput(i, reader->GetOutput(i-1));
    std::string theString = reader->GetRTStructSetProperties()->GetStructureSetROIName(i-1);
    roiNames->InsertValue(i, theString);
    theString = reader->GetRTStructSetProperties()->GetStructureSetROIGenerationAlgorithm(i-1);
    roiAlgorithms->InsertValue(i, theString);
    theString = reader->GetRTStructSetProperties()->GetStructureSetRTROIInterpretedType(i-1);
    roiTypes->InsertValue(i, theString);
  }
  

  /*
  //  Visu
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


  vtkRTStructSetProperties* theProperties = vtkRTStructSetProperties::New();
  DD(p->GetStudyInstanceUID());
  writer->SetRTStructSetProperties(theProperties);
  /*writer->InitializeRTStructSet2(p,"./",
                                 reader->GetRTStructSetProperties()->GetStructureSetLabel(),
                                 reader->GetRTStructSetProperties()->GetStructureSetName(),
                                 roiNames, roiAlgorithms, roiTypes);*/  
  writer->InitializeRTStructSet(m_DicomFolder,
                                reader->GetRTStructSetProperties()->GetStructureSetLabel(),
                                reader->GetRTStructSetProperties()->GetStructureSetName(),
                                roiNames, roiAlgorithms, roiTypes);
  
  DD("after init");
  writer->Write();
  DD("write done");

  reader->Delete();
  roiNames->Delete();
  roiTypes->Delete();
  //theProperties->Delete();
  roiAlgorithms->Delete();
  //blank->Delete();
  writer->Delete();
  
  ////////////////////
  

  /*

  // DicomRTStruct
  DD(m_StructureSet->GetName());
  clitk::DicomRT_ROI * roi = m_StructureSet->GetROIFromROINumber(1); // Aorta
  DD(roi->GetName());
  DD(roi->GetROINumber());


  // Get 
  



  // Add an image to the roi
  vvImage::Pointer im = vvImageFromITK<3, PixelType>(m_Input);
  roi->SetImage(im);

  // Get one contour
  DD("Compute Mesh");
  roi->ComputeContoursFromImage(); // FIXME do the set mesh for the moment (to change)
  // roi->ComputeMeshFromContour();
  vtkSmartPointer<vtkPolyData> mesh = roi->GetMesh();
  DD("done");
  
  // Change the mesh (shift by 10);
  // const vtkSmartPointer<vtkPoints> & points = mesh->GetPoints();
  // for(uint i=0; i<mesh->GetNumberOfVerts (); i++) {
  //   DD(i);
  //   double * p = points->GetPoint(i);
  //   p[0] += 30;
  //   points->SetPoint(i, p);
  // }
  roi->SetName("TOTO");
  roi->SetDicomUptodateFlag(true); // indicate that dicom info must be updated from the mesh.
    
  // Convert to dicom ?
  DD("TODO");
  
  // Write
  structset->Write("toto.dcm");
  */
}
//--------------------------------------------------------------------




