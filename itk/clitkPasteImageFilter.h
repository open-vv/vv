/*=========================================================================
 *
 *  COPY OF itkPasteImageFilter to remove VerifyInputInformation
 *
 *=========================================================================*/

#ifndef __clitkPasteImageFilter_h
#define __clitkPasteImageFilter_h

#include "itkPasteImageFilter.h"

namespace clitk
{
  using namespace itk;
  
  template< class TInputImage, class TSourceImage = TInputImage, class TOutputImage = TInputImage >
  class ITK_EXPORT PasteImageFilter:
    public itk::PasteImageFilter< TInputImage, TSourceImage, TOutputImage >
  {
  public:
    virtual void VerifyInputInformation() ITK_OVERRIDE { }

    /** Standard class typedefs. */
    typedef PasteImageFilter                                Self;
    typedef InPlaceImageFilter< TInputImage, TOutputImage > Superclass;
    typedef SmartPointer< Self >                            Pointer;
    typedef SmartPointer< const Self >                      ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(PasteImageFilter, InPlaceImageFilter);

  protected:
    PasteImageFilter();
    ~PasteImageFilter() {}
 
  private:
    PasteImageFilter(const Self &); //purposely not implemented
    void operator=(const Self &);   //purposely not implemented
  };
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkPasteImageFilter.hxx"
#endif

#endif
