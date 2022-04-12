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
#ifndef clitkPartitionEnergyWindowDicomGenericFilter_txx
#define clitkPartitionEnergyWindowDicomGenericFilter_txx

/* =================================================
 * @file   clitkPartitionEnergyWindowDicomGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

#include <sstream>
// clitk
#include "clitkResampleImageWithOptionsFilter.h"
#include "gdcmUIDGenerator.h"
#include <gdcmImageHelper.h>
#include <gdcmAttribute.h>
#include <gdcmReader.h>
#include <gdcmWriter.h>
#include <gdcmDataElement.h>
#include <gdcmTag.h>

#include "itkImageRegionIterator.h"
#include "itkMetaImageIO.h"
#include "itkMetaDataDictionary.h"


namespace clitk
{


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
PartitionEnergyWindowDicomGenericFilter<args_info_type>::PartitionEnergyWindowDicomGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void PartitionEnergyWindowDicomGenericFilter<args_info_type>::Update()
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
PartitionEnergyWindowDicomGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
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
PartitionEnergyWindowDicomGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{

#if GDCM_MAJOR_VERSION >= 2
  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;
  typedef itk::ImageSeriesReader< InputImageType >     ReaderType;
  typedef itk::ImageSeriesWriter< InputImageType, InputImageType >     WriterType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef typename InputImageType::RegionType RegionType;
  typedef typename RegionType::SizeType SizeType;


  // Read Dicom model file
  typename ReaderType::Pointer reader = ReaderType::New();
  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  gdcmIO->LoadPrivateTagsOn();
  gdcmIO->KeepOriginalUIDOn();
  reader->SetImageIO( gdcmIO );
  typename ReaderType::FileNamesContainer fileNamesInput;
  fileNamesInput.push_back(m_ArgsInfo.input_arg);
  reader->SetFileNames( fileNamesInput );
  try {
    reader->Update();
  } catch (itk::ExceptionObject &excp) {
    std::cerr << "Error: Exception thrown while reading the DICOM model file !!" << std::endl;
    std::cerr << excp << std::endl;
  }
  typename InputImageType::Pointer input = reader->GetOutput();

  typename ReaderType::DictionaryRawPointer inputDict = (*(reader->GetMetaDataDictionaryArray()))[0];
  typename ReaderType::DictionaryArrayType outputArray;
  typename ReaderType::DictionaryRawPointer outputDict = new typename ReaderType::DictionaryType;
  CopyDictionary (*inputDict, *outputDict);
  itk::EncapsulateMetaData<std::string>(*outputDict, "0054|0011", NumberToString(1));
  outputArray.push_back(outputDict);


  //Read the number of slices and energy window, rotation
  int nbSlice = input->GetLargestPossibleRegion().GetSize()[2];
  gdcm::Reader hreader;
  hreader.SetFileName(m_ArgsInfo.input_arg);
  hreader.Read();
  gdcm::DataSet& dsInput = hreader.GetFile().GetDataSet();
  gdcm::Attribute<0x54,0x11> series_number_att;
  series_number_att.SetFromDataSet(dsInput);
  int nbEnergyWindow = series_number_att.GetValue();

  int nbRotation;
  if (dsInput.FindDataElement(gdcm::Tag(0x54, 0x52))) {
    const gdcm::DataElement &seqRotation = dsInput.GetDataElement(gdcm::Tag(0x54, 0x52));
    gdcm::SmartPointer<gdcm::SequenceOfItems> sqiRotation = seqRotation.GetValueAsSQ();
    gdcm::Item &itemRotation = sqiRotation->GetItem(1);
    const gdcm::DataElement &deRotation = itemRotation.GetDataElement(gdcm::Tag(0x54, 0x53));
    unsigned short SamplesPerPixelRotation = *((unsigned short *) deRotation.GetByteValue()->GetPointer());
    std::stringstream s_SamplesPerPixelRotation;
    s_SamplesPerPixelRotation << SamplesPerPixelRotation;
    std::string p_StrRotation = s_SamplesPerPixelRotation.str();
    nbRotation = (int)atof(p_StrRotation.c_str());
  } else
    nbRotation = 1;

  int nbSlicePerEnergy = nbSlice / nbEnergyWindow;
  if (nbSlicePerEnergy*nbEnergyWindow != nbSlice)
    std::cerr << "Error: The number of slices is not correct !!" << std::endl;

  for (unsigned int energy = 0; energy < nbEnergyWindow; ++energy) {
    
    //Create the output
    typename InputImageType::Pointer energyImage = InputImageType::New();
    energyImage->SetRegions(input->GetLargestPossibleRegion());
    typename InputImageType::IndexType startIndex;
    startIndex[0] = 0;
    startIndex[1] = 0;
    startIndex[2] = 0;//energy*nbSlicePerEnergy;
    typename InputImageType::SizeType regionSize;
    regionSize[0] = input->GetLargestPossibleRegion().GetSize()[0];
    regionSize[1] = input->GetLargestPossibleRegion().GetSize()[1];
    regionSize[2] = nbSlicePerEnergy;
    typename InputImageType::RegionType region;
    region.SetSize(regionSize);
    region.SetIndex(startIndex);
    energyImage->SetRegions(region);
    energyImage->Allocate();

    //Create the iterator on the output
    itk::ImageRegionIterator<InputImageType> imageOutputIterator(energyImage,energyImage->GetLargestPossibleRegion());

    //Create the iterator on the region of the input
    typename InputImageType::IndexType startIndexIterator;
    startIndexIterator[0] = 0;
    startIndexIterator[1] = 0;
    startIndexIterator[2] = energy*nbSlicePerEnergy;
    typename InputImageType::RegionType regionIterator;
    regionIterator.SetSize(regionSize);
    regionIterator.SetIndex(startIndexIterator);
    itk::ImageRegionIterator<InputImageType> imageInputIterator(input,regionIterator);

    //Copy the requested region
    while(!imageInputIterator.IsAtEnd())
    {
      imageOutputIterator.Set(imageInputIterator.Get());

      ++imageInputIterator;
      ++imageOutputIterator;
    }

    // Output directory and filenames
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetInput( energyImage );
    writer->SetImageIO( gdcmIO );
    typename ReaderType::FileNamesContainer fileNamesOutput;
    std::string extension = NumberToString(energy) + ".dcm";
    std::string filename_out = m_ArgsInfo.output_arg + extension;
    fileNamesOutput.push_back(filename_out);
    writer->SetFileNames( fileNamesOutput );
    writer->SetMetaDataDictionaryArray(&outputArray);

    // Write
    try {
      if (m_ArgsInfo.verbose_flag)
        std::cout << writer << std::endl;
      writer->Update();
    } catch( itk::ExceptionObject & excp ) {
      std::cerr << "Error: Exception thrown while writing the series!!" << std::endl;
      std::cerr << excp << std::endl;
    }

    //Write sequence dicom tag with gdcm
    gdcm::Reader reader;
    reader.SetFileName( filename_out.c_str() );
    reader.Read();
    gdcm::File &fileOutput = reader.GetFile();
    gdcm::DataSet &dsOutput = fileOutput.GetDataSet();
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
    char essai = (char)nbRotation;
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

    //Read the caracteristics of this energy window : the name and the thresholds up and down
    const gdcm::DataElement &seqWindow = dsInput.GetDataElement(gdcm::Tag(0x54, 0x12));
    gdcm::SmartPointer<gdcm::SequenceOfItems> sqiWindow = seqWindow.GetValueAsSQ();
    gdcm::Item &itemWindow = sqiWindow->GetItem(energy+1);
    const gdcm::DataElement &deNameWindow = itemWindow.GetDataElement(gdcm::Tag(0x54, 0x18));
    std::string nameWindow( deNameWindow.GetByteValue()->GetPointer(), deNameWindow.GetByteValue()->GetLength() );

    const gdcm::DataElement &deThresholds = itemWindow.GetDataElement(gdcm::Tag(0x54, 0x13));
    gdcm::SmartPointer<gdcm::SequenceOfItems> sqiThresholds = deThresholds.GetValueAsSQ();
    for (unsigned int nbWindow=1; nbWindow <= sqiThresholds->GetNumberOfItems() ; ++nbWindow) {
      gdcm::Item &itemThresholds = sqiThresholds->GetItem(nbWindow);
      const gdcm::DataElement &deThresholdDown = itemThresholds.GetDataElement(gdcm::Tag(0x54, 0x14));
      std::string p_StrThresholdDown( deThresholdDown.GetByteValue()->GetPointer(), deThresholdDown.GetByteValue()->GetLength() );
      double thresholdDown = (double)atof(p_StrThresholdDown.c_str());
      const gdcm::DataElement &deThresholdUp = itemThresholds.GetDataElement(gdcm::Tag(0x54, 0x15));
      std::string p_StrThresholdUp( deThresholdUp.GetByteValue()->GetPointer(), deThresholdUp.GetByteValue()->GetLength() );
      double thresholdUp = (double)atof(p_StrThresholdUp.c_str());

      //Now write it !
      gdcm::SmartPointer<gdcm::SequenceOfItems> energyThresholdSq = new gdcm::SequenceOfItems();
      energyThresholdSq->SetLengthToUndefined();
      // Create a dataelement
      gdcm::DataElement energyThresholdDE( gdcm::Tag(0x54, 0x14) );
      gdcm::DataElement energyUpholdDE( gdcm::Tag(0x54, 0x15) );
      energyThresholdDE.SetVR( gdcm::VR::DS );
      energyUpholdDE.SetVR( gdcm::VR::DS );
      energyThresholdDE.SetByteValue(NumberToString(thresholdDown).c_str(), (uint32_t)strlen(NumberToString(thresholdDown).c_str()));
      energyUpholdDE.SetByteValue(NumberToString(thresholdUp).c_str(), (uint32_t)strlen(NumberToString(thresholdUp).c_str()));
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
      energyNameDE.SetByteValue(nameWindow.c_str(), (uint32_t)strlen(nameWindow.c_str()));
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
    dsOutput.Insert(energyDEParent);
    dsOutput.Insert(rotationDEParent);
    gdcm::Writer w;
    w.SetFile( fileOutput );
    w.SetFileName( filename_out.c_str() );
    w.Write();
  }
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

#endif //#define clitkPartitionEnergyWindowDicomGenericFilter_txx
