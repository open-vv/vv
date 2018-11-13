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
#ifndef clitkSpect2DicomGenericFilter_txx
#define clitkSpect2DicomGenericFilter_txx

/* =================================================
 * @file   clitkSpect2DicomGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

#include "itkVersion.h"
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"

#include "itkThresholdImageFilter.h"

#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
#include "itkShiftScaleImageFilter.h"
#endif

#include <string>
#include <sstream>
#if GDCM_MAJOR_VERSION >= 2
#include <gdcmUIDGenerator.h>
#include <gdcmImageHelper.h>
#include <gdcmAttribute.h>
#include <gdcmReader.h>
#include <gdcmWriter.h>
#include <gdcmDataElement.h>
#include <gdcmTag.h>
#else
#include "gdcmFile.h"
#include "gdcmUtil.h"
#endif

namespace clitk
{


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
Spect2DicomGenericFilter<args_info_type>::Spect2DicomGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void Spect2DicomGenericFilter<args_info_type>::Update()
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
Spect2DicomGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
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
Spect2DicomGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{
  // Validate input parameters

  const unsigned int InputDimension = Dimension;
  const unsigned int OutputDimension = Dimension;

  typedef itk::Image< PixelType, InputDimension > InputImageType;
  typedef itk::Image< PixelType, OutputDimension > OutputImageType;
  typedef itk::ImageSeriesReader< InputImageType > ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
  typedef itk::ShiftScaleImageFilter< InputImageType, InputImageType > ShiftScaleType;
#endif
  typedef itk::ImageSeriesWriter< InputImageType, OutputImageType > SeriesWriterType;

////////////////////////////////////////////////
// 1) Read the input series

// Read the input (MHD file)
  typedef typename InputImageType::RegionType RegionType;
  typedef typename RegionType::SizeType SizeType;
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer volumeReader = InputReaderType::New();
  volumeReader->SetFileName(m_ArgsInfo.input_arg);
  volumeReader->Update();
  typename InputImageType::Pointer input = volumeReader->GetOutput();

  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  gdcmIO->LoadPrivateTagsOn();
  typename ReaderType::FileNamesContainer filenames;
  filenames.push_back(m_ArgsInfo.inputDcm_arg);
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO(gdcmIO);
  reader->SetFileNames(filenames);
  try {
    reader->Update();
  } catch (itk::ExceptionObject &excp) {
    std::cerr << "Exception thrown while reading the series" << std::endl;
    std::cerr << excp << std::endl;
    return;
  }

  typename InputImageType::SpacingType outputSpacing;
  typename InputImageType::SizeType   outputSize;
  for (unsigned int i = 0; i < 3; i++) {
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

  // Create a new dictionary for this slice
  typename ReaderType::DictionaryRawPointer dict = new typename ReaderType::DictionaryType;

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
  value.str("1");

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
  index[2] = 0;
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
  itk::EncapsulateMetaData<std::string>(*dict,"0018|0050", value.str());
  // Spacing Between Slices
  itk::EncapsulateMetaData<std::string>(*dict,"0018|0088", value.str());
  // Save the dictionary
  outputArray.push_back(inputDict);

#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
////////////////////////////////////////////////
// 4) Shift data to undo the effect of a rescale intercept by the
//    DICOM reader
  std::string interceptTag("0028|1052");
  typedef itk::MetaDataObject<std::string> MetaDataStringType;
  itk::MetaDataObjectBase::Pointer entry = (*inputDict)[interceptTag];

  MetaDataStringType::ConstPointer interceptValue =
    dynamic_cast<const MetaDataStringType *>(entry.GetPointer());

  int interceptShift = 0;
  if(interceptValue ) {
    std::string tagValue = interceptValue->GetMetaDataObjectValue();
    interceptShift = -atoi (tagValue.c_str());
  }

  ShiftScaleType::Pointer shiftScale = ShiftScaleType::New();
  shiftScale->SetInput(resampler->GetOutput());
  shiftScale->SetShift(interceptShift );
#endif

////////////////////////////////////////////////
// 5) Write the new DICOM series
  // Generate the file names
  typename ReaderType::FileNamesContainer fileNamesOutput;
  fileNamesOutput.push_back(m_ArgsInfo.outputDcm_arg);

  typename SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
#if ( ( ITK_VERSION_MAJOR == 4 ) && ( ITK_VERSION_MINOR < 6 ) )
  seriesWriter->SetInput(input);
#else
  seriesWriter->SetInput(input);
#endif
  seriesWriter->SetImageIO(gdcmIO);
  seriesWriter->SetFileNames(fileNamesOutput);
  seriesWriter->SetMetaDataDictionaryArray(&outputArray);
  try {
    seriesWriter->Update();
  } catch(itk::ExceptionObject & excp) {
    std::cerr << "Exception thrown while writing the series " << std::endl;
    std::cerr << excp << std::endl;
    return;
  }

////////////////////////////////////////////////
// 5) Read the new dicom data tag and copy it in the model data tag to have all dicom tags
  gdcm::Reader readerModel, readerOutput;
  readerModel.SetFileName(filenames[0].c_str());
  readerOutput.SetFileName(fileNamesOutput[0].c_str());
  readerModel.Read();
  readerOutput.Read();
  gdcm::File &fileModel = readerModel.GetFile();
  gdcm::File &fileOutput = readerOutput.GetFile();
  gdcm::DataSet &dsModel = fileModel.GetDataSet();
  gdcm::DataSet &dsOutput = fileOutput.GetDataSet();
  const unsigned int ptr_len = 42;
  char *ptr = new char[ptr_len];
  memset(ptr,0,ptr_len);

  const gdcm::DataElement &dataOutput = dsOutput.GetDataElement(gdcm::Tag(0x7fe0, 0x10));
  dsModel.Replace(dataOutput);
  gdcm::Writer w;
  w.SetFile(fileModel);
  w.SetFileName(fileNamesOutput[0].c_str());
  w.Write();
  return;
}
}//end clitk

#endif //#define clitkSpect2DicomGenericFilter_txx
