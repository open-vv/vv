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
  ======================================================================-====*/

#include <vtkPolyDataToImageStencil.h>
#include <vtkLinearExtrusionFilter.h>
#include <vtkImageStencil.h>
#include <vtkMetaImageWriter.h>

#include "itkVTKImageImport.h"
#include "vtkImageExport.h"
#include "vtkImageData.h"



//--------------------------------------------------------------------
template <class ImageType>
clitk::MeshToBinaryImageFilter<ImageType>::
MeshToBinaryImageFilter():itk::ImageSource<ImageType>()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::MeshToBinaryImageFilter<ImageType>::
GenerateOutputInformation() 
{
  ImagePointer output = this->GetOutput(0);
  
  // Set the region to output
  typename ImageType::RegionType m_Region = m_LikeImage->GetLargestPossibleRegion();
  typename ImageType::SizeType size = m_Region.GetSize();
  size[0]++;
  size[1]++;
  size[2]++;
  m_Region.SetSize(size);  
  output->SetLargestPossibleRegion(m_Region);
  output->SetRequestedRegion(m_Region);
  output->SetBufferedRegion(m_Region);
  output->SetRegions(m_Region);
  output->Allocate();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::MeshToBinaryImageFilter<ImageType>::
GenerateData() 
{
  // GO
  vtkSmartPointer<vtkImageData> binary_image=vtkSmartPointer<vtkImageData>::New();
  binary_image->SetScalarTypeToUnsignedChar();

  // Set spacing
  PointType samp_origin = m_LikeImage->GetOrigin();
  SpacingType spacing=m_LikeImage->GetSpacing();
  double * spacing2 = new double[3];
  spacing2[0] = spacing[0];
  spacing2[1] = spacing[1];
  spacing2[2] = spacing[2];
  binary_image->SetSpacing(spacing2);

  // Set origin
  /// Put the origin on a voxel to avoid small skips
  binary_image->SetOrigin(samp_origin[0], samp_origin[1], samp_origin[2]);

  // Compute image bounds
  binary_image->SetExtent(0,m_LikeImage->GetLargestPossibleRegion().GetSize()[0],
                          0,m_LikeImage->GetLargestPossibleRegion().GetSize()[1],
                          0,m_LikeImage->GetLargestPossibleRegion().GetSize()[2] 
                          );
  
  // Allocate data
  binary_image->AllocateScalars();
  memset(binary_image->GetScalarPointer(),0,
         binary_image->GetDimensions()[0]*binary_image->GetDimensions()[1]*binary_image->GetDimensions()[2]*sizeof(unsigned char));

  // Image stencil 
  vtkSmartPointer<vtkPolyDataToImageStencil> sts=vtkSmartPointer<vtkPolyDataToImageStencil>::New();
  //The following line is extremely important
  //http://www.nabble.com/Bug-in-vtkPolyDataToImageStencil--td23368312.html#a23370933
  sts->SetTolerance(0);
  sts->SetInformationInput(binary_image);
    
  // Extrusion
  vtkSmartPointer<vtkLinearExtrusionFilter> extrude=vtkSmartPointer<vtkLinearExtrusionFilter>::New();
  extrude->SetInput(m_Mesh);
  // We extrude in the -slice_spacing direction to respect the FOCAL convention 
  extrude->SetVector(0, 0, -m_LikeImage->GetSpacing()[2]);
  sts->SetInput(extrude->GetOutput());

  // Stencil
  vtkSmartPointer<vtkImageStencil> stencil=vtkSmartPointer<vtkImageStencil>::New();
  stencil->SetStencil(sts->GetOutput());
  stencil->SetInput(binary_image);

  // Convert VTK to ITK
  vtkImageExport * m_Exporter = vtkImageExport::New();
  typedef itk::VTKImageImport< ImageType >   ImporterFilterType;
  typename ImporterFilterType::Pointer m_Importer = ImporterFilterType::New();

  m_Importer->SetUpdateInformationCallback( m_Exporter->GetUpdateInformationCallback());
  m_Importer->SetPipelineModifiedCallback( m_Exporter->GetPipelineModifiedCallback());
  m_Importer->SetWholeExtentCallback( m_Exporter->GetWholeExtentCallback());
  m_Importer->SetSpacingCallback( m_Exporter->GetSpacingCallback());
  m_Importer->SetOriginCallback( m_Exporter->GetOriginCallback());
  m_Importer->SetScalarTypeCallback( m_Exporter->GetScalarTypeCallback());
  m_Importer->SetNumberOfComponentsCallback( m_Exporter->GetNumberOfComponentsCallback());
  m_Importer->SetPropagateUpdateExtentCallback( m_Exporter->GetPropagateUpdateExtentCallback());
  m_Importer->SetUpdateDataCallback( m_Exporter->GetUpdateDataCallback());
  m_Importer->SetDataExtentCallback( m_Exporter->GetDataExtentCallback());
  m_Importer->SetBufferPointerCallback( m_Exporter->GetBufferPointerCallback());
  m_Importer->SetCallbackUserData( m_Exporter->GetCallbackUserData());

  m_Exporter->SetInput( stencil->GetOutput() );
  m_Importer->Update();

  writeImage<ImageType>(m_Importer->GetOutput(), "f.mhd");

  this->SetNthOutput(0, m_Importer->GetOutput());

  return;
}
//--------------------------------------------------------------------

