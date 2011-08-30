/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkConditionalGrayscaleDilateImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2009-04-28 14:36:20 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __clitkConditionalGrayscaleDilateImageFilter_txx
#define __clitkConditionalGrayscaleDilateImageFilter_txx

// First make sure that the configuration is available.
// This line can be removed once the optimized versions
// gets integrated into the main directories.
#include "itkConfigure.h"

#ifdef ITK_USE_CONSOLIDATED_MORPHOLOGY
#include "itkOptGrayscaleDilateImageFilter.txx"
#else


#include "clitkConditionalGrayscaleDilateImageFilter.h"

namespace itk {

template<class TInputImage, class TOutputImage, class TKernel>
ConditionalGrayscaleDilateImageFilter<TInputImage, TOutputImage, TKernel>
::ConditionalGrayscaleDilateImageFilter()
{
  m_DilateBoundaryCondition.SetConstant( NumericTraits<PixelType>::NonpositiveMin() );
  this->OverrideBoundaryCondition( &m_DilateBoundaryCondition );
}

template<class TInputImage, class TOutputImage, class TKernel>
typename ConditionalGrayscaleDilateImageFilter<TInputImage, TOutputImage, TKernel>::PixelType
ConditionalGrayscaleDilateImageFilter<TInputImage, TOutputImage, TKernel>
::Evaluate(const NeighborhoodIteratorType &nit,
           const KernelIteratorType kernelBegin,
           const KernelIteratorType kernelEnd)
{
  unsigned int i;
  PixelType max = NumericTraits<PixelType>::NonpositiveMin();
  PixelType temp;

  KernelIteratorType kernel_it;

  PixelType center = nit.GetCenterPixel ();
  // if (center != NumericTraits<KernelPixelType>::Zero)  {
  //   DD((int)center);
  // }

  if (center > NumericTraits<KernelPixelType>::Zero) return center;
  
  for( i=0, kernel_it=kernelBegin; kernel_it<kernelEnd; ++kernel_it, ++i )
    {
    // if structuring element is positive, use the pixel under that element
    // in the image
    if( *kernel_it > NumericTraits<KernelPixelType>::Zero )
      {
      // note we use GetPixel() on the SmartNeighborhoodIterator to
      // respect boundary conditions
      temp = nit.GetPixel(i);

      if( temp > max )
        {
        max = temp;
        }
      }
    }
  
  return max;
} 


}// end namespace itk
#endif

#endif
