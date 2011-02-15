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

// itk
#include <itkBoundingBox.h>

namespace clitk {

  //--------------------------------------------------------------------
  template<class ImageType>
  void ComputeBBFromImageRegion(typename ImageType::Pointer image, 
                                typename ImageType::RegionType region,
                                typename itk::BoundingBox<unsigned long, 
                                                          ImageType::ImageDimension>::Pointer bb);
  
  //--------------------------------------------------------------------
  template<int Dimension>
  void ComputeBBIntersection(typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbo, 
                             typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi1, 
                             typename itk::BoundingBox<unsigned long, Dimension>::Pointer bbi2);

  //--------------------------------------------------------------------
  template<class ImageType>
  void ComputeRegionFromBB(typename ImageType::Pointer image, 
                           const typename itk::BoundingBox<unsigned long, 
                           ImageType::ImageDimension>::Pointer bb, 
                           typename ImageType::RegionType & region);
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
  int GetNumberOfConnectedComponentLabels(typename ImageType::Pointer input, 
                                          typename ImageType::PixelType BG, 
                                          bool isFullyConnected);
  //--------------------------------------------------------------------


  //-------------------------------------------------------------------- 
  template<class TImageType>
  typename TImageType::Pointer
  Labelize(const TImageType * input, 
           typename TImageType::PixelType BG, 
           bool isFullyConnected, 
           int minimalComponentSize);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  RemoveLabels(typename ImageType::Pointer input, 
               typename ImageType::PixelType BG, 
               std::vector<typename ImageType::PixelType> & labelsToRemove);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  AutoCrop(typename ImageType::Pointer input, 
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
  LabelizeAndSelectLabels(typename TImageType::Pointer input,
                          typename TImageType::PixelType BG, 
                          typename TImageType::PixelType FG, 
                          bool isFullyConnected,
                          int minimalComponentSize,
                          LabelizeParameters<typename TImageType::PixelType> * param);

  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  ResizeImageLike(typename ImageType::Pointer input,
                  typename ImageType::Pointer like, 
                  typename ImageType::PixelType BG);


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
			       bool notflag=false);

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
  CropImageAlongOneAxis(typename ImageType::Pointer image, 
                        int dim, double min, double max, 
                        bool autoCrop = false,
                        typename ImageType::PixelType BG=0);
  template<class ImageType>
  typename ImageType::Pointer
  CropImageAbove(typename ImageType::Pointer image, 
                 int dim, double min, 
                 bool autoCrop = false,
                 typename ImageType::PixelType BG=0);
  template<class ImageType>
  typename ImageType::Pointer
  CropImageBelow(typename ImageType::Pointer image, 
                 int dim, double max,
                 bool autoCrop = false,
                 typename ImageType::PixelType BG=0);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  ComputeCentroids(typename ImageType::Pointer image, 
                   typename ImageType::PixelType BG, 
                   std::vector<typename ImageType::PointType> & centroids);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void
  ExtractSlices(typename ImageType::Pointer image, 
		int dim, 
		std::vector< typename itk::Image<typename ImageType::PixelType, 
		ImageType::ImageDimension-1>::Pointer > & slices);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  typename ImageType::Pointer
  JoinSlices(std::vector<typename itk::Image<typename ImageType::PixelType, 
	     ImageType::ImageDimension-1>::Pointer > & slices, 
	     typename ImageType::Pointer input, 
	     int dim);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  // Set of tools to manage 3D points and 2D points in slices  
  template<class ImageType>
  class PointsUtils
  {
    typedef typename ImageType::PointType PointType3D;
    typedef typename ImageType::PixelType PixelType;
    typedef typename ImageType::Pointer ImagePointer;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef itk::Image<PixelType, 2> SliceType;
    typedef typename SliceType::PointType PointType2D;
    
    typedef std::map<int, PointType2D> MapPoint2DType;
    typedef std::vector<PointType3D> VectorPoint3DType;
  public:
    static void Convert2DTo3D(const PointType2D & p2D, 
                              ImagePointer image, 
                              const int slice, 
                              PointType3D & p3D);
    static void Convert2DTo3DList(const MapPoint2DType & map, 
                                  ImagePointer image, 
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
  Dilate(typename ImageType::Pointer image, 
              double radiusInMM,               
              typename ImageType::PixelType BG, 
              typename ImageType::PixelType FG, 
              bool extendSupport);
  template<class ImageType>
  typename ImageType::Pointer
  Dilate(typename ImageType::Pointer image, 
              typename ImageType::SizeType radius, 
              typename ImageType::PixelType BG, 
              typename ImageType::PixelType FG, 
              bool extendSupport);
  template<class ImageType>
  typename ImageType::Pointer  
  Dilate(typename ImageType::Pointer image, 
              typename ImageType::PointType radiusInMM, 
              typename ImageType::PixelType BG, 
              typename ImageType::PixelType FG, 
              bool extendSupport);
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  template<class ValueType, class VectorType>
  void ConvertOption(std::string optionName, uint given, 
                     ValueType * values, VectorType & p, 
                     uint dim, bool required);
#define ConvertOptionMacro(OPTIONNAME, VAR, DIM, REQUIRED)         \
  ConvertOption(#OPTIONNAME, OPTIONNAME##_given, OPTIONNAME##_arg, VAR, DIM, REQUIRED);

}

#include "clitkSegmentationUtils.txx"

#endif
