#ifndef __clitkCorrelationRatioImageToImageMetric_h
#define __clitkCorrelationRatioImageToImageMetric_h

/**
 * @file   clitkCorrelationRatioImageToImageMetric.h
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   July 30  18:14:53 2007
 * 
 * @brief  Compute the correlation ratio between 2 images
 * 
 * 
 */

/* This computes the correlation ratio between 2 images
 *
 * This class is templated over the FixedImage type and the MovingImage 
 * type.
 *
 * The fixed and moving images are set via methods SetFixedImage() and
 * SetMovingImage(). This metric makes use of user specified Transform and
 * Interpolator. The Transform is used to map points from the fixed image to
 * the moving image domain. The Interpolator is used to evaluate the image
 * intensity at user specified geometric points in the moving image.
 * The Transform and Interpolator are set via methods SetTransform() and
 * SetInterpolator().
 *
 * The method GetValue() computes of the mutual information
 * while method GetValueAndDerivative() computes
 * both the mutual information and its derivatives with respect to the
 * transform parameters.
 *
 * The calculations are based on the method of Alexis Roche.
 *
 * The number of intensity bins used can be set through the SetNumberOfBins() method
 *
 * On Initialize(), we find the min and max intensities in the fixed image and compute the width of 
 * the intensity bins. The data structures which hold the bins and related measures are initialised.
 */


#include "itkImageToImageMetric.h"
#include "itkCovariantVector.h"
#include "itkPoint.h"

using namespace itk;
namespace clitk
{

template < class TFixedImage, class TMovingImage > 
class ITK_EXPORT CorrelationRatioImageToImageMetric: 
    public ImageToImageMetric< TFixedImage, TMovingImage>
{
public:

  /** Standard class typedefs. */
  typedef CorrelationRatioImageToImageMetric   Self;
  typedef ImageToImageMetric<TFixedImage, TMovingImage >  Superclass;

  typedef SmartPointer<Self>         Pointer;
  typedef SmartPointer<const Self>   ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
 
  /** Run-time type information (and related methods). */
  itkTypeMacro(CorrelationRatioImageToImageMetric, ImageToImageMetric);

 
  /** Types transferred from the base class */
  typedef typename Superclass::RealType                 RealType;
  typedef typename Superclass::TransformType            TransformType;
  typedef typename Superclass::TransformPointer         TransformPointer;
  typedef typename Superclass::TransformParametersType  TransformParametersType;
  typedef typename Superclass::TransformJacobianType    TransformJacobianType;
  typedef typename Superclass::GradientPixelType        GradientPixelType;

  typedef typename Superclass::MeasureType              MeasureType;
  typedef typename Superclass::DerivativeType           DerivativeType;
  typedef typename Superclass::FixedImageType           FixedImageType;
  typedef typename Superclass::MovingImageType          MovingImageType;
  typedef typename Superclass::FixedImageConstPointer   FixedImageConstPointer;
  typedef typename Superclass::MovingImageConstPointer  MovingImageConstPointer;


 /** 
   *  Initialize the Metric by 
   *  (1) making sure that all the components are present and plugged 
   *      together correctly,
   *  (3) allocate memory for bin data structures. */
  virtual void Initialize(void) throw ( ExceptionObject );

  /** Get the derivatives of the match measure. */
  void GetDerivative( const TransformParametersType & parameters,
                      DerivativeType & derivative ) const;

  /**  Get the value for single valued optimizers. */
  MeasureType GetValue( const TransformParametersType & parameters ) const;

  /**  Get value and derivatives for multiple valued optimizers. */
  void GetValueAndDerivative( const TransformParametersType & parameters,
                              MeasureType& Value, DerivativeType& Derivative ) const;

 /** Number of bins to used in the calculation. Typical value is 50. */
  itkSetClampMacro( NumberOfBins, unsigned long,
                    1, NumericTraits<unsigned long>::max() );
  itkGetConstReferenceMacro( NumberOfBins, unsigned long);   

protected:
  CorrelationRatioImageToImageMetric();
  virtual ~CorrelationRatioImageToImageMetric() {};

private:
  CorrelationRatioImageToImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  //the min and max in the fixed image intensity range
  double m_FixedImageMin;

  //The bin size in intensity
  double m_FixedImageBinSize;

  //The number of pixels in the fixed image bins are stored in a vector
   mutable std::vector<unsigned long> m_NumberOfPixelsCountedPerBin;

  //The mMSVPB and the mSMVPB are stored in vectors of realtype
  typedef float ValueType;
  typedef std::vector<ValueType> MeasurePerBinType;
  mutable MeasurePerBinType m_mMSVPB;
  mutable MeasurePerBinType m_mSMVPB;
  unsigned long m_NumberOfBins;


};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkCorrelationRatioImageToImageMetric.txx"
#endif

#endif



