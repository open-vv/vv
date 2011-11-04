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

#ifndef CLITKSEGMENTATIONUTILS_H
#define CLITKSEGMENTATIONUTILS_H

// clitk
#include "clitkCommon.h"
#include "clitkAutoCropFilter.h"
#include "clitkLabelizeParameters.h"
#include "clitkExtractSliceFilter.h"

// itk
#include <itkBoundingBox.h>
#include <itkJoinSeriesImageFilter.h>

/*
  According to 
  http://answerpot.com/showthread.php?357451-Itk::SmartPointer%20-%20problem%20making%20code%20const-correct
  it is better to take raw pointer as argument instead of SmartPointer.
*/

namespace clitk {

  //--------------------------------------------------------------------
  template<class TInternalImageType, class TMaskInternalImageType>
  typename TInternalImageType::Pointer
  SetBackground(const TInternalImageType * input,
                const TMaskInternalImageType * mask, 
                typename TMaskInternalImageType::PixelType maskBG, 
                typename TInternalImageType::PixelType outValue, 
                bool inPlace);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  int GetNumberOfConnectedComponentLabels(const ImageType * input, 
                                          typename ImageType::PixelType BG, 
                                          bool isFullyConnected);
  //--------------------------------------------------------------------


  //-------------------------------------------------------------------- 
  template<class TImageType>
  typename TImageType::Pointer
  Labelize(const TImageType * input, typename TImageType::PixelType BG, 
           bool isFullyConnected, int minimalComponentSize);
  template<class TImageType>
  typename TImageType::Pointer
  LabelizeAndCountNumberOfObjects(const TImageType * input, 
                                  typename TImageType::PixelType BG, 
                                  bool isFullyConnected, 
                                  int minimalComponentSize, 
                                  int & nb);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  RemoveLabels(const ImageType * input, 
               typename ImageType::PixelType BG, 
               std::vector<typename ImageType::PixelType> & labelsToRemove);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  AutoCrop(const ImageType * input, 
           typename ImageType::PixelType BG) {
    typedef clitk::AutoCropFilter<ImageType> AutoCropFilterType;
    typename AutoCropFilterType::Pointer autoCropFilter = AutoCropFilterType::New();
    autoCropFilter->SetInput(input);
    autoCropFilter->SetBackgroundValue(BG);
    autoCropFilter->Update();   
    return autoCropFilter->GetOutput();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class TImageType>
  typename TImageType::Pointer
  KeepLabels(const TImageType * input,
             typename TImageType::PixelType BG, 
             typename TImageType::PixelType FG,  
             typename TImageType::PixelType firstKeep, 
             typename TImageType::PixelType lastKeep, 
             bool useLastKeep);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class TImageType>
  typename TImageType::Pointer
  LabelizeAndSelectLabels(const TImageType * input,
                          typename TImageType::PixelType BG, 
                          typename TImageType::PixelType FG, 
                          bool isFullyConnected,
                          int minimalComponentSize,
                          LabelizeParameters<typename TImageType::PixelType> * param);


  //--------------------------------------------------------------------
  template<class MaskImageType>
  typename MaskImageType::Pointer
  SliceBySliceRelativePosition(const MaskImageType * input,
			       const MaskImageType * object,
			       int direction, 
			       double threshold, 
			       std::string orientation, 
                               bool uniqueConnectedComponent=false, 
                               double spacing=-1, 
                               bool autocropflag=true, 
                               bool singleObjectCCL=true);
  template<class MaskImageType>
  typename MaskImageType::Pointer
  SliceBySliceRelativePosition(const MaskImageType * input,
			       const MaskImageType * object,
			       int direction, 
			       double threshold, 
			       double angle, 
                               bool inverseflag,
                               bool uniqueConnectedComponent=false, 
                               double spacing=-1, 
                               bool autocropflag=true, 
                               bool singleObjectCCL=true);

  //--------------------------------------------------------------------
  // In a binary image, search for the point belonging to the FG that
  // is the most exterma in the direction 'direction' (or in the
  // opposite if notFlag is given). 
  template<class ImageType>
  bool
  FindExtremaPointInAGivenDirection(const ImageType * input, 
                                    typename ImageType::PixelType bg, 
                                    int direction, bool opposite, 
                                    typename ImageType::PointType & p);

  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  // Same as above but check that the found point is not more than
  // 'distanceMax' away from 'refPoint'
  template<class ImageType>
  bool
  FindExtremaPointInAGivenDirection(const ImageType * input, 
                                    typename ImageType::PixelType bg, 
                                    int direction, bool opposite, 
                                    typename ImageType::PointType refPoint,
                                    double distanceMax, 
                                    typename ImageType::PointType & p);

  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  CropImageAlongOneAxis(const ImageType * image, 
                        int dim, double min, double max, 
                        bool autoCrop = false,
                        typename ImageType::PixelType BG=0);
  template<class ImageType>
  typename ImageType::Pointer
  CropImageRemoveGreaterThan(const ImageType * image, 
                             int dim, double min, bool autoCrop = false,
                             typename ImageType::PixelType BG=0);
  template<class ImageType>
  typename ImageType::Pointer
  CropImageRemoveLowerThan(const ImageType * image, 
                           int dim, double max,bool autoCrop = false,
                           typename ImageType::PixelType BG=0);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType, class LabelType>
  typename itk::LabelMap< itk::ShapeLabelObject<LabelType, ImageType::ImageDimension> >::Pointer
  ComputeLabelMap(const ImageType * image, 
                  typename ImageType::PixelType BG,                   
                  bool computePerimeterFlag=false);
  template<class ImageType>
  void
  ComputeCentroids(const ImageType * image, 
                   typename ImageType::PixelType BG, 
                   std::vector<typename ImageType::PointType> & centroids);
  template<class ImageType>
  void
  ComputeCentroids2(const ImageType * image, 
                   typename ImageType::PixelType BG, 
                   std::vector<typename ImageType::PointType> & centroids);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  ExtractSlices(const ImageType * image, int direction,
                std::vector<typename itk::Image<typename ImageType::PixelType,
                                                ImageType::ImageDimension-1>::Pointer > & slices)
  {
    typedef ExtractSliceFilter<ImageType> ExtractSliceFilterType;
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
  JoinSlices(std::vector<typename itk::Image<typename ImageType::PixelType, 
                                             ImageType::ImageDimension-1>::Pointer > & slices, 
             const itk::ImageBase<ImageType::ImageDimension> * input, //const ImageType * input, 
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
  // Set of tools to manage 3D points and 2D points in slices  
  template<class ImageType>
  class PointsUtils
  {
    typedef typename ImageType::PointType PointType3D;
    typedef typename ImageType::IndexType IndexType3D;
    typedef typename ImageType::PixelType PixelType;
    typedef typename ImageType::Pointer ImagePointer;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef itk::Image<PixelType, 2> SliceType;
    typedef typename SliceType::PointType PointType2D;
    typedef typename SliceType::IndexType IndexType2D;
    
    typedef std::map<int, PointType2D> MapPoint2DType;
    typedef std::vector<PointType3D> VectorPoint3DType;
    typedef std::vector<PointType2D> VectorPoint2DType;

  public:
    static void Convert2DTo3D(const PointType2D & p2D, 
                              const ImageType * image, 
                              const int slice, 
                              PointType3D & p3D);
    static void Convert2DMapTo3DList(const MapPoint2DType & map, 
                                  const ImageType * image, 
                                  VectorPoint3DType & list);
    static void Convert2DListTo3DList(const VectorPoint2DType & p, 
                                      int slice,
                                      const ImageType * image, 
                                      VectorPoint3DType & list);
  };

  //--------------------------------------------------------------------
  template<class ImageType>
  void 
  WriteListOfLandmarks(std::vector<typename ImageType::PointType> points, 
                       std::string filename);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  Dilate(const ImageType * image, double radiusInMM,               
         typename ImageType::PixelType BG, 
         typename ImageType::PixelType FG, 
         bool extendSupport);
  template<class ImageType>
  typename ImageType::Pointer
  Dilate(const ImageType * image, typename ImageType::SizeType radius, 
         typename ImageType::PixelType BG, 
         typename ImageType::PixelType FG, 
         bool extendSupport);
  template<class ImageType>
  typename ImageType::Pointer  
  Dilate(const ImageType * image, typename ImageType::PointType radiusInMM, 
         typename ImageType::PixelType BG, 
         typename ImageType::PixelType FG, 
         bool extendSupport);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer 
  Opening(const ImageType * image, typename ImageType::SizeType radius,
          typename ImageType::PixelType BG, typename ImageType::PixelType FG);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ValueType, class VectorType>
  void ConvertOption(std::string optionName, uint given, 
                     ValueType * values, VectorType & p, 
                     uint dim, bool required);
#define ConvertOptionMacro(OPTIONNAME, VAR, DIM, REQUIRED)              \
  ConvertOption(#OPTIONNAME, OPTIONNAME##_given, OPTIONNAME##_arg, VAR, DIM, REQUIRED);
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  template<class ImageType>
  void 
  SliceBySliceSetBackgroundFromLineSeparation(ImageType * input, 
                                              std::vector<typename ImageType::PointType> & lA, 
                                              std::vector<typename ImageType::PointType> & lB, 
                                              typename ImageType::PixelType BG, 
                                              int mainDirection, 
                                              double offsetToKeep);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void AndNot(ImageType * input, 
              const ImageType * object, 
              typename ImageType::PixelType BG=0);
  template<class ImageType>
  void And(ImageType * input, 
           const ImageType * object, 
           typename ImageType::PixelType BG=0);
  template<class ImageType>
  void Or(ImageType * input, 
          const ImageType * object, 
          typename ImageType::PixelType BG=0);
  //--------------------------------------------------------------------
 

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  Binarize(const ImageType * input, 
           typename ImageType::PixelType lower, 
           typename ImageType::PixelType upper, 
           typename ImageType::PixelType BG=0,
           typename ImageType::PixelType FG=1);
  //--------------------------------------------------------------------
 
  
  //--------------------------------------------------------------------
  template<class ImageType>
  void
  GetMinMaxPointPosition(const ImageType * input, 
                         typename ImageType::PointType & min,
                         typename ImageType::PointType & max);
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::PointType
  FindExtremaPointInAGivenLine(const ImageType * input, 
                               int dimension, bool inverse, 
                               typename ImageType::PointType p, 
                               typename ImageType::PixelType BG, 
                               double distanceMax);
  //--------------------------------------------------------------------

  
  //--------------------------------------------------------------------
  template<class PointType>
  bool
  IsOnTheSameLineSide(PointType C, PointType A, PointType B, PointType like);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
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
                                                         std::vector<typename ImageType::PointType> & B);  
  template<class ImageType>
  void 
  SliceBySliceBuildLineSegmentAccordingToMinimalDistanceBetweenStructures(const ImageType * S1, 
                                                                          const ImageType * S2, 
                                                                          typename ImageType::PixelType BG, 
                                                                          int sliceDimension, 
                                                                          std::vector<typename ImageType::PointType> & A, 
                                                                          std::vector<typename ImageType::PointType> & B);  
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  SliceBySliceKeepMainCCL(const ImageType * input, 
                          typename ImageType::PixelType BG,
                          typename ImageType::PixelType FG);
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  Clone(const ImageType * input);
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  SliceBySliceSetBackgroundFromSingleLine(const ImageType * input, 
                                          typename ImageType::PixelType BG, 
                                          typename ImageType::PointType & A, 
                                          typename ImageType::PointType & B, 
                                          int dim1, int dim2, bool removeLowerPartFlag);
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  SliceBySliceSetBackgroundFromPoints(const ImageType * input, 
                                      typename ImageType::PixelType BG, 
                                      int sliceDim,
                                      std::vector<typename ImageType::PointType> & A, 
                                      bool removeGreaterThanXFlag,
                                      bool removeGreaterThanYFlag);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  FillRegionWithValue(ImageType * input, typename ImageType::PixelType value, 
                      typename ImageType::RegionType & region);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  GetMinMaxBoundary(ImageType * input, typename ImageType::PointType & min, 
                    typename ImageType::PointType & max);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename itk::Image<float, ImageType::ImageDimension>::Pointer//void
  DistanceMap(const ImageType * input, typename ImageType::PixelType BG);//, 
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::PointType
  ComputeClosestPoint(const ImageType * input, 
                      const itk::Image<float, ImageType::ImageDimension> * dmap, 
                      typename ImageType::PixelType & BG);
  //--------------------------------------------------------------------
  


} // end clitk namespace

#include "clitkSegmentationUtils.txx"

#endif
