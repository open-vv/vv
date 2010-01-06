#ifndef __clitkVectorBSplineResampleImageFunction_h
#define __clitkVectorBSplineResampleImageFunction_h

#include "clitkVectorBSplineInterpolateImageFunction.h"

namespace clitk
{
/** \class VectorBSplineResampleImageFunction
 * \brief Resample image intensity from a VectorBSpline coefficient image.
 *
 * This class resample the image intensity at a non-integer position
 * from the input VectorBSpline coefficient image.
 * 
 * Spline order may be from 0 to 5.
 *
 * In ITK, BSpline coefficient can be generated using a
 * BSplineDecompositionImageFilter. Using this image function in
 * conjunction with ResampleImageFunction allows the reconstruction
 * of the original image at different resolution and size.
 *
 * \sa VectorBSplineInterpolateImageFunction
 * \sa VectorBSplineDecompositionImageFilter
 * \sa VectorResampleImageFilter
 *
 * \ingroup ImageFunctions
 */
template <class TImageType, class TCoordRep = float>
class ITK_EXPORT VectorBSplineResampleImageFunction : 
    public VectorBSplineInterpolateImageFunction<
  TImageType,TCoordRep,ITK_TYPENAME TImageType::PixelType::ValueType > 
{
public:
  /** Standard class typedefs. */
  typedef VectorBSplineResampleImageFunction                          Self;
  typedef VectorBSplineInterpolateImageFunction<TImageType,TCoordRep, ITK_TYPENAME TImageType::PixelType::ValueType >  Superclass;
  typedef itk::SmartPointer<Self>                                    Pointer;
  typedef itk::SmartPointer<const Self>                              ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(VectorBSplineResampleImageFunction, 
               VectorBSplineInterpolateImageFunction);

  /** New macro for creation of through a Smart Pointer */
  itkNewMacro( Self );

  /** Set the input image representing the BSplineCoefficients */
  virtual void SetInputImage(const TImageType * inputData)
    {
      // bypass my superclass
      this->itk::VectorInterpolateImageFunction<TImageType,TCoordRep>::SetInputImage(inputData);
      this->m_Coefficients = inputData;
      if ( this->m_Coefficients.IsNotNull() )
	{
	  this->m_DataLength = this->m_Coefficients->GetBufferedRegion().GetSize();
	}
    }

protected:
  VectorBSplineResampleImageFunction() {};
  virtual ~VectorBSplineResampleImageFunction() {};

private:
  VectorBSplineResampleImageFunction(const Self&);//purposely not implemented
};

} // namespace clitk


#endif
