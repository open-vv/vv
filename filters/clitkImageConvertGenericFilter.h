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
  
  class ImageConvertGenericFilter: public clitk::ImageToImageGenericFilter {
    
  public: 
    // constructor - destructor
    ImageConvertGenericFilter();

    // Types
    typedef ImageConvertGenericFilter     Self;
    typedef ImageToImageGenericFilter     Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Members functions
    void SetOutputPixelType(std::string p) { mOutputPixelTypeName = p; }
    void Update();

  protected:
    std::string mOutputPixelTypeName;

    template<unsigned int Dim> void Update_WithDim();
    template<unsigned int Dim, class PixelType> void Update_WithDimAndPixelType();
    template<unsigned int Dim, class PixelType, class OutputPixelType> void Update_WithDimAndPixelTypeAndOutputType();

  }; // end class ImageConvertGenericFilter

#include "clitkImageConvertGenericFilter.txx"

} // end namespace

#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_H */

