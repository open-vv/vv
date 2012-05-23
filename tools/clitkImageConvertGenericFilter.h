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
    template<class InputImageType, class OutputPixelType> void UpdateWithOutputVectorType();

  }; // end class ImageConvertGenericFilter

#define VEC_UPDATE_DECL(TYPE_IN, COMP, DIM, TYPE_OUT) \
  template<> void ImageConvertGenericFilter::UpdateWithOutputType<itk::Image<itk::Vector<TYPE_IN, COMP>, DIM>, TYPE_OUT>()
  
VEC_UPDATE_DECL(char, 2, 2, unsigned char);
VEC_UPDATE_DECL(char, 2, 3, unsigned char);
VEC_UPDATE_DECL(char, 2, 4, unsigned char);
VEC_UPDATE_DECL(char, 2, 2, char);
VEC_UPDATE_DECL(char, 2, 3, char);
VEC_UPDATE_DECL(char, 2, 4, char);
VEC_UPDATE_DECL(char, 2, 2, unsigned short);
VEC_UPDATE_DECL(char, 2, 3, unsigned short);
VEC_UPDATE_DECL(char, 2, 4, unsigned short);
VEC_UPDATE_DECL(char, 2, 2, short);
VEC_UPDATE_DECL(char, 2, 3, short);
VEC_UPDATE_DECL(char, 2, 4, short);
VEC_UPDATE_DECL(char, 2, 2, int);
VEC_UPDATE_DECL(char, 2, 3, int);
VEC_UPDATE_DECL(char, 2, 4, int);
VEC_UPDATE_DECL(char, 2, 2, float);
VEC_UPDATE_DECL(char, 2, 3, float);
VEC_UPDATE_DECL(char, 2, 4, float);
VEC_UPDATE_DECL(char, 2, 2, double);
VEC_UPDATE_DECL(char, 2, 3, double);
VEC_UPDATE_DECL(char, 2, 4, double);

VEC_UPDATE_DECL(char, 3, 2, unsigned char);
VEC_UPDATE_DECL(char, 3, 3, unsigned char);
VEC_UPDATE_DECL(char, 3, 4, unsigned char);
VEC_UPDATE_DECL(char, 3, 2, char);
VEC_UPDATE_DECL(char, 3, 3, char);
VEC_UPDATE_DECL(char, 3, 4, char);
VEC_UPDATE_DECL(char, 3, 2, unsigned short);
VEC_UPDATE_DECL(char, 3, 3, unsigned short);
VEC_UPDATE_DECL(char, 3, 4, unsigned short);
VEC_UPDATE_DECL(char, 3, 2, short);
VEC_UPDATE_DECL(char, 3, 3, short);
VEC_UPDATE_DECL(char, 3, 4, short);
VEC_UPDATE_DECL(char, 3, 2, int);
VEC_UPDATE_DECL(char, 3, 3, int);
VEC_UPDATE_DECL(char, 3, 4, int);
VEC_UPDATE_DECL(char, 3, 2, float);
VEC_UPDATE_DECL(char, 3, 3, float);
VEC_UPDATE_DECL(char, 3, 4, float);
VEC_UPDATE_DECL(char, 3, 2, double);
VEC_UPDATE_DECL(char, 3, 3, double);
VEC_UPDATE_DECL(char, 3, 4, double);

VEC_UPDATE_DECL(unsigned char, 2, 2, unsigned char);
VEC_UPDATE_DECL(unsigned char, 2, 3, unsigned char);
VEC_UPDATE_DECL(unsigned char, 2, 4, unsigned char);
VEC_UPDATE_DECL(unsigned char, 2, 2, char);
VEC_UPDATE_DECL(unsigned char, 2, 3, char);
VEC_UPDATE_DECL(unsigned char, 2, 4, char);
VEC_UPDATE_DECL(unsigned char, 2, 2, unsigned short);
VEC_UPDATE_DECL(unsigned char, 2, 3, unsigned short);
VEC_UPDATE_DECL(unsigned char, 2, 4, unsigned short);
VEC_UPDATE_DECL(unsigned char, 2, 2, short);
VEC_UPDATE_DECL(unsigned char, 2, 3, short);
VEC_UPDATE_DECL(unsigned char, 2, 4, short);
VEC_UPDATE_DECL(unsigned char, 2, 2, int);
VEC_UPDATE_DECL(unsigned char, 2, 3, int);
VEC_UPDATE_DECL(unsigned char, 2, 4, int);
VEC_UPDATE_DECL(unsigned char, 2, 2, float);
VEC_UPDATE_DECL(unsigned char, 2, 3, float);
VEC_UPDATE_DECL(unsigned char, 2, 4, float);
VEC_UPDATE_DECL(unsigned char, 2, 2, double);
VEC_UPDATE_DECL(unsigned char, 2, 3, double);
VEC_UPDATE_DECL(unsigned char, 2, 4, double);

VEC_UPDATE_DECL(unsigned char, 3, 2, unsigned char);
VEC_UPDATE_DECL(unsigned char, 3, 3, unsigned char);
VEC_UPDATE_DECL(unsigned char, 3, 4, unsigned char);
VEC_UPDATE_DECL(unsigned char, 3, 2, char);
VEC_UPDATE_DECL(unsigned char, 3, 3, char);
VEC_UPDATE_DECL(unsigned char, 3, 4, char);
VEC_UPDATE_DECL(unsigned char, 3, 2, unsigned short);
VEC_UPDATE_DECL(unsigned char, 3, 3, unsigned short);
VEC_UPDATE_DECL(unsigned char, 3, 4, unsigned short);
VEC_UPDATE_DECL(unsigned char, 3, 2, short);
VEC_UPDATE_DECL(unsigned char, 3, 3, short);
VEC_UPDATE_DECL(unsigned char, 3, 4, short);
VEC_UPDATE_DECL(unsigned char, 3, 2, int);
VEC_UPDATE_DECL(unsigned char, 3, 3, int);
VEC_UPDATE_DECL(unsigned char, 3, 4, int);
VEC_UPDATE_DECL(unsigned char, 3, 2, float);
VEC_UPDATE_DECL(unsigned char, 3, 3, float);
VEC_UPDATE_DECL(unsigned char, 3, 4, float);
VEC_UPDATE_DECL(unsigned char, 3, 2, double);
VEC_UPDATE_DECL(unsigned char, 3, 3, double);
VEC_UPDATE_DECL(unsigned char, 3, 4, double);

VEC_UPDATE_DECL(short, 2, 2, unsigned char);
VEC_UPDATE_DECL(short, 2, 3, unsigned char);
VEC_UPDATE_DECL(short, 2, 4, unsigned char);
VEC_UPDATE_DECL(short, 2, 2, char);
VEC_UPDATE_DECL(short, 2, 3, char);
VEC_UPDATE_DECL(short, 2, 4, char);
VEC_UPDATE_DECL(short, 2, 2, unsigned short);
VEC_UPDATE_DECL(short, 2, 3, unsigned short);
VEC_UPDATE_DECL(short, 2, 4, unsigned short);
VEC_UPDATE_DECL(short, 2, 2, short);
VEC_UPDATE_DECL(short, 2, 3, short);
VEC_UPDATE_DECL(short, 2, 4, short);
VEC_UPDATE_DECL(short, 2, 2, int);
VEC_UPDATE_DECL(short, 2, 3, int);
VEC_UPDATE_DECL(short, 2, 4, int);
VEC_UPDATE_DECL(short, 2, 2, float);
VEC_UPDATE_DECL(short, 2, 3, float);
VEC_UPDATE_DECL(short, 2, 4, float);
VEC_UPDATE_DECL(short, 2, 2, double);
VEC_UPDATE_DECL(short, 2, 3, double);
VEC_UPDATE_DECL(short, 2, 4, double);

VEC_UPDATE_DECL(short, 3, 2, unsigned char);
VEC_UPDATE_DECL(short, 3, 3, unsigned char);
VEC_UPDATE_DECL(short, 3, 4, unsigned char);
VEC_UPDATE_DECL(short, 3, 2, char);
VEC_UPDATE_DECL(short, 3, 3, char);
VEC_UPDATE_DECL(short, 3, 4, char);
VEC_UPDATE_DECL(short, 3, 2, unsigned short);
VEC_UPDATE_DECL(short, 3, 3, unsigned short);
VEC_UPDATE_DECL(short, 3, 4, unsigned short);
VEC_UPDATE_DECL(short, 3, 2, short);
VEC_UPDATE_DECL(short, 3, 3, short);
VEC_UPDATE_DECL(short, 3, 4, short);
VEC_UPDATE_DECL(short, 3, 2, int);
VEC_UPDATE_DECL(short, 3, 3, int);
VEC_UPDATE_DECL(short, 3, 4, int);
VEC_UPDATE_DECL(short, 3, 2, float);
VEC_UPDATE_DECL(short, 3, 3, float);
VEC_UPDATE_DECL(short, 3, 4, float);
VEC_UPDATE_DECL(short, 3, 2, double);
VEC_UPDATE_DECL(short, 3, 3, double);
VEC_UPDATE_DECL(short, 3, 4, double);

VEC_UPDATE_DECL(unsigned short, 2, 2, unsigned char);
VEC_UPDATE_DECL(unsigned short, 2, 3, unsigned char);
VEC_UPDATE_DECL(unsigned short, 2, 4, unsigned char);
VEC_UPDATE_DECL(unsigned short, 2, 2, char);
VEC_UPDATE_DECL(unsigned short, 2, 3, char);
VEC_UPDATE_DECL(unsigned short, 2, 4, char);
VEC_UPDATE_DECL(unsigned short, 2, 2, unsigned short);
VEC_UPDATE_DECL(unsigned short, 2, 3, unsigned short);
VEC_UPDATE_DECL(unsigned short, 2, 4, unsigned short);
VEC_UPDATE_DECL(unsigned short, 2, 2, short);
VEC_UPDATE_DECL(unsigned short, 2, 3, short);
VEC_UPDATE_DECL(unsigned short, 2, 4, short);
VEC_UPDATE_DECL(unsigned short, 2, 2, int);
VEC_UPDATE_DECL(unsigned short, 2, 3, int);
VEC_UPDATE_DECL(unsigned short, 2, 4, int);
VEC_UPDATE_DECL(unsigned short, 2, 2, float);
VEC_UPDATE_DECL(unsigned short, 2, 3, float);
VEC_UPDATE_DECL(unsigned short, 2, 4, float);
VEC_UPDATE_DECL(unsigned short, 2, 2, double);
VEC_UPDATE_DECL(unsigned short, 2, 3, double);
VEC_UPDATE_DECL(unsigned short, 2, 4, double);

VEC_UPDATE_DECL(unsigned short, 3, 2, unsigned char);
VEC_UPDATE_DECL(unsigned short, 3, 3, unsigned char);
VEC_UPDATE_DECL(unsigned short, 3, 4, unsigned char);
VEC_UPDATE_DECL(unsigned short, 3, 2, char);
VEC_UPDATE_DECL(unsigned short, 3, 3, char);
VEC_UPDATE_DECL(unsigned short, 3, 4, char);
VEC_UPDATE_DECL(unsigned short, 3, 2, unsigned short);
VEC_UPDATE_DECL(unsigned short, 3, 3, unsigned short);
VEC_UPDATE_DECL(unsigned short, 3, 4, unsigned short);
VEC_UPDATE_DECL(unsigned short, 3, 2, short);
VEC_UPDATE_DECL(unsigned short, 3, 3, short);
VEC_UPDATE_DECL(unsigned short, 3, 4, short);
VEC_UPDATE_DECL(unsigned short, 3, 2, int);
VEC_UPDATE_DECL(unsigned short, 3, 3, int);
VEC_UPDATE_DECL(unsigned short, 3, 4, int);
VEC_UPDATE_DECL(unsigned short, 3, 2, float);
VEC_UPDATE_DECL(unsigned short, 3, 3, float);
VEC_UPDATE_DECL(unsigned short, 3, 4, float);
VEC_UPDATE_DECL(unsigned short, 3, 2, double);
VEC_UPDATE_DECL(unsigned short, 3, 3, double);
VEC_UPDATE_DECL(unsigned short, 3, 4, double);

VEC_UPDATE_DECL(int, 2, 2, unsigned char);
VEC_UPDATE_DECL(int, 2, 3, unsigned char);
VEC_UPDATE_DECL(int, 2, 4, unsigned char);
VEC_UPDATE_DECL(int, 2, 2, char);
VEC_UPDATE_DECL(int, 2, 3, char);
VEC_UPDATE_DECL(int, 2, 4, char);
VEC_UPDATE_DECL(int, 2, 2, unsigned short);
VEC_UPDATE_DECL(int, 2, 3, unsigned short);
VEC_UPDATE_DECL(int, 2, 4, unsigned short);
VEC_UPDATE_DECL(int, 2, 2, short);
VEC_UPDATE_DECL(int, 2, 3, short);
VEC_UPDATE_DECL(int, 2, 4, short);
VEC_UPDATE_DECL(int, 2, 2, int);
VEC_UPDATE_DECL(int, 2, 3, int);
VEC_UPDATE_DECL(int, 2, 4, int);
VEC_UPDATE_DECL(int, 2, 2, float);
VEC_UPDATE_DECL(int, 2, 3, float);
VEC_UPDATE_DECL(int, 2, 4, float);
VEC_UPDATE_DECL(int, 2, 2, double);
VEC_UPDATE_DECL(int, 2, 3, double);
VEC_UPDATE_DECL(int, 2, 4, double);

VEC_UPDATE_DECL(int, 3, 2, unsigned char);
VEC_UPDATE_DECL(int, 3, 3, unsigned char);
VEC_UPDATE_DECL(int, 3, 4, unsigned char);
VEC_UPDATE_DECL(int, 3, 2, char);
VEC_UPDATE_DECL(int, 3, 3, char);
VEC_UPDATE_DECL(int, 3, 4, char);
VEC_UPDATE_DECL(int, 3, 2, unsigned short);
VEC_UPDATE_DECL(int, 3, 3, unsigned short);
VEC_UPDATE_DECL(int, 3, 4, unsigned short);
VEC_UPDATE_DECL(int, 3, 2, short);
VEC_UPDATE_DECL(int, 3, 3, short);
VEC_UPDATE_DECL(int, 3, 4, short);
VEC_UPDATE_DECL(int, 3, 2, int);
VEC_UPDATE_DECL(int, 3, 3, int);
VEC_UPDATE_DECL(int, 3, 4, int);
VEC_UPDATE_DECL(int, 3, 2, float);
VEC_UPDATE_DECL(int, 3, 3, float);
VEC_UPDATE_DECL(int, 3, 4, float);
VEC_UPDATE_DECL(int, 3, 2, double);
VEC_UPDATE_DECL(int, 3, 3, double);
VEC_UPDATE_DECL(int, 3, 4, double);

VEC_UPDATE_DECL(float, 2, 2, unsigned char);
VEC_UPDATE_DECL(float, 2, 3, unsigned char);
VEC_UPDATE_DECL(float, 2, 4, unsigned char);
VEC_UPDATE_DECL(float, 2, 2, char);
VEC_UPDATE_DECL(float, 2, 3, char);
VEC_UPDATE_DECL(float, 2, 4, char);
VEC_UPDATE_DECL(float, 2, 2, unsigned short);
VEC_UPDATE_DECL(float, 2, 3, unsigned short);
VEC_UPDATE_DECL(float, 2, 4, unsigned short);
VEC_UPDATE_DECL(float, 2, 2, short);
VEC_UPDATE_DECL(float, 2, 3, short);
VEC_UPDATE_DECL(float, 2, 4, short);
VEC_UPDATE_DECL(float, 2, 2, int);
VEC_UPDATE_DECL(float, 2, 3, int);
VEC_UPDATE_DECL(float, 2, 4, int);
VEC_UPDATE_DECL(float, 2, 2, float);
VEC_UPDATE_DECL(float, 2, 3, float);
VEC_UPDATE_DECL(float, 2, 4, float);
VEC_UPDATE_DECL(float, 2, 2, double);
VEC_UPDATE_DECL(float, 2, 3, double);
VEC_UPDATE_DECL(float, 2, 4, double);

VEC_UPDATE_DECL(float, 3, 2, unsigned char);
VEC_UPDATE_DECL(float, 3, 3, unsigned char);
VEC_UPDATE_DECL(float, 3, 4, unsigned char);
VEC_UPDATE_DECL(float, 3, 2, char);
VEC_UPDATE_DECL(float, 3, 3, char);
VEC_UPDATE_DECL(float, 3, 4, char);
VEC_UPDATE_DECL(float, 3, 2, unsigned short);
VEC_UPDATE_DECL(float, 3, 3, unsigned short);
VEC_UPDATE_DECL(float, 3, 4, unsigned short);
VEC_UPDATE_DECL(float, 3, 2, short);
VEC_UPDATE_DECL(float, 3, 3, short);
VEC_UPDATE_DECL(float, 3, 4, short);
VEC_UPDATE_DECL(float, 3, 2, int);
VEC_UPDATE_DECL(float, 3, 3, int);
VEC_UPDATE_DECL(float, 3, 4, int);
VEC_UPDATE_DECL(float, 3, 2, float);
VEC_UPDATE_DECL(float, 3, 3, float);
VEC_UPDATE_DECL(float, 3, 4, float);
VEC_UPDATE_DECL(float, 3, 2, double);
VEC_UPDATE_DECL(float, 3, 3, double);
VEC_UPDATE_DECL(float, 3, 4, double);
  
VEC_UPDATE_DECL(double, 2, 2, unsigned char);
VEC_UPDATE_DECL(double, 2, 3, unsigned char);
VEC_UPDATE_DECL(double, 2, 4, unsigned char);
VEC_UPDATE_DECL(double, 2, 2, char);
VEC_UPDATE_DECL(double, 2, 3, char);
VEC_UPDATE_DECL(double, 2, 4, char);
VEC_UPDATE_DECL(double, 2, 2, unsigned short);
VEC_UPDATE_DECL(double, 2, 3, unsigned short);
VEC_UPDATE_DECL(double, 2, 4, unsigned short);
VEC_UPDATE_DECL(double, 2, 2, short);
VEC_UPDATE_DECL(double, 2, 3, short);
VEC_UPDATE_DECL(double, 2, 4, short);
VEC_UPDATE_DECL(double, 2, 2, int);
VEC_UPDATE_DECL(double, 2, 3, int);
VEC_UPDATE_DECL(double, 2, 4, int);
VEC_UPDATE_DECL(double, 2, 2, float);
VEC_UPDATE_DECL(double, 2, 3, float);
VEC_UPDATE_DECL(double, 2, 4, float);
VEC_UPDATE_DECL(double, 2, 2, double);
VEC_UPDATE_DECL(double, 2, 3, double);
VEC_UPDATE_DECL(double, 2, 4, double);

VEC_UPDATE_DECL(double, 3, 2, unsigned char);
VEC_UPDATE_DECL(double, 3, 3, unsigned char);
VEC_UPDATE_DECL(double, 3, 4, unsigned char);
VEC_UPDATE_DECL(double, 3, 2, char);
VEC_UPDATE_DECL(double, 3, 3, char);
VEC_UPDATE_DECL(double, 3, 4, char);
VEC_UPDATE_DECL(double, 3, 2, unsigned short);
VEC_UPDATE_DECL(double, 3, 3, unsigned short);
VEC_UPDATE_DECL(double, 3, 4, unsigned short);
VEC_UPDATE_DECL(double, 3, 2, short);
VEC_UPDATE_DECL(double, 3, 3, short);
VEC_UPDATE_DECL(double, 3, 4, short);
VEC_UPDATE_DECL(double, 3, 2, int);
VEC_UPDATE_DECL(double, 3, 3, int);
VEC_UPDATE_DECL(double, 3, 4, int);
VEC_UPDATE_DECL(double, 3, 2, float);
VEC_UPDATE_DECL(double, 3, 3, float);
VEC_UPDATE_DECL(double, 3, 4, float);
VEC_UPDATE_DECL(double, 3, 2, double);
VEC_UPDATE_DECL(double, 3, 3, double);
VEC_UPDATE_DECL(double, 3, 4, double);  

//#include "clitkImageConvertGenericFilter.txx"

} // end namespace

#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_H */

