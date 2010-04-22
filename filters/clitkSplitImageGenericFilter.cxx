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
/**
 -------------------------------------------------------------------
 * @file   clitkSplitImageGenericFilter.cxx
 * @author Joël Schaerer
 * @date   20 April 2009

 * @brief  
 -------------------------------------------------------------------*/

#include "clitkSplitImageGenericFilter.h"

#include "clitkSplitImageGenericFilter.txx"
//--------------------------------------------------------------------
clitk::SplitImageGenericFilter::SplitImageGenericFilter():
  clitk::ImageToImageGenericFilter<Self>("SplitImage") {
  mSplitDimension = 0;
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::SplitImageGenericFilter::InitializeImageType() {      
  ADD_DEFAULT_IMAGE_TYPES(Dim);
  ADD_VEC_IMAGE_TYPE(Dim, 3,float);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
void clitk::SplitImageGenericFilter::UpdateWithInputImageType() {

  // Read input
  typedef typename ImageType::PixelType PixelType;
  typedef itk::Image<PixelType,ImageType::ImageDimension-1> OutputImageType;
  typename ImageType::Pointer input = this->GetInput<ImageType>(0);
  typedef itk::ExtractImageFilter<ImageType,OutputImageType> FilterType;
  typename FilterType::Pointer filter= FilterType::New();

  filter->SetInput(input);
  typename ImageType::SizeType size=input->GetLargestPossibleRegion().GetSize();
  size[mSplitDimension]=0;
  typename ImageType::RegionType extracted_region;
  extracted_region.SetSize(size);
  filter->SetExtractionRegion(extracted_region);
  filter->Update();

  typename ImageType::IndexType index=input->GetLargestPossibleRegion().GetIndex();
  std::string base_filename=GetOutputFilename();
  unsigned int number_of_output_images=input->GetLargestPossibleRegion().GetSize()[mSplitDimension];
  for (unsigned int i=0;i<number_of_output_images;i++)
  {
      std::ostringstream ss;
      ss << i;
      index[mSplitDimension]=i;
      extracted_region.SetIndex(index);
      filter->SetExtractionRegion(extracted_region);
      filter->Update();
      SetOutputFilename(base_filename+"_"+ss.str()+".mhd");
      typename OutputImageType::Pointer output=filter->GetOutput();
      SetNextOutput<OutputImageType>(output);
  }
}
//--------------------------------------------------------------------
