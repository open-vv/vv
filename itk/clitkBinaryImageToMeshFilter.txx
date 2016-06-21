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


//--------------------------------------------------------------------
template <class ImageType>
clitk::BinaryImageToMeshFilter<ImageType>::
BinaryImageToMeshFilter():itk::Object()
{
  m_ThresholdValue = 0.5; // (for image with 0=background and 1=foreground) 
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::BinaryImageToMeshFilter<ImageType>::
Update()
{
  // Convert itk image into vtk image
  const ImageType * im = this->GetInput();
  typedef itk::ImageToVTKImageFilter<ImageType> ConvertType;
  typename ConvertType::Pointer convert = ConvertType::New();
  convert->SetInput(im);
  convert->Update();
  vtkImageData * input_vtk = convert->GetOutput();
  
  // Get extend
  vtkSmartPointer<vtkImageClip> clipper = vtkSmartPointer<vtkImageClip>::New();
#if VTK_MAJOR_VERSION <= 5
  clipper->SetInput(input_vtk);
#else
  clipper->SetInputData(input_vtk);
#endif
  int* extent = input_vtk->GetExtent();

  // Loop on slices
  vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  uint n = input_vtk->GetDimensions()[2];
  // std::vector<vtkSmartPointer<vtkPolyData> > contours;
  for(uint i=0; i<n; i++) {
    vtkSmartPointer<vtkMarchingSquares> squares = vtkSmartPointer<vtkMarchingSquares>::New();
#if VTK_MAJOR_VERSION <= 5
    squares->SetInput(input_vtk);
#else
    squares->SetInputData(input_vtk);
#endif
    squares->SetImageRange(extent[0], extent[1], extent[2], extent[3], i, i);
    squares->SetNumberOfContours(1);
    squares->SetValue(0, m_ThresholdValue);
    squares->Update();    
    vtkSmartPointer<vtkPolyData> m = squares->GetOutput();

    // Strip (needed)
    vtkSmartPointer<vtkStripper> vs = vtkSmartPointer<vtkStripper>::New();
#if VTK_MAJOR_VERSION <= 5
    vs->SetInput(squares->GetOutput());
#else
    vs->SetInputData(squares->GetOutput());
#endif
    vs->Update();
    m = vs->GetOutput();

    // Decimate
    if (false) { // FIXME (do not work)
      vtkSmartPointer<vtkDecimatePro> psurface = vtkDecimatePro::New();
      psurface->SetInputConnection(squares->GetOutputPort());
      psurface->SetTargetReduction(0.5);
      psurface->Update();
      m = psurface->GetOutput();
    }

    // only add if lines>0
    if (m->GetNumberOfLines() > 0) {
#if VTK_MAJOR_VERSION <= 5
      append->AddInput(m);//contours[i]);
#else
      append->AddInputData(m);//contours[i]);
#endif
    }
  }
  append->Update();
  
  m_OutputMesh = vtkSmartPointer<vtkPolyData>::New();
  m_OutputMesh->DeepCopy(append->GetOutput());
}
//--------------------------------------------------------------------

