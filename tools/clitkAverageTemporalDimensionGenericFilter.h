#ifndef clitkAverageTemporalDimensionGenericFilter_h
#define clitkAverageTemporalDimensionGenericFilter_h

/* =================================================
 * @file   clitkAverageTemporalDimensionGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkAverageTemporalDimension_ggo.h"

//itk include
#include "itkLightObject.h"

namespace clitk 
{

  template<class args_info_type>
  class ITK_EXPORT AverageTemporalDimensionGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef AverageTemporalDimensionGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( AverageTemporalDimensionGenericFilter, LightObject );


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
      m_InputFileName=m_ArgsInfo.input_arg[0];
    }
    
    
    //----------------------------------------  
    // Update
    //----------------------------------------  
    void Update();

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    AverageTemporalDimensionGenericFilter();
    ~AverageTemporalDimensionGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension>  void UpdateWithDim(const std::string PixelType, const int Components);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();


    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_type m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkAverageTemporalDimensionGenericFilter.txx"
#endif

#endif // #define clitkAverageTemporalDimensionGenericFilter_h
