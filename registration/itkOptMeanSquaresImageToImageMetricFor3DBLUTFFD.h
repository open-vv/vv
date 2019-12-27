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

/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkOptMeanSquaresImageToImageMetricFor3DBLUTFFD.h,v $
  Language:  C++
  Date:      $Date: 2010/06/14 17:32:07 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkOptMeanSquaresImageToImageMetricFor3DBLUTFFD_h
#define __itkOptMeanSquaresImageToImageMetricFor3DBLUTFFD_h

#include "itkImageToImageMetric.h"
#include "itkCovariantVector.h"
#include "itkPoint.h"
#include "itkIndex.h"

namespace itk
{

template <class TFixedImage,class TMovingImage >
class ITK_EXPORT MeanSquaresImageToImageMetricFor3DBLUTFFD :
  public ImageToImageMetric< TFixedImage, TMovingImage >
{
public:

  /** Standard class typedefs. */
  typedef MeanSquaresImageToImageMetricFor3DBLUTFFD                     Self;
  typedef ImageToImageMetric< TFixedImage, TMovingImage >   Superclass;
  typedef SmartPointer<Self>                                Pointer;
  typedef SmartPointer<const Self>                          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MeanSquaresImageToImageMetricFor3DBLUTFFD, ImageToImageMetric);

  /** Types inherited from Superclass. */
  typedef typename Superclass::TransformType            TransformType;
  typedef typename Superclass::TransformPointer         TransformPointer;
  typedef typename Superclass::TransformJacobianType    TransformJacobianType;
  typedef typename Superclass::InterpolatorType         InterpolatorType;
  typedef typename Superclass::MeasureType              MeasureType;
  typedef typename Superclass::DerivativeType           DerivativeType;
  typedef typename Superclass::ParametersType           ParametersType;
  typedef typename Superclass::FixedImageType           FixedImageType;
  typedef typename Superclass::MovingImageType          MovingImageType;
  typedef typename Superclass::MovingImagePointType     MovingImagePointType;
  typedef typename Superclass::FixedImageConstPointer   FixedImageConstPointer;
  typedef typename Superclass::MovingImageConstPointer  MovingImageConstPointer;
  typedef typename Superclass::CoordinateRepresentationType
  CoordinateRepresentationType;
  typedef typename Superclass::FixedImageSampleContainer
  FixedImageSampleContainer;
  typedef typename Superclass::ImageDerivativesType     ImageDerivativesType;
  typedef typename Superclass::WeightsValueType         WeightsValueType;
  typedef typename Superclass::IndexValueType           IndexValueType;

  // Needed for evaluation of Jacobian.
  typedef typename Superclass::FixedImagePointType     FixedImagePointType;

  /** The moving image dimension. */
  itkStaticConstMacro( MovingImageDimension, unsigned int,
                       MovingImageType::ImageDimension );

  /**
   *  Initialize the Metric by
   *  (1) making sure that all the components are present and plugged
   *      together correctly,
   *  (2) uniformly select NumberOfSpatialSamples within
   *      the FixedImageRegion, and
   *  (3) allocate memory for pdf data structures. */
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR > 12 ) || ( ITK_VERSION_MAJOR > 4 ))
  virtual void Initialize(void) ITK_OVERRIDE;
#else
  virtual void Initialize(void) throw ( ExceptionObject ) ITK_OVERRIDE;
#endif

  /**  Get the value. */
  MeasureType GetValue( const ParametersType & parameters ) const ITK_OVERRIDE;

  /** Get the derivatives of the match measure. */
  void GetDerivative( const ParametersType & parameters,
                      DerivativeType & Derivative ) const ITK_OVERRIDE;

  /**  Get the value and derivatives for single valued optimizers. */
  void GetValueAndDerivative( const ParametersType & parameters,
                              MeasureType & Value,
                              DerivativeType & Derivative ) const ITK_OVERRIDE;

protected:

  MeanSquaresImageToImageMetricFor3DBLUTFFD();
  virtual ~MeanSquaresImageToImageMetricFor3DBLUTFFD();
  void PrintSelf(std::ostream& os, Indent indent) const ITK_OVERRIDE;

private:

  //purposely not implemented
  MeanSquaresImageToImageMetricFor3DBLUTFFD(const Self &);
  //purposely not implemented
  void operator=(const Self &);

  inline bool GetValueThreadProcessSample( itk::ThreadIdType threadID,
      itk::SizeValueType fixedImageSample,
      const MovingImagePointType & mappedPoint,
      double movingImageValue ) const ITK_OVERRIDE;

  inline bool GetValueAndDerivativeThreadProcessSample( itk::ThreadIdType threadID,
      itk::SizeValueType fixedImageSample,
      const MovingImagePointType & mappedPoint,
      double movingImageValue,
      const ImageDerivativesType &
      movingImageGradientValue ) const ITK_OVERRIDE;

  MeasureType    * m_ThreaderMSE;
  DerivativeType * m_ThreaderMSEDerivatives;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOptMeanSquaresImageToImageMetricFor3DBLUTFFD.txx"
#endif

#endif
