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


//--------------------------------------------------------------------
template <class ImageType>
clitk::ExtractSliceFilter<ImageType>::
ExtractSliceFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetDirection(2);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractSliceFilter<ImageType>::
SetInput(const ImageType * image) {
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<ImageType *>(image));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractSliceFilter<ImageType>::
GenerateOutputInformation() { 
  DD("GenerateOutputInformation");
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  //   ImagePointer outputImage = this->GetOutput(0);
  //   outputImage->SetRegions(input->GetLargestPossibleRegion());
  
  output = this->GetOutput(0);
  
  // create vector
  typename SliceType::RegionType SliceRegionType;
  typename SliceType::SizeType SliceSizeType;
  typename SliceType::IndexType SliceIndexType;
  // SliceRegionType region;

  // create region
  // loop ExtractImageFilter with region updated, push_back


}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractSliceFilter<ImageType>::
GenerateInputRequestedRegion() {
  DD("GenerateInputRequestedRegion");
  // Call default
  itk::ImageToImageFilter<ImageType, ImageType>::GenerateInputRequestedRegion();
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
  DD("GenerateData");

  // Get input pointer
  input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));



  //--------------------------------------------------------------------
  //--------------------------------------------------------------------  
  // Final Step -> set output
  //this->SetNthOutput(0, working_image);
  return;
}
//--------------------------------------------------------------------

