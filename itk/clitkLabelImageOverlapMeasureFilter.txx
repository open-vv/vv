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
  ===========================================================================**/

//--------------------------------------------------------------------
template <class ImageType>
clitk::LabelImageOverlapMeasureFilter<ImageType>::
LabelImageOverlapMeasureFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(2);
  SetLabel1(1);
  SetLabel2(1);
  SetBackgroundValue(0);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::LabelImageOverlapMeasureFilter<ImageType>::
GenerateOutputInformation() 
{ 
  // DD("GenerateOutputInformation");
  //ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  // ImagePointer outputImage = this->GetOutput(0);
  // outputImage->SetRegions(outputImage->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::LabelImageOverlapMeasureFilter<ImageType>::
GenerateInputRequestedRegion() 
{
  // DD("GenerateInputRequestedRegion");
  // Call default
  itk::ImageToImageFilter<ImageType, ImageType>::GenerateInputRequestedRegion();
  // Get input pointers and set requested region to common region
  ImagePointer input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  ImagePointer input2 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  input1->SetRequestedRegion(input1->GetLargestPossibleRegion());
  input2->SetRequestedRegion(input2->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::LabelImageOverlapMeasureFilter<ImageType>::
GenerateData() 
{
  // DD("GenerateData");

  // Get input pointer
  m_Input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  m_Input2 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(1));
  static const unsigned int dim = ImageType::ImageDimension;

  // Compute union of bounding boxes
  typedef itk::BoundingBox<unsigned long, dim> BBType;
  typename BBType::Pointer bb1 = BBType::New();
  ComputeBBFromImageRegion<ImageType>(m_Input1, m_Input1->GetLargestPossibleRegion(), bb1);
  typename BBType::Pointer bb2 = BBType::New();
  ComputeBBFromImageRegion<ImageType>(m_Input2, m_Input2->GetLargestPossibleRegion(), bb2);
  typename BBType::Pointer bbo = BBType::New();
  ComputeBBUnion<dim>(bbo, bb1, bb2);

  // Resize like the union
  ImagePointer input1 = clitk::ResizeImageLike<ImageType>(m_Input1, bbo, GetBackgroundValue());
  ImagePointer input2 = clitk::ResizeImageLike<ImageType>(m_Input2, bbo, GetBackgroundValue());

  // Compute overlap image
  ImagePointer image_union = clitk::Clone<ImageType>(input1);
  ImagePointer image_intersection = clitk::Clone<ImageType>(input1);
  clitk::Or<ImageType>(image_union, input2, GetBackgroundValue());
  clitk::And<ImageType>(image_intersection, input2, GetBackgroundValue());
  
  writeImage<ImageType>(image_union, "union.mha");
  writeImage<ImageType>(image_intersection, "intersection.mha");
  
  // Compute size
  typedef itk::LabelStatisticsImageFilter<ImageType, ImageType> StatFilterType;
  typename StatFilterType::Pointer statFilter = StatFilterType::New();
  statFilter->SetInput(image_union);
  statFilter->SetLabelInput(image_union);
  statFilter->Update();
  int u = statFilter->GetCount(GetLabel1());

  statFilter->SetInput(image_intersection);
  statFilter->SetLabelInput(image_intersection);
  statFilter->Update();
  int inter = statFilter->GetCount(GetLabel1());
  
  statFilter->SetInput(m_Input1);
  statFilter->SetLabelInput(m_Input1);
  statFilter->Update();
  int in1 = statFilter->GetCount(GetLabel1());

  statFilter->SetInput(m_Input2);
  statFilter->SetLabelInput(m_Input2);
  statFilter->Update();
  int in2 = statFilter->GetCount(GetLabel1());

  std::cout << in1 << " " << in2 << " " << inter << " " << u << " " << 2.0*(double)inter/(double)(in1+in2) << std::endl;
}
//--------------------------------------------------------------------

