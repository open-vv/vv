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

#ifndef CLITKCROPLIKEIMAGEFILTER_TXX
#define CLITKCROPLIKEIMAGEFILTER_TXX

// clitk
#include "clitkCommon.h"
#include "clitkPasteImageFilter.h"

//--------------------------------------------------------------------
template <class ImageType>
clitk::CropLikeImageFilter<ImageType>::
CropLikeImageFilter():itk::ImageToImageFilter<ImageType, ImageType>() {
  this->SetNumberOfRequiredInputs(1);
  m_LikeImage = NULL;
  m_LikeFilenameIsGiven = false;
  this->SetBackgroundValue(typename PixelTraits<typename ImageType::PixelType>::ValueType(0));
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
GenerateInputRequestedRegion() {
  // Needed because output region can be larger than input
  ImagePointer input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  input->SetRequestedRegion(input->GetLargestPossibleRegion());
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
    output->CopyInformation(m_LikeImage);
  }
  else {
    // Only load the header (allows to use 'like' with any image type)
    if (m_LikeFilenameIsGiven) {
      itk::ImageIOBase::Pointer header = readImageHeader(m_LikeFilename);
      for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
        likeSize[i] = header->GetDimensions(i);
        likeStart[i] = 0;//header->GetIORegion().GetIndex()[i];
        likeOrigin[i] = header->GetOrigin(i);
        likeSpacing[i] = header->GetSpacing(i);
      }
    }
    else {
      clitkExceptionMacro("You should provide SetCropLikeFilename or SetCropLike to CropLikeImageFilter");
    }
  }

  // Check spacing
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    if (likeSpacing[i] != input->GetSpacing()[i]) {
      clitkExceptionMacro("Images must have the same spacing, but input's spacing(" << i
                          <<") is " << input->GetSpacing()[i] << " while the spacing(" << i 
                          << ") of 'like' is " << likeSpacing[i] << ".");
    }
  }

  // Check that we must crop along each dimension. If not, we use the
  // size of the input image
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    if (m_CropAlongThisDimension[i] == false) {
      likeStart[i] = input->GetLargestPossibleRegion().GetIndex()[i];
      likeSize[i] = input->GetLargestPossibleRegion().GetSize()[i];
    }
  }

  // Define output region 
  m_OutputRegion.SetIndex(likeStart);
  m_OutputRegion.SetSize(likeSize);
  output->SetRegions(m_OutputRegion);
  output->SetRequestedRegion(m_OutputRegion);
  output->SetBufferedRegion(m_OutputRegion);
  output->SetSpacing(likeSpacing);  
  output->SetOrigin(likeOrigin);
  output->Allocate(); // Needed ?

  // get startpoint source/dest
  // for each dim
  // if source < dest -> start from dest, compute in source
  // if source > dest -> start from source, compute in dest
  m_StartDestIndex = output->GetLargestPossibleRegion().GetIndex();
  m_StartSourceIndex = input->GetLargestPossibleRegion().GetIndex();
  PointType m_StartPointInSource;
  PointType m_StartPointInDest;
  m_StartSourceIndex = input->GetLargestPossibleRegion().GetIndex();
  input->TransformIndexToPhysicalPoint(m_StartSourceIndex, m_StartPointInSource);
  m_StartDestIndex = output->GetLargestPossibleRegion().GetIndex();
  output->TransformIndexToPhysicalPoint(m_StartDestIndex, m_StartPointInDest);
  IndexType startDestInSource;
  IndexType startSourceInDest;
  input->TransformPhysicalPointToIndex(m_StartPointInDest, startDestInSource);
  output->TransformPhysicalPointToIndex(m_StartPointInSource, startSourceInDest);
  for(int i=0; i<ImageType::ImageDimension; i++) {
    if (m_StartPointInSource[i] < m_StartPointInDest[i]) {
      m_StartSourceIndex[i] = startDestInSource[i];
    }
    else {
      m_StartDestIndex[i] = startSourceInDest[i];
    }
  }
  m_Region.SetIndex(m_StartSourceIndex);

  // Stop index
  m_StopSourceIndex = input->GetLargestPossibleRegion().GetIndex()+
    input->GetLargestPossibleRegion().GetSize();
  m_StopDestIndex = output->GetLargestPossibleRegion().GetIndex()+
    output->GetLargestPossibleRegion().GetSize();
  PointType m_StopPointInSource;
  PointType m_StopPointInDest;
  input->TransformIndexToPhysicalPoint(m_StopSourceIndex, m_StopPointInSource);
  output->TransformIndexToPhysicalPoint(m_StopDestIndex, m_StopPointInDest);
  IndexType stopDestInSource;
  IndexType stopSourceInDest;
  input->TransformPhysicalPointToIndex(m_StopPointInDest, stopDestInSource);
  output->TransformPhysicalPointToIndex(m_StopPointInSource, stopSourceInDest);

  for(int i=0; i<ImageType::ImageDimension; i++) {
    if (m_StopPointInSource[i] > m_StopPointInDest[i]) {
      m_StopSourceIndex[i] = stopDestInSource[i];
    }
    else {
      m_StopDestIndex[i] = stopSourceInDest[i];
    }
  }

  // Set size to the region we want to paste
  SizeType s;
  for(int i=0; i<ImageType::ImageDimension; i++)
    s[i] = m_StopSourceIndex[i]-m_StartSourceIndex[i];
  m_Region.SetSize(s);

}
//--------------------------------------------------------------------
   
//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::CropLikeImageFilter<ImageType>::
GenerateData() {
  // Get input pointers
  ImageConstPointer input = dynamic_cast<const ImageType*>(itk::ProcessObject::GetInput(0));

  // Get output pointer, fill with Background
  ImagePointer output = this->GetOutput(0);
  
  output->FillBuffer(GetBackgroundValue());
  
  // Paste image inside
  typedef clitk::PasteImageFilter<ImageType,ImageType> PasteFilterType;
  typename PasteFilterType::Pointer pasteFilter = PasteFilterType::New();
  //pasteFilter->ReleaseDataFlagOn(); // change nothing ?
  //  pasteFilter->InPlaceOn(); // makt it seg fault
  pasteFilter->SetSourceImage(input);
  pasteFilter->SetDestinationImage(output);
  pasteFilter->SetDestinationIndex(m_StartDestIndex);
  pasteFilter->SetSourceRegion(m_Region);
  pasteFilter->Update();

  // Get (graft) output (SetNthOutput does not fit here because of Origin).
  this->GraftOutput(pasteFilter->GetOutput());
}
//--------------------------------------------------------------------
   

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::ResizeImageLike(const ImageType * input,                       
                       const itk::ImageBase<ImageType::ImageDimension> * like, 
                       typename ImageType::PixelType backgroundValue) 
{
  typedef clitk::CropLikeImageFilter<ImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(input);
  cropFilter->SetCropLikeImage(like);
  cropFilter->SetBackgroundValue(backgroundValue);
  cropFilter->Update();
  return cropFilter->GetOutput();  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::ResizeImageLike(const ImageType * input,                       
                       typename itk::ImageBase<ImageType::ImageDimension>::RegionType * region, 
                       typename ImageType::PixelType backgroundValue) 
{
  typename ImageType::Pointer output = ImageType::New();
  output->CopyInformation(input);
  typename ImageType::RegionType reg;
  reg.SetIndex(region->GetIndex());
  reg.SetSize(region->GetSize());
  output->SetRegions(reg);
  output->Allocate();
  return clitk::ResizeImageLike<ImageType>(input, output, backgroundValue);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::ResizeImageLike(const ImageType * input, 
                       typename itk::BoundingBox<unsigned long, ImageType::ImageDimension>::Pointer bb, 
                       typename ImageType::PixelType BG)
{
  typename ImageType::RegionType region;
  clitk::ComputeRegionFromBB<ImageType>(input, bb, region);
  typename ImageType::Pointer output = ImageType::New();
  output->CopyInformation(input);
  output->SetRegions(region);
  output->Allocate();
  return clitk::ResizeImageLike<ImageType>(input, output, BG);   
}
//--------------------------------------------------------------------

#endif //#define CLITKCROPLIKEIMAGEFILTER_TXX
