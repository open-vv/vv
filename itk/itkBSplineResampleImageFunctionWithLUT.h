#ifndef __itkBSplineResampleImageFunctionWithLUT_h
#define __itkBSplineResampleImageFunctionWithLUT_h

#include "itkBSplineInterpolateImageFunctionWithLUT.h"

namespace itk
{
/** \class BSplineResampleImageFunctionWithLUT
 * \brief Resample image intensity from a BSpline coefficient image using a LUT.
 *
 * This class resample the image intensity at a non-integer position
 * from the input BSpline coefficient image using a LUT.
 * 
 * Spline order may be from 0 to 5.
 *
 * In ITK, BSpline coefficient can be generated using a
 * BSplineDecompositionImageFilter. Using this image function in
 * conjunction with ResampleImageFunction allows the reconstruction
 * of the original image at different resolution and size.
 *
 * \sa BSplineInterpolateImageFunctionWithLUT
 * \sa BSplineDecompositionImageFilter
 * \sa ResampleImageFilter
 *
 * \ingroup ImageFunctions
 */
template <class TImageType, class TCoordRep = float>
class ITK_EXPORT BSplineResampleImageFunctionWithLUT : 
    public BSplineInterpolateImageFunctionWithLUT<
  TImageType,TCoordRep,ITK_TYPENAME TImageType::PixelType > 
{
public:
  /** Standard class typedefs. */
  typedef BSplineResampleImageFunctionWithLUT                   Self;
  typedef BSplineInterpolateImageFunctionWithLUT< 
    TImageType,TCoordRep, ITK_TYPENAME TImageType::PixelType >  Superclass;
  typedef SmartPointer<Self>                                    Pointer;
  typedef SmartPointer<const Self>                              ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(BSplineReconstructionImageFunction, 
               BSplineInterpolateImageFunctionWithLUT);

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** Set the input image representing the BSplineCoefficients */
  virtual void SetInputImage(const TImageType * inputData)
  {
    // bypass my superclass
    this->InterpolateImageFunction<TImageType,TCoordRep>::SetInputImage(inputData);
    this->m_Coefficients = inputData;
    if ( this->m_Coefficients.IsNotNull() )
      {
      this->m_DataLength = this->m_Coefficients->GetBufferedRegion().GetSize();

      // JV specific for BLUT ( contains the call to UpdatePrecomputedWeights() )
      this->UpdateWeightsProperties();
      }
  }

protected:
  BSplineResampleImageFunctionWithLUT() {};
  virtual ~BSplineResampleImageFunctionWithLUT() {};
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    this->Superclass::PrintSelf( os, indent );
  }

private:
  BSplineResampleImageFunctionWithLUT(const Self&);//purposely not implemented
};

} // namespace itk


#endif

