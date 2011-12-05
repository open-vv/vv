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

#ifndef __clitkOptNormalizedCorrelationImageToImageMetricFor3DBLUTFFD_txx
#define __clitkOptNormalizedCorrelationImageToImageMetricFor3DBLUTFFD_txx

#include "clitkOptNormalizedCorrelationImageToImageMetricFor3DBLUTFFD.h"
#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageIterator.h"
#include "vnl/vnl_math.h"

namespace clitk
{

/**
 * Constructor
 */
template < class TFixedImage, class TMovingImage >
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::NormalizedCorrelationImageToImageMetricFor3DBLUTFFD()
{
  this->SetComputeGradient(true);

  m_ThreaderSFF = NULL;
  m_ThreaderSMM = NULL;
  m_ThreaderSFM = NULL;
  m_ThreaderSF = NULL;
  m_ThreaderSM = NULL;
  m_ThreaderDerivativeF = NULL;
  m_ThreaderDerivativeM = NULL;
  this->m_WithinThreadPreProcess = false;
  this->m_WithinThreadPostProcess = false;

  //  For backward compatibility, the default behavior is to use all the pixels
  //  in the fixed image.
  this->UseAllPixelsOn();

  m_SubtractMean=false;

}

template < class TFixedImage, class TMovingImage >
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::~NormalizedCorrelationImageToImageMetricFor3DBLUTFFD()
{
  if(m_ThreaderSFF != NULL) {
    delete [] m_ThreaderSFF;
  }
  m_ThreaderSFF = NULL;

  if(m_ThreaderSMM != NULL) {
    delete [] m_ThreaderSMM;
  }
  m_ThreaderSMM = NULL;

  if(m_ThreaderSFM != NULL) {
    delete [] m_ThreaderSFM;
  }
  m_ThreaderSFM = NULL;

  if(m_ThreaderSF != NULL) {
    delete [] m_ThreaderSF;
  }
  m_ThreaderSF = NULL;

  if(m_ThreaderSM != NULL) {
    delete [] m_ThreaderSM;
  }
  m_ThreaderSM = NULL;

  if(m_ThreaderDerivativeF != NULL) {
    delete [] m_ThreaderDerivativeF;
  }
  m_ThreaderDerivativeF = NULL;

  if(m_ThreaderDerivativeM != NULL) {
    delete [] m_ThreaderDerivativeM;
  }
  m_ThreaderDerivativeM = NULL;
}

/**
 * Print out internal information about this class
 */
template < class TFixedImage, class TMovingImage  >
void
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{

  Superclass::PrintSelf(os, indent);

}


/**
 * Initialize
 */
template <class TFixedImage, class TMovingImage>
void
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::Initialize(void) throw ( itk::ExceptionObject )
{

  this->Superclass::Initialize();
  this->Superclass::MultiThreadingInitialize();


  /**
   * Allocate memory for the accumulators (set to zero in GetValue)
   */
  if(m_ThreaderSFF != NULL) {
    delete [] m_ThreaderSFF;
  }
  m_ThreaderSFF = new double[this->m_NumberOfThreads];


  if(m_ThreaderSMM != NULL) {
    delete [] m_ThreaderSMM;
  }
  m_ThreaderSMM = new double[this->m_NumberOfThreads];

  if(m_ThreaderSFM != NULL) {
    delete [] m_ThreaderSFM;
  }
  m_ThreaderSFM = new double[this->m_NumberOfThreads];

  if(this->m_SubtractMean) {
    if(m_ThreaderSF != NULL) {
      delete [] m_ThreaderSF;
    }
    m_ThreaderSF = new double[this->m_NumberOfThreads];

    if(m_ThreaderSM != NULL) {
      delete [] m_ThreaderSM;
    }
    m_ThreaderSM = new double[this->m_NumberOfThreads];
  }

  if(m_ThreaderDerivativeF != NULL) {
    delete [] m_ThreaderDerivativeF;
  }
  m_ThreaderDerivativeF = new DerivativeType[this->m_NumberOfThreads];
  for(ThreadIdType threadID=0; threadID<this->m_NumberOfThreads; threadID++) {
    m_ThreaderDerivativeF[threadID].SetSize( this->m_NumberOfParameters );
  }

  if(m_ThreaderDerivativeM != NULL) {
    delete [] m_ThreaderDerivativeM;
  }
  m_ThreaderDerivativeM = new DerivativeType[this->m_NumberOfThreads];
  for(ThreadIdType threadID=0; threadID<this->m_NumberOfThreads; threadID++) {
    m_ThreaderDerivativeM[threadID].SetSize( this->m_NumberOfParameters );
  }
}


template < class TFixedImage, class TMovingImage  >
inline bool
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValueThreadProcessSample(
  ThreadIdType threadID,
  unsigned long fixedImageSample,
  const MovingImagePointType & itkNotUsed(mappedPoint),
  double movingImageValue) const
{
  const RealType fixedImageValue= this->m_FixedImageSamples[fixedImageSample].value;
  m_ThreaderSFF[threadID] += fixedImageValue  * fixedImageValue;
  m_ThreaderSMM[threadID] += movingImageValue * movingImageValue;
  m_ThreaderSFM[threadID] += fixedImageValue  * movingImageValue;
  if ( this->m_SubtractMean ) {
    m_ThreaderSF[threadID] += fixedImageValue;
    m_ThreaderSM[threadID] += movingImageValue;
  }

  return true;
}

template < class TFixedImage, class TMovingImage  >
typename NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::MeasureType
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValue( const ParametersType & parameters ) const
{
  itkDebugMacro("GetValue( " << parameters << " ) ");

  if( !this->m_FixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }


  //Reset the accumulators
  memset( m_ThreaderSFF,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
  memset( m_ThreaderSMM,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
  memset( m_ThreaderSFM,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
  if(this->m_SubtractMean) {
    memset( m_ThreaderSF,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
    memset( m_ThreaderSM,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
  }


  // Set up the parameters in the transform
  this->m_Transform->SetParameters( parameters );
#if ITK_VERSION_MAJOR < 4
  this->m_Parameters = parameters;
#endif

  // MUST BE CALLED TO INITIATE PROCESSING
  this->GetValueMultiThreadedInitiate();

  itkDebugMacro( "Ratio of voxels mapping into moving image buffer: "
                 << this->m_NumberOfPixelsCounted << " / "
                 << this->m_NumberOfFixedImageSamples
                 << std::endl );

  if( this->m_NumberOfPixelsCounted <
      this->m_NumberOfFixedImageSamples / 4 ) {
    itkExceptionMacro( "Too many samples map outside moving image buffer: "
                       << this->m_NumberOfPixelsCounted << " / "
                       << this->m_NumberOfFixedImageSamples
                       << std::endl );
  }

  // Accumulate the threads
  AccumulateType sff, smm, sfm, sf, sm;
  sff = m_ThreaderSFF[0];
  smm = m_ThreaderSMM[0];
  sfm = m_ThreaderSFM[0];
  sf  = m_ThreaderSF[0];
  sm  = m_ThreaderSM[0];

  for(unsigned int t=1; t<this->m_NumberOfThreads; t++) {
    sff +=  m_ThreaderSFF[t];
    smm +=  m_ThreaderSMM[t];
    sfm +=  m_ThreaderSFM[t];
    if ( this->m_SubtractMean ) {
      sf +=  m_ThreaderSF[t];
      sm +=  m_ThreaderSM[t];
    }
  }

  if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 ) {
    sff -= ( sf * sf / this->m_NumberOfPixelsCounted );
    smm -= ( sm * sm / this->m_NumberOfPixelsCounted );
    sfm -= ( sf * sm / this->m_NumberOfPixelsCounted );
  }


  const RealType denom = -1.0 * vcl_sqrt(sff * smm );
  MeasureType measure;
  if( this->m_NumberOfPixelsCounted > 0 && denom != 0.0) {
    measure = sfm / denom;
  } else {
    measure = itk::NumericTraits< MeasureType >::Zero;
  }


  return measure;
}


template < class TFixedImage, class TMovingImage  >
typename NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::MeasureType
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::ComputeSums( const ParametersType & parameters ) const
{
  //No checking for the fixed image,  done in the caller
  //Reset the accumulators
  memset( m_ThreaderSFF,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
  memset( m_ThreaderSMM,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
  memset( m_ThreaderSFM,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
  if(this->m_SubtractMean) {
    memset( m_ThreaderSF,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
    memset( m_ThreaderSM,  0,  this->m_NumberOfThreads * sizeof(AccumulateType) );
  }


  // Set up the parameters in the transform
  this->m_Transform->SetParameters( parameters );
#if ITK_VERSION_MAJOR < 4
  this->m_Parameters = parameters;
#endif

  // MUST BE CALLED TO INITIATE PROCESSING
  this->GetValueMultiThreadedInitiate();

  itkDebugMacro( "Ratio of voxels mapping into moving image buffer: "
                 << this->m_NumberOfPixelsCounted << " / "
                 << this->m_NumberOfFixedImageSamples
                 << std::endl );

  if( this->m_NumberOfPixelsCounted <
      this->m_NumberOfFixedImageSamples / 4 ) {
    itkExceptionMacro( "Too many samples map outside moving image buffer: "
                       << this->m_NumberOfPixelsCounted << " / "
                       << this->m_NumberOfFixedImageSamples
                       << std::endl );
  }

  // Accumulate the threads
  m_SFF = m_ThreaderSFF[0];
  m_SMM = m_ThreaderSMM[0];
  m_SFM = m_ThreaderSFM[0];
  m_SF  = m_ThreaderSF[0];
  m_SM  = m_ThreaderSM[0];

  for(unsigned int t=1; t<this->m_NumberOfThreads; t++) {
    m_SFF +=  m_ThreaderSFF[t];
    m_SMM +=  m_ThreaderSMM[t];
    m_SFM +=  m_ThreaderSFM[t];
    if ( this->m_SubtractMean ) {
      m_SF +=  m_ThreaderSF[t];
      m_SM +=  m_ThreaderSM[t];
    }
  }

  if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 ) {
    m_SFF -= ( m_SF * m_SF / this->m_NumberOfPixelsCounted );
    m_SMM -= ( m_SM * m_SM / this->m_NumberOfPixelsCounted );
    m_SFM -= ( m_SF * m_SM / this->m_NumberOfPixelsCounted );
    m_FixedMean=m_SF / this->m_NumberOfPixelsCounted;
    m_MovingMean=m_SM / this->m_NumberOfPixelsCounted;
  }


  m_Denom = -1.0 * vcl_sqrt(m_SFF * m_SMM );
  MeasureType measure;
  if( this->m_NumberOfPixelsCounted > 0 && m_Denom != 0.0) {
    measure = m_SFM / m_Denom;
  } else {
    measure = itk::NumericTraits< MeasureType >::Zero;
  }

  return measure;
}


template < class TFixedImage, class TMovingImage  >
inline bool
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValueAndDerivativeThreadProcessSample(
  ThreadIdType threadID,
  unsigned long fixedImageSample,
  const MovingImagePointType & itkNotUsed(mappedPoint),
  double movingImageValue,
  const ImageDerivativesType &
  movingImageGradientValue
) const
{

  const RealType fixedImageValue=this->m_FixedImageSamples[fixedImageSample].value;
  const FixedImagePointType fixedImagePoint = this->m_FixedImageSamples[fixedImageSample].point;

  // Need to use one of the threader transforms if we're
  // not in thread 0.
  //
  // Use a raw pointer here to avoid the overhead of smart pointers.
  // For instance, Register and UnRegister have mutex locks around
  // the reference counts.
  TransformType* transform;

  if (threadID > 0) {
    transform = this->m_ThreaderTransform[threadID - 1];
  } else {
    transform = this->m_Transform;
  }

  // Jacobian should be evaluated at the unmapped (fixed image) point.
#if ITK_VERSION_MAJOR >= 4
  TransformJacobianType jacobian;
  transform->ComputeJacobianWithRespectToParameters( fixedImagePoint, jacobian );
#else
  const TransformJacobianType & jacobian = transform->GetJacobian( fixedImagePoint );
#endif

//          for(unsigned int par=0; par<this->m_NumberOfParameters; par++)
//            {
//     	 RealType sumF = itk::NumericTraits< RealType >::Zero;
//     	 RealType sumM = itk::NumericTraits< RealType >::Zero;
//     	 for(unsigned int dim=0; dim<MovingImageDimension; dim++)
//     	   {
//     	     const RealType differential = jacobian( dim, par ) * movingImageGradientValue[dim];
//     	     if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 )
//     	       {
//     		 sumF += (fixedImageValue-m_FixedMean)  * differential;
//     		 sumM += (movingImageValue-m_MovingMean) * differential;
//     	       }
//     	     else
//     	       {
//     		 sumF += differential * fixedImageValue;
//     		 sumM += differential * movingImageValue;
//     	       }
//     	   }
//     	 m_ThreaderDerivativeF[threadID][par] += sumF;
//     	 m_ThreaderDerivativeM[threadID][par] += sumM;
//            }

  // JV
  unsigned int par, dim;
  RealType differential;
  for( par=0; par<this->m_NumberOfParameters; par+=3) {
    // JV only for 3D Space BLUT FFD: if J(0, par)=0, then J(1,par+1)=0 & ...
    if (jacobian( 0, par ) ) {
      for(dim=0; dim<3; dim++) {
        differential = jacobian( dim, par+dim ) * movingImageGradientValue[dim];

        if ( this->m_SubtractMean && this->m_NumberOfPixelsCounted > 0 ) {
          m_ThreaderDerivativeF[threadID][par+dim]+= (fixedImageValue-m_FixedMean)  * differential;
          m_ThreaderDerivativeM[threadID][par+dim]+= (movingImageValue-m_MovingMean) * differential;
        } else {
          m_ThreaderDerivativeF[threadID][par+dim]+= differential * fixedImageValue;
          m_ThreaderDerivativeM[threadID][par+dim]+= differential * movingImageValue;
        }
      }
    }
  }

  return true;
}


/**
 * Get the both Value and Derivative Measure
 */
template < class TFixedImage, class TMovingImage  >
void
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValueAndDerivative( const ParametersType & parameters,
                         MeasureType & value,
                         DerivativeType & derivative) const
{

  if( !this->m_FixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  // Set up the parameters in the transform
  this->m_Transform->SetParameters( parameters );
#if ITK_VERSION_MAJOR < 4
  this->m_Parameters = parameters;
#endif

  //We need the sums and the value to be calculated first
  value=this->ComputeSums(parameters);

  //Set output values to zero
  if(derivative.GetSize() != this->m_NumberOfParameters) {
    derivative = DerivativeType( this->m_NumberOfParameters );
  }
  memset( derivative.data_block(),
          0,
          this->m_NumberOfParameters * sizeof(typename DerivativeType::ValueType) );

  for( ThreadIdType threadID = 0; threadID<this->m_NumberOfThreads; threadID++ ) {
    memset( m_ThreaderDerivativeF[threadID].data_block(),
            0,
            this->m_NumberOfParameters * sizeof(typename DerivativeType::ValueType) );

    memset( m_ThreaderDerivativeM[threadID].data_block(),
            0,
            this->m_NumberOfParameters * sizeof(typename DerivativeType::ValueType) );
  }

  // MUST BE CALLED TO INITIATE PROCESSING
  this->GetValueAndDerivativeMultiThreadedInitiate();

  // Accumulate over the threads
  DerivativeType derivativeF(this->m_NumberOfParameters), derivativeM(this->m_NumberOfParameters);
  for(unsigned int t=0; t<this->m_NumberOfThreads; t++) {
    for(unsigned int parameter = 0; parameter < this->m_NumberOfParameters; parameter++) {
      derivativeF[parameter] += m_ThreaderDerivativeF[t][parameter];
      derivativeM[parameter] += m_ThreaderDerivativeM[t][parameter];
    }
  }

  //Compute derivatives
  if( this->m_NumberOfPixelsCounted > 0 && m_Denom != 0.0) {
    for(unsigned int i=0; i<this->m_NumberOfParameters; i++) {
      derivative[i] = ( derivativeF[i] - (m_SFM/m_SMM)* derivativeM[i] ) / m_Denom;
    }
  } else {
    for(unsigned int i=0; i<this->m_NumberOfParameters; i++) {
      derivative[i] = itk::NumericTraits< MeasureType >::Zero;
    }
  }

}


/**
 * Get the match measure derivative
 */
template < class TFixedImage, class TMovingImage  >
void
NormalizedCorrelationImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetDerivative( const ParametersType & parameters,
                 DerivativeType & derivative ) const
{
  if( !this->m_FixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  MeasureType value;
  // call the combined version
  this->GetValueAndDerivative( parameters, value, derivative );
}

} // end namespace clitk


#endif
