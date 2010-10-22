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
#include "clitkSetBackgroundImageFilter.h"

// itk
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkPasteImageFilter.h>

//--------------------------------------------------------------------
template<class ImageType>
void clitk::ComputeBBFromImageRegion(typename ImageType::Pointer image, 
                                     typename ImageType::RegionType region,
                                     typename itk::BoundingBox<unsigned long, 
                                     ImageType::ImageDimension>::Pointer bb) {
  typedef typename ImageType::IndexType IndexType;
  IndexType firstIndex;
  IndexType lastIndex;
  for(unsigned int i=0; i<image->GetImageDimension(); i++) {
    firstIndex[i] = region.GetIndex()[i];
    lastIndex[i] = firstIndex[i]+region.GetSize()[i];
  }

  typedef itk::BoundingBox<unsigned long, 
    ImageType::ImageDimension> BBType;
  typedef typename BBType::PointType PointType;
  PointType lastPoint;
  PointType firstPoint;
  image->TransformIndexToPhysicalPoint(firstIndex, firstPoint);
  image->TransformIndexToPhysicalPoint(lastIndex, lastPoint);

  bb->SetMaximum(lastPoint);
  bb->SetMinimum(firstPoint);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<int Dimension>
void clitk::ComputeBBIntersection(typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbo, 
                                  typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi1, 
                                  typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi2) {

  typedef itk::BoundingBox<unsigned long, Dimension> BBType;
  typedef typename BBType::PointType PointType;
  PointType lastPoint;
  PointType firstPoint;

  for(unsigned int i=0; i<Dimension; i++) {
    firstPoint[i] = std::max(bbi1->GetMinimum()[i], 
                             bbi2->GetMinimum()[i]);
    lastPoint[i] = std::min(bbi1->GetMaximum()[i], 
                            bbi2->GetMaximum()[i]);
  }

  bbo->SetMaximum(lastPoint);
  bbo->SetMinimum(firstPoint);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void clitk::ComputeRegionFromBB(typename ImageType::Pointer image, 
                                const typename itk::BoundingBox<unsigned long, 
                                ImageType::ImageDimension>::Pointer bb, 
                                typename ImageType::RegionType & region) {
  // Types
  typedef typename ImageType::IndexType  IndexType;
  typedef typename ImageType::PointType  PointType;
  typedef typename ImageType::RegionType RegionType;
  typedef typename ImageType::SizeType   SizeType;

  // Region starting point
  IndexType regionStart;
  PointType start = bb->GetMinimum();
  image->TransformPhysicalPointToIndex(start, regionStart);
    
  // Region size
  SizeType regionSize;
  PointType maxs = bb->GetMaximum();
  PointType mins = bb->GetMinimum();
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    // DD(maxs[i]);
    // DD(mins[i]);
    // DD((maxs[i] - mins[i])/image->GetSpacing()[i]);
    regionSize[i] = lrint((maxs[i] - mins[i])/image->GetSpacing()[i]);
    // DD(regionSize[i]);
  }
   
  // Create region
  region.SetIndex(regionStart);
  region.SetSize(regionSize);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType, class TMaskImageType>
typename ImageType::Pointer
clitk::SetBackground(//typename ImageType::ConstPointer input, 
                     const ImageType * input, 
                     const TMaskImageType * mask, 
                     typename TMaskImageType::PixelType maskBG,
                     typename ImageType::PixelType outValue) {
  typedef clitk::SetBackgroundImageFilter<ImageType, TMaskImageType, ImageType> SetBackgroundImageFilterType;
  typename SetBackgroundImageFilterType::Pointer setBackgroundFilter = SetBackgroundImageFilterType::New();
  setBackgroundFilter->SetInput(input);
  setBackgroundFilter->SetInput2(mask);
  setBackgroundFilter->SetMaskValue(maskBG);
  setBackgroundFilter->SetOutsideValue(outValue);
  setBackgroundFilter->Update();
  return setBackgroundFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
int clitk::GetNumberOfConnectedComponentLabels(typename ImageType::Pointer input, 
                                               typename ImageType::PixelType BG, 
                                               bool isFullyConnected) {
  // Connected Component label 
  typedef itk::ConnectedComponentImageFilter<ImageType, ImageType> ConnectFilterType;
  typename ConnectFilterType::Pointer connectFilter = ConnectFilterType::New();
  connectFilter->SetInput(input);
  connectFilter->SetBackgroundValue(BG);
  connectFilter->SetFullyConnected(isFullyConnected);
  connectFilter->Update();
  
  // Return result
  return connectFilter->GetObjectCount();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::Labelize(typename ImageType::Pointer input, 
                typename ImageType::PixelType BG, 
                bool isFullyConnected, 
                int minimalComponentSize) {
  // InternalImageType for storing large number of component
  typedef itk::Image<int, ImageType::ImageDimension> InternalImageType;
  
  // Connected Component label 
  typedef itk::ConnectedComponentImageFilter<ImageType, InternalImageType> ConnectFilterType;
  typename ConnectFilterType::Pointer connectFilter = ConnectFilterType::New();
  connectFilter->SetInput(input);
  connectFilter->SetBackgroundValue(BG);
  connectFilter->SetFullyConnected(isFullyConnected);
  
  // Sort by size and remove too small area.
  typedef itk::RelabelComponentImageFilter<InternalImageType, ImageType> RelabelFilterType;
  typename RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
  relabelFilter->InPlaceOn();
  relabelFilter->SetInput(connectFilter->GetOutput());
  relabelFilter->SetMinimumObjectSize(minimalComponentSize);
  relabelFilter->Update();

  // Return result
  return relabelFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::RemoveLabels(typename ImageType::Pointer input, 
                    typename ImageType::PixelType BG,
                    std::vector<typename ImageType::PixelType> & labelsToRemove) {
  typename ImageType::Pointer working_image = input;
  for (unsigned int i=0; i <labelsToRemove.size(); i++) {
    typedef clitk::SetBackgroundImageFilter<ImageType, ImageType> SetBackgroundImageFilterType;
    typename SetBackgroundImageFilterType::Pointer setBackgroundFilter = SetBackgroundImageFilterType::New();
    setBackgroundFilter->SetInput(input);
    setBackgroundFilter->SetInput2(input);
    setBackgroundFilter->SetMaskValue(labelsToRemove[i]);
    setBackgroundFilter->SetOutsideValue(BG);
    setBackgroundFilter->Update();
    working_image = setBackgroundFilter->GetOutput();
  }
  return working_image;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::KeepLabels(typename ImageType::Pointer input, 
                  typename ImageType::PixelType BG, 
                  typename ImageType::PixelType FG, 
                  typename ImageType::PixelType firstKeep, 
                  typename ImageType::PixelType lastKeep, 
                  bool useLastKeep) {
  typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> BinarizeFilterType; 
  typename BinarizeFilterType::Pointer binarizeFilter = BinarizeFilterType::New();
  binarizeFilter->SetInput(input);
  binarizeFilter->SetLowerThreshold(firstKeep);
  if (useLastKeep) binarizeFilter->SetUpperThreshold(lastKeep);
  binarizeFilter->SetInsideValue(FG);
  binarizeFilter->SetOutsideValue(BG);
  binarizeFilter->Update();
  return binarizeFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::LabelizeAndSelectLabels(typename ImageType::Pointer input,
                               typename ImageType::PixelType BG, 
                               typename ImageType::PixelType FG, 
                               bool isFullyConnected,
                               int minimalComponentSize,
                               LabelizeParameters<typename ImageType::PixelType> * param)
{
  typename ImageType::Pointer working_image;
  working_image = Labelize<ImageType>(input, BG, isFullyConnected, minimalComponentSize);
  working_image = RemoveLabels<ImageType>(working_image, BG, param->GetLabelsToRemove());
  working_image = KeepLabels<ImageType>(working_image, 
                                        BG, FG, 
                                        param->GetFirstKeep(), 
                                        param->GetLastKeep(), 
                                        param->GetUseLastKeep());
  return working_image;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::EnlargeImageLike(typename ImageType::Pointer input,
                        typename ImageType::Pointer like, 
                        typename ImageType::PixelType backgroundValue) 
{
  if (!HaveSameSpacing<ImageType, ImageType>(input, like)) {
    FATAL("Images must have the same spacing");
  }

  typename ImageType::Pointer output = ImageType::New();
  typename ImageType::SizeType size;
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    size[i] = lrint((like->GetLargestPossibleRegion().GetSize()[i]*like->GetSpacing()[i])/
                    (double)like->GetSpacing()[i]);
  }
  // DD(size);
  typename ImageType::RegionType region;
  region.SetSize(size);
  output->SetRegions(region);
  output->SetSpacing(like->GetSpacing());
  output->SetOrigin(like->GetOrigin());
  output->Allocate();
  output->FillBuffer(backgroundValue);
  typedef itk::PasteImageFilter<ImageType,ImageType> PasteFilterType;
  typename PasteFilterType::Pointer pasteFilter = PasteFilterType::New();
  typename PasteFilterType::InputImageIndexType index;
  for(unsigned int i=0; i<ImageType::ImageDimension; i++) {
    index[i] = lrint((input->GetOrigin()[i] - like->GetOrigin()[i])/(double)input->GetSpacing()[i]);
  }
  // DD(index);
  pasteFilter->SetSourceImage(input);
  pasteFilter->SetDestinationImage(output);
  pasteFilter->SetDestinationIndex(index);
  pasteFilter->SetSourceRegion(input->GetLargestPossibleRegion());
  pasteFilter->Update();
  return pasteFilter->GetOutput();  
}
//--------------------------------------------------------------------
