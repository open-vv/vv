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
#ifndef clitkImage2DicomGenericFilter_h
#define clitkImage2DicomGenericFilter_h

/* =================================================
 * @file   clitkImage2DicomGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkImage2Dicom_ggo.h"

//itk include
#include "itkLightObject.h"
#include "itkGDCMImageIO.h"
#include "itkMetaDataDictionary.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include <vector>
#include <itksys/SystemTools.hxx>

#include "itkImage.h"
#include "itkMinimumMaximumImageFilter.h"
 
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
 
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
 
#include "itkResampleImageFilter.h"
 
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
#include "itkShiftScaleImageFilter.h"
#endif
 
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
 
#include <itksys/SystemTools.hxx>
 
#if ITK_VERSION_MAJOR >= 4
#include "gdcmUIDGenerator.h"
#else
#include "gdcm/src/gdcmFile.h"
#include "gdcm/src/gdcmUtil.h"
#endif
 
#include <string>
#include <sstream>

namespace clitk 
{
  template<class args_info_type>
  class ITK_EXPORT Image2DicomGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef Image2DicomGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( Image2DicomGenericFilter, LightObject );


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
    Image2DicomGenericFilter();
    ~Image2DicomGenericFilter() {};

    
    //void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict);
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int inputDimension, unsigned int outputDimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int inputDimension, unsigned int outputDimension, class PixelType>  void UpdateWithDimAndPixelType();
    
    
     //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_type m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkImage2DicomGenericFilter.txx"
#endif

#endif // #define clitkImage2DicomGenericFilter_h
