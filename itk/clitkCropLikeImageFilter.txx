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

#ifndef CLITKCROPLIKEIMAGEFILTER_TXX
#define CLITKCROPLIKEIMAGEFILTER_TXX

// clitk
#include "clitkCommon.h"

// itk
#include "itkRegionOfInterestImageFilter.h"


//--------------------------------------------------------------------
template <class ImageType>
clitk::CropLikeImageFilter<ImageType>::
CropLikeImageFilter():itk::ImageToImageFilter<ImageType, ImageType>() {
  this->SetNumberOfRequiredInputs(1);
  m_LikeImage = NULL;
  m_LikeFilenameIsGiven = false;
  m_CropAlongThisDimension.resize(ImageType::ImageDimension);
  for(uint i=0; i<ImageType::ImageDimension; i++)
    m_CropAlongThisDimension[i] = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::CropLikeImageFilter<ImageType>::
SetCropLikeFilename(std::string f) 
{
  m_LikeFilename = f;
  m_LikeFilenameIsGiven = true;
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::CropLikeImageFilter<ImageType>::
SetCropLikeImage(const itk::ImageBase<ImageType::ImageDimension> * like)
{
  m_LikeImage = like;
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::CropLikeImageFilter<ImageType>::
SetCropLikeImage(const itk::ImageBase<ImageType::ImageDimension> * like, int axe)
{
  m_LikeImage = like;
  for(uint i=0; i<ImageType::ImageDimension; i++)
    m_CropAlongThisDimension[i] = false;
  m_CropAlongThisDimension[axe] = true;
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::CropLikeImageFilter<ImageType>::
SetInput(const ImageType * image) {
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<ImageType *>( image ));
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::CropLikeImageFilter<ImageType>::
GenerateOutputInformation() {    
  // Get input pointers
  ImageConstPointer input = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
    
  // Get output pointer
  ImagePointer output = this->GetOutput(0);
  
  // Get input info
  typename ImageType::SizeType likeSize;
  typename ImageType::IndexType likeStart;
  typename ImageType::PointType likeOrigin;  
  typename ImageType::SpacingType likeSpacing;  
  if (m_LikeImage) {     
    likeSize = m_LikeImage->GetLargestPossibleRegion().GetSize();
    likeStart = m_LikeImage->GetLargestPossibleRegion().GetIndex();
    likeOrigin = m_LikeImage->GetOrigin();
    likeSpacing = m_LikeImage->GetSpacing();
  }
  else {
    if (m_LikeFilenameIsGiven) {
      itk::ImageIOBase::Pointer header = readImageHeader(m_LikeFilename);
      for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
        likeSize[i] = header->GetIORegion().GetSize()[i]; //GetDimensions(i);
        likeStart[i] = header->GetIORegion().GetIndex()[i];
        likeOrigin[i] = header->GetOrigin(i);
        likeSpacing[i] = header->GetSpacing(i);
      }
    }
    else {
      clitkExceptionMacro("You should provide SetCropLikeFilename or SetCropLike to CropLikeImageFilter");
    }
  }

  // Compute region
  typename ImageType::SizeType size;
  typename ImageType::IndexType start;
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    double ol;
    if (m_CropAlongThisDimension[i]) {
      size[i] = likeSize[i];
      ol = likeOrigin[i];
    }
    else {
      size[i] = input->GetLargestPossibleRegion().GetSize()[i];
      ol = input->GetOrigin()[i];
    }
    double oi = input->GetOrigin()[i];
    start[i] = lrint((ol-oi)/input->GetSpacing()[i]);
    m_Origin[i] = likeOrigin[i];
    if (likeSpacing[i] != input->GetSpacing()[i]) {
      clitkExceptionMacro("Images must have the same spacing, but input's spacing(" << i
                          <<") is " << input->GetSpacing()[i] << " while like's spacing(" << i 
                          << ") is " << likeSpacing[i] << ".");
    }
  }

  m_Region.SetSize(size);
  m_Region.SetIndex(start);
  output->SetRegions(m_Region);
  output->SetSpacing(input->GetSpacing());
}
//--------------------------------------------------------------------
   
//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::CropLikeImageFilter<ImageType>::
GenerateData() {
  // Get input pointers
  ImageConstPointer input = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));
  
  typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(input);
  cropFilter->SetReleaseDataFlag(this->GetReleaseDataFlag());
  cropFilter->SetRegionOfInterest(m_Region);
  cropFilter->Update();

  // Get (graft) output (SetNthOutput does not fit here because of Origin).
  this->GraftOutput(cropFilter->GetOutput());
}
//--------------------------------------------------------------------
   
 
#endif //#define CLITKAUTOCROPFILTER
