#ifndef CLITKBINARIZEIMAGEGENERICFILTER_H
#define CLITKBINARIZEIMAGEGENERICFILTER_H
#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"

//--------------------------------------------------------------------
namespace clitk 
{
  
  template<class args_info_type>
  class ITK_EXPORT BinarizeImageGenericFilter: 
    public ImageToImageGenericFilter<BinarizeImageGenericFilter<args_info_type> >
  {
    
  public:

    //--------------------------------------------------------------------
    BinarizeImageGenericFilter();

    //--------------------------------------------------------------------
    typedef BinarizeImageGenericFilter         Self;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    //--------------------------------------------------------------------
    // Method for creation through the object factory
    // and Run-time type information (and related methods)
    itkNewMacro(Self);  
    itkTypeMacro(BinarizeImageGenericFilter, LightObject);

    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type & a);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    args_info_type mArgsInfo;
    
  }; // end class
  //--------------------------------------------------------------------
    
} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkBinarizeImageGenericFilter.txx"
#endif

#endif // #define clitkBinarizeImageGenericFilter_h
