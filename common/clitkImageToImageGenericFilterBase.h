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

#ifndef CLITKIMAGETOIMAGEGENERICFILTERBASE_H
#define CLITKIMAGETOIMAGEGENERICFILTERBASE_H

// clitk 
#include "clitkImageCommon.h"
#include "clitkCommonGenericFilter.h"
#include "clitkFilterBase.h"

// itk 
#include <itkImage.h>

// vv 
#include "vvImage.h"
#include "vvFromITK.h"
#include "vvToITK.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class ImageToImageGenericFilterBase: public itk::Object {
    
  public: 
    // constructor - destructor
    ImageToImageGenericFilterBase(std::string filterName);
    virtual ~ImageToImageGenericFilterBase();

    // Types
    typedef ImageToImageGenericFilterBase Self;
    typedef Object                        Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // Filters information
    const std::string & GetFilterName();
    void SetFilterName(std::string & n);
    
    // Error management
    // itkSetMacro(LastError, std::string);
    // itkGetConstMacro(LastError, std::string);
    // bool HasError() { return (GetLastError() != ""); }

    // Generic IO
    /// Returns the dimension and pixel type of the *first* input
    void GetInputImageDimensionAndPixelType(unsigned int& dim, std::string& pixeltype,
                                            unsigned int & components);
    // File IO
    void SetInputFilename(const std::string & filename);
    void AddInputFilename(const std::string & filename);
    void SetInputFilenames(const std::vector<std::string> & filenames);
    void EnableReadOnDisk(bool b);
    void SetOutputFilename(const std::string & filename);
    void AddOutputFilename(const std::string & filename);
    void SetOutputFilenames(const std::vector<std::string> & filenames);
    std::string GetOutputFilename();
    void SetIOVerbose(bool b);

    // VVImage IO
    void SetInputVVImage (vvImage::Pointer input);
    void SetInputVVImages (std::vector<vvImage::Pointer> input);
    void AddInputVVImage (vvImage::Pointer input);
    vvImage::Pointer GetOutputVVImage ();
    std::vector<vvImage::Pointer> GetOutputVVImages ();

    // Information on available image types
    void PrintAvailableImageTypes();
    virtual std::string GetAvailableImageTypes() = 0;

    virtual bool CheckImageType(unsigned int dim,unsigned int ncomp, std::string pixeltype) = 0;
    virtual bool CheckImageType() = 0;

    // Main function to call for using the filter. 
    virtual bool Update() = 0;

    // Get the associated filter
    FilterBase * GetFilterBase() { return m_FilterBase; }
    
    // Indicate that the filter must stop as soon as possible (if threaded)
    // void MustStop();
    void DeleteLastOutputImage();
    // itkSetMacro(StopOnError, bool);
    // itkGetConstMacro(StopOnError, bool);
    // itkBooleanMacro(StopOnError);    

  protected:  
    bool m_ReadOnDisk;
    /// Call this function to dispatch an output towards the correct sink
    template<class ImageType> 
    void SetNextOutput(typename ImageType::Pointer output);

    /// Call this function to get the nth itk input image, regardless of input source
    template<class ImageType> 
    typename ImageType::Pointer GetInput(unsigned int n);

    std::vector<std::string> m_InputFilenames;
    std::list<std::string> m_OutputFilenames;

    bool m_IOVerbose;
    unsigned int m_Dim;
    std::string m_PixelTypeName;
    unsigned int m_NbOfComponents;
    std::string m_FilterName;

    std::vector<vvImage::Pointer> m_InputVVImages;
    std::vector<vvImage::Pointer> m_OutputVVImages;

    void ImageTypeError();
    void SetImageTypeError();
    bool m_FailOnImageTypeError;
    
    // std::string m_LastError;
    void SetFilterBase(FilterBase * f) { m_FilterBase = f; }
    FilterBase * m_FilterBase;
    // bool m_StopOnError;

  }; // end class clitk::ImageToImageGenericFilter

#define ADD_VEC_IMAGE_TYPE(DIM, COMP, PT) this->mImageTypesManager.template AddNewDimensionAndPixelType<DIM,COMP, PT>();
#define ADD_IMAGE_TYPE(DIM, PT) this->mImageTypesManager.template AddNewDimensionAndPixelType<DIM, PT>();
#define ADD_DEFAULT_IMAGE_TYPES(DIM) ADD_IMAGE_TYPE(DIM, char);   \
                                     ADD_IMAGE_TYPE(DIM, uchar);  \
                                     ADD_IMAGE_TYPE(DIM, short);  \
                                     ADD_IMAGE_TYPE(DIM, ushort); \
                                     ADD_IMAGE_TYPE(DIM, int);    \
                                     ADD_IMAGE_TYPE(DIM, float);  \
                                     ADD_IMAGE_TYPE(DIM, double);

  //#include "clitkImageToImageGenericFilterBase.txx"

} // end namespace

#endif /* end #define CLITKIMAGETOIMAGEGENERICFILTERBASE_H */

