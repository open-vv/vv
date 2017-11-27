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
#ifndef clitkImage2DicomDoseGenericFilter_h
#define clitkImage2DicomDoseGenericFilter_h

/* =================================================
 * @file   clitkImage2DicomDoseGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkImage2DicomDose_ggo.h"

//itk include
#include "itkLightObject.h"
#include "itkGDCMImageIO.h"
#include "itkMetaDataDictionary.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <vector>
#include <itksys/SystemTools.hxx>

namespace clitk 
{
  template<class args_info_type>
  class ITK_EXPORT Image2DicomDoseGenericFilter : public itk::LightObject
  {

  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef Image2DicomDoseGenericFilter   Self;
    typedef itk::LightObject                          Superclass;
    typedef itk::SmartPointer<Self>                   Pointer;
    typedef itk::SmartPointer<const Self>             ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( Image2DicomDoseGenericFilter, LightObject );

    //methods
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
    Image2DicomDoseGenericFilter();
    ~Image2DicomDoseGenericFilter() {};

    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();


    gdcm::File * mDCMFile;

    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_type m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };

  //Usefull functions
  void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict);

} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkImage2DicomDoseGenericFilter.txx"
#endif

#endif // #define clitkImage2DicomDoseGenericFilter_h
