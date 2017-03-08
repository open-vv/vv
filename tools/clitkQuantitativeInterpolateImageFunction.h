/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef clitkQuantitativeInterpolateImageFunction_h
#define clitkQuantitativeInterpolateImageFunction_h

#include "itkInterpolateImageFunction.h"
#include "itkVariableLengthVector.h"

namespace itk
{
/** \class QuantitativeInterpolateImageFunction
 * \brief Quantitative interpolate an image at specified positions.
 *
 */
template< typename TInputImage, typename TCoordRep = double >
class QuantitativeInterpolateImageFunction:
  public InterpolateImageFunction< TInputImage, TCoordRep >
{
public:
  /** Standard class typedefs. */
  typedef QuantitativeInterpolateImageFunction               Self;
  typedef InterpolateImageFunction< TInputImage, TCoordRep > Superclass;
  typedef SmartPointer< Self >                               Pointer;
  typedef SmartPointer< const Self >                         ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(QuantitativeInterpolateImageFunction, InterpolateImageFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** OutputType typedef support. */
  typedef typename Superclass::OutputType OutputType;

  /** InputImageType typedef support. */
  typedef typename Superclass::InputImageType InputImageType;
  
  /** InputSpacingType typedef support. */
  typedef typename InputImageType::SpacingType InputSpacingType;

  /** InputPixelType typedef support. */
  typedef typename Superclass::InputPixelType InputPixelType;

  /** RealType typedef support. */
  typedef typename Superclass::RealType RealType;

  /** Dimension underlying input image. */
  itkStaticConstMacro(ImageDimension, unsigned int, Superclass::ImageDimension);

  /** Index typedef support. */
  typedef typename Superclass::IndexType      IndexType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;
  typedef typename ContinuousIndexType::ValueType  InternalComputationType;

  void SetOutputSpacing(const InputSpacingType spacing);

  /** Evaluate the function at a ContinuousIndex position
   *
   * Returns the image intensity at a
   * specified point position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual OutputType EvaluateAtContinuousIndex(const
                                               ContinuousIndexType &
                                               index) const ITK_OVERRIDE;

protected:
  QuantitativeInterpolateImageFunction();
  ~QuantitativeInterpolateImageFunction();
  void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;
  double ComputeOverlap(const IndexType currentIndex, const ContinuousIndexType &index) const;

private:
  QuantitativeInterpolateImageFunction(const Self &); //purposely not implemented
  void operator=(const Self &);                       //purposely not implemented

  /** Number of neighbors used in the interpolation */
  static const unsigned long m_Neighbors;
  typename InputImageType::SpacingType m_NewSpacing; 


};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkQuantitativeInterpolateImageFunction.hxx"
#endif

#endif
