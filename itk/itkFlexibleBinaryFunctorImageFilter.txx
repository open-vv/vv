/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFlexibleBinaryFunctorImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-07 17:31:02 $
  Version:   $Revision: 1.40 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFlexibleBinaryFunctorImageFilter_txx
#define __itkFlexibleBinaryFunctorImageFilter_txx

#include "itkFlexibleBinaryFunctorImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkProgressReporter.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage1, class TInputImage2, 
          class TOutputImage, class TFunction  >
FlexibleBinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage,TFunction>
::FlexibleBinaryFunctorImageFilter()
{
  this->SetNumberOfRequiredInputs( 1 );
  this->SetInPlace(false);
}


/**
 * Connect one of the operands for pixel-wise addition
 */
template <class TInputImage1, class TInputImage2, 
          class TOutputImage, class TFunction  >
void
FlexibleBinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage,TFunction>
::SetInput1( const TInputImage1 * image1 ) 
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<TInputImage1 *>( image1 ));
}


/**
 * Connect one of the operands for pixel-wise addition
 */
template <class TInputImage1, class TInputImage2, 
          class TOutputImage, class TFunction  >
void
FlexibleBinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage,TFunction>
::SetInput2( const TInputImage2 * image2 ) 
{
  // Process object is not const-correct so the const casting is required.
  //this->SetNthInput(1, const_cast<TInputImage2 *>( image2 ));
  
  m_Input2 = image2;
}

template <class TInputImage1, class TInputImage2, 
          class TOutputImage, class TFunction  >
void
FlexibleBinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage,TFunction>
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  
  // Process object is not const-correct so the const casting is required.
  // This "manual" update step is necessary because m_Input2 is not in the pipeline, since
  // it's dimensions can be different from input 1.
  TInputImage2* image2 = const_cast<TInputImage2 *>( m_Input2.GetPointer() );
  image2->Update();
}

template <class TInputImage1, class TInputImage2, 
          class TOutputImage, class TFunction  >
void
FlexibleBinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage,TFunction>
::GenerateOutputInformation() 
{
  Superclass::GenerateOutputInformation() ;
}

/**
 * ThreadedGenerateData Performs the pixel-wise addition
 */
template <class TInputImage1, class TInputImage2, class TOutputImage, class TFunction  >
void
FlexibleBinaryFunctorImageFilter<TInputImage1, TInputImage2, TOutputImage, TFunction>
::ThreadedGenerateData( const OutputImageRegionType &outputRegionForThread,
                        int threadId)
{
  const unsigned int dim = Input1ImageType::ImageDimension;
  
  // We use dynamic_cast since inputs are stored as DataObjects.  The
  // ImageToImageFilter::GetInput(int) always returns a pointer to a
  // TInputImage1 so it cannot be used for the second input.
  Input1ImagePointer inputPtr1
    = dynamic_cast<const TInputImage1*>(ProcessObject::GetInput(0));
  Input2ImagePointer inputPtr2 = m_Input2;
/*    = dynamic_cast<const TInputImage2*>(ProcessObject::GetInput(1));*/
  OutputImagePointer outputPtr = this->GetOutput(0);
  
  typename Input1ImageType::RegionType region2 = inputPtr2->GetLargestPossibleRegion();
  
  typename Input1ImageType::IndexType index1;
  typename Input2ImageType::IndexType index2;
  typename Input1ImageType::PointType point1;
  typename Input2ImageType::PointType point2;

  ImageRegionConstIterator<TInputImage1> inputIt1(inputPtr1, outputRegionForThread);
  ImageRegionConstIterator<TInputImage2> inputIt2(inputPtr2, outputRegionForThread);

  ImageRegionIterator<TOutputImage> outputIt(outputPtr, outputRegionForThread);

  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());
  
  inputIt1.GoToBegin();
  index1 = inputIt1.GetIndex();
  inputPtr1->TransformIndexToPhysicalPoint(index1, point1);
  for (unsigned int i = 0; i < dim; i++)
    point2[i] = point1[i];
  inputPtr2->TransformPhysicalPointToIndex(point2, index2);
  inputIt2.SetIndex(index2);
  outputIt.GoToBegin();

  while( !inputIt1.IsAtEnd() ) {
    
    if (region2.IsInside(index2))
      outputIt.Set( m_Functor( inputIt1.Get(), inputIt2.Get() ) );
    else
      outputIt.Set(inputIt1.Get());
    
    ++inputIt1;
    index1 = inputIt1.GetIndex();
    inputPtr1->TransformIndexToPhysicalPoint(index1, point1);
    for (unsigned int i = 0; i < dim; i++)
      point2[i] = point1[i];
    inputPtr2->TransformPhysicalPointToIndex(point2, index2);
    inputIt2.SetIndex(index2);
    ++outputIt;
    
    progress.CompletedPixel(); // potential exception thrown here
  }
}

} // end namespace itk

#endif
