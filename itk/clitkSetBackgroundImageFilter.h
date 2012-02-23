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
#ifndef __clitkSetBackgroundImageFilter_h
#define __clitkSetBackgroundImageFilter_h
#include "itkFlexibleBinaryFunctorImageFilter.h"
#include "itkNumericTraits.h"


namespace clitk
{
  
namespace Functor {  
  
template< class TInput, class TMask, class TOutput=TInput >
class SetBackground
{
public:
  typedef typename itk::NumericTraits< TInput >::AccumulateType AccumulatorType;

  //SetBackground(): m_OutsideValue(itk::NumericTraits< TOutput >::Zero),m_Fg(false),m_MaskValue( static_cast<TMask>(0) ) {};
  SetBackground()
  { 
    m_OutsideValue=itk::NumericTraits< TOutput >::Zero;
    m_Fg=false;
    m_MaskValue= static_cast<TMask>(0);
  }
  ~SetBackground() {};
  bool operator!=( const SetBackground & ) const
    {
    return false;
    }
  bool operator==( const SetBackground & other ) const
    {
    return !(*this != other);
    }

  inline TOutput operator()( const TInput & A, const TMask & B)
    {
      // Background mode: set everything = background value
      if(!m_Fg)
	{
	  if (B == m_MaskValue ) 
	    {
	      return static_cast<TOutput>( m_OutsideValue );
	    }
	  else
	    {
	      return A;
	    }
	}

      // Foreground mode: set everything != foreground value
      else
	{
	  if (B != m_MaskValue ) 
	    {
	      return static_cast<TOutput>( m_OutsideValue );
	    }
	  else
	    {
	      return A;
	    }
	}
    }
  
  /** Method to explicitly set the outside value of the mask */
  void SetOutsideValue( const TOutput &outsideValue )
  {
    m_OutsideValue = static_cast<TOutput>(outsideValue);
  }
  
  /** Method to get the outside value of the mask */
  const TOutput &GetOutsideValue() const
  {
    return m_OutsideValue;
  }
  
  /** Method to explicitly set the relevant value of the mask */
  void SetMaskValue( const TMask &maskValue )
  {
    m_MaskValue = static_cast<TMask>(maskValue);
  }
  
  /** Method to get the relevant value of the mask */
  const TMask &GetMaskValue() const
  {
    return m_MaskValue;
  }

  /** Method to explicitly set the foreground mode of the mask */
  void SetForeground( const bool &fg )
  {
    m_Fg = fg;
  }
  
  /** Method to get the foregroundmode of the mask */
  const bool &GetForeground() const
  {
    return m_Fg;
  }

private:
  TOutput m_OutsideValue;
  TMask m_MaskValue;
  bool m_Fg;
}; 

} //end namespace

template <class TInputImage, class TMaskImage, class TOutputImage=TInputImage>
class ITK_EXPORT SetBackgroundImageFilter :
    public
    itk::FlexibleBinaryFunctorImageFilter<TInputImage,TMaskImage,TOutputImage, 
                         Functor::SetBackground< 
  typename TInputImage::PixelType, 
  typename TMaskImage::PixelType,
  typename TOutputImage::PixelType>   >


{
public:
  /** Standard class typedefs. */
  typedef SetBackgroundImageFilter           Self;
  typedef itk::FlexibleBinaryFunctorImageFilter<TInputImage,TMaskImage,TOutputImage, 
                                   Functor::SetBackground< 
    typename TInputImage::PixelType, 
    typename TMaskImage::PixelType,
    typename TOutputImage::PixelType>   
  >                                 Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(SetBackgroundImageFilter, 
               FlexibleBinaryFunctorImageFilter);

  /** Method to explicitly set the outside value of the mask. Defaults to 0 */
  void SetOutsideValue( const typename TOutputImage::PixelType & outsideValue ) 
    {
    if( this->GetOutsideValue() != outsideValue )
      {
      this->Modified();
      this->GetFunctor().SetOutsideValue( outsideValue );
      }
    }

  const typename TOutputImage::PixelType & GetOutsideValue() const
    {
    return this->GetFunctor().GetOutsideValue();
    }

  /** Method to explicitly set the value of the mask. Defaults to 0 */
  void SetMaskValue( const typename TMaskImage::PixelType & maskValue ) 
    {
    if( this->GetMaskValue() != maskValue )
      {
      this->Modified();
      this->GetFunctor().SetMaskValue( maskValue );
      }
    }

  const typename TMaskImage::PixelType & GetMaskValue() const
    {
    return this->GetFunctor().GetMaskValue();
    }

  /** Method to set the foreground mode. Defaults to 0 */
  void SetForeground( const bool & fg ) 
    {
    if( this->GetForeground() != fg )
      {
      this->Modified();
      this->GetFunctor().SetForeground( fg );
      }
    }

  const bool & GetForeground() const
    {
    return this->GetFunctor().GetForeground();
    }

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(MaskEqualityComparableCheck,
		  (itk::Concept::EqualityComparable<typename TMaskImage::PixelType>));
  itkConceptMacro(InputConvertibleToOutputCheck,
    (itk::Concept::Convertible<typename TInputImage::PixelType,
                          typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  SetBackgroundImageFilter() {}
  virtual ~SetBackgroundImageFilter() {}

  void PrintSelf(std::ostream &os, itk::Indent indent) const
    {
    Superclass::PrintSelf(os, indent);
    os << indent << "OutsideValue: "  << this->GetOutsideValue() << std::endl;
    os << indent << "MaskValue: "  << this->GetMaskValue() << std::endl;
    os << indent << "Foreground mode: "  << this->GetForeground() << std::endl;
    }

private:
  SetBackgroundImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace clitk


#endif
