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

#ifndef _clitkNormalizedCorrelationImageToImageMetricFor3DBLUTFFD_txx
#define _clitkNormalizedCorrelationImageToImageMetricFor3DBLUTFFD_txx

// First make sure that the configuration is available.
// This line can be removed once the optimized versions
// gets integrated into the main directories.
#include "itkConfigure.h"

#ifdef ITK_USE_OPTIMIZED_REGISTRATION_METHODS
#include "clitkOptNormalizedCorrelationImageToImageMetricFor3DBLUTFFD.txx"
#else


#include "clitkNormalizedCorrelationImageToImageMetricFor3DBLUTFFD.h"

#include "itkImageRegionConstIteratorWithIndex.h"


namespace clitk
{

/*
 * Constructor
 */
template <class TFixedImage, class TMovingImage>
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::NormalizedCorrelationImageToImageMetricFor3DBLUTFFD()
{
  m_SubtractMean = false;
}

/*
 * Get the match Measure
 */
template <class TFixedImage, class TMovingImage>
typename NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>::MeasureType
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValue( const TransformParametersType & parameters ) const
{

  FixedImageConstPointer fixedImage = this->m_FixedImage;

  if( !fixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  typedef  itk::ImageRegionConstIteratorWithIndex<FixedImageType> FixedIteratorType;

  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  MeasureType measure;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  typedef  typename itk::NumericTraits< MeasureType >::AccumulateType AccumulateType;

  AccumulateType sff = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType smm = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sfm = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sf  = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sm  = itk::NumericTraits< AccumulateType >::Zero;

  while(!ti.IsAtEnd()) {

    index = ti.GetIndex();

    InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) ) {
      ++ti;
      continue;
    }

    OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( transformedPoint ) ) {
      ++ti;
      continue;
    }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) ) {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );
      const RealType fixedValue   = ti.Get();
      sff += fixedValue  * fixedValue;
      smm += movingValue * movingValue;
      sfm += fixedValue  * movingValue;
      if ( this->m_SubtractMean ) {
        sf += fixedValue;
        sm += movingValue;
      }
      this->m_NumberOfPixelsCounted++;
    }

    ++ti;
  }

  if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 ) {
    sff -= ( sf * sf / this->m_NumberOfPixelsCounted );
    smm -= ( sm * sm / this->m_NumberOfPixelsCounted );
    sfm -= ( sf * sm / this->m_NumberOfPixelsCounted );
  }

  const RealType denom = -1.0 * vcl_sqrt(sff * smm );

  if( this->m_NumberOfPixelsCounted > 0 && denom != 0.0) {
    measure = sfm / denom;
  } else {
    measure = itk::NumericTraits< MeasureType >::Zero;
  }

  return measure;

}





/*
 * Get the Derivative Measure
 */
template < class TFixedImage, class TMovingImage>
void
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetDerivative( const TransformParametersType & parameters,
                 DerivativeType & derivative ) const
{

  if( !this->GetGradientImage() ) {
    itkExceptionMacro(<<"The gradient image is null, maybe you forgot to call Initialize()");
  }

  FixedImageConstPointer fixedImage = this->m_FixedImage;

  if( !fixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  const unsigned int dimension = FixedImageType::ImageDimension;

  typedef  itk::ImageRegionConstIteratorWithIndex<FixedImageType> FixedIteratorType;

  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  typedef  typename itk::NumericTraits< MeasureType >::AccumulateType AccumulateType;

  AccumulateType sff  = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType smm  = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sfm = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sf  = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sm  = itk::NumericTraits< AccumulateType >::Zero;

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType( ParametersDimension );
  derivative.Fill( itk::NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  DerivativeType derivativeF = DerivativeType( ParametersDimension );
  derivativeF.Fill( itk::NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  DerivativeType derivativeM = DerivativeType( ParametersDimension );
  derivativeM.Fill( itk::NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  ti.GoToBegin();
  // First compute the sums
  while(!ti.IsAtEnd()) {

    index = ti.GetIndex();

    InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) ) {
      ++ti;
      continue;
    }

    OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( transformedPoint ) ) {
      ++ti;
      continue;
    }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) ) {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );
      const RealType fixedValue   = ti.Get();
      sff += fixedValue  * fixedValue;
      smm += movingValue * movingValue;
      sfm += fixedValue  * movingValue;
      if ( this->m_SubtractMean ) {
        sf += fixedValue;
        sm += movingValue;
      }
      this->m_NumberOfPixelsCounted++;
    }

    ++ti;
  }

  // Compute contributions to derivatives
  ti.GoToBegin();
  while(!ti.IsAtEnd()) {

    index = ti.GetIndex();

    InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if ( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) ) {
      ++ti;
      continue;
    }

    OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if ( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( transformedPoint ) ) {
      ++ti;
      continue;
    }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) ) {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );
      const RealType fixedValue   = ti.Get();

      const TransformJacobianType & jacobian =
        this->m_Transform->GetJacobian( inputPoint );

      // Get the gradient by NearestNeighboorInterpolation:
      // which is equivalent to round up the point components.
      typedef typename OutputPointType::CoordRepType CoordRepType;
      typedef itk::ContinuousIndex<CoordRepType,MovingImageType::ImageDimension>
      MovingImageContinuousIndexType;

      MovingImageContinuousIndexType tempIndex;
      this->m_MovingImage->TransformPhysicalPointToContinuousIndex( transformedPoint, tempIndex );

      typename MovingImageType::IndexType mappedIndex;
      mappedIndex.CopyWithRound( tempIndex );

      const GradientPixelType gradient =
        this->GetGradientImage()->GetPixel( mappedIndex );

      for(unsigned int par=0; par<ParametersDimension; par++) {
        RealType sumF = itk::NumericTraits< RealType >::Zero;
        RealType sumM = itk::NumericTraits< RealType >::Zero;
        for(unsigned int dim=0; dim<dimension; dim++) {
          const RealType differential = jacobian( dim, par ) * gradient[dim];
          sumF += fixedValue  * differential;
          sumM += movingValue * differential;
          if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 ) {
            sumF -= differential * sf / this->m_NumberOfPixelsCounted;
            sumM -= differential * sm / this->m_NumberOfPixelsCounted;
          }
        }
        derivativeF[par] += sumF;
        derivativeM[par] += sumM;
      }
    }

    ++ti;
  }

  if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 ) {
    sff -= ( sf * sf / this->m_NumberOfPixelsCounted );
    smm -= ( sm * sm / this->m_NumberOfPixelsCounted );
    sfm -= ( sf * sm / this->m_NumberOfPixelsCounted );
  }

  const RealType denom = -1.0 * vcl_sqrt(sff * smm );

  if( this->m_NumberOfPixelsCounted > 0 && denom != 0.0) {
    for(unsigned int i=0; i<ParametersDimension; i++) {
      derivative[i] = ( derivativeF[i] - (sfm/smm)* derivativeM[i] ) / denom;
    }
  } else {
    for(unsigned int i=0; i<ParametersDimension; i++) {
      derivative[i] = itk::NumericTraits< MeasureType >::Zero;
    }
  }

}


/*
 * Get both the match Measure and theDerivative Measure
 */
template <class TFixedImage, class TMovingImage>
void
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValueAndDerivative(const TransformParametersType & parameters,
                        MeasureType & value, DerivativeType  & derivative) const
{


  if( !this->GetGradientImage() ) {
    itkExceptionMacro(<<"The gradient image is null, maybe you forgot to call Initialize()");
  }

  FixedImageConstPointer fixedImage = this->m_FixedImage;

  if( !fixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  const unsigned int dimension = FixedImageType::ImageDimension;

  typedef  itk::ImageRegionConstIteratorWithIndex<FixedImageType> FixedIteratorType;

  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  typedef  typename itk::NumericTraits< MeasureType >::AccumulateType AccumulateType;

  AccumulateType sff  = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType smm  = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sfm  = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sf   = itk::NumericTraits< AccumulateType >::Zero;
  AccumulateType sm   = itk::NumericTraits< AccumulateType >::Zero;

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType( ParametersDimension );
  derivative.Fill( itk::NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  DerivativeType derivativeF = DerivativeType( ParametersDimension );
  derivativeF.Fill( itk::NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  DerivativeType derivativeM = DerivativeType( ParametersDimension );
  derivativeM.Fill( itk::NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  DerivativeType derivativeM1 = DerivativeType( ParametersDimension );
  derivativeM1.Fill( itk::NumericTraits<ITK_TYPENAME DerivativeType::ValueType>::Zero );

  ti.GoToBegin();
  // First compute the sums
  while(!ti.IsAtEnd()) {

    index = ti.GetIndex();

    InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) ) {
      ++ti;
      continue;
    }

    OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( transformedPoint ) ) {
      ++ti;
      continue;
    }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) ) {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );
      const RealType fixedValue   = ti.Get();
      sff += fixedValue  * fixedValue;
      smm += movingValue * movingValue;
      sfm += fixedValue  * movingValue;
      if ( this->m_SubtractMean ) {
        sf += fixedValue;
        sm += movingValue;
      }
      this->m_NumberOfPixelsCounted++;
    }

    ++ti;
  }


  // Compute contributions to derivatives
  ti.GoToBegin();
  while(!ti.IsAtEnd()) {

    index = ti.GetIndex();

    InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) ) {
      ++ti;
      continue;
    }

    OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( transformedPoint ) ) {
      ++ti;
      continue;
    }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) ) {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );
      const RealType fixedValue     = ti.Get();

      const TransformJacobianType & jacobian =
        this->m_Transform->GetJacobian( inputPoint );

      // Get the gradient by NearestNeighboorInterpolation:
      // which is equivalent to round up the point components.
      typedef typename OutputPointType::CoordRepType CoordRepType;
      typedef itk::ContinuousIndex<CoordRepType,MovingImageType::ImageDimension>
      MovingImageContinuousIndexType;

      MovingImageContinuousIndexType tempIndex;
      this->m_MovingImage->TransformPhysicalPointToContinuousIndex( transformedPoint, tempIndex );

      typename MovingImageType::IndexType mappedIndex;
      mappedIndex.CopyWithRound( tempIndex );

      const GradientPixelType gradient =
        this->GetGradientImage()->GetPixel( mappedIndex );

      for(unsigned int par=0; par<ParametersDimension; par++) {
        RealType sumF = itk::NumericTraits< RealType >::Zero;
        RealType sumM = itk::NumericTraits< RealType >::Zero;
        for(unsigned int dim=0; dim<dimension; dim++) {
          const RealType differential = jacobian( dim, par ) * gradient[dim];
          sumF += fixedValue  * differential;
          sumM += movingValue * differential;
          if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 ) {
            sumF -= differential * sf / this->m_NumberOfPixelsCounted;
            sumM -= differential * sm / this->m_NumberOfPixelsCounted;
          }
        }
        derivativeF[par] += sumF;
        derivativeM[par] += sumM;
      }
    }
    ++ti;
  }

  if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 ) {
    sff -= ( sf * sf / this->m_NumberOfPixelsCounted );
    smm -= ( sm * sm / this->m_NumberOfPixelsCounted );
    sfm -= ( sf * sm / this->m_NumberOfPixelsCounted );
  }

  const RealType denom = -1.0 * vcl_sqrt(sff * smm );

  if( this->m_NumberOfPixelsCounted > 0 && denom != 0.0) {
    for(unsigned int i=0; i<ParametersDimension; i++) {
      derivative[i] = ( derivativeF[i] - (sfm/smm)* derivativeM[i] ) / denom;
    }
    value = sfm / denom;
  } else {
    for(unsigned int i=0; i<ParametersDimension; i++) {
      derivative[i] = itk::NumericTraits< MeasureType >::Zero;
    }
    value = itk::NumericTraits< MeasureType >::Zero;
  }



}

template < class TFixedImage, class TMovingImage>
void
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "SubtractMean: " << m_SubtractMean << std::endl;
}

} // end namespace itk


#endif // opt

#endif // _clitkNormalizedCorrelationImageToImageMetricFor3DBLUTFFD.txx
