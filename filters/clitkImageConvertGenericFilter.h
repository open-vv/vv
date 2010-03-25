#ifndef CLITKIMAGECONVERTGENERICFILTER_H
#define CLITKIMAGECONVERTGENERICFILTER_H
/**
 ===================================================================
 * @file   clitkImageConvertGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   05 May 2008 10:40:24

 * @brief  

 ===================================================================*/

// clitk include
#include "clitkImageToImageGenericFilter.h"

// itk include
#include "itkCastImageFilter.h"

namespace clitk {
  
  class ImageConvertGenericFilter: 
    public clitk::ImageToImageGenericFilter<ImageConvertGenericFilter> {
    
  public: 
    // constructor - destructor
    ImageConvertGenericFilter();

    // Types
    typedef ImageConvertGenericFilter     Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Members functions
    void SetOutputPixelType(std::string p) { mOutputPixelTypeName = p; }

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    std::string mOutputPixelTypeName;

    template<class InputImageType, class OutputPixelType> void UpdateWithOutputType();

  }; // end class ImageConvertGenericFilter

  //#include "clitkImageConvertGenericFilter.txx"

} // end namespace

#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_H */

