#ifndef clitkAffineTransformGenericFilter_h
#define clitkAffineTransformGenericFilter_h

/* =================================================
 * @file   clitkAffineTransformGenericFilter.h
 * @author jefvdmb@gmail.com
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkAffineTransform_ggo.h"
#include "clitkTransformUtilities.h"
#include "clitkGenericInterpolator.h"
#include "clitkGenericVectorInterpolator.h"

//itk include
#include "itkLightObject.h"
#include "itkAffineTransform.h"
#include "itkResampleImageFilter.h"
#include "itkVectorResampleImageFilter.h"


namespace clitk 
{


  template<class args_info_type>
  class ITK_EXPORT AffineTransformGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef AffineTransformGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( AffineTransformGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_type & a)
    {
      m_ArgsInfo=a;
      m_Verbose=m_ArgsInfo.verbose_flag;
      m_InputFileName=m_ArgsInfo.input_arg;
    }
    
    
    //----------------------------------------  
    // Update
    //----------------------------------------  
    void Update();

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    AffineTransformGenericFilter();
    ~AffineTransformGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType, int Components);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndVectorType();

    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_type m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkAffineTransformGenericFilter.txx"
#endif

#endif // #define clitkAffineTransformGenericFilter_h
