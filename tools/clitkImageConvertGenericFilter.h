/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
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
    bool IsWarningOccur() { return mWarningOccur; }
    std::string & GetWarning() { return mWarning; }
    void EnableDisplayWarning(bool b) { mDisplayWarning = b; }

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    std::string mOutputPixelTypeName;
    std::string mWarning;
    bool mWarningOccur;
    bool mDisplayWarning;

    template<class InputImageType, class OutputPixelType> void UpdateWithOutputType();

  }; // end class ImageConvertGenericFilter

  //#include "clitkImageConvertGenericFilter.txx"

} // end namespace

#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_H */

