#ifndef __clitkZeroVFGenericFilter_h
#define __clitkZeroVFGenericFilter_h
#include "clitkImageCommon.h"
#include "clitkImageCommon.h"

//itk include
#include "itkLightObject.h"
#include "itkImage.h"
#include "itkVector.h"


namespace clitk
{
  
  class ITK_EXPORT ZeroVFGenericFilter : public itk::LightObject
  
  {
  public:
    typedef ZeroVFGenericFilter     Self;
    typedef itk::LightObject     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  

    /** Run-time type information (and related methods) */
    itkTypeMacro( ZeroVFGenericFilter, ImageToImageFilter );
  
     //Set Methods (inline)
    void SetInput(const std::string m){m_InputName=m;}
    void SetOutput(const std::string m){m_OutputName=m;}
    void SetVerbose(const bool m){m_Verbose=m;}

    //Update
    void Update( );


  protected:

    ZeroVFGenericFilter();
    ~ZeroVFGenericFilter() {};

    //Templated members
    template<unsigned int Dimension> void UpdateWithDim(std::string PixelType);
    template<unsigned int Dimension, class PixelType> void UpdateWithDimAndPixelType(); 

    std::string m_InputName;
    std::string m_OutputName;
    bool m_Verbose;
  
};


} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkZeroVFGenericFilter.txx"
#endif

#endif // #define __clitkZeroVFGenericFilter_h
