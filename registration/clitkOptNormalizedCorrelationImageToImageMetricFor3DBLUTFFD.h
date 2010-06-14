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

#ifndef __clitkOptNormalizedCorrelationImageToImageMetricFor3DBLUTFFD_h
#define __clitkOptNormalizedCorrelationImageToImageMetricFor3DBLUTFFD_h

#include "itkOptImageToImageMetric.h"
#include "itkCovariantVector.h"
#include "itkPoint.h"
#include "itkIndex.h"

#include "itkMultiThreader.h"

namespace clitk
{

template <class TFixedImage,class TMovingImage >
class ITK_EXPORT NormalizedCorrelationImageToImageMetricFor3DBLUTFFD :
  public itk::ImageToImageMetric< TFixedImage, TMovingImage >
{
public:

  /** Standard class typedefs. */
  typedef NormalizedCorrelationImageToImageMetricFor3DBLUTFFD                     Self;
  typedef itk::ImageToImageMetric< TFixedImage, TMovingImage >   Superclass;
  typedef itk::SmartPointer<Self>                                Pointer;
  typedef itk::SmartPointer<const Self>                          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NormalizedCorrelationImageToImageMetricFor3DBLUTFFD, ImageToImageMetric);

  /** Types inherited from Superclass. */
  typedef typename Superclass::RealType                 RealType;
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

  //Accumulators
  typedef  typename itk::NumericTraits< MeasureType >::AccumulateType AccumulateType;

  /** The moving image dimension. */
  itkStaticConstMacro( MovingImageDimension, unsigned int,
                       MovingImageType::ImageDimension );


  /** Set/Get SubtractMean boolean. If true, the sample mean is subtracted
   * from the sample values in the cross-correlation formula and
   * typically results in narrower valleys in the cost fucntion.
   * Default value is false. */
  itkSetMacro( SubtractMean, bool );
  itkGetConstReferenceMacro( SubtractMean, bool );
  itkBooleanMacro( SubtractMean );

  /**
   *  Initialize the Metric by
   *  (1) making sure that all the components are present and plugged
   *      together correctly,
   *  (2) uniformly select NumberOfSpatialSamples within
   *      the FixedImageRegion, and
   *  (3) allocate memory for pdf data structures. */
  virtual void Initialize(void) throw ( itk::ExceptionObject );

  /**  Get the value. */
  MeasureType GetValue( const ParametersType & parameters ) const;

  /** Get the derivatives of the match measure. */
  void GetDerivative( const ParametersType & parameters,
                      DerivativeType & Derivative ) const;

  /**  Get the value and derivatives for single valued optimizers. */
  void GetValueAndDerivative( const ParametersType & parameters,
                              MeasureType & Value,
                              DerivativeType & Derivative ) const;

protected:

  NormalizedCorrelationImageToImageMetricFor3DBLUTFFD();
  virtual ~NormalizedCorrelationImageToImageMetricFor3DBLUTFFD();
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:

  //purposely not implemented
  NormalizedCorrelationImageToImageMetricFor3DBLUTFFD(const Self &);
  //purposely not implemented
  void operator=(const Self &);

  /**  Get the value for the derivative computation. */
  MeasureType ComputeSums( const ParametersType & parameters ) const;


  inline bool GetValueThreadProcessSample( unsigned int threadID,
      unsigned long fixedImageSample,
      const MovingImagePointType & mappedPoint,
      double movingImageValue ) const;


  inline bool GetValueAndDerivativeThreadProcessSample( unsigned int threadID,
      unsigned long fixedImageSample,
      const MovingImagePointType & mappedPoint,
      double movingImageValue,
      const ImageDerivativesType &
      movingImageGradientValue ) const;

  AccumulateType *m_ThreaderSFF, *m_ThreaderSMM, *m_ThreaderSFM, *m_ThreaderSF, *m_ThreaderSM;
  mutable AccumulateType m_SFF, m_SMM, m_SFM, m_SF, m_SM;
  mutable RealType m_Denom, m_FixedMean, m_MovingMean;
  DerivativeType * m_ThreaderDerivativeF, *m_ThreaderDerivativeM;


  bool m_SubtractMean;
};

} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkOptNormalizedCorrelationImageToImageMetricFor3DBLUTFFD.txx"
#endif

#endif


