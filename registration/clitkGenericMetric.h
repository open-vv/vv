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

#ifndef __clitkGenericMetric_h
#define __clitkGenericMetric_h

//clitk include
#include "clitkNormalizedCorrelationImageToImageMetric.h"
#include "clitkCorrelationRatioImageToImageMetric.h"
#include "itkMeanSquaresImageToImageMetricFor3DBLUTFFD.h"
#include "itkMattesMutualInformationImageToImageMetricFor3DBLUTFFD.h"
#include "clitkNormalizedCorrelationImageToImageMetricFor3DBLUTFFD.h"

//itk include
#include "itkSpatialObject.h"
#include "itkNormalizeImageFilter.h"
#include "itkImageToImageMetric.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkCorrelationCoefficientHistogramImageToImageMetric.h"
#include "itkGradientDifferenceImageToImageMetric.h"
#include "itkMutualInformationImageToImageMetric.h"
#include "itkMutualInformationHistogramImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkNormalizedMutualInformationHistogramImageToImageMetric.h"



/*

Requires at least the following section is the .ggo file. Adapt the defaults to the application


section "Metric (optimized, threaded versions are available for *, compile ITK with REVIEW and OPT_REGISTRATION enabled. Further optimized versions ** for  BLUT FFD optimizing a !3D! vector field  )"

option "metric" 	-       "Type: 0=SSD*, 1=Normalized CC*, 2=Histogram CC, 3=Gradient-Difference, 4=Viola-Wells MI, 5=Histogram MI, 6=Mattes' MI*, 7=Normalized MI, 8=CR, 9=SSD for BLUT FFD**, 10=CC for BLUT FFD**, 11=Mattes' MI for BLUT FFD**" 	int 	no 	default="0"
option "samples" 	-       "Specify fraction [0, 1] of samples of the reference image used for the metric (* only). Use high fraction for detailed images (eg. 0.2, 0.5), for smooth images 0.01 might be enough." float 	no 	default="1"
option "intThreshold"	-       "Fixed image samples intensity threshold (* only)" 			                               	float	no
option "subtractMean"	-       "1: Subtract mean for NCC calculation (narrows optimal)" 		                               	flag	on
option "bins"	 	-       "2,5-8: Number of histogram bins" 	                                                                int 	no      default="50"
option "random" 	-       "4,6: Samples should be taken randomly, otherwise uniformly"			                       	flag 	off
option "stdDev"		-       "4: specify the standard deviation in mm of the gaussian kernels for both PDF estimations" 		float 	no 	default="0.4"
option "explicitPDFDerivatives"	-       "6: Calculate PDF derivatives explicitly (rigid=true; FFD=false)" 		                flag    on


The use will look something like

typedef clitk::GenericMetric<args_info_type, FixedImageType, MovingImageType> GenericMetricType;
typename GenericMetricType::Pointer genericMetric=GenericMetricType::New();
genericMetric->SetArgsInfo(m_ArgsInfo);
genericMetric->SetFixedImageRegion(fixedImage->GetLargestPossibleRegion());
genericMetric->SetFixedImage(fixedImage);
genericMetric->SetFixedImageMask(fixedImageMask);
typedef itk::ImageToImageMetric<FixedImageType, MovingImageType> MetricType;
typename MetricType::Pointer metric=genericMetric->GetMetricPointer();

*/


namespace clitk
{

template <  class args_info_type, class FixedImageType,  class MovingImageType >
class GenericMetric : public itk::LightObject
{
public:
  //==============================================
  typedef GenericMetric     Self;
  typedef itk::LightObject     Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::SmartPointer<const Self>      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Determine the image dimension. */
  itkStaticConstMacro(FixedImageDimension, unsigned int,
                      FixedImageType::ImageDimension );
  itkStaticConstMacro(MovingImageDimension, unsigned int,
                      MovingImageType::ImageDimension );

  // Typedef
  typedef itk::ImageToImageMetric<FixedImageType, MovingImageType> MetricType;
  typedef typename MetricType::Pointer MetricPointer;
  typedef typename FixedImageType::RegionType FixedImageRegionType;
  typedef itk::SpatialObject<itkGetStaticConstMacro(FixedImageDimension)> FixedImageMaskType;
  typedef typename FixedImageMaskType::Pointer MaskPointer;
  typedef typename FixedImageType::PixelType FixedImagePixelType;
  typedef typename FixedImageType::IndexType FixedImageIndexType;
  typedef typename FixedImageType::PointType FixedImagePointType;

  //==============================================
  //Set members
  void SetArgsInfo(args_info_type args_info) {
    m_ArgsInfo= args_info;
    m_Verbose=m_ArgsInfo.verbose_flag;
  }
  void SetFixedImageRegion(const FixedImageRegionType f) {
    m_FixedImageRegion=f;
    m_FixedImageRegionGiven=true;
  }
  void SetFixedImage(typename FixedImageType::Pointer f) {
    m_FixedImage=f;
  }
  void SetFixedImageMask( const FixedImageMaskType* f) {
    m_FixedImageMask=f;
  }

  //==============================================
  //Get members
  MetricPointer GetMetricPointer(void);
  bool GetMaximize(void) {
    return m_Maximize;
  }


  //==============================================
protected:
  GenericMetric();
  ~GenericMetric() {};

private:
  args_info_type m_ArgsInfo;
  MetricPointer m_Metric;
  bool m_Maximize;
  bool m_Verbose;
  bool m_FixedImageRegionGiven;
  FixedImageRegionType m_FixedImageRegion;
  typename FixedImageType::Pointer m_FixedImage;
  typename FixedImageMaskType::ConstPointer m_FixedImageMask;

  FixedImagePixelType m_FixedImageSamplesIntensityThreshold;
  bool m_UseFixedImageSamplesIntensityThreshold;
};

} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkGenericMetric.txx"
#endif

#endif // #define __clitkGenericMetric_h
