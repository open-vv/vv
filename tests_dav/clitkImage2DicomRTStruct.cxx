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

#include "clitkImage2DicomRTStructFilter.h"
#include "clitkDicomRT_StructureSet.h"
#include "clitkImage2DicomRTStruct_ggo.h"

#include <vtkGDCMPolyDataReader.h>
#include <vtkGDCMPolyDataWriter.h>
#include <vtkRTStructSetProperties.h>
#include <vtkImageClip.h>
#include <vtkMarchingSquares.h>
#include <vtkAppendPolyData.h>

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkImage2DicomRTStruct, args_info);

  // // Read
  // DD("read");
  // vtkSmartPointer<vtkGDCMPolyDataReader> reader = vtkGDCMPolyDataReader::New();
  // reader->SetFileName(args_info.rtstruct_arg);
  // reader->Update();
  // DD("reader done");
  
  // //======================================
  // // access to properties
  // vtkRTStructSetProperties * p = reader->GetRTStructSetProperties();
  // DD(p->GetNumberOfStructureSetROIs());
  // DD(p->GetStructureSetROIName(0));
  // DD(p->GetStructureSetROINumber(0));  
  

  // //======================================
  // // Test read binary image, convert to polydata
  // typedef float PixelType;
  // typedef itk::Image<PixelType, 3> ImageType;
  // ImageType::Pointer input_itk = clitk::readImage<ImageType>(args_info.input_arg, true);
  // vvImage::Pointer input_vv = vvImageFromITK<3, PixelType>(input_itk);
  // vtkImageData * input_vtk = input_vv->GetVTKImages()[0];
  
  // vtkSmartPointer<vtkImageClip> clipper = vtkSmartPointer<vtkImageClip>::New();
  // clipper->SetInput(input_vtk);
  // int* extent = input_vtk->GetExtent();
  // DDV(extent, 6);
  // clipper->Delete();

  // uint n = input_vtk->GetDimensions()[2];
  // DD(n);
  // std::vector<vtkSmartPointer<vtkPolyData> > contours;
  // for(uint i=0; i<n; i++) {
  //   // DD(i);
  //   // FIXME     vtkDiscreteMarchingCubes INSTEAD
  //   vtkSmartPointer<vtkMarchingSquares> squares = vtkSmartPointer<vtkMarchingSquares>::New();
  //   squares->SetInput(input_vtk);
  //   squares->SetImageRange(extent[0], extent[1], extent[2], extent[3], i, i);
  //   squares->SetValue(1, 1.0);
  //   squares->Update();
  //   // DD(squares->GetNumberOfContours());
    
  //   vtkSmartPointer<vtkPolyData> m = squares->GetOutput();
  //   contours.push_back(m);
  // }
  // DD("done");
 
  // vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  // for(unsigned int i=0; i<n; i++) {
  //   append->AddInput(contours[i]);
  // }
  // append->Update();
 
  // vtkSmartPointer<vtkPolyData> mMesh = vtkSmartPointer<vtkPolyData>::New();
  // mMesh->DeepCopy(append->GetOutput());

  // // Write vtk
  // vtkPolyDataWriter * w = vtkPolyDataWriter::New();
  // w->SetInput(mMesh);
  // w->SetFileName("toto.vtk");
  // w->Write();
  // //======================================


  // // Set polydata
  // vtkPolyData * c =  reader->GetOutput(1); // second one (Oso, not Aorta)
  // DD(c->GetNumberOfVerts());
  // DD(c->GetNumberOfLines());
  // DD(c->GetNumberOfPolys());
  // DD(c->GetNumberOfStrips());
  

  // //======================================
  
  // DD(p->GetNumberOfContourReferencedFrameOfReferences());
  // DD(p->GetNumberOfReferencedFrameOfReferences ());
  // DD(p->GetNumberOfStructureSetROIs ());

  // // Write 
  // vtkSmartPointer<vtkGDCMPolyDataWriter> writer = vtkGDCMPolyDataWriter::New();
  // DD(reader->GetNumberOfOutputPorts() );
  // int numMasks =   reader->GetNumberOfOutputPorts()+1;
  // DD(numMasks);
  
  // writer->SetNumberOfInputPorts( numMasks ); // Add one
  // // // vtkRTStructSetProperties * p = reader->GetRTStructSetProperties();
  // // std::string ref = p->GetReferenceFrameOfReferenceUID();
  // // const char * algogen = p->GetStructureSetROIGenerationAlgorithm(0);
  // // DD(ref);
  // // DD(algogen);
  // // vtkIdType id = p->GetNumberOfContourReferencedFrameOfReferences();
  // // p->AddStructureSetROI(id, ref.c_str(), "BIDON", algogen);
  // // // int n = p->GetNumberOfContourReferencedFrameOfReferences();
  // // const char * classuid = p->GetContourReferencedFrameOfReferenceClassUID(0, 0);
  // // const char *instanceuid = p->GetContourReferencedFrameOfReferenceInstanceUID(0, 0);
  // // DD(classuid);
  // // DD(instanceuid);
  // // p->AddContourReferencedFrameOfReference(id, classuid, instanceuid);
  // // p->AddContourReferencedFrameOfReference(id+1, classuid, instanceuid);

  // // DD(p->GetNumberOfContourReferencedFrameOfReferences());
  // // DD(p->GetNumberOfReferencedFrameOfReferences ());
  // // DD(p->GetNumberOfStructureSetROIs ());
  
  //   writer->SetFileName("./bidon.dcm");
  // writer->SetMedicalImageProperties(reader->GetMedicalImageProperties());

  // vtkStringArray* roiNames = vtkStringArray::New();
  // vtkStringArray* roiAlgorithms = vtkStringArray::New();
  // vtkStringArray* roiTypes = vtkStringArray::New();
  // roiNames->SetNumberOfValues(numMasks);
  // roiAlgorithms->SetNumberOfValues(numMasks);
  // roiTypes->SetNumberOfValues(numMasks);

  // // Add one
  // int num = reader->GetNumberOfOutputPorts();
  // DD(num);
  // // writer->SetInput(0, mMesh); // FIXME first one
  // vtkPolyData* blank = vtkPolyData::New();
  // writer->SetInput(0, blank);
  //   roiNames->InsertValue(0, "blank");
  //   roiAlgorithms->InsertValue(0, "blank");
  //   roiTypes->InsertValue(0, "ORGAN");

  // for (int i = 1; i < numMasks; ++i) {
  //   DD(i);
  //   writer->SetInput(i, reader->GetOutput(i-1) );
  //     std::string theString = reader->GetRTStructSetProperties()->GetStructureSetROIName(i-1);
  //   roiNames->InsertValue(i, theString);
  //   DD(theString);
  //   theString = reader->GetRTStructSetProperties()->GetStructureSetROIGenerationAlgorithm(i-1);
  //   roiAlgorithms->InsertValue(i, theString);
  //   theString = reader->GetRTStructSetProperties()->GetStructureSetRTROIInterpretedType(i-1);
  //   roiTypes->InsertValue(i, theString);
  // }

  // vtkRTStructSetProperties* theProperties = vtkRTStructSetProperties::New();
  // // writer->SetRTStructSetProperties(reader->GetRTStructSetProperties());
  // writer->SetRTStructSetProperties(theProperties);
  // writer->InitializeRTStructSet("./",
  //                               reader->GetRTStructSetProperties()->GetStructureSetLabel(),
  //                               reader->GetRTStructSetProperties()->GetStructureSetName(),
  //                               roiNames, roiAlgorithms, roiTypes);
  // writer->SetRTStructSetProperties(theProperties);

  // writer->Write();
  // DD("write done");

  // DD("end");
  // writer->Delete();
  // reader->Delete();    
  // exit(0); // FIXME stop here (test)

  // Read initial 3D image
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  ImageType::Pointer input = clitk::readImage<ImageType>(args_info.input_arg, true);

  // // Read initial RT Struct
  // clitk::DicomRT_StructureSet::Pointer structset = clitk::DicomRT_StructureSet::New();
  // structset->Read(args_info.rtstruct_arg);

  // Create a filter to convert image into dicomRTStruct
  clitk::Image2DicomRTStructFilter<PixelType> filter;
  filter.SetInput(input);
  filter.SetDicomFolder(args_info.dicom_arg);
  filter.SetStructureSetFilename(args_info.rtstruct_arg);
  filter.SetOutputFilename(args_info.output_arg);
  filter.Update();
  
  // // Write result
  // clitk::DicomRT_StructureSet::Pointer s = filter.GetDicomRTStruct();
  // s->Write(args_info.output_arg);

  // This is the end my friend 
  return 0;
}
//--------------------------------------------------------------------
