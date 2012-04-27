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
#ifndef clitkMotionMaskGenericFilter_h
#define clitkMotionMaskGenericFilter_h

/* =================================================
 * @file   clitkMotionMaskGenericFilter.h
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkMotionMask_ggo.h"
#include "clitkSetBackgroundImageFilter.h"

//itk include
#include <itkLightObject.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkMirrorPadImageFilter.h>
#include <itkImageMomentsCalculator.h>
#include <itkResampleImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkGeodesicActiveContourLevelSetImageFilter.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>
#include <itkLabelStatisticsImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkCropImageFilter.h>
#include <itkBinaryMorphologicalClosingImageFilter.h>
#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkApproximateSignedDistanceMapImageFilter.h>

namespace clitk
{
class ITK_EXPORT MotionMaskGenericFilter : public itk::LightObject
{
public:
  //----------------------------------------
  // ITK
  //----------------------------------------
  typedef MotionMaskGenericFilter                   Self;
  typedef itk::LightObject                   Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::SmartPointer<const Self>      ConstPointer;

  // Method for creation through the object factory
  itkNewMacro(Self);

  // Run-time type information (and related methods)
  itkTypeMacro( MotionMaskGenericFilter, LightObject );

  //----------------------------------------
  // Typedefs
  //----------------------------------------
  typedef int InternalPixelType;


  //----------------------------------------
  // Set & Get
  //----------------------------------------
  void SetArgsInfo(const args_info_clitkMotionMask & a) {
    m_ArgsInfo=a;
    m_Verbose=m_ArgsInfo.verbose_flag;
    m_InputFileName=m_ArgsInfo.input_arg;
  }


  //----------------------------------------
  // Update
  //----------------------------------------
  void Update();

protected:

  //----------------------------------------
  // Constructor & Destructor
  //----------------------------------------
  MotionMaskGenericFilter();
  ~MotionMaskGenericFilter() {};


  //----------------------------------------
  // Templated members
  //----------------------------------------
  template <unsigned int Dimension, class PixelType>
  typename itk::Image<InternalPixelType, Dimension>::Pointer GetAirImage(typename itk::Image<PixelType, Dimension>::Pointer input,
      typename itk::Image<MotionMaskGenericFilter::InternalPixelType, Dimension>::Pointer lungs);
  template <unsigned int Dimension, class PixelType>
  typename itk::Image<InternalPixelType, Dimension>::Pointer GetBonesImage(typename itk::Image<PixelType, Dimension>::Pointer input );
  template <unsigned int Dimension, class PixelType>
  typename itk::Image<InternalPixelType, Dimension>::Pointer GetLungsImage(typename itk::Image<PixelType, Dimension>::Pointer input );
  template <unsigned int Dimension, class PixelType>
  typename itk::Image<InternalPixelType, Dimension>::Pointer Resample(typename itk::Image<InternalPixelType, Dimension>::Pointer input );
  template <unsigned int Dimension, class PixelType>
  typename itk::Image<InternalPixelType, Dimension>::Pointer InitializeEllips( typename itk::Vector<double,Dimension> center, typename itk::Image<InternalPixelType, Dimension>::Pointer bones_low, typename itk::Image<InternalPixelType,Dimension>::Pointer lungs_low);


  template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
  template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();


  //----------------------------------------
  // Data members
  //----------------------------------------
  args_info_clitkMotionMask m_ArgsInfo;
  bool m_Verbose;
  std::string m_InputFileName;

};


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkMotionMaskGenericFilter.txx"
#endif

#endif // #define clitkMotionMaskGenericFilter_h
