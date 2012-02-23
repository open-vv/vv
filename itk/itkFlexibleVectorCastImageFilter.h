/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFlexibleVectorCastImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2008-10-17 16:30:53 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkFlexibleVectorCastImageFilter_h
#define __itkFlexibleVectorCastImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkNumericTraitsFixedArrayPixel.h"

namespace itk
{
  
/** \class FlexibleVectorCastImageFilter
 *
 * \brief Casts input vector pixels to output vector pixel type.
 *
 * This filter is templated over the input image type and 
 * output image type.
 * 
 * The filter expect both images to have the same number of dimensions,
 * and that both the input and output have itk::Vector pixel types
 * of the same VectorDimension.
 *
 * \sa Vector
 *
 * \ingroup IntensityImageFilters  Multithreaded
 */
namespace Functor {  
  
template< class TInput, class TOutput>
class FlexibleVectorCast
{
public:
  FlexibleVectorCast() {}
  ~FlexibleVectorCast() {}
  bool operator!=( const FlexibleVectorCast & ) const
    {
    return false;
    }
  bool operator==( const FlexibleVectorCast & other ) const
    {
    return !(*this != other);
    }
  inline TOutput operator()( const TInput & A ) const
    {
    typedef typename TOutput::ValueType OutputValueType;

    TOutput value;
    unsigned int k;
    for( k = 0; k < TOutput::Dimension && k < TInput::Dimension; k++ )
      {
      value[k] = static_cast<OutputValueType>( A[k] );
      }
      
    for (; k < TOutput::Dimension; k++)
      value[k] = 0;
    
    return value;
    }
}; 
}

template <class TInputImage, class TOutputImage = itk::Vector<float, 3> >
class ITK_EXPORT FlexibleVectorCastImageFilter :
    public
UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                        Functor::FlexibleVectorCast< typename TInputImage::PixelType, 
                                             typename TOutputImage::PixelType>   >
{
public:
  /** Standard class typedefs. */
  typedef FlexibleVectorCastImageFilter                               Self;
  typedef UnaryFunctorImageFilter<
    TInputImage,TOutputImage, 
    Functor::FlexibleVectorCast< typename TInputImage::PixelType, 
                         typename TOutputImage::PixelType> >  Superclass;
  typedef SmartPointer<Self>                                  Pointer;
  typedef SmartPointer<const Self>                            ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(FlexibleVectorCastImageFilter, 
               UnaryFunctorImageFilter);

protected:
  FlexibleVectorCastImageFilter() {}
  virtual ~FlexibleVectorCastImageFilter() {}

private:
  FlexibleVectorCastImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
