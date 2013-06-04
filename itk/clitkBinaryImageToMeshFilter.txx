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
BinaryImageToMeshFilter()//:ProcessObject()
{
  DD("BinaryImageToMeshFilter constructor");
  // this->ProcessObject::SetNumberOfRequiredInputs(1);
  // this->ProcessObject::SetNumberOfRequiredOutputs(1);

  // //m_OutputMesh = vtkPolyData::New();

  // this->ProcessObject::SetNumberOfRequiredOutputs(1);
  // itk::DataObject::Pointer a = itk::DataObject::New();
  // this->ProcessObject::SetNthOutput( 0, a);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*template <class ImageType>
  void 
  clitk::BinaryImageToMeshFilter<ImageType>::
  GenerateOutputInformation() 
  {
  DD("GenerateOutputInformation");
  // ImagePointer output = this->GetOutput(0);
  
  // // Set the region to output
  // typename ImageType::RegionType m_Region = m_LikeImage->GetLargestPossibleRegion();
  // typename ImageType::SizeType size = m_Region.GetSize();
  // size[0]++;
  // size[1]++;
  // size[2]++;
  // m_Region.SetSize(size);  
  // output->SetLargestPossibleRegion(m_Region);
  // output->SetRequestedRegion(m_Region);
  // output->SetBufferedRegion(m_Region);
  // output->SetRegions(m_Region);
  // output->Allocate();
  }
*/
//--------------------------------------------------------------------


// template <class ImageType>
// void
// clitk::BinaryImageToMeshFilter<ImageType>::
// SetInput(unsigned int idx, const ImageType *input)
// {
//   DD(idx);
//   // process object is not const-correct, the const_cast
//   // is required here.
//   this->ProcessObject::SetNthInput( idx,
//                                     const_cast< ImageType * >( input ) );
//   DD("end");
// }

// template <class ImageType>
// const ImageType *
// clitk::BinaryImageToMeshFilter<ImageType>::
// GetInput(unsigned int idx)
// {
//   DD("GetInput");
//   DD(idx);
//   return dynamic_cast< const ImageType * >
//          ( this->ProcessObject::GetInput(idx) );
// }


//--------------------------------------------------------------------
template <class ImageType>
void
clitk::BinaryImageToMeshFilter<ImageType>::
Update()
//GenerateOutputInformation()
{
  DD("Update");

  // Convert itk image into vtk image
  const ImageType * im = this->GetInput();
  typedef itk::ImageToVTKImageFilter<ImageType> ConvertType;
  typename ConvertType::Pointer convert = ConvertType::New();
  convert->SetInput(im);
  convert->Update();
  vtkImageData * input_vtk = convert->GetOutput();
  
  // Get extend
  vtkSmartPointer<vtkImageClip> clipper = vtkSmartPointer<vtkImageClip>::New();
  clipper->SetInput(input_vtk);
  int* extent = input_vtk->GetExtent();
  DDV(extent, 6);

  // Loop on slices
  vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
  uint n = input_vtk->GetDimensions()[2];
  DD(n);
  std::vector<vtkSmartPointer<vtkPolyData> > contours;
  for(uint i=0; i<n; i++) {
    DD(i);
    // FIXME     vtkDiscreteMarchingCubes INSTEAD ?
    vtkSmartPointer<vtkMarchingSquares> squares = vtkSmartPointer<vtkMarchingSquares>::New();
    squares->SetInput(input_vtk);
    squares->SetImageRange(extent[0], extent[1], extent[2], extent[3], i, i);
    squares->SetNumberOfContours(1);
    squares->SetValue(0, 0.5); // FIXME background (?)
    squares->Update();
    // DD(squares->GetNumberOfContours());
    
    vtkSmartPointer<vtkPolyData> m = squares->GetOutput();

    DD(m->GetMaxCellSize());
    DD(m->GetNumberOfVerts());
    DD(m->GetNumberOfLines());
    DD(m->GetNumberOfPolys());
    DD(m->GetNumberOfStrips());

    // Decimate
    if (false) { // FIXME
      vtkSmartPointer<vtkDecimatePro> psurface = vtkDecimatePro::New();
      psurface->SetInputConnection(squares->GetOutputPort());
      psurface->SetTargetReduction(0.5);
      psurface->Update();
      m = psurface->GetOutput();
    }
    if (true) {
      vtkSmartPointer<vtkStripper> vs = vtkSmartPointer<vtkStripper>::New();
      vs->SetInput(squares->GetOutput());
      vs->Update();
      m = vs->GetOutput();
    }
    
    //vtkSmartPointer<vtkPolyData> m = squares->GetOutput();
    contours.push_back(m);
    DD(m->GetMaxCellSize());
    DD(m->GetNumberOfVerts());
    DD(m->GetNumberOfLines());
    DD(m->GetNumberOfPolys());
    DD(m->GetNumberOfStrips());
    // m->Print(std::cout);

    // FIXME : only add if lines>0
    if (m->GetNumberOfLines() > 0) {
      append->AddInput(contours[i]);
    }
  }
  DD("done");
 
  DD("now append");
  // for(unsigned int i=0; i<n; i++) {
  //   append->AddInput(contours[i]);
  // }
  append->Update();
  
  DD(" copy");
  m_OutputMesh = vtkSmartPointer<vtkPolyData>::New();
  m_OutputMesh->DeepCopy(append->GetOutput());

  /* // NO (3D)
  vtkSmartPointer<vtkContourFilter> pcontour = vtkContourFilter::New();
  pcontour->SetValue(0, 0.5);
  pcontour->SetInput(input_vtk);
  pcontour->Update();
  // vtkAlgorithmOutput *data = pcontour->GetOutputPort();
  // vtkSmartPointer<vtkPolyDataMapper> skinMapper = vtkPolyDataMapper::New();
  // skinMapper->SetInputConnection(data); //psurface->GetOutputPort()
  // skinMapper->ScalarVisibilityOff();
  m_OutputMesh = pcontour->GetOutput();//data
  */

  DD("end");
  int a=12;
  DD("erelre");
}
//--------------------------------------------------------------------

