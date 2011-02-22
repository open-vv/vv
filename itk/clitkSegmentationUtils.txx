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
#include "clitkSliceBySliceRelativePositionFilter.h"
#include "clitkCropLikeImageFilter.h"
#include "clitkMemoryUsage.h"

// itk
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkPasteImageFilter.h>
#include <itkStatisticsLabelMapFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkImageSliceIteratorWithIndex.h>

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
clitk::SetBackground(const ImageType * input, 
                     const TMaskImageType * mask, 
                     typename TMaskImageType::PixelType maskBG,
                     typename ImageType::PixelType outValue, 
                     bool inPlace) {
  typedef clitk::SetBackgroundImageFilter<ImageType, TMaskImageType, ImageType> 
    SetBackgroundImageFilterType;
  typename SetBackgroundImageFilterType::Pointer setBackgroundFilter 
    = SetBackgroundImageFilterType::New();
  //  if (inPlace) setBackgroundFilter->ReleaseDataFlagOn(); // No seg fault
  setBackgroundFilter->SetInPlace(inPlace); // This is important to keep memory low
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
/*
  Warning : in this cas, we consider outputType like inputType, not
  InternalImageType. Be sure it fits.
 */
template<class ImageType>
typename ImageType::Pointer
clitk::Labelize(const ImageType * input, 
                typename ImageType::PixelType BG, 
                bool isFullyConnected, 
                int minimalComponentSize) {
  // InternalImageType for storing large number of component
  typedef itk::Image<int, ImageType::ImageDimension> InternalImageType;
  
  // Connected Component label 
  typedef itk::ConnectedComponentImageFilter<ImageType, InternalImageType> ConnectFilterType;
  typename ConnectFilterType::Pointer connectFilter = ConnectFilterType::New();
  //  connectFilter->ReleaseDataFlagOn(); 
  connectFilter->SetInput(input);
  connectFilter->SetBackgroundValue(BG);
  connectFilter->SetFullyConnected(isFullyConnected);
  
  // Sort by size and remove too small area.
  typedef itk::RelabelComponentImageFilter<InternalImageType, ImageType> RelabelFilterType;
  typename RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
  //  relabelFilter->ReleaseDataFlagOn(); // if yes, fail when ExplosionControlledThresholdConnectedImageFilter ???
  relabelFilter->SetInput(connectFilter->GetOutput());
  relabelFilter->SetMinimumObjectSize(minimalComponentSize);
  relabelFilter->Update();

  // DD(relabelFilter->GetNumberOfObjects());
  // DD(relabelFilter->GetOriginalNumberOfObjects());
  // DD(relabelFilter->GetSizeOfObjectsInPhysicalUnits()[0]);

  // Return result
  typename ImageType::Pointer output = relabelFilter->GetOutput();
  return output;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*
  Warning : in this cas, we consider outputType like inputType, not
  InternalImageType. Be sure it fits.
 */
template<class ImageType>
typename ImageType::Pointer
clitk::LabelizeAndCountNumberOfObjects(const ImageType * input, 
                                       typename ImageType::PixelType BG, 
                                       bool isFullyConnected, 
                                       int minimalComponentSize, 
                                       int & nb) {
  // InternalImageType for storing large number of component
  typedef itk::Image<int, ImageType::ImageDimension> InternalImageType;
  
  // Connected Component label 
  typedef itk::ConnectedComponentImageFilter<ImageType, InternalImageType> ConnectFilterType;
  typename ConnectFilterType::Pointer connectFilter = ConnectFilterType::New();
  //  connectFilter->ReleaseDataFlagOn(); 
  connectFilter->SetInput(input);
  connectFilter->SetBackgroundValue(BG);
  connectFilter->SetFullyConnected(isFullyConnected);
  
  // Sort by size and remove too small area.
  typedef itk::RelabelComponentImageFilter<InternalImageType, ImageType> RelabelFilterType;
  typename RelabelFilterType::Pointer relabelFilter = RelabelFilterType::New();
  //  relabelFilter->ReleaseDataFlagOn(); // if yes, fail when ExplosionControlledThresholdConnectedImageFilter ???
  relabelFilter->SetInput(connectFilter->GetOutput());
  relabelFilter->SetMinimumObjectSize(minimalComponentSize);
  relabelFilter->Update();

  nb = relabelFilter->GetNumberOfObjects();
  // DD(relabelFilter->GetNumberOfObjects());
  // DD(relabelFilter->GetOriginalNumberOfObjects());
  // DD(relabelFilter->GetSizeOfObjectsInPhysicalUnits()[0]);

  // Return result
  typename ImageType::Pointer output = relabelFilter->GetOutput();
  return output;
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
clitk::KeepLabels(const ImageType * input, 
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
clitk::ResizeImageLike(typename ImageType::Pointer input,
                       typename ImageType::Pointer like, 
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
template<class MaskImageType>
typename MaskImageType::Pointer
clitk::SliceBySliceRelativePosition(const MaskImageType * input,
				    const MaskImageType * object,
				    int direction, 
				    double threshold, 
				    std::string orientation, 
                                    bool uniqueConnectedComponent, 
                                    double spacing, 
				    bool inverseflag) 
{
  typedef clitk::SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
  typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
  sliceRelPosFilter->VerboseStepFlagOff();
  sliceRelPosFilter->WriteStepFlagOff();
  sliceRelPosFilter->SetInput(input);
  sliceRelPosFilter->SetInputObject(object);
  sliceRelPosFilter->SetDirection(direction);
  sliceRelPosFilter->SetFuzzyThreshold(threshold);
  sliceRelPosFilter->AddOrientationTypeString(orientation);
  sliceRelPosFilter->SetResampleBeforeRelativePositionFilter((spacing != -1));
  sliceRelPosFilter->SetIntermediateSpacing(spacing);
  sliceRelPosFilter->SetUniqueConnectedComponentBySlice(uniqueConnectedComponent);
  sliceRelPosFilter->SetInverseOrientationFlag(inverseflag);
  //  sliceRelPosFilter->SetAutoCropFlag(true); ??
  sliceRelPosFilter->Update();
  return sliceRelPosFilter->GetOutput();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
bool
clitk::FindExtremaPointInAGivenDirection(const ImageType * input, 
                                         typename ImageType::PixelType bg, 
                                         int direction, bool opposite, 
                                         typename ImageType::PointType & point)
{
  typename ImageType::PointType dummy;
  return clitk::FindExtremaPointInAGivenDirection(input, bg, direction, 
                                                  opposite, dummy, 0, point);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
bool
clitk::FindExtremaPointInAGivenDirection(const ImageType * input, 
                                         typename ImageType::PixelType bg, 
                                         int direction, bool opposite, 
                                         typename ImageType::PointType refpoint,
                                         double distanceMax, 
                                         typename ImageType::PointType & point)
{
  /*
    loop over input pixels, store the index in the fg that is max
    according to the given direction. 
  */
  typedef itk::ImageRegionConstIteratorWithIndex<ImageType> IteratorType;
  IteratorType iter(input, input->GetLargestPossibleRegion());
  iter.GoToBegin();
  typename ImageType::IndexType max = input->GetLargestPossibleRegion().GetIndex();
  if (opposite) max = max+input->GetLargestPossibleRegion().GetSize();
  bool found=false;
  while (!iter.IsAtEnd()) {
    if (iter.Get() != bg) {
      bool test = iter.GetIndex()[direction] >  max[direction];
      if (opposite) test = !test;
      if (test) {
        typename ImageType::PointType p;
        input->TransformIndexToPhysicalPoint(iter.GetIndex(), p);
        if ((distanceMax==0) || (p.EuclideanDistanceTo(refpoint) < distanceMax)) {
          max = iter.GetIndex();
          found = true;
        }
      }
    }
    ++iter;
  }
  if (!found) return false;
  input->TransformIndexToPhysicalPoint(max, point);
  return true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::CropImageAbove(typename ImageType::Pointer image, 
                      int dim, double min, 
                      bool autoCrop,
                      typename ImageType::PixelType BG) 
{
  return clitk::CropImageAlongOneAxis<ImageType>(image, dim, 
                                                 image->GetOrigin()[dim], 
                                                 min,
                                                 autoCrop, BG);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::CropImageBelow(typename ImageType::Pointer image, 
                      int dim, double max, 
                      bool autoCrop,
                      typename ImageType::PixelType BG) 
{
  typename ImageType::PointType p;
  image->TransformIndexToPhysicalPoint(image->GetLargestPossibleRegion().GetIndex()+
                                       image->GetLargestPossibleRegion().GetSize(), p);
  return clitk::CropImageAlongOneAxis<ImageType>(image, dim, max, p[dim], autoCrop, BG);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::CropImageAlongOneAxis(typename ImageType::Pointer image, 
                             int dim, double min, double max, 
                             bool autoCrop,
                             typename ImageType::PixelType BG) 
{
  // Compute region size
  typename ImageType::RegionType region;
  typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
  typename ImageType::PointType p = image->GetOrigin();
  p[dim] = min;
  typename ImageType::IndexType start;
  image->TransformPhysicalPointToIndex(p, start);
  p[dim] = max;
  typename ImageType::IndexType end;
  image->TransformPhysicalPointToIndex(p, end);
  size[dim] = fabs(end[dim]-start[dim]);
  region.SetIndex(start);
  region.SetSize(size);
  
  // Perform Crop
  typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(image);
  cropFilter->SetRegionOfInterest(region);
  cropFilter->Update();
  typename ImageType::Pointer result = cropFilter->GetOutput();
  
  // Auto Crop
  if (autoCrop) {
    result = clitk::AutoCrop<ImageType>(result, BG);
  }
  return result;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void
clitk::ComputeCentroids(typename ImageType::Pointer image, 
                        typename ImageType::PixelType BG, 
                        std::vector<typename ImageType::PointType> & centroids) 
{
  typedef long LabelType;
  static const unsigned int Dim = ImageType::ImageDimension;
  typedef itk::ShapeLabelObject< LabelType, Dim > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;
  typedef itk::LabelImageToLabelMapFilter<ImageType, LabelMapType> ImageToMapFilterType;
  typename ImageToMapFilterType::Pointer imageToLabelFilter = ImageToMapFilterType::New(); 
  typedef itk::ShapeLabelMapFilter<LabelMapType, ImageType> ShapeFilterType; 
  typename ShapeFilterType::Pointer statFilter = ShapeFilterType::New();
  imageToLabelFilter->SetBackgroundValue(BG);
  imageToLabelFilter->SetInput(image);
  statFilter->SetInput(imageToLabelFilter->GetOutput());
  statFilter->Update();
  typename LabelMapType::Pointer labelMap = statFilter->GetOutput();

  centroids.clear();
  typename ImageType::PointType dummy;
  centroids.push_back(dummy); // label 0 -> no centroid, use dummy point
  for(uint i=1; i<labelMap->GetNumberOfLabelObjects()+1; i++) {
    centroids.push_back(labelMap->GetLabelObject(i)->GetCentroid());
  } 
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void
clitk::ExtractSlices(typename ImageType::Pointer image, 
		     int direction, 
		     std::vector<typename itk::Image<typename ImageType::PixelType, 
		     ImageType::ImageDimension-1>::Pointer > & slices) 
{
  typedef clitk::ExtractSliceFilter<ImageType> ExtractSliceFilterType;
  typedef typename ExtractSliceFilterType::SliceType SliceType;
  typename ExtractSliceFilterType::Pointer 
    extractSliceFilter = ExtractSliceFilterType::New();
  extractSliceFilter->SetInput(image);
  extractSliceFilter->SetDirection(direction);
  extractSliceFilter->Update();
  extractSliceFilter->GetOutputSlices(slices);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::JoinSlices(std::vector<typename itk::Image<typename ImageType::PixelType, 
		  ImageType::ImageDimension-1>::Pointer > & slices, 
		  typename ImageType::Pointer input, 
		  int direction) {
  typedef typename itk::Image<typename ImageType::PixelType, ImageType::ImageDimension-1> SliceType;
  typedef itk::JoinSeriesImageFilter<SliceType, ImageType> JoinSeriesFilterType;
  typename JoinSeriesFilterType::Pointer joinFilter = JoinSeriesFilterType::New();
  joinFilter->SetOrigin(input->GetOrigin()[direction]);
  joinFilter->SetSpacing(input->GetSpacing()[direction]);
  for(unsigned int i=0; i<slices.size(); i++) {
    joinFilter->PushBackInput(slices[i]);
  }
  joinFilter->Update();
  return joinFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void
clitk::PointsUtils<ImageType>::Convert2DTo3D(const PointType2D & p, 
                                             ImagePointer image, 
                                             const int slice, 
                                             PointType3D & p3D)  
{
  p3D[0] = p[0]; 
  p3D[1] = p[1];
  p3D[2] = (image->GetLargestPossibleRegion().GetIndex()[2]+slice)*image->GetSpacing()[2] 
    + image->GetOrigin()[2];
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void 
clitk::PointsUtils<ImageType>::Convert2DTo3DList(const MapPoint2DType & map, 
                                                 ImagePointer image, 
                                                 VectorPoint3DType & list)
{
  typename MapPoint2DType::const_iterator iter = map.begin();
  while (iter != map.end()) {
    PointType3D p;
    Convert2DTo3D(iter->second, image, iter->first, p);
    list.push_back(p);
    ++iter;
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
void 
clitk::WriteListOfLandmarks(std::vector<typename ImageType::PointType> points, 
                            std::string filename)
{
  std::ofstream os; 
  openFileForWriting(os, filename); 
  os << "LANDMARKS1" << std::endl;  
  for(uint i=0; i<points.size(); i++) {
    const typename ImageType::PointType & p = points[i];
    // Write it in the file
    os << i << " " << p[0] << " " << p[1] << " " << p[2] << " 0 0 " << std::endl;
  }
  os.close();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer 
clitk::Dilate(typename ImageType::Pointer image, 
              double radiusInMM,               
              typename ImageType::PixelType BG,
              typename ImageType::PixelType FG,  
              bool extendSupport)
{
  typename ImageType::SizeType r;
  for(uint i=0; i<ImageType::ImageDimension; i++) 
    r[i] = (uint)lrint(radiusInMM/image->GetSpacing()[i]);
  return clitk::Dilate<ImageType>(image, r, BG, FG, extendSupport);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer 
clitk::Dilate(typename ImageType::Pointer image, 
              typename ImageType::PointType radiusInMM, 
              typename ImageType::PixelType BG, 
              typename ImageType::PixelType FG, 
              bool extendSupport)
{
  typename ImageType::SizeType r;
  for(uint i=0; i<ImageType::ImageDimension; i++) 
    r[i] = (uint)lrint(radiusInMM[i]/image->GetSpacing()[i]);
  return clitk::Dilate<ImageType>(image, r, BG, FG, extendSupport);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer 
clitk::Dilate(typename ImageType::Pointer image, 
              typename ImageType::SizeType radius, 
              typename ImageType::PixelType BG, 
              typename ImageType::PixelType FG, 
              bool extendSupport)
{
  // Create kernel for dilatation
  typedef itk::BinaryBallStructuringElement<typename ImageType::PixelType, 
                                            ImageType::ImageDimension> KernelType;
  KernelType structuringElement;
  structuringElement.SetRadius(radius);
  structuringElement.CreateStructuringElement();

  if (extendSupport) {
    typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadFilterType;
    typename PadFilterType::Pointer padFilter = PadFilterType::New();
    padFilter->SetInput(image);
    typename ImageType::SizeType lower;
    typename ImageType::SizeType upper;
    for(uint i=0; i<3; i++) {
      lower[i] = upper[i] = 2*(radius[i]+1);
    }
    padFilter->SetPadLowerBound(lower);
    padFilter->SetPadUpperBound(upper);
    padFilter->Update();
    image = padFilter->GetOutput();
  }

  // Dilate  filter
  typedef itk::BinaryDilateImageFilter<ImageType, ImageType , KernelType> DilateFilterType;
  typename DilateFilterType::Pointer dilateFilter = DilateFilterType::New();
  dilateFilter->SetBackgroundValue(BG);
  dilateFilter->SetForegroundValue(FG);
  dilateFilter->SetBoundaryToForeground(false);
  dilateFilter->SetKernel(structuringElement);
  dilateFilter->SetInput(image);
  dilateFilter->Update();
  return image = dilateFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ValueType, class VectorType>
void clitk::ConvertOption(std::string optionName, uint given, 
                          ValueType * values, VectorType & p, 
                          uint dim, bool required) 
{
  if (required && (given == 0)) {
    clitkExceptionMacro("The option --" << optionName << " must be set and have 1 or " 
                        << dim << " values.");
  }
  if (given == 1) {
    for(uint i=0; i<dim; i++) p[i] = values[0];
    return;
  }
  if (given == dim) {
    for(uint i=0; i<dim; i++) p[i] = values[i];
    return;
  }
  if (given == 0) return;
  clitkExceptionMacro("The option --" << optionName << " must have 1 or " 
                      << dim << " values.");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
/*
  http://www.gamedev.net/community/forums/topic.asp?topic_id=542870
  Assuming the points are (Ax,Ay) (Bx,By) and (Cx,Cy), you need to compute:
  (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax)
  This will equal zero if the point C is on the line formed by
  points A and B, and will have a different sign depending on the
  side. Which side this is depends on the orientation of your (x,y)
  coordinates, but you can plug test values for A,B and C into this
  formula to determine whether negative values are to the left or to
  the right.
  => to accelerate, start with formula, when change sign -> stop and fill
*/
template<class ImageType>
void 
clitk::SliceBySliceSetBackgroundFromLineSeparation(typename ImageType::Pointer input, 
                                                   std::vector<typename ImageType::PointType> & lA, 
                                                   std::vector<typename ImageType::PointType> & lB, 
                                                   typename ImageType::PixelType BG, 
                                                   int mainDirection, 
                                                   double offsetToKeep)
{
  
  typedef itk::ImageSliceIteratorWithIndex<ImageType> SliceIteratorType;
  SliceIteratorType siter = SliceIteratorType(input, 
                                              input->GetLargestPossibleRegion());
  siter.SetFirstDirection(0);
  siter.SetSecondDirection(1);
  siter.GoToBegin();
  int i=0;
  typename ImageType::PointType A;
  typename ImageType::PointType B;
  typename ImageType::PointType C;
  while (!siter.IsAtEnd()) {
    // Check that the current slice correspond to the current point
    input->TransformIndexToPhysicalPoint(siter.GetIndex(), C);
    if (C[2] != lA[i][2]) {
      // DD(C);
      // DD(lA[i]);
    }
    else {
      // Define A,B,C points
      A = lA[i];
      B = lB[i];
      C = A;
      C[mainDirection] += offsetToKeep; // I know I must keep this point
      double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
      bool isPositive = s<0;
      while (!siter.IsAtEndOfSlice()) {
        while (!siter.IsAtEndOfLine()) {
          // Very slow, I know ... but image should be very small
          input->TransformIndexToPhysicalPoint(siter.GetIndex(), C);
          double s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);
          if (s == 0) siter.Set(BG); // on the line, we decide to remove
          if (isPositive) {
            if (s > 0) siter.Set(BG);
          }
          else {
            if (s < 0) siter.Set(BG); 
          }
          ++siter;
        }
        siter.NextLine();
      }
      ++i;
    }
    siter.NextSlice();
  }
}                                                   
//--------------------------------------------------------------------
