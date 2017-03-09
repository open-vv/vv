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
#ifndef clitkQuantitativeInterpolateImageFunction_hxx
#define clitkQuantitativeInterpolateImageFunction_hxx

#include "itkConceptChecking.h"
#include "itkImageRegionConstIterator.h"
#include "clitkQuantitativeInterpolateImageFunction.h"

#include "vnl/vnl_math.h"

namespace itk
{
/**
 * Define the number of neighbors
 */
template< typename TInputImage, typename TCoordRep >
const unsigned long
QuantitativeInterpolateImageFunction< TInputImage, TCoordRep >
::m_Neighbors = 1 << TInputImage::ImageDimension;

/**
 * Constructor
 */
template< typename TInputImage, typename TCoordRep >
QuantitativeInterpolateImageFunction< TInputImage, TCoordRep >
::QuantitativeInterpolateImageFunction()
{}

template< typename TInputImage, typename TCoordRep >
QuantitativeInterpolateImageFunction< TInputImage, TCoordRep >
::~QuantitativeInterpolateImageFunction()
{}

template< typename TInputImage, typename TCoordRep >
void
QuantitativeInterpolateImageFunction< TInputImage, TCoordRep >
::SetOutputSpacing(const InputSpacingType spacing)
{
  for ( unsigned int dim = 0; dim < ImageDimension; ++dim )
    m_NewSpacing[dim] = spacing[dim];
}

/**
 * PrintSelf
 */
template< typename TInputImage, typename TCoordRep >
void
QuantitativeInterpolateImageFunction< TInputImage, TCoordRep >
::PrintSelf(std::ostream & os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}


/**
 * Evaluate at image index position
 */
template< typename TInputImage, typename TCoordRep >
typename QuantitativeInterpolateImageFunction< TInputImage, TCoordRep >
::OutputType
QuantitativeInterpolateImageFunction< TInputImage, TCoordRep >
::EvaluateAtContinuousIndex(const ContinuousIndexType &index) const
{
  // Avoid the smartpointer de-reference in the loop for
  // "return m_InputImage.GetPointer()"
  const TInputImage * const inputImagePtr = this->GetInputImage();
  /**
   * Compute base index = closet index below point
   * Compute distance from point to base index
   */

  //Determine the region, ie. the index and the size
  IndexType regionIndex;
  typename TInputImage::SizeType regionSize;
  typename TInputImage::RegionType region;
  double value(0.0);
  for ( unsigned int dim = 0; dim < ImageDimension; ++dim )
  {
    regionIndex[dim] = Math::Round< IndexValueType >(Math::Round< IndexValueType >((index[dim] - 0.5*m_NewSpacing[dim]/inputImagePtr->GetSpacing()[dim])*10.0)/10.0); //Be sure that 0.49999... is rounded to 1.0 like 0.5
    regionSize[dim] = Math::Round< IndexValueType >(m_NewSpacing[dim] / inputImagePtr->GetSpacing()[dim]) + 1;
  }
  region.SetSize(regionSize);
  region.SetIndex(regionIndex);
  region.Crop(inputImagePtr->GetLargestPossibleRegion());

  //Iterate on it
  itk::ImageRegionConstIterator<TInputImage> imageIterator(inputImagePtr,region);

  imageIterator.GoToBegin();
  while(!imageIterator.IsAtEnd())
  {
    //Compute overlap according to distance from index
    double overlap = this->ComputeOverlap(imageIterator.GetIndex(), index);
    // Get the value of the current pixel by the overlap
    value += imageIterator.Get()*overlap;
    ++imageIterator;
  }

  return ( static_cast< OutputType >( value ) );
}

template< typename TInputImage, typename TCoordRep >
double
QuantitativeInterpolateImageFunction< TInputImage, TCoordRep >
::ComputeOverlap(const IndexType currentIndex, const ContinuousIndexType &index) const
{
  double overlap(1.0);

  for ( unsigned int dim = 0; dim < ImageDimension; ++dim )
  {
    //Check the lower bound
    double lowerBound = index[dim] - 0.5*m_NewSpacing[dim]/this->GetInputImage()->GetSpacing()[dim];
    bool lowerBoundInCurrentPixel(false), computeLowerBoundOverlap(false);
    if (lowerBound >= currentIndex[dim] + 0.5)
      overlap *= 0.0;
    else if (lowerBound <= currentIndex[dim] - 0.5)
      overlap *= 1.0;
    else
    {
      computeLowerBoundOverlap = true;
      lowerBoundInCurrentPixel = true; //Just check if upperBound is in the same pixel before to apply overlap
    }

    //Check the upper bound
    double upperBound = index[dim] + 0.5*m_NewSpacing[dim]/this->GetInputImage()->GetSpacing()[dim];
    if (upperBound <= currentIndex[dim] - 0.5)
    {
      overlap *= 0.0;
    }
    else if (upperBound >= currentIndex[dim] + 0.5)
    {
      overlap *= 1.0;
    }
    else
    {
      if (lowerBoundInCurrentPixel)
      {
        computeLowerBoundOverlap = false;
        overlap *= upperBound - lowerBound; //The lower bound is in the same pixel
      }
      else
        overlap *= 0.5 + upperBound - currentIndex[dim]; //The lower bound is not in the same pixel
    }
    if (computeLowerBoundOverlap) //The lower and upper bounds are not in the same pixel: apply the previous overlap
      overlap *= 0.5 - lowerBound + currentIndex[dim];
  }

  return(overlap);
}
} // end namespace itk

#endif
