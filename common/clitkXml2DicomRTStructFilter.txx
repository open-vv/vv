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

// clitk
#include "clitkXml2DicomRTStructFilter.h"

// xml parser
#include "../utilities/pugixml/pugixml.hpp"

// vtk
#include <vtkSmartPointer.h>
#include <vtkMetaImageWriter.h>
#include <vtkImageData.h>
#include <vtkPolygon.h>
#include <vtkAppendPolyData.h>

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

// gdcm
#include <vtkRTStructSetProperties.h>
#include <vtkGDCMPolyDataReader.h>
#include <vtkGDCMPolyDataWriter.h>

//--------------------------------------------------------------------
template<class PixelType>
clitk::Xml2DicomRTStructFilter<PixelType>::Xml2DicomRTStructFilter()
{
  m_StructureSetFilename = "";
  m_DicomFolder = "";
  m_OutputFilename = "default-output.dcm";
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
clitk::Xml2DicomRTStructFilter<PixelType>::~Xml2DicomRTStructFilter()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class PixelType>
void clitk::Xml2DicomRTStructFilter<PixelType>::Update()
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

  std::map<std::string, vtkSmartPointer<vtkAppendPolyData> > mapName2Data;

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(m_InputFilename.c_str());
  if (!result) {
    std::cout << "ERROR: no result" << std::endl;
    return;
  }

  //Take the main dict and look for point_mm in each struct in each slice
  pugi::xml_node mainDict = doc.child("plist").child("dict").child("array");
  for (pugi::xml_node_iterator mainDictIt = mainDict.begin(); mainDictIt != mainDict.end(); ++mainDictIt) { //Look for all slice (one slice per dict)
    if (!std::strcmp(mainDictIt->name(), "dict")) {
      for (pugi::xml_node_iterator sliceIt = mainDictIt->child("array").begin(); sliceIt != mainDictIt->child("array").end(); ++sliceIt) { //Look for all struct in the current slice (one struct per dict)
        if (!std::strcmp(sliceIt->name(), "dict")) {
          pugi::xml_node_iterator structureIt = sliceIt->begin();
          while (structureIt != sliceIt->end() && std::abs(std::strcmp(structureIt->child_value(), "Name"))) //Look for the name for the current struct
            ++structureIt;
          if (structureIt != sliceIt->end()) { //take the following node to have the values
            ++structureIt;
            std::string name(structureIt->child_value());
            while (structureIt != sliceIt->end() && std::abs(std::strcmp(structureIt->child_value(), "Point_mm"))) //Look for the Point_mm for the current struct
              ++structureIt;
            if (structureIt != sliceIt->end()) { //take the following node to have the values
              ++structureIt;
              // Insert all points for 1 struct into vtkPoints
              vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
              for (pugi::xml_node_iterator pointIt = structureIt->begin(); pointIt != structureIt->end(); ++pointIt) { //Look for all points in mm in the current struct (one point per string)
                if (!std::strcmp(pointIt->name(), "string")) {
                  // Split the string to save the 3 values (remove the useless char) as double
                  char* copy = (char*)pointIt->child_value();
                  std::vector<char*> v;
                  char* chars_array = strtok(copy, ", ");
                  while(chars_array) {
                      v.push_back(chars_array);
                      chars_array = strtok(NULL, ", ");
                  }
                  v[0] = v[0] + 1;
                  v[2][strlen(v[2])-1] = '\0';
                  double x, y, z;
                  x = std::atof(v[0]);
                  y = std::atof(v[1]);
                  z = std::atof(v[2]);
                  points->InsertNextPoint(x, y, z);
                }
              }

              //Create the polygon attached to the points
              vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
              polygon->GetPointIds()->SetNumberOfIds(points->GetNumberOfPoints()); //make a quad
              for (int pointIdx=0; pointIdx<points->GetNumberOfPoints(); ++pointIdx)
                polygon->GetPointIds()->SetId(pointIdx, pointIdx);
              // Add the polygon to a list of polygons
              vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
              polygons->InsertNextCell(polygon);
              // Create a PolyData
              vtkSmartPointer<vtkPolyData> polygonPolyData = vtkSmartPointer<vtkPolyData>::New();
              polygonPolyData->SetPoints(points);
              polygonPolyData->SetPolys(polygons);

              //Append the polyData into the map at the correct stuct name entry
              std::map<std::string, vtkSmartPointer<vtkAppendPolyData> >::const_iterator it = mapName2Data.find(name);
              if (it == mapName2Data.end())
                mapName2Data[name] = vtkSmartPointer<vtkAppendPolyData>::New();
#if VTK_MAJOR_VERSION <= 5
              mapName2Data[name]->AddInput(polygonPolyData);
#else
              mapName2Data[name]->AddInputData(polygonPolyData);
#endif
            }
          }
        }
      }
    }
  }

  for (std::map<std::string, vtkSmartPointer<vtkAppendPolyData> >::iterator it = mapName2Data.begin(); it != mapName2Data.end(); ++it)
    it->second->Update();

  // number of contours
  int numMasks = mapName2Data.size();

  // Init writer
  vtkGDCMPolyDataWriter * writer = vtkGDCMPolyDataWriter::New();
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

  // Add new structures
  std::map<std::string, vtkSmartPointer<vtkAppendPolyData> >::iterator it = mapName2Data.begin();
  for (unsigned int i = 0; i < numMasks; ++i) {
#if VTK_MAJOR_VERSION <= 5
    writer->SetInput(i, it->second->GetOutput());
#else
    writer->SetInputData(i, it->second->GetOutput());
#endif
    roiNames->InsertValue(i, it->first);
    roiAlgorithms->InsertValue(i, "CLITK_CREATED");
    roiTypes->InsertValue(i, "coucou"); //Roi type instead of coucou
    ++it;
  }

  /*
  //  Visu DEBUG
  vtkPolyDataMapper *cubeMapper = vtkPolyDataMapper::New();
  cubeMapper->SetInputData( mapName2Data[0]->GetOutput() );
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

