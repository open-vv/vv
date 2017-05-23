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
#include <gdcmWriter.h>
#else
#include "gdcmFile.h"
#include "gdcmUtil.h"
#endif

#include "itkImageRegionIterator.h"
#include "itkMetaImageIO.h"
#include "itkMetaDataDictionary.h"


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


  //Read the metadata informations in the mhd
  MetaObject tObj(3);
  tObj.AddUserField("NumberOfEnergyWindows", MET_STRING);
  tObj.AddUserField("NbOfHeads", MET_STRING);
  tObj.AddUserField("NbOfProjections", MET_STRING);
  tObj.Read(m_InputFileName.c_str());
  char *p_energyNumber, *p_headNumber, *p_rotationNumber;
  int energyNumber, headNumber, rotationNumber;
  p_energyNumber = static_cast<char*>(tObj.GetUserField("NumberOfEnergyWindows"));
  p_headNumber = static_cast<char*>(tObj.GetUserField("NbOfHeads"));
  p_rotationNumber = static_cast<char*>(tObj.GetUserField("NbOfProjections"));
  energyNumber = atoi(p_energyNumber);
  headNumber = atoi(p_headNumber);
  rotationNumber = atoi(p_rotationNumber);
  for (unsigned int i=0; i<energyNumber; ++i) {
    std::string tagEnergyName("EnergyWindow_");
    tagEnergyName += NumberToString(i).c_str();
    std::string tagEnergyThreshold(tagEnergyName), tagEnergyUphold(tagEnergyName);
    tagEnergyThreshold += "_threshold";
    tagEnergyUphold += "_uphold";
    tObj.AddUserField(tagEnergyName.c_str(), MET_STRING);
    tObj.AddUserField(tagEnergyThreshold.c_str(), MET_STRING);
    tObj.AddUserField(tagEnergyUphold.c_str(), MET_STRING);
  }
  tObj.Read(m_InputFileName.c_str());
  std::vector<char*> p_EnergyWindowName(energyNumber), p_EnergyWindowThreshold(energyNumber), p_EnergyWindowUphold(energyNumber);
  std::vector<int> energyWindowThreshold(energyNumber), energyWindowUphold(energyNumber);
  for (unsigned int i=0; i<energyNumber; ++i) {
    std::string tagEnergyName("EnergyWindow_");
    tagEnergyName += NumberToString(i).c_str();
    std::string tagEnergyThreshold(tagEnergyName), tagEnergyUphold(tagEnergyName);
    tagEnergyThreshold += "_threshold";
    tagEnergyUphold += "_uphold";
    p_EnergyWindowName[i] = static_cast<char*>(tObj.GetUserField(tagEnergyName.c_str()));
    p_EnergyWindowThreshold[i] = static_cast<char*>(tObj.GetUserField(tagEnergyThreshold.c_str()));
    p_EnergyWindowUphold[i] = static_cast<char*>(tObj.GetUserField(tagEnergyUphold.c_str()));
    energyWindowThreshold[i] = atoi(p_EnergyWindowThreshold[i]);
    energyWindowUphold[i] = atoi(p_EnergyWindowUphold[i]);
  }

//TODO if the input size/spacing and dicom model ones are different

  // Create a new mhd image with the correct dicom order slices
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
  // string for distinguishing items inside sequence:
  const std::string ITEM_ENCAPSULATE_STRING("DICOM_ITEM_ENCAPSULATE");
  std::string tempString = ITEM_ENCAPSULATE_STRING + "01";
  typename ReaderSeriesType::DictionaryRawPointer inputDict = (*(readerSeries->GetMetaDataDictionaryArray()))[0];
  typename ReaderSeriesType::DictionaryArrayType outputArray;
  typename ReaderSeriesType::DictionaryRawPointer outputDict = new typename ReaderSeriesType::DictionaryType;
  CopyDictionary (*inputDict, *outputDict);
  itk::EncapsulateMetaData<std::string>(*outputDict, "0054|0011", NumberToString(energyNumber));
  itk::EncapsulateMetaData<std::string>(*outputDict, "0054|0021", NumberToString(headNumber));
  outputArray.push_back(outputDict);


  // Output directory and filenames
  typedef itk::ImageFileWriter<OutputImageType>  WriterType;
  typedef itk::ImageSeriesWriter<OutputImageType, OutputImageType>  WriterSerieType;
  typename WriterType::Pointer writer = WriterType::New();
  typename WriterSerieType::Pointer writerSerie = WriterSerieType::New();
  writer->SetInput( mhdCorrectOrder );
  writer->SetImageIO( gdcmIO );
  writer->SetFileName( filename_out );
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


  //Write sequence dicom tag with gdcm
  gdcm::Reader reader2;
  reader2.SetFileName( fileNamesOutput[0].c_str() );
  reader2.Read();
  gdcm::File &file = reader2.GetFile();
  gdcm::DataSet &ds2 = file.GetDataSet();
  const unsigned int ptr_len = 42;
  char *ptr = new char[ptr_len];
  memset(ptr,0,ptr_len);

  //Write rotation tag
  // Create a Sequence
  gdcm::SmartPointer<gdcm::SequenceOfItems> rotationSq = new gdcm::SequenceOfItems();
  rotationSq->SetLengthToUndefined();
  // Create a dataelement
  gdcm::DataElement rotationDE( gdcm::Tag(0x54, 0x53) );
  rotationDE.SetVR( gdcm::VR::US );
  char essai = (char)rotationNumber;
  char *p_essai = &essai;
  rotationDE.SetByteValue(p_essai, 1);
  // Create an item
  gdcm::Item rotationIt;
  rotationIt.SetVLToUndefined();
  gdcm::DataSet &rotationDS = rotationIt.GetNestedDataSet();
  rotationDS.Insert(rotationDE);
  rotationSq->AddItem(rotationIt);
  // Insert sequence into data set
  gdcm::DataElement rotationDEParent( gdcm::Tag(0x54, 0x52) );
  rotationDEParent.SetVR(gdcm::VR::SQ);
  rotationDEParent.SetValue(*rotationSq);
  rotationDEParent.SetVLToUndefined();

  //Write energy
  gdcm::DataElement energyDEParent( gdcm::Tag(0x54, 0x12) );
  energyDEParent.SetVR(gdcm::VR::SQ);
  // Create a Sequence
  gdcm::SmartPointer<gdcm::SequenceOfItems> energySq = new gdcm::SequenceOfItems();
  energySq->SetLengthToUndefined();
  for (unsigned int i=0; i<energyNumber; ++i) {
    gdcm::SmartPointer<gdcm::SequenceOfItems> energyThresholdSq = new gdcm::SequenceOfItems();
    energyThresholdSq->SetLengthToUndefined();
    // Create a dataelement
    gdcm::DataElement energyThresholdDE( gdcm::Tag(0x54, 0x14) );
    gdcm::DataElement energyUpholdDE( gdcm::Tag(0x54, 0x15) );
    energyThresholdDE.SetVR( gdcm::VR::DS );
    energyUpholdDE.SetVR( gdcm::VR::DS );
    energyThresholdDE.SetByteValue(p_EnergyWindowThreshold[i], (uint32_t)strlen(p_EnergyWindowThreshold[i]));
    energyUpholdDE.SetByteValue(p_EnergyWindowUphold[i], (uint32_t)strlen(p_EnergyWindowUphold[i]));
    // Create an item
    gdcm::Item energyThresholdIt;
    energyThresholdIt.SetVLToUndefined();
    gdcm::DataSet &energyThresholdDS = energyThresholdIt.GetNestedDataSet();
    energyThresholdDS.Insert(energyThresholdDE);
    energyThresholdDS.Insert(energyUpholdDE);
    energyThresholdSq->AddItem(energyThresholdIt);
    // Insert sequence into data set
    gdcm::DataElement energyThresholdDEParent( gdcm::Tag(0x54, 0x13) );
    energyThresholdDEParent.SetVR(gdcm::VR::SQ);
    energyThresholdDEParent.SetValue(*energyThresholdSq);
    energyThresholdDEParent.SetVLToUndefined();
    // Create a dataelement
    gdcm::DataElement energyNameDE( gdcm::Tag(0x54, 0x18) );
    energyNameDE.SetVR( gdcm::VR::SH );
    energyNameDE.SetByteValue(p_EnergyWindowName[i], (uint32_t)strlen(p_EnergyWindowName[i]));
    // Create an item
    gdcm::Item energyIt;
    energyIt.SetVLToUndefined();
    gdcm::DataSet &energyDS = energyIt.GetNestedDataSet();
    energyDS.Insert(energyNameDE);
    energyDS.Insert(energyThresholdDEParent);
    energySq->AddItem(energyIt);
  }
  // Insert sequence into data set
  energyDEParent.SetValue(*energySq);
  energyDEParent.SetVLToUndefined();
  ds2.Insert(energyDEParent);
  ds2.Insert(rotationDEParent);

  gdcm::Writer w;
  w.SetFile( file );
  w.SetFileName( fileNamesOutput[0].c_str() );
  w.Write();

#else
  std::cout << "Use GDCM2" << std::endl;
#endif
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template <typename T> std::string NumberToString ( T Number )
{
   std::ostringstream ss;
   ss << Number;
   return ss.str();
}
//---------------------------------------------------------------------------

}//end clitk

#endif //#define clitkGateSimulation2DicomGenericFilter_txx
