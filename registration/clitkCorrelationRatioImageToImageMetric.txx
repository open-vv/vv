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

#ifndef _clitkCorrelationRatioImageToImageMetric_txx
#define _clitkCorrelationRatioImageToImageMetric_txx

/**
 * @file   clitkCorrelationRatioImageToImageMetric.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   July 30  18:14:53 2007
 *
 * @brief  Compute the correlation ratio between 2 images
 *
 *
 */

#include "clitkCorrelationRatioImageToImageMetric.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

namespace clitk
{

/*
 * Constructor
 */
template <class TFixedImage, class TMovingImage>
CorrelationRatioImageToImageMetric<TFixedImage,TMovingImage>
::CorrelationRatioImageToImageMetric()
{
  m_NumberOfBins = 50;

}

template <class TFixedImage, class TMovingImage>
void
CorrelationRatioImageToImageMetric<TFixedImage,TMovingImage>
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR > 12 ) || ( ITK_VERSION_MAJOR > 4 ))
::Initialize(void)
#else
::Initialize(void) throw ( ExceptionObject )
#endif
{

  this->Superclass::Initialize();

  // Compute the minimum and maximum for the FixedImage over the FixedImageRegion.
  // We can't use StatisticsImageFilter to do this because the filter computes the min/max for the largest possible region
  double fixedImageMin = NumericTraits<double>::max();
  double fixedImageMax = NumericTraits<double>::NonpositiveMin();

  typedef ImageRegionConstIterator<FixedImageType> FixedIteratorType;
  FixedIteratorType fixedImageIterator(
    this->m_FixedImage, this->GetFixedImageRegion() );

  for ( fixedImageIterator.GoToBegin();
        !fixedImageIterator.IsAtEnd(); ++fixedImageIterator ) {

    double sample = static_cast<double>( fixedImageIterator.Get() );

    if ( sample < fixedImageMin ) {
      fixedImageMin = sample;
    }

    if ( sample > fixedImageMax ) {
      fixedImageMax = sample;
    }
  }

  // Compute binsize for the fixedImage
  m_FixedImageBinSize = ( fixedImageMax - fixedImageMin ) / m_NumberOfBins;
  m_FixedImageMin=fixedImageMin;
  //Allocate mempry and initialise the fixed image bin
  m_NumberOfPixelsCountedPerBin.resize( m_NumberOfBins, 0 );
  m_mMSVPB.resize( m_NumberOfBins, 0.0 );
  m_mSMVPB.resize( m_NumberOfBins, 0.0 );
}


/*
 * Get the match Measure
 */
template <class TFixedImage, class TMovingImage>
typename CorrelationRatioImageToImageMetric<TFixedImage,TMovingImage>::MeasureType
CorrelationRatioImageToImageMetric<TFixedImage,TMovingImage>
::GetValue( const TransformParametersType & parameters ) const
{

  itkDebugMacro("GetValue( " << parameters << " ) ");

  FixedImageConstPointer fixedImage = this->m_FixedImage;

  if( !fixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  typedef  itk::ImageRegionConstIteratorWithIndex<FixedImageType> FixedIteratorType;


  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  MeasureType measure = itk::NumericTraits< MeasureType >::Zero;

  this->m_NumberOfPixelsCounted = 0;
  this->SetTransformParameters( parameters );


  //temporary measures for the calculation
  RealType mSMV=0;
  RealType mMSV=0;

  while(!ti.IsAtEnd()) {

    index = ti.GetIndex();

    typename Superclass::InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    // Verify that the point is in the fixed Image Mask
    if( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) ) {
      ++ti;
      continue;
    }

    typename Superclass::OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    //Verify that the point is in the moving Image Mask
    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( transformedPoint ) ) {
      ++ti;
      continue;
    }

    // Verify is the interpolated value is in the buffer
    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) ) {
      //Accumulate calculations for the correlation ratio
      //For each pixel the is in both masks and the buffer we adapt the following measures:
      //movingMeanSquaredValue mMSV; movingSquaredMeanValue mSMV;
      //movingMeanSquaredValuePerBin[i] mSMVPB; movingSquaredMeanValuePerBin[i] mSMVPB
      //NumberOfPixelsCounted, NumberOfPixelsCountedPerBin[i]

      //get the value of the moving image
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );
      // for the variance of the overlapping moving image we accumulate the following measures
      const RealType movingSquaredValue=movingValue*movingValue;
      mMSV+=movingSquaredValue;
      mSMV+=movingValue;

      //get the fixed value
      const RealType fixedValue   = ti.Get();

      //check in which bin the fixed value belongs, get the index
      const double fixedImageBinTerm =        (fixedValue - m_FixedImageMin) / m_FixedImageBinSize;
      const unsigned int fixedImageBinIndex = static_cast<unsigned int>( vcl_floor(fixedImageBinTerm ) );
      //adapt the measures per bin
      this->m_mMSVPB[fixedImageBinIndex]+=movingSquaredValue;
      this->m_mSMVPB[fixedImageBinIndex]+=movingValue;
      //increase the fixed image bin and the total pixel count
      this->m_NumberOfPixelsCountedPerBin[fixedImageBinIndex]+=1;
      this->m_NumberOfPixelsCounted++;
    }

    ++ti;
  }

  if( !this->m_NumberOfPixelsCounted ) {
    itkExceptionMacro(<<"All the points mapped to outside of the moving image");
  } else {

    //apdapt the measures per bin
    for (unsigned int i=0; i< m_NumberOfBins; i++ ) {
      if (this->m_NumberOfPixelsCountedPerBin[i]>0) {
        measure+=(this->m_mMSVPB[i]-((this->m_mSMVPB[i]*this->m_mSMVPB[i])/this->m_NumberOfPixelsCountedPerBin[i]));
      }
    }

    //Normalize with the global measures
    measure /= (mMSV-((mSMV*mSMV)/ this->m_NumberOfPixelsCounted));
    return measure;

  }
}





/*
 * Get the Derivative Measure
 */
template < class TFixedImage, class TMovingImage>
void
CorrelationRatioImageToImageMetric<TFixedImage,TMovingImage>
::GetDerivative( const TransformParametersType & parameters,
                 DerivativeType & derivative  ) const
{

  itkDebugMacro("GetDerivative( " << parameters << " ) ");

  if( !this->GetGradientImage() ) {
    itkExceptionMacro(<<"The gradient image is null, maybe you forgot to call Initialize()");
  }

  FixedImageConstPointer fixedImage = this->m_FixedImage;

  if( !fixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  const unsigned int ImageDimension = FixedImageType::ImageDimension;


  typedef  itk::ImageRegionConstIteratorWithIndex<
  FixedImageType> FixedIteratorType;

  typedef  itk::ImageRegionConstIteratorWithIndex<
  typename Superclass::GradientImageType> GradientIteratorType;


  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType( ParametersDimension );
  derivative.Fill( itk::NumericTraits<typename DerivativeType::ValueType>::Zero );

  ti.GoToBegin();

  while(!ti.IsAtEnd()) {

    index = ti.GetIndex();

    typename Superclass::InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) ) {
      ++ti;
      continue;
    }

    typename Superclass::OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( transformedPoint ) ) {
      ++ti;
      continue;
    }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) ) {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );

      TransformJacobianType jacobian;
      this->m_Transform->ComputeJacobianWithRespectToParameters( inputPoint , jacobian);

      const RealType fixedValue     = ti.Value();
      this->m_NumberOfPixelsCounted++;
      const RealType diff = movingValue - fixedValue;

      // Get the gradient by NearestNeighboorInterpolation:
      // which is equivalent to round up the point components.
      typedef typename Superclass::OutputPointType OutputPointType;
      typedef typename OutputPointType::CoordRepType CoordRepType;
      typedef ContinuousIndex<CoordRepType,MovingImageType::ImageDimension>
      MovingImageContinuousIndexType;

      MovingImageContinuousIndexType tempIndex;
      this->m_MovingImage->TransformPhysicalPointToContinuousIndex( transformedPoint, tempIndex );

      typename MovingImageType::IndexType mappedIndex;
      for( unsigned int j = 0; j < MovingImageType::ImageDimension; j++ ) {
        mappedIndex[j] = static_cast<long>( vnl_math_rnd( tempIndex[j] ) );
      }

      const GradientPixelType gradient =
        this->GetGradientImage()->GetPixel( mappedIndex );

      for(unsigned int par=0; par<ParametersDimension; par++) {
        RealType sum = NumericTraits< RealType >::Zero;
        for(unsigned int dim=0; dim<ImageDimension; dim++) {
          sum += 2.0 * diff * jacobian( dim, par ) * gradient[dim];
        }
        derivative[par] += sum;
      }
    }

    ++ti;
  }

  if( !this->m_NumberOfPixelsCounted ) {
    itkExceptionMacro(<<"All the points mapped to outside of the moving image");
  } else {
    for(unsigned int i=0; i<ParametersDimension; i++) {
      derivative[i] /= this->m_NumberOfPixelsCounted;
    }
  }

}


/*
 * Get both the match Measure and the Derivative Measure
 */
template <class TFixedImage, class TMovingImage>
void
CorrelationRatioImageToImageMetric<TFixedImage,TMovingImage>
::GetValueAndDerivative(const TransformParametersType & parameters,
                        MeasureType & value, DerivativeType  & derivative) const
{

  itkDebugMacro("GetValueAndDerivative( " << parameters << " ) ");

  if( !this->GetGradientImage() ) {
    itkExceptionMacro(<<"The gradient image is null, maybe you forgot to call Initialize()");
  }

  FixedImageConstPointer fixedImage = this->m_FixedImage;

  if( !fixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  const unsigned int ImageDimension = FixedImageType::ImageDimension;

  typedef  itk::ImageRegionConstIteratorWithIndex<
  FixedImageType> FixedIteratorType;

  typedef  itk::ImageRegionConstIteratorWithIndex<
  typename Superclass::GradientImageType> GradientIteratorType;


  FixedIteratorType ti( fixedImage, this->GetFixedImageRegion() );

  typename FixedImageType::IndexType index;

  MeasureType measure = NumericTraits< MeasureType >::Zero;

  this->m_NumberOfPixelsCounted = 0;

  this->SetTransformParameters( parameters );

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType( ParametersDimension );
  derivative.Fill( NumericTraits<typename DerivativeType::ValueType>::Zero );

  ti.GoToBegin();

  while(!ti.IsAtEnd()) {

    index = ti.GetIndex();

    typename Superclass::InputPointType inputPoint;
    fixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

    if( this->m_FixedImageMask && !this->m_FixedImageMask->IsInside( inputPoint ) ) {
      ++ti;
      continue;
    }

    typename Superclass::OutputPointType transformedPoint = this->m_Transform->TransformPoint( inputPoint );

    if( this->m_MovingImageMask && !this->m_MovingImageMask->IsInside( transformedPoint ) ) {
      ++ti;
      continue;
    }

    if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) ) {
      const RealType movingValue  = this->m_Interpolator->Evaluate( transformedPoint );

      TransformJacobianType jacobian;
        this->m_Transform->ComputeJacobianWithRespectToParameters( inputPoint, jacobian );

      const RealType fixedValue     = ti.Value();
      this->m_NumberOfPixelsCounted++;

      const RealType diff = movingValue - fixedValue;

      measure += diff * diff;

      // Get the gradient by NearestNeighboorInterpolation:
      // which is equivalent to round up the point components.
      typedef typename Superclass::OutputPointType OutputPointType;
      typedef typename OutputPointType::CoordRepType CoordRepType;
      typedef ContinuousIndex<CoordRepType,MovingImageType::ImageDimension>
      MovingImageContinuousIndexType;

      MovingImageContinuousIndexType tempIndex;
      this->m_MovingImage->TransformPhysicalPointToContinuousIndex( transformedPoint, tempIndex );

      typename MovingImageType::IndexType mappedIndex;
      for( unsigned int j = 0; j < MovingImageType::ImageDimension; j++ ) {
        mappedIndex[j] = static_cast<long>( vnl_math_rnd( tempIndex[j] ) );
      }

      const GradientPixelType gradient =
        this->GetGradientImage()->GetPixel( mappedIndex );

      for(unsigned int par=0; par<ParametersDimension; par++) {
        RealType sum = NumericTraits< RealType >::Zero;
        for(unsigned int dim=0; dim<ImageDimension; dim++) {
          sum += 2.0 * diff * jacobian( dim, par ) * gradient[dim];
        }
        derivative[par] += sum;
      }
    }

    ++ti;
  }

  if( !this->m_NumberOfPixelsCounted ) {
    itkExceptionMacro(<<"All the points mapped to outside of the moving image");
  } else {
    for(unsigned int i=0; i<ParametersDimension; i++) {
      derivative[i] /= this->m_NumberOfPixelsCounted;
    }
    measure /= this->m_NumberOfPixelsCounted;
  }

  value = measure;

}

} // end namespace itk


#endif

