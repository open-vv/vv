/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ======================================================================-====*/

// clitk
#include "clitkCommon.h"

// itk
#include <itkExtractImageFilter.h>

//--------------------------------------------------------------------
template <class ImageType>
clitk::ExtractSliceFilter<ImageType>::
ExtractSliceFilter():
  clitk::FilterBase(),
  Superclass()
{
  this->SetNumberOfRequiredInputs(1);
  SetDirection(2);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractSliceFilter<ImageType>::
SetInput(const ImageType * image) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void
clitk::ExtractSliceFilter<ImageType>::
GetOutputSlices(std::vector<typename SliceType::Pointer> & o)
{
  o.clear();
  for(unsigned int i=0; i<this->GetNumberOfOutputs(); i++) {
    o.push_back(this->GetOutput(i));
  }
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractSliceFilter<ImageType>::
GenerateOutputInformation() 
{ 
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  
  // Create region to extract
  m_region = input->GetLargestPossibleRegion();
  m_size   = m_region.GetSize();
  m_index  = m_region.GetIndex();
  m_NumberOfSlices = m_region.GetSize()[GetDirection()];
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractSliceFilter<ImageType>::
GenerateInputRequestedRegion() {
  // Call default
  Superclass::GenerateInputRequestedRegion();
  // Get input pointers and set requested region to common region
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  input->SetRequestedRegion(input->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractSliceFilter<ImageType>::
GenerateData() {
  //--------------------------------------------------------------------
  // Get input pointer
  input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));

  //--------------------------------------------------------------------
  // Create region to extract in 3D (and 2D = not used)
  m_size[GetDirection()] = 0;
  m_region.SetSize(m_size);
  int start = m_index[GetDirection()];
  this->SetNumberOfOutputs(m_NumberOfSlices);

  //--------------------------------------------------------------------
  // loop ExtractImageFilter with region updated, push_back
  typedef itk::ExtractImageFilter<ImageType, SliceType> ExtractImageFilterType;
  typename ExtractImageFilterType::Pointer extract = ExtractImageFilterType::New();
  extract->SetInput(input);
  for(int i=0; i<m_NumberOfSlices; i++) {
    extract = ExtractImageFilterType::New();
    extract->SetInput(input);
    m_index[GetDirection()] = start + i;
    m_region.SetIndex(m_index);
    extract->SetExtractionRegion(m_region);
    extract->Update();
    SetNthOutput(i, extract->GetOutput());
  }
  return;
}
//--------------------------------------------------------------------

