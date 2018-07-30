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
#ifndef clitkImage2DicomSeriesGenericFilter_txx
#define clitkImage2DicomSeriesGenericFilter_txx

/* =================================================
 * @file   clitkImage2DicomSeriesGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

// clitk
#include "clitkResampleImageWithOptionsFilter.h"
#if GDCM_MAJOR_VERSION >= 2
#include "gdcmUIDGenerator.h"
#else
#include "gdcmFile.h"
#include "gdcmUtil.h"
#endif
#include "itkVersion.h"
 
#include "itkImage.h"
#include "itkMinimumMaximumImageFilter.h"
 
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
 
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"

#include <itkThresholdImageFilter.h>
 
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


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
Image2DicomSeriesGenericFilter<args_info_type>::Image2DicomSeriesGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void Image2DicomSeriesGenericFilter<args_info_type>::Update()
{
  // Read the Dimension and PixelType
  int Dimension;
  std::string PixelType;
  ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);


  // Call UpdateWithDim
  if(Dimension==2) UpdateWithDim<2>(PixelType);
  else if(Dimension==3) UpdateWithDim<3>(PixelType);
  // else if (Dimension==4)UpdateWithDim<4>(PixelType);
  else {
    std::cout<<"Error, Only for 2 or 3  Dimensions!!!"<<std::endl ;
    return;
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions
//-------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dimension>
void
Image2DicomSeriesGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

  if(PixelType == "short") {
    if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, signed short>();
  }
  else if(PixelType == "unsigned_short"){
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned short>();
  }

  else if (PixelType == "unsigned_char") {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned char>();
  }

  //     else if (PixelType == "char"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, signed char>();
  //     }
  else if (PixelType == "double") {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and double..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, double>();
  }
  else {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, float>();
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype read from
// the dicom files. The MHD files may be resampled to match the
// dicom spacing (and number of slices). Rounding errors in resampling
// are handled by removing files when generating the output dicom
// series.
//-------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
Image2DicomSeriesGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{


// Resample a DICOM study
//   Usage: ResampleDICOM InputDirectory OutputDirectory
//                        xSpacing ySpacing zSpacing
//
//   Example: ResampleDICOM CT CTResample 0 0 1.5
//            will read a series from the CT directory and create a
//            new series in the CTResample directory. The new series
//            will have the same x,y spacing as the input series, but
//            will have a z-spacing of 1.5.
//
// Description:
// ResampleDICOM resamples a DICOM series with user-specified
// spacing. The program outputs a new DICOM series with a series
// number set to 1001. All non-private DICOM tags are moved from the input
// series to the output series. The Image Position Patient is adjusted
// for each slice to reflect the z-spacing. The number of slices in
// the output series may be larger or smaller due to changes in the
// z-spacing. To retain the spacing for a given dimension, specify 0.
//
// The program progresses as follows:
// 1) Read the input series
// 2) Resample the series according to the user specified x-y-z
//    spacing.
// 3) Create a MetaDataDictionary for each slice.
// 4) Shift data to undo the effect of a rescale intercept by the
//    DICOM reader (only for ITK < 4.6)
// 5) Write the new DICOM series
//
 

 
 
  // Validate input parameters
 
  const unsigned int InputDimension = 3;
  const unsigned int OutputDimension = 2;
 
 
  typedef itk::Image< PixelType, InputDimension >
    InputImageType;
  typedef itk::Image< PixelType, OutputDimension >
    OutputImageType;
  typedef itk::ImageSeriesReader< InputImageType >
    ReaderType;
  typedef itk::GDCMImageIO
    ImageIOType;
  typedef itk::GDCMSeriesFileNames
    InputNamesGeneratorType;
  typedef itk::NumericSeriesFileNames
    OutputNamesGeneratorType;
  typedef itk::IdentityTransform< double, InputDimension >
    TransformType;
  typedef itk::LinearInterpolateImageFunction< InputImageType, double >
    InterpolatorType;
  typedef itk::ResampleImageFilter< InputImageType, InputImageType >
    ResampleFilterType;
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
  typedef itk::ShiftScaleImageFilter< InputImageType, InputImageType >
    ShiftScaleType;
#endif
  typedef itk::ImageSeriesWriter< InputImageType, OutputImageType >
    SeriesWriterType;
 
////////////////////////////////////////////////
// 1) Read the input series

// Read the input (MHD file)
  typedef typename InputImageType::RegionType RegionType;
  typedef typename RegionType::SizeType SizeType;
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer volumeReader = InputReaderType::New();
  volumeReader->SetFileName( m_ArgsInfo.input_arg );
  volumeReader->Update();
  
  typename InputImageType::Pointer input = volumeReader->GetOutput();
 
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  gdcmIO->LoadPrivateTagsOn();
  InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
  inputNames->SetInputDirectory( m_ArgsInfo.inputDir_arg );
 
  const typename ReaderType::FileNamesContainer & filenames =
                            inputNames->GetInputFileNames();
 
  typename ReaderType::Pointer reader = ReaderType::New();
 
  reader->SetImageIO( gdcmIO );
  reader->SetFileNames( filenames );
  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cerr << "Exception thrown while reading the series" << std::endl;
    std::cerr << excp << std::endl;
    return;
    }
    typename InputImageType::SpacingType outputSpacing;
    typename InputImageType::SizeType   outputSize;
    for (unsigned int i = 0; i < 3; i++)
    {
      outputSpacing[i] = input->GetSpacing()[i];
      outputSize[i] = input->GetLargestPossibleRegion().GetSize()[i];
    }

////////////////////////////////////////////////
// 2) Ensure to have value >= -1024

  typedef itk::ThresholdImageFilter <InputImageType> ThresholdImageFilterType;
  typename ThresholdImageFilterType::Pointer thresholdFilter = ThresholdImageFilterType::New();
  thresholdFilter->SetInput(input);
  thresholdFilter->ThresholdBelow(-1024);
  thresholdFilter->SetOutsideValue(-1024);
  thresholdFilter->Update();

  input=thresholdFilter->GetOutput();



////////////////////////////////////////////////
// 2) Resample the series
/*  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
 
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();
 
  const typename InputImageType::SpacingType& inputSpacing =
    reader->GetOutput()->GetSpacing();
  const typename InputImageType::RegionType& inputRegion =
    reader->GetOutput()->GetLargestPossibleRegion();
  const typename InputImageType::SizeType& inputSize =
    inputRegion.GetSize();
 
  std::cout << "The input series in directory " << m_ArgsInfo.inputDir_arg
            << " has " << filenames.size() << " files with spacing "
            << inputSpacing
            << std::endl;
 
  // Compute the size of the output. The user specifies a spacing on
  // the command line. If the spacing is 0, the input spacing will be
  // used. The size (# of pixels) in the output is recomputed using
  // the ratio of the input and output sizes.
  typename InputImageType::SpacingType outputSpacing;
 
  bool changeInSpacing = false;
  for (unsigned int i = 0; i < 3; i++)
    {
      outputSpacing[i] = inputSpacing[i];
    }
  typename InputImageType::SizeType   outputSize;
  typedef typename InputImageType::SizeType::SizeValueType SizeValueType;
  outputSize[0] = static_cast<SizeValueType>(inputSize[0] * inputSpacing[0] / outputSpacing[0] + .5);
  outputSize[1] = static_cast<SizeValueType>(inputSize[1] * inputSpacing[1] / outputSpacing[1] + .5);
  outputSize[2] = static_cast<SizeValueType>(inputSize[2] * inputSpacing[2] / outputSpacing[2] + .5);
 
  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  resampler->SetInput( reader->GetOutput() );
  resampler->SetTransform( transform );
  resampler->SetInterpolator( interpolator );
  resampler->SetOutputOrigin ( reader->GetOutput()->GetOrigin());
  resampler->SetOutputSpacing ( outputSpacing );
  resampler->SetOutputDirection ( reader->GetOutput()->GetDirection());
  resampler->SetSize ( outputSize );
  resampler->Update ();
 
 */
 
////////////////////////////////////////////////
// 3) Create a MetaDataDictionary for each slice.
 
  // Copy the dictionary from the first image and override slice
  // specific fields
  typename ReaderType::DictionaryRawPointer inputDict = (*(reader->GetMetaDataDictionaryArray()))[0];
  typename ReaderType::DictionaryArrayType outputArray;
 
  // To keep the new series in the same study as the original we need
  // to keep the same study UID. But we need new series and frame of
  // reference UID's.
#if ITK_VERSION_MAJOR >= 4
  gdcm::UIDGenerator suid;
  std::string seriesUID = suid.Generate();
  gdcm::UIDGenerator fuid;
  std::string frameOfReferenceUID = fuid.Generate();
#else
  std::string seriesUID = gdcm::Util::CreateUniqueUID( gdcmIO->GetUIDPrefix());
  std::string frameOfReferenceUID = gdcm::Util::CreateUniqueUID( gdcmIO->GetUIDPrefix());
#endif
  std::string studyUID;
  std::string sopClassUID;
  itk::ExposeMetaData<std::string>(*inputDict, "0020|000d", studyUID);
  itk::ExposeMetaData<std::string>(*inputDict, "0008|0016", sopClassUID);
  gdcmIO->KeepOriginalUIDOn();
 
  for (unsigned int f = 0; f < outputSize[2]; f++)
    {
    // Create a new dictionary for this slice
    typename ReaderType::DictionaryRawPointer dict = new typename ReaderType::DictionaryType;
 
    // Copy the dictionary from the first slice
    //CopyDictionary (*inputDict, *dict);
    
    typedef itk::MetaDataDictionary DictionaryType;

    DictionaryType::ConstIterator itrDic = (*inputDict).Begin();
    DictionaryType::ConstIterator endDic = (*inputDict).End();
    typedef itk::MetaDataObject< std::string > MetaDataStringType;
 
    while( itrDic != endDic )
    {
      itk::MetaDataObjectBase::Pointer  entry = itrDic->second;
 
      MetaDataStringType::Pointer entryvalue =
      dynamic_cast<MetaDataStringType *>( entry.GetPointer() ) ;
      if( entryvalue )
      {
        std::string tagkey   = itrDic->first;
        std::string tagvalue = entryvalue->GetMetaDataObjectValue();
        itk::EncapsulateMetaData<std::string>(*dict, tagkey, tagvalue);
      }
      ++itrDic;
    }
 
    // Set the UID's for the study, series, SOP  and frame of reference
    itk::EncapsulateMetaData<std::string>(*dict,"0020|000d", studyUID);
    itk::EncapsulateMetaData<std::string>(*dict,"0020|000e", seriesUID);
    itk::EncapsulateMetaData<std::string>(*dict,"0020|0052", frameOfReferenceUID);
 
#if ITK_VERSION_MAJOR >= 4
    gdcm::UIDGenerator sopuid;
    std::string sopInstanceUID = sopuid.Generate();
#else
    std::string sopInstanceUID = gdcm::Util::CreateUniqueUID( gdcmIO->GetUIDPrefix());
#endif
    itk::EncapsulateMetaData<std::string>(*dict,"0008|0018", sopInstanceUID);
    itk::EncapsulateMetaData<std::string>(*dict,"0002|0003", sopInstanceUID);
 
    // Change fields that are slice specific
    std::ostringstream value;
    value.str("");
    //unsigned int f = 0;
    value << f + 1;
 
    // Image Number
    itk::EncapsulateMetaData<std::string>(*dict,"0020|0013", value.str());
 
    // Series Description - Append new description to current series
    // description
    std::string oldSeriesDesc;
    itk::ExposeMetaData<std::string>(*inputDict, "0008|103e", oldSeriesDesc);
 
    value.str("");
    value << oldSeriesDesc
          << ": Resampled with pixel spacing "
          << outputSpacing[0] << ", "
          << outputSpacing[1] << ", "
          << outputSpacing[2];
    // This is an long string and there is a 64 character limit in the
    // standard
    unsigned lengthDesc = value.str().length();
 
    std::string seriesDesc( value.str(), 0,
                            lengthDesc > 64 ? 64
                            : lengthDesc);
    itk::EncapsulateMetaData<std::string>(*dict,"0008|103e", seriesDesc);
 
    // Series Number
    value.str("");
    value << 1001;
    itk::EncapsulateMetaData<std::string>(*dict,"0020|0011", value.str());
 
    // Derivation Description - How this image was derived
    value.str("");

    value << ": " << ITK_SOURCE_VERSION;
 
    lengthDesc = value.str().length();
    std::string derivationDesc( value.str(), 0,
                                lengthDesc > 1024 ? 1024
                                : lengthDesc);
    itk::EncapsulateMetaData<std::string>(*dict,"0008|2111", derivationDesc);
 
    // Image Position Patient: This is calculated by computing the
    // physical coordinate of the first pixel in each slice.
    typename InputImageType::PointType position;
    typename InputImageType::IndexType index;
    index[0] = 0;
    index[1] = 0;
    index[2] = f;
    input->TransformIndexToPhysicalPoint(index, position);
 
    value.str("");
    value << position[0] << "\\" << position[1] << "\\" << position[2];
    itk::EncapsulateMetaData<std::string>(*dict,"0020|0032", value.str());
    // Slice Location: For now, we store the z component of the Image
    // Position Patient.
    value.str("");
    value << position[2];
    itk::EncapsulateMetaData<std::string>(*dict,"0020|1041", value.str());
 

      // Slice Thickness: For now, we store the z spacing
      value.str("");
      value << outputSpacing[2];
      itk::EncapsulateMetaData<std::string>(*dict,"0018|0050",
                                            value.str());
      // Spacing Between Slices
      itk::EncapsulateMetaData<std::string>(*dict,"0018|0088",
                                            value.str());
      
      /*
      value.str("");
      value << 0.999987 << "\\" << -0.005061 << "\\" << 0.000000 << "\\" << 0.000000 << "\\" << 0.000000 << "\\" << -1.000000;
      itk::EncapsulateMetaData<std::string>(*dict, "0020|0037", value.str());
       
      value.str("");
      value << 196;
      itk::EncapsulateMetaData<std::string>(*dict, "0002|0000", value.str());
      value.str("");
      value << 510;
      itk::EncapsulateMetaData<std::string>(*dict, "0008|0000", value.str());
      value.str("");
      value << 362;
      itk::EncapsulateMetaData<std::string>(*dict, "0009|0000", value.str());
      value.str("");
      value << 116;
      itk::EncapsulateMetaData<std::string>(*dict, "0010|0000", value.str());
      value.str("");
      value << 1008;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|0000", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1011", value.str());
      value.str("");
      value << 1 << "\\" << 1;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1015", value.str());
      value.str("");
      value << 1 << "\\" << 2;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1016", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1017", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1018", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1019", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|101a", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|101f", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1026", value.str());
      value.str("");
      value << 0 << "\\" << 0 << "\\" << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1027", value.str());
      value.str("");
      value << 0 << "\\" << 0 << "\\" << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1028", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|102c", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|102d", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|102e", value.str());
      std::ostringstream valueVec[2];
      valueVec[0].str("");
      valueVec[0] << 32767;
      valueVec[1].str("");
      valueVec[1] << 32767;
      std::string valueVec2[2];
      valueVec2[0]=valueVec[0].str();
      valueVec2[1]=valueVec[1].str();
      itk::EncapsulateMetaData<std::string*>(*dict, "0011|102f", valueVec2);
      value.str("");
      value << 1 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0020|1011", value.str());
      value.str("");
      value << 63 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0028|0107", value.str());
      
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1130", value.str());
      value.str("");
      value << 563.7 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1131", value.str());
      value.str("");
      value << "CC" ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1140", value.str());
      value.str("");
      value << 243.5 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1142", value.str());
      value.str("");
      value << 360 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1143", value.str());
      value.str("");
      value << 6 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1144", value.str());
      value.str("");
      value << 10000 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1242", value.str());
      value.str("");
      value << 60 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0054|0053", value.str());
      value.str("");
      value << 180.29 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0054|0200", value.str());
      value.str("");
      value << "F-10450" ;
      itk::EncapsulateMetaData<std::string>(*dict, "0008|0100", value.str());
      value.str("");
      value << "99SDM" ;
      itk::EncapsulateMetaData<std::string>(*dict, "0008|0102", value.str());
      value.str("");
      value << "recumbent" ;
      itk::EncapsulateMetaData<std::string>(*dict, "0008|0104", value.str());
      value.str("");
      value << 114 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0054|0014", value.str());
      value.str("");
      value << 126 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0054|0015", value.str());
      value.str("");
      value << "Tc99m_SC" ;
      itk::EncapsulateMetaData<std::string>(*dict, "0054|0018", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1120", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1145", value.str());
      value.str("");
      value << 0 << "\\" << 0;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1149", value.str());
      value.str("");
      value << "LEHR" ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1180", value.str());
      value.str("");
      value << "PARA" ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1181", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1182", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1183", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0018|1184", value.str());
      value.str("");
      value << 1.000000 << "\\" << 1.000000 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0028|0031", value.str());
      value.str("");
      value << 0.000000 << "\\" << 0.000000 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0028|0032", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|101c", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|101d", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0013|1016", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0013|1017", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1023", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1024", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1025", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|1029", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0011|103e", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0013|1018", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0013|1019", value.str());
      value.str("");
      value << 0 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0013|101a", value.str());
      value.str("");
      value << "GEMS_GENIE_1" ;
      itk::EncapsulateMetaData<std::string>(*dict, "0035|0010", value.str());
      value.str("");
      value << 29 ;
      itk::EncapsulateMetaData<std::string>(*dict, "0035|1001", value.str());
      
 */
    // Save the dictionary
    outputArray.push_back(dict);
    }
 
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
////////////////////////////////////////////////
// 4) Shift data to undo the effect of a rescale intercept by the
//    DICOM reader
  std::string interceptTag("0028|1052");
  typedef itk::MetaDataObject< std::string > MetaDataStringType;
  itk::MetaDataObjectBase::Pointer entry = (*inputDict)[interceptTag];
 
  MetaDataStringType::ConstPointer interceptValue =
    dynamic_cast<const MetaDataStringType *>( entry.GetPointer() ) ;
 
  int interceptShift = 0;
  if( interceptValue )
    {
    std::string tagValue = interceptValue->GetMetaDataObjectValue();
    interceptShift = -atoi ( tagValue.c_str() );
    }
 
  ShiftScaleType::Pointer shiftScale = ShiftScaleType::New();
  shiftScale->SetInput( resampler->GetOutput());
  shiftScale->SetShift( interceptShift );
#endif
 
////////////////////////////////////////////////
// 5) Write the new DICOM series
 
  // Make the output directory and generate the file names.
  itksys::SystemTools::MakeDirectory( m_ArgsInfo.outputDir_arg );
 
  // Generate the file names
  OutputNamesGeneratorType::Pointer outputNames = OutputNamesGeneratorType::New();
  std::string seriesFormat(m_ArgsInfo.outputDir_arg);
  seriesFormat = seriesFormat + "/" + "IM%03d.dcm";
  outputNames->SetSeriesFormat (seriesFormat.c_str());
  outputNames->SetStartIndex (1);
  //outputNames->SetEndIndex (1);
  outputNames->SetEndIndex (outputSize[2]);
 
  typename SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
  seriesWriter->SetInput( input );
#else
  seriesWriter->SetInput( input );
#endif
    seriesWriter->SetImageIO( gdcmIO );
    seriesWriter->SetFileNames( outputNames->GetFileNames() );
    seriesWriter->SetMetaDataDictionaryArray( &outputArray );
  try
    {
    seriesWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown while writing the series " << std::endl;
    std::cerr << excp << std::endl;
    return;
    }
  std::cout << "The output series in directory " << m_ArgsInfo.outputDir_arg
            << " has " << outputSize[2] << " files with spacing "
            << outputSpacing
            << std::endl;
  return;
}

 
/*void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict)
{
  typedef itk::MetaDataDictionary DictionaryType;
 
  DictionaryType::ConstIterator itr = fromDict.Begin();
  DictionaryType::ConstIterator end = fromDict.End();
  typedef itk::MetaDataObject< std::string > MetaDataStringType;
 
  while( itr != end )
    {
    itk::MetaDataObjectBase::Pointer  entry = itr->second;
 
    MetaDataStringType::Pointer entryvalue =
      dynamic_cast<MetaDataStringType *>( entry.GetPointer() ) ;
    if( entryvalue )
      {
      std::string tagkey   = itr->first;
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
      itk::EncapsulateMetaData<std::string>(toDict, tagkey, tagvalue);
      }
    ++itr;
    }
} */


}//end clitk

#endif //#define clitkImage2DicomSeriesGenericFilter_txx
