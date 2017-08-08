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
#ifndef clitkUpdateVRTagDicomGenericFilter_h
#define clitkUpdateVRTagDicomGenericFilter_h

/* =================================================
 * @file   clitkUpdateVRTagDicomGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkUpdateVRTagDicom_ggo.h"

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
enum valrep_t {VR_AE,
                                VR_AS,
                                VR_AT,
                                VR_CS,
                                VR_DA,
                                VR_DS,
                                VR_DT,
                                VR_FL,
                                VR_FD,
                                VR_IS,
                                VR_LO,
                                VR_LT,
                                VR_OB,
                                VR_OF,
                                VR_OW,
                                VR_PN,
                                VR_SH,
                                VR_SL,
                                VR_SQ,
                                VR_SS,
                                VR_ST,
                                VR_TM,
                                VR_UI,
                                VR_UL,
                                VR_UN,
                                VR_US,
                                VR_UT};
namespace clitk 
{
  template<class args_info_type>
  class ITK_EXPORT UpdateVRTagDicomGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef UpdateVRTagDicomGenericFilter   Self;
    typedef itk::LightObject                          Superclass;
    typedef itk::SmartPointer<Self>                   Pointer;
    typedef itk::SmartPointer<const Self>             ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);

    // Run-time type information (and related methods)
    itkTypeMacro( UpdateVRTagDicomGenericFilter, LightObject );


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
    UpdateVRTagDicomGenericFilter();
    ~UpdateVRTagDicomGenericFilter() {};


    //----------------------------------------
    // Templated members
    //----------------------------------------
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int Dimension, class PixelType> void UpdateWithDimAndPixelType();
    template <unsigned int Dimension, class PixelType> void ReadDataElement(std::ifstream& f, std::ifstream& model);
    template <unsigned int Dimension, class PixelType> void ReadDicomObject(std::ifstream& f, std::ifstream& model);
    template <unsigned int Dimension, class PixelType> void InitVRMap();
    template <unsigned int Dimension, class PixelType> void UpdateVRMap();


    //----------------------------------------
    // Data members
    //----------------------------------------
    args_info_type m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;
    int counter;
    uint16_t Group;
    uint16_t Element;
    std::string VR;
    uint32_t Length;
    uint32_t LengthModel;
    char* Value;
    char* ValueModel;
    std::map<std::string, valrep_t> VRMap;
    std::map<std::string, std::string> VRMapModel;

    std::ofstream output;
    std::ifstream f;
    std::ifstream model;

    unsigned long size;
    unsigned long endSQ;

  };

//Copy dicom dictionary
void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict);

//convert to std::string
template <typename T> std::string NumberToString ( T Number );
template <typename T> std::string int_to_hex ( T Number );
std::string string_to_hex( std::string &input );

} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkUpdateVRTagDicomGenericFilter.txx"
#endif

#endif // #define clitkUpdateVRTagDicomGenericFilter_h
