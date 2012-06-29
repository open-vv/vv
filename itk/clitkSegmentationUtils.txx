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
#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkSignedMaurerDistanceMapImageFilter.h>

namespace clitk {

  //--------------------------------------------------------------------
  template<class ImageType, class TMaskImageType>
  typename ImageType::Pointer
  SetBackground(const ImageType * input, 
                const TMaskImageType * mask, 
                typename TMaskImageType::PixelType maskBG,
                typename ImageType::PixelType outValue, 
                bool inPlace) {
    typedef SetBackgroundImageFilter<ImageType, TMaskImageType, ImageType> 
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
  int GetNumberOfConnectedComponentLabels(const ImageType * input, 
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
  Labelize(const ImageType * input, 
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
  LabelizeAndCountNumberOfObjects(const ImageType * input, 
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
  RemoveLabels(const ImageType * input, 
               typename ImageType::PixelType BG,
               std::vector<typename ImageType::PixelType> & labelsToRemove) {
    assert(labelsToRemove.size() != 0);
    typename ImageType::Pointer working_image;// = input;
    for (unsigned int i=0; i <labelsToRemove.size(); i++) {
      typedef SetBackgroundImageFilter<ImageType, ImageType> SetBackgroundImageFilterType;
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
  KeepLabels(const ImageType * input, 
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
  LabelizeAndSelectLabels(const ImageType * input,
                          typename ImageType::PixelType BG, 
                          typename ImageType::PixelType FG, 
                          bool isFullyConnected,
                          int minimalComponentSize,
                          LabelizeParameters<typename ImageType::PixelType> * param)
  {
    typename ImageType::Pointer working_image;
    working_image = Labelize<ImageType>(input, BG, isFullyConnected, minimalComponentSize);
    if (param->GetLabelsToRemove().size() != 0)
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
  template<class MaskImageType>
  typename MaskImageType::Pointer
  SliceBySliceRelativePosition(const MaskImageType * input,
                               const MaskImageType * object,
                               int direction, 
                               double threshold, 
                               std::string orientation, 
                               bool uniqueConnectedComponent, 
                               double spacing, 
                               bool autocropFlag, 
                               bool singleObjectCCL) 
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
    sliceRelPosFilter->SetIntermediateSpacingFlag((spacing != -1));
    sliceRelPosFilter->SetIntermediateSpacing(spacing);
    sliceRelPosFilter->SetUniqueConnectedComponentBySliceFlag(uniqueConnectedComponent);
    sliceRelPosFilter->ObjectCCLSelectionFlagOff();
    sliceRelPosFilter->SetUseTheLargestObjectCCLFlag(singleObjectCCL);
    //    sliceRelPosFilter->SetInverseOrientationFlag(inverseflag); 
    sliceRelPosFilter->SetAutoCropFlag(autocropFlag); 
    sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
    sliceRelPosFilter->Update();
    return sliceRelPosFilter->GetOutput();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class MaskImageType>
  typename MaskImageType::Pointer
  SliceBySliceRelativePosition(const MaskImageType * input,
                               const MaskImageType * object,
                               int direction, 
                               double threshold, 
                               double angle,
                               bool inverseflag,
                               bool uniqueConnectedComponent, 
                               double spacing, 
                               bool autocropFlag, 
                               bool singleObjectCCL) 
  {
    typedef clitk::SliceBySliceRelativePositionFilter<MaskImageType> SliceRelPosFilterType;
    typename SliceRelPosFilterType::Pointer sliceRelPosFilter = SliceRelPosFilterType::New();
    sliceRelPosFilter->VerboseStepFlagOff();
    sliceRelPosFilter->WriteStepFlagOff();
    sliceRelPosFilter->SetInput(input);
    sliceRelPosFilter->SetInputObject(object);
    sliceRelPosFilter->SetDirection(direction);
    sliceRelPosFilter->SetFuzzyThreshold(threshold);
    //    sliceRelPosFilter->AddOrientationTypeString(orientation);
    sliceRelPosFilter->AddAnglesInRad(angle, 0.0);
    sliceRelPosFilter->SetIntermediateSpacingFlag((spacing != -1));
    sliceRelPosFilter->SetIntermediateSpacing(spacing);
    sliceRelPosFilter->SetUniqueConnectedComponentBySliceFlag(uniqueConnectedComponent);
    sliceRelPosFilter->ObjectCCLSelectionFlagOff();
    sliceRelPosFilter->SetUseTheLargestObjectCCLFlag(singleObjectCCL);
    sliceRelPosFilter->SetInverseOrientationFlag(inverseflag); 
    sliceRelPosFilter->SetAutoCropFlag(autocropFlag); 
    sliceRelPosFilter->IgnoreEmptySliceObjectFlagOn();
    sliceRelPosFilter->Update();
    return sliceRelPosFilter->GetOutput();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  bool
  FindExtremaPointInAGivenDirection(const ImageType * input, 
                                    typename ImageType::PixelType bg, 
                                    int direction, bool opposite, 
                                    typename ImageType::PointType & point)
  {
    typename ImageType::PointType dummy;
    return FindExtremaPointInAGivenDirection(input, bg, direction, 
                                             opposite, dummy, 0, point);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  bool
  FindExtremaPointInAGivenDirection(const ImageType * input, 
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
  CropImageRemoveGreaterThan(const ImageType * image, 
                 int dim, double min, bool autoCrop,
                 typename ImageType::PixelType BG) 
  {
    return CropImageAlongOneAxis<ImageType>(image, dim, 
                                            image->GetOrigin()[dim], 
                                            min,
                                            autoCrop, BG);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  CropImageRemoveLowerThan(const ImageType * image, 
                 int dim, double max, bool autoCrop,
                 typename ImageType::PixelType BG) 
  {
    typename ImageType::PointType p;
    image->TransformIndexToPhysicalPoint(image->GetLargestPossibleRegion().GetIndex()+
                                         image->GetLargestPossibleRegion().GetSize(), p);
    // Add GetSpacing because remove Lower or equal than
    // DD(max);
    // DD(p);
    // DD(max+image->GetSpacing()[dim]);
    return CropImageAlongOneAxis<ImageType>(image, dim, max+image->GetSpacing()[dim], p[dim], autoCrop, BG);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  CropImageAlongOneAxis(const ImageType * image, 
                        int dim, double min, double max, 
                        bool autoCrop, typename ImageType::PixelType BG) 
  {
    // Compute region size
    typename ImageType::RegionType region;
    typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    typename ImageType::PointType p = image->GetOrigin();
    if (min > p[dim]) p[dim] = min; // Check if not outside the image
    typename ImageType::IndexType start;
    image->TransformPhysicalPointToIndex(p, start);
    double m = image->GetOrigin()[dim] + size[dim]*image->GetSpacing()[dim];
    if (max > m) p[dim] = m; // Check if not outside the image
    else p[dim] = max;
    typename ImageType::IndexType end;
    image->TransformPhysicalPointToIndex(p, end);
    size[dim] = abs(end[dim]-start[dim]);
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
      result = AutoCrop<ImageType>(result, BG);
    }
    return result;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  ComputeCentroids(const ImageType * image, 
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
    centroids.push_back(dummy); // label 0 -> no centroid, use dummy point for BG 
    //DS FIXME (not useful ! to change ..)
    for(uint i=0; i<labelMap->GetNumberOfLabelObjects(); i++) {
      int label = labelMap->GetLabels()[i];
      centroids.push_back(labelMap->GetLabelObject(label)->GetCentroid());
    } 
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType, class LabelType>
  typename itk::LabelMap< itk::ShapeLabelObject<LabelType, ImageType::ImageDimension> >::Pointer
  ComputeLabelMap(const ImageType * image, 
                  typename ImageType::PixelType BG, 
                  bool computePerimeterFlag) 
  {
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
    statFilter->SetComputePerimeter(computePerimeterFlag);
    statFilter->Update();
    return statFilter->GetOutput();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  ComputeCentroids2(const ImageType * image, 
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
    
    for(uint i=1; i<labelMap->GetNumberOfLabelObjects()+1; i++) {
      DD(labelMap->GetLabelObject(i)->GetBinaryPrincipalAxes());
      DD(labelMap->GetLabelObject(i)->GetBinaryFlatness());
      DD(labelMap->GetLabelObject(i)->GetRoundness ());      

      // search for the point on the boundary alog PA

    }

  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  PointsUtils<ImageType>::Convert2DTo3D(const PointType2D & p2D, 
                                        const ImageType * image, 
                                        const int slice, 
                                        PointType3D & p3D)  
  {
    IndexType3D index3D;
    index3D[0] = index3D[1] = 0;
    index3D[2] = image->GetLargestPossibleRegion().GetIndex()[2]+slice;
    image->TransformIndexToPhysicalPoint(index3D, p3D);
    p3D[0] = p2D[0]; 
    p3D[1] = p2D[1];
    //  p3D[2] = p[2];//(image->GetLargestPossibleRegion().GetIndex()[2]+slice)*image->GetSpacing()[2] 
    //    + image->GetOrigin()[2];
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void 
  PointsUtils<ImageType>::Convert2DMapTo3DList(const MapPoint2DType & map, 
                                            const ImageType * image, 
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
  PointsUtils<ImageType>::Convert2DListTo3DList(const VectorPoint2DType & p2D, 
                                                int slice,
                                                const ImageType * image, 
                                                VectorPoint3DType & list) 
  {
    for(uint i=0; i<p2D.size(); i++) {
      PointType3D p;
      Convert2DTo3D(p2D[i], image, slice, p);
      list.push_back(p);
    }
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void 
  WriteListOfLandmarks(std::vector<typename ImageType::PointType> points, 
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
  Dilate(const ImageType * image, double radiusInMM,               
         typename ImageType::PixelType BG,
         typename ImageType::PixelType FG,  
         bool extendSupport)
  {
    typename ImageType::SizeType r;
    for(uint i=0; i<ImageType::ImageDimension; i++) 
      r[i] = (uint)lrint(radiusInMM/image->GetSpacing()[i]);
    return Dilate<ImageType>(image, r, BG, FG, extendSupport);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer 
  Dilate(const ImageType * image, typename ImageType::PointType radiusInMM, 
         typename ImageType::PixelType BG, 
         typename ImageType::PixelType FG, 
         bool extendSupport)
  {
    typename ImageType::SizeType r;
    for(uint i=0; i<ImageType::ImageDimension; i++) 
      r[i] = (uint)lrint(radiusInMM[i]/image->GetSpacing()[i]);
    return Dilate<ImageType>(image, r, BG, FG, extendSupport);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer 
  Dilate(const ImageType * image, typename ImageType::SizeType radius, 
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

    typename ImageType::Pointer output;
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
      output = padFilter->GetOutput();
    }

    // Dilate  filter
    typedef itk::BinaryDilateImageFilter<ImageType, ImageType , KernelType> DilateFilterType;
    typename DilateFilterType::Pointer dilateFilter = DilateFilterType::New();
    dilateFilter->SetBackgroundValue(BG);
    dilateFilter->SetForegroundValue(FG);
    dilateFilter->SetBoundaryToForeground(false);
    dilateFilter->SetKernel(structuringElement);
    if (extendSupport) dilateFilter->SetInput(output);
    else dilateFilter->SetInput(image);
    dilateFilter->Update();
    return dilateFilter->GetOutput();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer 
  Opening(const ImageType * image, typename ImageType::SizeType radius,
         typename ImageType::PixelType BG,
         typename ImageType::PixelType FG)
  {
    // Kernel 
    typedef itk::BinaryBallStructuringElement<typename ImageType::PixelType, 
                                              ImageType::ImageDimension> KernelType;    
    KernelType structuringElement;
    structuringElement.SetRadius(radius);
    structuringElement.CreateStructuringElement();
    
    // Filter
    typedef itk::BinaryMorphologicalOpeningImageFilter<ImageType, ImageType , KernelType> OpeningFilterType;
    typename OpeningFilterType::Pointer open = OpeningFilterType::New();
    open->SetInput(image);
    open->SetBackgroundValue(BG);
    open->SetForegroundValue(FG);
    open->SetKernel(structuringElement);
    open->Update();
    return open->GetOutput();
  }
  //--------------------------------------------------------------------



  //--------------------------------------------------------------------
  template<class ValueType, class VectorType>
  void ConvertOption(std::string optionName, uint given, 
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

    offsetToKeep = is used to determine which side of the line we
    keep. The point along the mainDirection but 'offsetToKeep' mm away
    is kept.
  
  */
  template<class ImageType>
  void 
  SliceBySliceSetBackgroundFromLineSeparation(ImageType * input, 
                                              std::vector<typename ImageType::PointType> & lA, 
                                              std::vector<typename ImageType::PointType> & lB, 
                                              typename ImageType::PixelType BG, 
                                              int mainDirection, 
                                              double offsetToKeep)
  {
    assert((mainDirection==0) || (mainDirection==1));
    typedef itk::ImageSliceIteratorWithIndex<ImageType> SliceIteratorType;
    SliceIteratorType siter = SliceIteratorType(input, input->GetLargestPossibleRegion());
    siter.SetFirstDirection(0);
    siter.SetSecondDirection(1);
    siter.GoToBegin();
    uint i=0;
    typename ImageType::PointType A;
    typename ImageType::PointType B;
    typename ImageType::PointType C;
    assert(lA.size() == lB.size());
    while ((i<lA.size()) && (!siter.IsAtEnd())) {
      // Check that the current slice correspond to the current point
      input->TransformIndexToPhysicalPoint(siter.GetIndex(), C);
      if ((fabs(C[2] - lA[i][2]))>0.01) { // is !equal with a tolerance of 0.01 mm
      }
      else {
        // Define A,B,C points
        A = lA[i];
        B = lB[i];
        C = A;
        // Check that the line is not a point (A=B)
        bool p = (A[0] == B[0]) && (A[1] == B[1]);
      
        if (!p) {
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
          } // end loop slice
        }      

        ++i;
      } // End of current slice
      siter.NextSlice();
    }
  }                                                   
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void 
  AndNot(ImageType * input, 
         const ImageType * object, 
         typename ImageType::PixelType BG)
  {
    typename ImageType::Pointer o;
    bool resized=false;
    if (!clitk::HaveSameSizeAndSpacing<ImageType, ImageType>(input, object)) {
      o = clitk::ResizeImageLike<ImageType>(object, input, BG);
      resized = true;
    }

    typedef clitk::BooleanOperatorLabelImageFilter<ImageType> BoolFilterType;
    typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
    boolFilter->InPlaceOn();
    boolFilter->SetInput1(input);
    if (resized) boolFilter->SetInput2(o);  
    else boolFilter->SetInput2(object);
    boolFilter->SetBackgroundValue1(BG);
    boolFilter->SetBackgroundValue2(BG);
    boolFilter->SetOperationType(BoolFilterType::AndNot);
    boolFilter->Update();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void 
  And(ImageType * input, 
      const ImageType * object, 
      typename ImageType::PixelType BG)
  {
    typename ImageType::Pointer o;
    bool resized=false;
    if (!clitk::HaveSameSizeAndSpacing<ImageType, ImageType>(input, object)) {
      o = clitk::ResizeImageLike<ImageType>(object, input, BG);
      resized = true;
    }

    typedef clitk::BooleanOperatorLabelImageFilter<ImageType> BoolFilterType;
    typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
    boolFilter->InPlaceOn();
    boolFilter->SetInput1(input);
    if (resized) boolFilter->SetInput2(o);  
    else boolFilter->SetInput2(object);
    boolFilter->SetBackgroundValue1(BG);
    boolFilter->SetBackgroundValue2(BG);
    boolFilter->SetOperationType(BoolFilterType::And);
    boolFilter->Update();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void 
  Or(ImageType * input, 
     const ImageType * object, 
     typename ImageType::PixelType BG)
  {
    typename ImageType::Pointer o;
    bool resized=false;
    if (!clitk::HaveSameSizeAndSpacing<ImageType, ImageType>(input, object)) {
      o = clitk::ResizeImageLike<ImageType>(object, input, BG);
      resized = true;
    }

    typedef clitk::BooleanOperatorLabelImageFilter<ImageType> BoolFilterType;
    typename BoolFilterType::Pointer boolFilter = BoolFilterType::New(); 
    boolFilter->InPlaceOn();
    boolFilter->SetInput1(input);
    if (resized) boolFilter->SetInput2(o);  
    else boolFilter->SetInput2(object);
    boolFilter->SetBackgroundValue1(BG);
    boolFilter->SetBackgroundValue2(BG);
    boolFilter->SetOperationType(BoolFilterType::Or);
    boolFilter->Update();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  Binarize(const ImageType * input, 
           typename ImageType::PixelType lower, 
           typename ImageType::PixelType upper, 
           typename ImageType::PixelType BG,
           typename ImageType::PixelType FG) 
  {
    typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> BinaryThresholdFilterType;
    typename BinaryThresholdFilterType::Pointer binarizeFilter = BinaryThresholdFilterType::New();
    binarizeFilter->SetInput(input);
    binarizeFilter->InPlaceOff();
    binarizeFilter->SetLowerThreshold(lower);
    binarizeFilter->SetUpperThreshold(upper);
    binarizeFilter->SetInsideValue(FG);
    binarizeFilter->SetOutsideValue(BG);
    binarizeFilter->Update();
    return binarizeFilter->GetOutput();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  GetMinMaxPointPosition(const ImageType * input, 
                         typename ImageType::PointType & min,
                         typename ImageType::PointType & max) 
  {
    typename ImageType::IndexType index = input->GetLargestPossibleRegion().GetIndex();
    input->TransformIndexToPhysicalPoint(index, min);
    index = index+input->GetLargestPossibleRegion().GetSize();
    input->TransformIndexToPhysicalPoint(index, max);
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::PointType
  FindExtremaPointInAGivenLine(const ImageType * input, 
                               int dimension, 
                               bool inverse, 
                               typename ImageType::PointType p, 
                               typename ImageType::PixelType BG, 
                               double distanceMax) 
  {
    // Which direction ?  Increasing or decreasing.
    int d=1;
    if (inverse) d=-1;
  
    // Transform to pixel index
    typename ImageType::IndexType index;
    input->TransformPhysicalPointToIndex(p, index);

    // Loop while inside the mask;
    while (input->GetPixel(index) != BG) {
      index[dimension] += d;
    }

    // Transform back to Physical Units
    typename ImageType::PointType result;
    input->TransformIndexToPhysicalPoint(index, result);

    // Check that is is not too far away
    double distance = p.EuclideanDistanceTo(result);
    if (distance > distanceMax) {
      result = p; // Get back to initial value
    }

    return result;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class PointType>
  bool
  IsOnTheSameLineSide(PointType C, PointType A, PointType B, PointType like) 
  {
    // Look at the position of point 'like' according to the AB line
    double s = (B[0] - A[0]) * (like[1] - A[1]) - (B[1] - A[1]) * (like[0] - A[0]);
    bool negative = s<0;
  
    // Look the C position
    s = (B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0]);

    if (negative && (s<=0)) return true;
    if (!negative && (s>=0)) return true;
    return false;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  /* Consider an input object, for each slice, find the extrema
     position according to a given direction and build a line segment
     passing throught this point in a given direction.  Output is a
     vector of line (from point A to B), for each slice;
   */
  template<class ImageType>
  void 
  SliceBySliceBuildLineSegmentAccordingToExtremaPosition(const ImageType * input, 
                                                         typename ImageType::PixelType BG, 
                                                         int sliceDimension, 
                                                         int extremaDirection, 
                                                         bool extremaOppositeFlag, 
                                                         int lineDirection,
                                                         double margin,
                                                         std::vector<typename ImageType::PointType> & A, 
                                                         std::vector<typename ImageType::PointType> & B)
  {
    // Type of a slice
    typedef typename itk::Image<typename ImageType::PixelType, ImageType::ImageDimension-1> SliceType;
    
    // Build the list of slices
    std::vector<typename SliceType::Pointer> slices;
    clitk::ExtractSlices<ImageType>(input, sliceDimension, slices);

    // Build the list of 2D points
    std::map<int, typename SliceType::PointType> position2D;
    for(uint i=0; i<slices.size(); i++) {
      typename SliceType::PointType p;
      bool found = 
        clitk::FindExtremaPointInAGivenDirection<SliceType>(slices[i], BG, 
                                                            extremaDirection, extremaOppositeFlag, p);
      if (found) {
        position2D[i] = p;
      }
    }
    
    // Convert 2D points in slice into 3D points
    clitk::PointsUtils<ImageType>::Convert2DMapTo3DList(position2D, input, A);
    
    // Create additional point just right to the previous ones, on the
    // given lineDirection, in order to create a horizontal/vertical line.
    for(uint i=0; i<A.size(); i++) {
      typename ImageType::PointType p = A[i];
      p[lineDirection] += 10;
      B.push_back(p);
      // Margins ?
      A[i][extremaDirection] += margin;
      B[i][extremaDirection] += margin;
    }

  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  SliceBySliceKeepMainCCL(const ImageType * input, 
                          typename ImageType::PixelType BG,
                          typename ImageType::PixelType FG)  {
    
    // Extract slices
    const int d = ImageType::ImageDimension-1;
    typedef typename itk::Image<typename ImageType::PixelType, d> SliceType;
    std::vector<typename SliceType::Pointer> slices;
    clitk::ExtractSlices<ImageType>(input, d, slices);
    
    // Labelize and keep the main one
    std::vector<typename SliceType::Pointer> o;
    for(uint i=0; i<slices.size(); i++) {
      o.push_back(clitk::Labelize<SliceType>(slices[i], BG, false, 1));
      o[i] = clitk::KeepLabels<SliceType>(o[i], BG, FG, 1, 1, true);
    }
    
    // Join slices
    typename ImageType::Pointer output;
    output = clitk::JoinSlices<ImageType>(o, input, d);
    return output;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  Clone(const ImageType * input) {
    typedef itk::ImageDuplicator<ImageType> DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(input);
    duplicator->Update();
    return duplicator->GetOutput();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  /* Consider an input object, start at A, for each slice (dim1): 
     - compute the intersection between the AB line and the current slice
     - remove what is at lower or greater according to dim2 of this point
     - stop at B
  */
  template<class ImageType>
  typename ImageType::Pointer
  SliceBySliceSetBackgroundFromSingleLine(const ImageType * input, 
                                          typename ImageType::PixelType BG, 
                                          typename ImageType::PointType & A, 
                                          typename ImageType::PointType & B, 
                                          int dim1, int dim2, bool removeLowerPartFlag)
    
  {
    // Extract slices
    typedef typename itk::Image<typename ImageType::PixelType, ImageType::ImageDimension-1> SliceType;
    typedef typename SliceType::Pointer SlicePointer;
    std::vector<SlicePointer> slices;
    clitk::ExtractSlices<ImageType>(input, dim1, slices);

    // Start at slice that contains A, and stop at B
    typename ImageType::IndexType Ap;
    typename ImageType::IndexType Bp;
    input->TransformPhysicalPointToIndex(A, Ap);
    input->TransformPhysicalPointToIndex(B, Bp);
    
    // Determine slice largest region
    typename SliceType::RegionType region = slices[0]->GetLargestPossibleRegion();
    typename SliceType::SizeType size = region.GetSize();
    typename SliceType::IndexType index = region.GetIndex();

    // Line slope
    double a = (Bp[dim2]-Ap[dim2])/(Bp[dim1]-Ap[dim1]);
    double b = Ap[dim2];

    // Loop from slice A to slice B
    for(uint i=0; i<(Bp[dim1]-Ap[dim1]); i++) {
      // Compute intersection between line AB and current slice for the dim2
      double p = a*i+b;
      // Change region (lower than dim2)
      if (removeLowerPartFlag) {
        size[dim2] = p-Ap[dim2];
      }
      else {
        size[dim2] = slices[0]->GetLargestPossibleRegion().GetSize()[dim2]-p;
        index[dim2] = p;
      }
      region.SetSize(size);
      region.SetIndex(index);
      // Fill region with BG (simple region iterator)
      FillRegionWithValue<SliceType>(slices[i+Ap[dim1]], BG, region);
      /*
      typedef itk::ImageRegionIterator<SliceType> IteratorType;
      IteratorType iter(slices[i+Ap[dim1]], region);
      iter.GoToBegin();
      while (!iter.IsAtEnd()) {
        iter.Set(BG);
        ++iter;
      }
      */
      // Loop
    }
    
    // Merge slices
    typename ImageType::Pointer output;
    output = clitk::JoinSlices<ImageType>(slices, input, dim1);
    return output;
  }
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  /* Consider an input object, slice by slice, use the point A and set
     pixel to BG according to their position relatively to A
  */
  template<class ImageType>
  typename ImageType::Pointer
  SliceBySliceSetBackgroundFromPoints(const ImageType * input, 
                                      typename ImageType::PixelType BG, 
                                      int sliceDim,
                                      std::vector<typename ImageType::PointType> & A, 
                                      bool removeGreaterThanXFlag,
                                      bool removeGreaterThanYFlag)
    
  {
    // Extract slices
    typedef typename itk::Image<typename ImageType::PixelType, ImageType::ImageDimension-1> SliceType;
    typedef typename SliceType::Pointer SlicePointer;
    std::vector<SlicePointer> slices;
    clitk::ExtractSlices<ImageType>(input, sliceDim, slices);

    // Start at slice that contains A
    typename ImageType::IndexType Ap;
    
    // Determine slice largest region
    typename SliceType::RegionType region = slices[0]->GetLargestPossibleRegion();
    typename SliceType::SizeType size = region.GetSize();
    typename SliceType::IndexType index = region.GetIndex();

    // Loop from slice A to slice B
    for(uint i=0; i<A.size(); i++) {
      input->TransformPhysicalPointToIndex(A[i], Ap);
      uint sliceIndex = Ap[2] - input->GetLargestPossibleRegion().GetIndex()[2];
      if ((sliceIndex < 0) || (sliceIndex >= slices.size())) {
        continue; // do not consider this slice
      }
      
      // Compute region for BG
      if (removeGreaterThanXFlag) {
        index[0] = Ap[0];
        size[0] = region.GetSize()[0]-(index[0]-region.GetIndex()[0]);
      }
      else {
        index[0] = region.GetIndex()[0];
        size[0] = Ap[0] - index[0];
      }

      if (removeGreaterThanYFlag) {
        index[1] = Ap[1];
        size[1] = region.GetSize()[1]-(index[1]-region.GetIndex()[1]);
      }
      else {
        index[1] = region.GetIndex()[1];
        size[1] = Ap[1] - index[1];
      }

      // Set region
      region.SetSize(size);
      region.SetIndex(index);

      // Fill region with BG (simple region iterator)
      FillRegionWithValue<SliceType>(slices[sliceIndex], BG, region);
      // Loop
    }
    
    // Merge slices
    typename ImageType::Pointer output;
    output = clitk::JoinSlices<ImageType>(slices, input, sliceDim);
    return output;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  FillRegionWithValue(ImageType * input, typename ImageType::PixelType value, typename ImageType::RegionType & region)
  {
    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    IteratorType iter(input, region);
    iter.GoToBegin();
    while (!iter.IsAtEnd()) {
      iter.Set(value);
      ++iter;
    }    
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  GetMinMaxBoundary(ImageType * input, typename ImageType::PointType & min, 
                    typename ImageType::PointType & max)
  {
    typedef typename ImageType::PointType PointType;
    typedef typename ImageType::IndexType IndexType;
    IndexType min_i, max_i;
    min_i = input->GetLargestPossibleRegion().GetIndex();
    for(uint i=0; i<ImageType::ImageDimension; i++)
      max_i[i] = input->GetLargestPossibleRegion().GetSize()[i] + min_i[i];
    input->TransformIndexToPhysicalPoint(min_i, min);
    input->TransformIndexToPhysicalPoint(max_i, max);  
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename itk::Image<float, ImageType::ImageDimension>::Pointer
  DistanceMap(const ImageType * input, typename ImageType::PixelType BG)//, 
  //              typename itk::Image<float, ImageType::ImageDimension>::Pointer dmap) 
  {
    typedef itk::Image<float,ImageType::ImageDimension> FloatImageType;
    typedef itk::SignedMaurerDistanceMapImageFilter<ImageType, FloatImageType> DistanceMapFilterType;
    typename DistanceMapFilterType::Pointer filter = DistanceMapFilterType::New();
    filter->SetInput(input);
    filter->SetUseImageSpacing(true);
    filter->SquaredDistanceOff();
    filter->SetBackgroundValue(BG);
    filter->Update();
    return filter->GetOutput();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void 
  SliceBySliceBuildLineSegmentAccordingToMinimalDistanceBetweenStructures(const ImageType * S1, 
                                                                          const ImageType * S2, 
                                                                          typename ImageType::PixelType BG, 
                                                                          int sliceDimension, 
                                                                          std::vector<typename ImageType::PointType> & A, 
                                                                          std::vector<typename ImageType::PointType> & B)
  {
    // Extract slices
    typedef typename itk::Image<typename ImageType::PixelType, 2> SliceType;
    typedef typename SliceType::Pointer SlicePointer;
    std::vector<SlicePointer> slices_s1;
    std::vector<SlicePointer> slices_s2;
    clitk::ExtractSlices<ImageType>(S1, sliceDimension, slices_s1);
    clitk::ExtractSlices<ImageType>(S2, sliceDimension, slices_s2);

    assert(slices_s1.size() == slices_s2.size());

    // Prepare dmap
    typedef itk::Image<float,2> FloatImageType;
    typedef itk::SignedMaurerDistanceMapImageFilter<SliceType, FloatImageType> DistanceMapFilterType;
    std::vector<typename FloatImageType::Pointer> dmaps1;
    std::vector<typename FloatImageType::Pointer> dmaps2;
    typename FloatImageType::Pointer dmap;

    // loop on slices
    for(uint i=0; i<slices_s1.size(); i++) {
      // Compute dmap for S1 *TO PUT IN FONCTION*
      dmap = clitk::DistanceMap<SliceType>(slices_s1[i], BG);
      dmaps1.push_back(dmap);
      writeImage<FloatImageType>(dmap, "dmap1.mha");
      // Compute dmap for S2
      dmap = clitk::DistanceMap<SliceType>(slices_s2[i], BG);
      dmaps2.push_back(dmap);
      writeImage<FloatImageType>(dmap, "dmap2.mha");
      
      // Look in S2 for the point the closest to S1
      typename SliceType::PointType p = ComputeClosestPoint<SliceType>(slices_s1[i], dmaps2[i], BG);
      typename ImageType::PointType p3D;
      clitk::PointsUtils<ImageType>::Convert2DTo3D(p, S1, i, p3D);
      A.push_back(p3D);

      // Look in S2 for the point the closest to S1
      p = ComputeClosestPoint<SliceType>(slices_s2[i], dmaps1[i], BG);
      clitk::PointsUtils<ImageType>::Convert2DTo3D(p, S2, i, p3D);
      B.push_back(p3D);

    }

    // Debug dmap
    /*
      typedef itk::Image<float,3> FT;
      FT::Pointer f = FT::New();
      typename FT::Pointer d1 = clitk::JoinSlices<FT>(dmaps1, S1, 2);
      typename FT::Pointer d2 = clitk::JoinSlices<FT>(dmaps2, S2, 2);
      writeImage<FT>(d1, "d1.mha");
      writeImage<FT>(d2, "d2.mha");
    */
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::PointType
  ComputeClosestPoint(const ImageType * input, 
                      const itk::Image<float, ImageType::ImageDimension> * dmap, 
                      typename ImageType::PixelType & BG) 
  {
    // Loop dmap + S2, if FG, get min
    typedef itk::Image<float,ImageType::ImageDimension> FloatImageType;
    typedef itk::ImageRegionConstIteratorWithIndex<ImageType> ImageIteratorType;
    typedef itk::ImageRegionConstIterator<FloatImageType> DMapIteratorType;
    ImageIteratorType iter1(input, input->GetLargestPossibleRegion());
    DMapIteratorType iter2(dmap, dmap->GetLargestPossibleRegion());
    
    iter1.GoToBegin();
    iter2.GoToBegin();
    double dmin = 100000.0;
    typename ImageType::IndexType indexmin;
    indexmin.Fill(0);
    while (!iter1.IsAtEnd()) {
      if (iter1.Get() != BG) {
        double d = iter2.Get();
        if (d<dmin) {
          indexmin = iter1.GetIndex();
          dmin = d;
        }
      }
      ++iter1;
      ++iter2;
    }
    
    // Convert in Point
    typename ImageType::PointType p;
    input->TransformIndexToPhysicalPoint(indexmin, p);
    return p;
  }
  //--------------------------------------------------------------------
     

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  RemoveNegativeIndexFromRegion(ImageType * input) {
    typedef itk::ChangeInformationImageFilter< ImageType > InfoFilterType; 
    typename InfoFilterType::Pointer indexChangeFilter = InfoFilterType::New(); 
    indexChangeFilter->ChangeRegionOn(); 
    // The next line is commented because not exist in itk 3
    // typename InfoFilterType::OutputImageOffsetValueType indexShift[3];
    long indexShift[3];
    typename ImageType::IndexType index = input->GetLargestPossibleRegion().GetIndex();
    for(uint i=0;i<ImageType::ImageDimension; i++)
      indexShift[i] = (index[i]<0 ? -index[i]:0);
    typename ImageType::PointType origin;
    for(uint i=0;i<ImageType::ImageDimension; i++)
    origin[i] = input->GetOrigin()[i] - indexShift[i]*input->GetSpacing()[i];
    indexChangeFilter->SetOutputOffset( indexShift ); 
    indexChangeFilter->SetInput(input); 
    indexChangeFilter->SetOutputOrigin(origin);
    indexChangeFilter->ChangeOriginOn();
    indexChangeFilter->Update();
    return indexChangeFilter->GetOutput();
  }
  //--------------------------------------------------------------------


} // end of namespace

