#ifndef __itkBinaryGuerreroFilter_h
#define __itkBinaryGuerreroFilter_h
#include "itkBinaryFunctorImageFilter.h"
#include "itkNumericTraits.h"

#include <cmath>

namespace itk
{
  
/** \class BinaryGuerreroFilter
 * \brief  *
 * The images to be added are set using the methods:
 * SetInput1( image1 );
 * SetInput2( image2 );
 * 
 * \warning No numeric overflow checking is performed in this filter.
 *
 * \ingroup IntensityImageFilters  Multithreaded
 */
namespace Functor {  
  
template< class TInput1, class TInput2=TInput1, class TOutput=TInput1>
class GuerreroFunctor
{
    public:
        GuerreroFunctor() : use_correct_formula(false)
            {};
        ~GuerreroFunctor() {};
        bool operator!=( const GuerreroFunctor & ) const
        {
            return false;
        }
        bool operator==( const GuerreroFunctor & other ) const
        {
            return !(*this != other);
        }
        inline TOutput operator() ( const TInput1 & A, const TInput2 & B) const
        {
            //A is the reference image
            TInput2 Bstar = B - 1000.*blood_mass_factor*(1+B/1000.);
            TOutput vol_change;
            if (use_correct_formula)
                vol_change=static_cast<TOutput>( (A-Bstar) / (1000.+Bstar) );
            else //Use the original formula as described in Guerrero's paper
                vol_change=static_cast<TOutput>( 1000. * (Bstar-A) / (A*(1000.+Bstar)) );

            if (IsNormal(vol_change))
                return vol_change;
            else
                return 0.;
        }
        double blood_mass_factor;
        bool use_correct_formula;
}; 

}

//==========================================================================
template <class TInputImage1, class TInputImage2=TInputImage1, class TOutputImage=TInputImage1>
class ITK_EXPORT BinaryGuerreroFilter :
    public
BinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage, 
                         Functor::GuerreroFunctor< 
  typename TInputImage1::PixelType, 
  typename TInputImage2::PixelType,
  typename TOutputImage::PixelType>   >


{
public:
  /** Standard class typedefs. */
  typedef BinaryGuerreroFilter               Self;
  typedef BinaryFunctorImageFilter<TInputImage1,TInputImage2,TOutputImage, 
                                   Functor::GuerreroFunctor< 
    typename TInputImage1::PixelType, 
    typename TInputImage2::PixelType,
    typename TOutputImage::PixelType> > Superclass;

  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(BinaryGuerreroFilter, 
               BinaryFunctorImageFilter);
  void SetBloodCorrectionFactor(double f)
  {
      this->GetFunctor().blood_mass_factor=f;
  }
  void SetUseCorrectFormula(bool use_correct_formula)
  {
      this->GetFunctor().use_correct_formula=use_correct_formula;
  }


protected:
  BinaryGuerreroFilter() {}
  virtual ~BinaryGuerreroFilter() {}

private:
  BinaryGuerreroFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk


#endif
