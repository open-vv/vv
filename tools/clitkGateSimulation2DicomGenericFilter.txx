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
#ifndef clitkGateSimulation2DicomGenericFilter_txx
#define clitkGateSimulation2DicomGenericFilter_txx

/* =================================================
 * @file   clitkGateSimulation2DicomGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

#include <sstream>
// clitk
#include "clitkResampleImageWithOptionsFilter.h"
#if GDCM_MAJOR_VERSION >= 2
#include "gdcmUIDGenerator.h"
#include <gdcmImageHelper.h>
#include <gdcmAttribute.h>
#include <gdcmReader.h>
#else
#include "gdcmFile.h"
#include "gdcmUtil.h"
#endif

#include "itkImageRegionIterator.h"


namespace clitk
{


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
GateSimulation2DicomGenericFilter<args_info_type>::GateSimulation2DicomGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void GateSimulation2DicomGenericFilter<args_info_type>::Update()
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
GateSimulation2DicomGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
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
GateSimulation2DicomGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{

#if GDCM_MAJOR_VERSION == 2
  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;
  typedef itk::ImageFileReader< InputImageType >     ReaderType;
  typedef itk::ImageSeriesReader< InputImageType >     ReaderSeriesType;
  typedef itk::GDCMImageIO ImageIOType;


  // Read Dicom model file
  typename ReaderType::Pointer reader = ReaderType::New();
  typename ReaderSeriesType::Pointer readerSeries = ReaderSeriesType::New();
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  std::string filename_out = m_ArgsInfo.outputFilename_arg;
  gdcmIO->LoadPrivateTagsOn();
  gdcmIO->KeepOriginalUIDOn();
  reader->SetImageIO( gdcmIO );
  reader->SetFileName( m_ArgsInfo.inputModelFilename_arg );
  typename ReaderSeriesType::FileNamesContainer fileNames;
  fileNames.push_back(m_ArgsInfo.inputModelFilename_arg);
  readerSeries->SetImageIO( gdcmIO );
  readerSeries->SetFileNames( fileNames );
  try {
    reader->Update();
    readerSeries->Update();
  } catch (itk::ExceptionObject &excp) {
    std::cerr << "Error: Exception thrown while reading the DICOM model file !!" << std::endl;
    std::cerr << excp << std::endl;
  }


  // Read the input (MHD file)
  typedef typename InputImageType::RegionType RegionType;
  typedef typename RegionType::SizeType SizeType;
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer volumeReader = InputReaderType::New();
  volumeReader->SetFileName( m_InputFileName);
  volumeReader->Update();
  typename InputImageType::Pointer input = volumeReader->GetOutput();


//TODO if the input size/spacing and dicom model ones are different
/*  if (input->GetLargestPossibleRegion().GetSize() != reader->GetOutput()->GetLargestPossibleRegion().GetSize()) {
        
    // resampling is carried out on the fly if resolution or size between 
    // the input mhd and input dicom series is different
    
    // Filter
    typedef clitk::ResampleImageWithOptionsFilter<InputImageType, InputImageType> ResampleImageFilterType;
    typename ResampleImageFilterType::Pointer filter = ResampleImageFilterType::New();
    filter->SetInput(input);
    filter->SetVerboseOptions(m_Verbose);
    filter->SetGaussianFilteringEnabled(false);
    filter->SetDefaultPixelValue(0);

    const SizeType& input_size = input->GetLargestPossibleRegion().GetSize();
    SizeType output_size;
    for (unsigned int i = 0; i < Dimension; i++)
      output_size[i] = input_size[i];
    filter->SetOutputSize(output_size);
    if (m_Verbose) {
        std::cout << "Warning: The image size differs between the MHD file and the input dicom series. Performing resampling with default options using mhd size as reference (for advanced options, use clitkResampleImage)." << std::endl;
        std::cout << "MHD -> " << input->GetLargestPossibleRegion().GetSize() << std::endl;
        std::cout << "dicom -> " << reader->GetOutput()->GetLargestPossibleRegion().GetSize() << std::endl;
    }

    try {
      filter->Update();
      input = filter->GetOutput();
    } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error: Exception thrown while resampling!!" << std::endl;
    std::cerr << excp << std::endl;
    }
  }
*/


  //Read the dicom file to find the energy, the head & the steps (TODO: do it on the mhd filetext)
  gdcm::Reader hreader;
  hreader.SetFileName(m_ArgsInfo.inputModelFilename_arg);
  hreader.Read();
  gdcm::DataSet& ds = hreader.GetFile().GetDataSet();
  int energyNumber, headNumber, rotationNumber; //Read the number of energy, of head and rotation steps
  gdcm::Attribute<0x54,0x11> energyNumber_att;
  energyNumber_att.SetFromDataSet(hreader.GetFile().GetDataSet());
  energyNumber = energyNumber_att.GetValue();

  gdcm::Attribute<0x54,0x21> headNumber_att;
  headNumber_att.SetFromDataSet(hreader.GetFile().GetDataSet());
  headNumber = headNumber_att.GetValue();

  gdcm::Tag squenceTag(0x54,0x52); //rotation, read a sequence first
  const gdcm::DataElement &sequence = hreader.GetFile().GetDataSet().GetDataElement( squenceTag );
  gdcm::DataSet &sequenceDataSet = sequence.GetValueAsSQ()->GetItem(1).GetNestedDataSet();
  gdcm::Tag rotationNumber_Tag(0x54,0x53);
  const gdcm::DataElement &rotationNumber_Element = sequenceDataSet.GetDataElement( rotationNumber_Tag );
  gdcm::Attribute<0x54,0x53> rotationNumber_att;
  rotationNumber_att.SetFromDataElement(rotationNumber_Element);
  rotationNumber = rotationNumber_att.GetValue();

energyNumber = 3;
headNumber = 4;
rotationNumber = 6;
  // Create a new mhd image with the dicom order slices
  typename InputImageType::Pointer mhdCorrectOrder = InputImageType::New();
  mhdCorrectOrder->SetRegions(input->GetLargestPossibleRegion());
  mhdCorrectOrder->Allocate();
  unsigned int zAxis(0); //z value for the input mhd image
  for (unsigned int energy = 0; energy < energyNumber; ++energy) {
    for (unsigned int head = 0; head < headNumber; ++head) {
      for (unsigned int rotation = 0; rotation < rotationNumber; ++rotation) {
        std::cout << "Energy " << energy << " Head " << head << " Rotation " << rotation << std::endl;

        typename InputImageType::IndexType startIteratorIndexCorrectOrder; //pixel index of mhdCorrectOrder
        startIteratorIndexCorrectOrder[0] = 0;
        startIteratorIndexCorrectOrder[1] = 0;
        startIteratorIndexCorrectOrder[2] = rotation + head*rotationNumber + energy*headNumber;

        typename InputImageType::IndexType startIteratorIndexOriginalOrder; //pixel index of input mhd
        startIteratorIndexOriginalOrder[0] = 0;
        startIteratorIndexOriginalOrder[1] = 0;
        startIteratorIndexOriginalOrder[2] = head + energy*headNumber + rotation*energyNumber;

        typename InputImageType::SizeType regionSizeIterator;
        regionSizeIterator[0] = input->GetLargestPossibleRegion().GetSize()[0];
        regionSizeIterator[1] = input->GetLargestPossibleRegion().GetSize()[1];
        regionSizeIterator[2] = 1;

        typename InputImageType::RegionType regionIteratorCorrectOrder;
        regionIteratorCorrectOrder.SetSize(regionSizeIterator);
        regionIteratorCorrectOrder.SetIndex(startIteratorIndexCorrectOrder);

        typename InputImageType::RegionType regionIteratorOriginalOrder;
        regionIteratorOriginalOrder.SetSize(regionSizeIterator);
        regionIteratorOriginalOrder.SetIndex(startIteratorIndexOriginalOrder);

        itk::ImageRegionIterator<InputImageType> CorrectOrderIterator(mhdCorrectOrder,regionIteratorCorrectOrder);
        itk::ImageRegionIterator<InputImageType> OriginalOrderIterator(input,regionIteratorOriginalOrder);
        while(!CorrectOrderIterator.IsAtEnd()) {
          CorrectOrderIterator.Set(OriginalOrderIterator.Get());
          ++CorrectOrderIterator;
          ++OriginalOrderIterator;
        }

        ++zAxis;
      }
    }
  }


  // update output dicom keys/tags
  typename ReaderSeriesType::DictionaryRawPointer inputDict = (*(readerSeries->GetMetaDataDictionaryArray()))[0];
  typename ReaderSeriesType::DictionaryArrayType outputArray;
  typename ReaderSeriesType::DictionaryRawPointer outputDict = new typename ReaderSeriesType::DictionaryType;
  CopyDictionary (*inputDict, *outputDict);

  std::string entryId, value;
  entryId = "0054|0011";
  value = NumberToString(energyNumber);
  itk::EncapsulateMetaData<std::string>(*outputDict, "0054|0011", value );
  entryId = "0054|0021";
  value = NumberToString(headNumber);
  itk::EncapsulateMetaData<std::string>(*outputDict, "0054|0021", value );

outputArray.push_back(outputDict);
  // Output directory and filenames
  //itksys::SystemTools::MakeDirectory( m_ArgsInfo.outputFilename_arg ); // create if it doesn't exist
  typedef itk::ImageFileWriter<OutputImageType>  WriterType;
  typedef itk::ImageSeriesWriter<OutputImageType, OutputImageType>  WriterSerieType;
  typename WriterType::Pointer writer = WriterType::New();
  typename WriterSerieType::Pointer writerSerie = WriterSerieType::New();

  writer->SetInput( mhdCorrectOrder );
  writer->SetImageIO( gdcmIO );
  
  writer->SetFileName( filename_out );
  //writer->SetMetaDataDictionary(outputDict);
  writerSerie->SetInput( mhdCorrectOrder );
  writerSerie->SetImageIO( gdcmIO );
  typename ReaderSeriesType::FileNamesContainer fileNamesOutput;
  fileNamesOutput.push_back(filename_out);
  writerSerie->SetFileNames( fileNamesOutput );
  writerSerie->SetMetaDataDictionaryArray(&outputArray);

  // Write
  try {
    if (m_ArgsInfo.verbose_flag)
      std::cout << writer << std::endl;
    //writer->Update();
    writerSerie->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error: Exception thrown while writing the series!!" << std::endl;
    std::cerr << excp << std::endl;
  }
#else
  std::cout << "Use GDCM2" << std::endl;
#endif
}

void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict)
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
}

template <typename T>
  std::string NumberToString ( T Number )
  {
     std::ostringstream ss;
     ss << Number;
     return ss.str();
  }
}//end clitk

#endif //#define clitkGateSimulation2DicomGenericFilter_txx
