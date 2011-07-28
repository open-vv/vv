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
#include "clitkImageCommon.h"
#include "vvFromITK.h"

// vtk
#include <vtkPolyDataToImageStencil.h>
#include <vtkSmartPointer.h>
#include <vtkImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkMetaImageWriter.h>
#include <vtkImageData.h>

// itk
#include <itkImage.h>
#include <itkVTKImageToImageFilter.h>

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

  // Read DicomRTStruct
  std::string filename = "RS.zzQAnotmt_french01_.dcm";
  clitk::DicomRT_StructureSet::Pointer structset = clitk::DicomRT_StructureSet::New();
  structset->Read(filename);
  
  DD(structset->GetName());
  clitk::DicomRT_ROI * roi = structset->GetROIFromROINumber(1); // Aorta
  DD(roi->GetName());
  DD(roi->GetROINumber());

  // Add an image to the roi
  vvImage::Pointer im = vvImageFromITK<3, PixelType>(m_Input);
  roi->SetImage(im);

  // Get one contour
  DD("Compute Mesh");
  roi->ComputeMeshFromImage();
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
  roi->SetDicomUptodateFlag(false); // indicate that dicom info must be updated from the mesh.
    
  // Convert to dicom ?
  DD("TODO");
  
  // Write
  structset->Write("toto.dcm");
}
//--------------------------------------------------------------------




