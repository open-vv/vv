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
  Module:    $RCSfile: itkOptMeanSquaresImageToImageMetricFor3DBLUTFFD.txx,v $
  Language:  C++
  Date:      $Date: 2010/06/14 17:32:07 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkOptMeanSquaresImageToImageMetricFor3DBLUTFFD_txx
#define __itkOptMeanSquaresImageToImageMetricFor3DBLUTFFD_txx

#include "itkOptMeanSquaresImageToImageMetricFor3DBLUTFFD.h"
#include "itkCovariantVector.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageIterator.h"
#include "vnl/vnl_math.h"

namespace itk
{

/**
 * Constructor
 */
template < class TFixedImage, class TMovingImage >
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::MeanSquaresImageToImageMetricFor3DBLUTFFD()
{
  this->SetComputeGradient(true);

  m_ThreaderMSE = NULL;
  m_ThreaderMSEDerivatives = NULL;
  this->m_WithinThreadPreProcess = false;
  this->m_WithinThreadPostProcess = false;

  //  For backward compatibility, the default behavior is to use all the pixels
  //  in the fixed image.
  this->UseAllPixelsOn();
}

template < class TFixedImage, class TMovingImage >
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::~MeanSquaresImageToImageMetricFor3DBLUTFFD()
{
  if(m_ThreaderMSE != NULL) {
    delete [] m_ThreaderMSE;
  }
  m_ThreaderMSE = NULL;

  if(m_ThreaderMSEDerivatives != NULL) {
    delete [] m_ThreaderMSEDerivatives;
  }
  m_ThreaderMSEDerivatives = NULL;
}

/**
 * Print out internal information about this class
 */
template < class TFixedImage, class TMovingImage  >
void
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::PrintSelf(std::ostream& os, Indent indent) const
{

  Superclass::PrintSelf(os, indent);

}


/**
 * Initialize
 */
template <class TFixedImage, class TMovingImage>
void
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::Initialize(void) throw ( ExceptionObject )
{

  this->Superclass::Initialize();
  this->Superclass::MultiThreadingInitialize();

  if(m_ThreaderMSE != NULL) {
    delete [] m_ThreaderMSE;
  }
  m_ThreaderMSE = new double[this->m_NumberOfThreads];

  if(m_ThreaderMSEDerivatives != NULL) {
    delete [] m_ThreaderMSEDerivatives;
  }
  m_ThreaderMSEDerivatives = new DerivativeType[this->m_NumberOfThreads];
  for(ThreadIdType threadID=0; threadID<this->m_NumberOfThreads; threadID++) {
    m_ThreaderMSEDerivatives[threadID].SetSize( this->m_NumberOfParameters );
  }
}

template < class TFixedImage, class TMovingImage  >
inline bool
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValueThreadProcessSample( ThreadIdType threadID,
                               unsigned long fixedImageSample,
                               const MovingImagePointType & itkNotUsed(mappedPoint),
                               double movingImageValue) const
{
  double diff = movingImageValue - this->m_FixedImageSamples[fixedImageSample].value;

  m_ThreaderMSE[threadID] += diff*diff;

  return true;
}

template < class TFixedImage, class TMovingImage  >
typename MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::MeasureType
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValue( const ParametersType & parameters ) const
{
  itkDebugMacro("GetValue( " << parameters << " ) ");

  if( !this->m_FixedImage ) {
    itkExceptionMacro( << "Fixed image has not been assigned" );
  }

  memset( m_ThreaderMSE,
          0,
          this->m_NumberOfThreads * sizeof(MeasureType) );

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

  double mse = m_ThreaderMSE[0];
  for(unsigned int t=1; t<this->m_NumberOfThreads; t++) {
    mse += m_ThreaderMSE[t];
  }
  mse /= this->m_NumberOfPixelsCounted;

  return mse;
}


template < class TFixedImage, class TMovingImage  >
inline bool
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
::GetValueAndDerivativeThreadProcessSample( ThreadIdType threadID,
    unsigned long fixedImageSample,
    const MovingImagePointType & itkNotUsed(mappedPoint),
    double movingImageValue,
    const ImageDerivativesType &
    movingImageGradientValue ) const
{
  double diff = movingImageValue - this->m_FixedImageSamples[fixedImageSample].value;

  m_ThreaderMSE[threadID] += diff*diff;

  //JV
  //FixedImagePointType fixedImagePoint = this->m_FixedImageSamples[fixedImageSample].point;

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
  transform->ComputeJacobianWithRespectToParameters(this->m_FixedImageSamples[fixedImageSample].point, jacobian);
#else
  const TransformJacobianType & jacobian = transform ->GetJacobian( this->m_FixedImageSamples[fixedImageSample].point );
#endif
  //double sum;
  unsigned int par, dim;
  for( par=0; par<this->m_NumberOfParameters; par+=3) {
    //     double sum = 0.0;
    //     for(unsigned int dim=0; dim<MovingImageDimension; dim++)
    //       {
    //       sum += 2.0 * diff * jacobian( dim, par ) * movingImageGradientValue[dim];
    //       }
    //     m_ThreaderMSEDerivatives[threadID][par] += sum;

    // JV only for 3D BLUT FFD
    if (jacobian( 0, par ) )
      for( dim=0; dim<3; dim++)
        m_ThreaderMSEDerivatives[threadID][par+dim  ]  += 2.0 * diff* jacobian( dim, par+dim   ) * movingImageGradientValue[dim];

  }
  return true;
}

/**
 * Get the both Value and Derivative Measure
 */
template < class TFixedImage, class TMovingImage  >
void
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
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

  // Reset the joint pdfs to zero
  memset( m_ThreaderMSE,
          0,
          this->m_NumberOfThreads * sizeof(MeasureType) );

  // Set output values to zero
  if(derivative.GetSize() != this->m_NumberOfParameters) {
    derivative = DerivativeType( this->m_NumberOfParameters );
  }
  memset( derivative.data_block(),
          0,
          this->m_NumberOfParameters * sizeof(double) );

  for( ThreadIdType threadID = 0; threadID<this->m_NumberOfThreads; threadID++ ) {
    memset( m_ThreaderMSEDerivatives[threadID].data_block(),
            0,
            this->m_NumberOfParameters * sizeof(double) );
  }

  // MUST BE CALLED TO INITIATE PROCESSING
  this->GetValueAndDerivativeMultiThreadedInitiate();

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

  value = 0;
  for(unsigned int t=0; t<this->m_NumberOfThreads; t++) {
    value += m_ThreaderMSE[t];
    for(unsigned int parameter = 0; parameter < this->m_NumberOfParameters;
        parameter++) {
      derivative[parameter] += m_ThreaderMSEDerivatives[t][parameter];
    }
  }

  value /= this->m_NumberOfPixelsCounted;
  for(unsigned int parameter = 0; parameter < this->m_NumberOfParameters;
      parameter++) {
    derivative[parameter] /= this->m_NumberOfPixelsCounted;
    // JV
    //DD(parameter<<"\t"<<derivative[parameter]);
  }

}


/**
 * Get the match measure derivative
 */
template < class TFixedImage, class TMovingImage  >
void
MeanSquaresImageToImageMetricFor3DBLUTFFD<TFixedImage,TMovingImage>
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

} // end namespace itk


#endif
