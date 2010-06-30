#ifndef clitkFillMaskGenericFilter_h
#define clitkFillMaskGenericFilter_h

/* =================================================
 * @file   clitkFillMaskGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkFillMask_ggo.h"
#include "clitkImageCommon.h"
#include "clitkExtractImageFilter.h"

//itk include
#include "itkLightObject.h"
#include "itkJoinSeriesImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkPermuteAxesImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkCastImageFilter.h"

namespace clitk 
{


  class ITK_EXPORT FillMaskGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef FillMaskGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( FillMaskGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_clitkFillMask & a)
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
    FillMaskGenericFilter();
    ~FillMaskGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <class PixelType>  void UpdateWithPixelType();


    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_clitkFillMask m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkFillMaskGenericFilter.txx"
#endif

#endif // #define clitkFillMaskGenericFilter_h
