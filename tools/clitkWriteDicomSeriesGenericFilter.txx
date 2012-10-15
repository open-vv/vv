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
#ifndef clitkWriteDicomSeriesGenericFilter_txx
#define clitkWriteDicomSeriesGenericFilter_txx

/* =================================================
 * @file   clitkWriteDicomSeriesGenericFilter.txx
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


namespace clitk
{


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
WriteDicomSeriesGenericFilter<args_info_type>::WriteDicomSeriesGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void WriteDicomSeriesGenericFilter<args_info_type>::Update()
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
WriteDicomSeriesGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
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
WriteDicomSeriesGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{

  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, 2> OutputImageType;

  // Read the dicom directory
  typedef itk::ImageSeriesReader< InputImageType >     ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames NamesGeneratorType;

  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
  namesGenerator->SetInputDirectory( m_ArgsInfo.inputDir_arg );
  namesGenerator->SetOutputDirectory( m_ArgsInfo.outputDir_arg  );
  typename   ReaderType::FileNamesContainer filenames_in = namesGenerator->GetInputFileNames();
  typename   ReaderType::FileNamesContainer filenames_out;

  // Output the dicom files
  unsigned int numberOfFilenames =  filenames_in.size();
  if (m_Verbose) {
    std::cout << numberOfFilenames <<" were read in the directory "<<m_ArgsInfo.inputDir_arg<<"..."<<std::endl<<std::endl;
    for(unsigned int fni = 0; fni<numberOfFilenames; fni++) {
      std::cout << "filename # " << fni << " = ";
      std::cout << filenames_in[fni] << std::endl;
    }
  }

  // Read the series
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO( gdcmIO );
  reader->SetFileNames( filenames_in );
  try {
    reader->Update();
  } catch (itk::ExceptionObject &excp) {
    std::cerr << "Error: Exception thrown while reading the DICOM series!!" << std::endl;
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
  if ((!m_ArgsInfo.useSizeAsReference_flag && (input->GetSpacing() != reader->GetOutput()->GetSpacing())) || 
      (m_ArgsInfo.useSizeAsReference_flag && (input->GetLargestPossibleRegion().GetSize() != reader->GetOutput()->GetLargestPossibleRegion().GetSize()))) {
        
    // resampling is carried out on the fly if resolution or size between 
    // the input mhd and input dicom series is different
    
    // Filter
    typedef clitk::ResampleImageWithOptionsFilter<InputImageType, InputImageType> ResampleImageFilterType;
    typename ResampleImageFilterType::Pointer filter = ResampleImageFilterType::New();
    filter->SetInput(input);
    filter->SetVerboseOptions(m_Verbose);
    filter->SetGaussianFilteringEnabled(false);
    filter->SetDefaultPixelValue(0);
    
    if (!m_ArgsInfo.useSizeAsReference_flag) {
      filter->SetOutputSpacing(reader->GetOutput()->GetSpacing());
      if (m_Verbose) {
        std::cout << "Warning: The image spacing differs between the MHD file and the input dicom series. Performing resampling with default options using spacing as reference (for advanced options, use clitkResampleImage)." << std::endl;
        std::cout << "MHD -> " << input->GetSpacing() << std::endl;
        std::cout << "dicom -> " << reader->GetOutput()->GetSpacing() << std::endl;
      }
    }
    else {
      const SizeType& dicom_size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
      SizeType output_size;
      for (unsigned int i = 0; i < Dimension; i++)
        output_size[i] = dicom_size[i];
      filter->SetOutputSize(output_size);
      if (m_Verbose) {
          std::cout << "Warning: The image size differs between the MHD file and the input dicom series. Performing resampling with default options using size as reference (for advanced options, use clitkResampleImage)." << std::endl;
          std::cout << "MHD -> " << input->GetLargestPossibleRegion().GetSize() << std::endl;
          std::cout << "dicom -> " << reader->GetOutput()->GetLargestPossibleRegion().GetSize() << std::endl;
      }
    }

    try {
      filter->Update();
      input = filter->GetOutput();
    } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error: Exception thrown while resampling!!" << std::endl;
    std::cerr << excp << std::endl;
    }
  }
  
  //	In some cases, due to resampling approximation issues, 
  //	the number of slices in the MHD file may be different (smaller)
  //	from the number of files in the template dicom directory. 
  //	To avoid ITK generating an exception, we reduce the number 
  //	of DCM files to be considered, and a warning is printed
  //	in verbose mode
  const RegionType volumeRegion = input->GetLargestPossibleRegion();
  const SizeType& volumeSize = volumeRegion.GetSize();
  if (m_Verbose) {
    std::cout << volumeRegion << volumeSize << std::endl;
  }
  if (Dimension == 3 && volumeSize[2] < numberOfFilenames) {
    if (m_Verbose)
      std::cout << "Warning: The number of files in " << m_ArgsInfo.inputDir_arg << " (" << filenames_in.size() << " files) is greater than the number of slices in MHD (" << volumeSize[2] << " slices). Using only " << volumeSize[2] << " files." << std::endl;
    
    filenames_in.resize(volumeSize[2]);
    filenames_out.resize(filenames_in.size());
    numberOfFilenames =  filenames_in.size();
  }

  // Modify the meta dictionary
  typedef itk::MetaDataDictionary   DictionaryType;
  const std::vector<DictionaryType*>* dictionary = reader->GetMetaDataDictionaryArray();

  // Get keys
  unsigned int numberOfKeysGiven=m_ArgsInfo.key_given;
    if (m_ArgsInfo.verbose_flag) 
      DD(numberOfKeysGiven);

  std::string seriesUID;
  std::string frameOfReferenceUID;
  std::string studyUID;
  
  // one pass through the keys given on the cmd-line, to check what will be recreated
  std::string seriesUIDkey = "0020|000e";
  std::string seriesNumberKey = "0020|0011";
  std::string seriesDescriptionKey = "0008|103e";
  std::string frameOfReferenceUIDKey = "0020|0052";
  std::string studyUIDKey = "0020|000d";
  std::string studyIDKey = "0020|0010";
  std::string studyDescriptionKey = "0008|1030";
  bool seriesUIDGiven = false;
  bool seriesNumberGiven = false;
  bool seriesDescriptionGiven = false;
  bool studyUIDGiven = false;
  bool studyIDGiven = false;
  bool studyDescriptionGiven = false;
  for (unsigned int i = 0; i < numberOfKeysGiven; i++) {
    std::string entryId( m_ArgsInfo.key_arg[i] );
    if (m_ArgsInfo.verbose_flag) 
      DD(entryId);
    
    seriesUIDGiven |= (entryId ==  seriesUIDkey || entryId ==  frameOfReferenceUIDKey);
    seriesNumberGiven |= (entryId == seriesNumberKey);
    seriesDescriptionGiven |= (entryId == seriesDescriptionKey);
    studyUIDGiven |= (entryId == studyUIDKey);
    studyIDGiven |= (entryId == studyIDKey);
    studyDescriptionGiven |= (entryId == studyDescriptionKey);
  }

  // force the creation of a new series if a new study was specified
  if (!studyUIDGiven && m_ArgsInfo.newStudyUID_flag) {
    m_ArgsInfo.newSeriesUID_flag = true;
#if GDCM_MAJOR_VERSION >= 2
    gdcm::UIDGenerator suid;
    studyUID = suid.Generate();
#else
    studyUID = gdcm::Util::CreateUniqueUID( gdcmIO->GetUIDPrefix());
#endif
  }
    
  if (!seriesUIDGiven && m_ArgsInfo.newSeriesUID_flag) {
#if GDCM_MAJOR_VERSION >= 2
    gdcm::UIDGenerator suid;
    seriesUID = suid.Generate();
    gdcm::UIDGenerator fuid;
    frameOfReferenceUID = fuid.Generate();
#else
    seriesUID = gdcm::Util::CreateUniqueUID( gdcmIO->GetUIDPrefix());
    frameOfReferenceUID = gdcm::Util::CreateUniqueUID( gdcmIO->GetUIDPrefix());
#endif
  }

  if (m_ArgsInfo.verbose_flag) {
    DD(seriesUID);
    DD(frameOfReferenceUID);
    DD(studyUID);
  }

  // check if file UIDs will be be preserved
  bool useInputFileUID = true;
  if (m_ArgsInfo.newSeriesUID_flag || m_ArgsInfo.newStudyUID_flag || seriesUIDGiven || studyUIDGiven) {
    useInputFileUID = false;
  }
  else {
#if GDCM_MAJOR_VERSION < 2
    gdcmIO->SetKeepOriginalUID(true);
#endif
    namesGenerator->SetOutputDirectory( m_ArgsInfo.outputDir_arg  );
    filenames_out = namesGenerator->GetOutputFileNames();
  }
  
  filenames_out.resize(numberOfFilenames);
  
  time_t t;
  t = time(&t);
  struct tm* instanceDateTimeTm = localtime(&t);
  char datetime[16];
  strftime(datetime, 16, "%Y%m%d", instanceDateTimeTm);
  std::ostringstream instanceDate;
  instanceDate << datetime;
  std::ostringstream instanceTime;
  strftime(datetime, 16, "%H%M%S", instanceDateTimeTm);
  instanceTime << datetime;
  
  // update output dicom keys/tags
  for(unsigned int fni = 0; fni<numberOfFilenames; fni++) {
    for (unsigned int i = 0; i < numberOfKeysGiven; i++) {
      std::string entryId(m_ArgsInfo.key_arg[i]  );
      std::string value( m_ArgsInfo.tag_arg[i] );

      itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), entryId, value );
    }

    // series UID
    if (!seriesUIDGiven) {
      if (m_ArgsInfo.newSeriesUID_flag) {
        itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), seriesUIDkey, seriesUID );
        itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), frameOfReferenceUIDKey, frameOfReferenceUID );
      }
    }
    
    // study UID
    if (!studyUIDGiven) {
      if (m_ArgsInfo.newStudyUID_flag) 
        itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), studyUIDKey, studyUID );
    }
    
    // study description
    if (studyUIDGiven || m_ArgsInfo.newStudyUID_flag) {
      if (!studyIDGiven)
        itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), studyIDKey,itksys::SystemTools::GetFilenameName( m_ArgsInfo.outputDir_arg ));
      if (!studyDescriptionGiven)
        itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), studyDescriptionKey,itksys::SystemTools::GetFilenameName( m_ArgsInfo.outputDir_arg ));
      
      itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), "0008|0020", instanceDate.str() );
      itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), "0008|0030", instanceTime.str() );
    }
    
    // series description/number
    if (seriesUIDGiven || m_ArgsInfo.newSeriesUID_flag) {
      if (!seriesDescriptionGiven)
        itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), seriesDescriptionKey, itksys::SystemTools::GetFilenameName(m_ArgsInfo.outputDir_arg) );
      if (!seriesNumberGiven)
        itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), seriesNumberKey, itksys::SystemTools::GetFilenameName(m_ArgsInfo.outputDir_arg) );

      itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), "0008|0012", instanceDate.str() );
      itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), "0008|0013", instanceTime.str() );
    }

    // file UIDs are recreated for new studies or series
    if (!useInputFileUID)
    {
      std::string fileUID;
#if GDCM_MAJOR_VERSION >= 2
      gdcm::UIDGenerator fid;
      fileUID = fid.Generate();
#else
      fileUID = gdcm::Util::CreateUniqueUID( gdcmIO->GetUIDPrefix());
#endif
      itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), "0008|0018", fileUID );
      itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), "0002|0003", fileUID );
      
      filenames_out[fni] = itksys::SystemTools::CollapseFullPath(fileUID.c_str(), m_ArgsInfo.outputDir_arg) + std::string(".dcm"); 
    }
  }
  
  // Output directory and filenames
  itksys::SystemTools::MakeDirectory( m_ArgsInfo.outputDir_arg ); // create if it doesn't exist
  typedef itk::ImageSeriesWriter<InputImageType, OutputImageType >  SeriesWriterType;
  typename SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();

  seriesWriter->SetInput( input );
  seriesWriter->SetImageIO( gdcmIO );
  
  seriesWriter->SetFileNames( filenames_out );
  seriesWriter->SetMetaDataDictionaryArray( dictionary );

  // Write
  try {
    seriesWriter->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error: Exception thrown while writing the series!!" << std::endl;
    std::cerr << excp << std::endl;
  }

}

/*
//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//-------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
WriteDicomSeriesGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{

  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, 2> OutputImageType;

  // Read the input (volumetric)
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer volumeReader = InputReaderType::New();
  volumeReader->SetFileName( m_InputFileName);
  volumeReader->Update();
  typename InputImageType::Pointer input= volumeReader->GetOutput();

  // Read the dicom directory
  typedef itk::ImageSeriesReader< InputImageType >     ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames NamesGeneratorType;

  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
  namesGenerator->SetInputDirectory( m_ArgsInfo.inputDir_arg );
  namesGenerator->SetOutputDirectory( m_ArgsInfo.outputDir_arg  );
  typename   ReaderType::FileNamesContainer filenames_in = namesGenerator->GetInputFileNames();
  typename   ReaderType::FileNamesContainer filenames_out = namesGenerator->GetOutputFileNames();

  // Output the dicom files
  unsigned int numberOfFilenames =  filenames_in.size();
  if (m_Verbose) {
    std::cout << numberOfFilenames <<" were read in the directory "<<m_ArgsInfo.inputDir_arg<<"..."<<std::endl<<std::endl;
    for(unsigned int fni = 0; fni<numberOfFilenames; fni++) {
      std::cout << "filename # " << fni << " = ";
      std::cout << filenames_in[fni] << std::endl;
    }
  }
  
  // RP: 16/03/2011
  //	In some cases, due to resampling approximation issues, 
  //	the number of slices in the MHD file may be different 
  //	from the number of slices in the template DCM directory. 
  //	To avoid ITK generating an exception, we reduce the number 
  //	of DCM files to be considered, provided the --force
  //	option is set.
  typedef typename InputImageType::RegionType RegionType;
  typedef typename RegionType::SizeType SizeType;
  const RegionType volumeRegion = input->GetLargestPossibleRegion();
  const SizeType& volumeSize = volumeRegion.GetSize();
  if (m_ArgsInfo.force_given && Dimension == 3 && volumeSize[2] < numberOfFilenames)
  {
    std::cout << "Warning: Number of files in " << m_ArgsInfo.inputDir_arg << " is greater than the number of slices in MHD. Using only " << volumeSize[2] << " files." << std::endl;
    filenames_in.resize(volumeSize[2]);
    filenames_out.resize(filenames_in.size());
    numberOfFilenames =  filenames_in.size();
  }

  // Read the series
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO( gdcmIO );
  reader->SetFileNames( filenames_in );
  try {
    reader->Update();
  } catch (itk::ExceptionObject &excp) {
    std::cerr << "Error: Exception thrown while reading the DICOM series!!" << std::endl;
    std::cerr << excp << std::endl;
  }


  // Modify the meta dictionary
  typedef itk::MetaDataDictionary   DictionaryType;
  const std::vector<DictionaryType*>* dictionary = reader->GetMetaDataDictionaryArray();

  // Get keys
  unsigned int numberOfKeysGiven=0;
  if(m_ArgsInfo.midP_flag && m_ArgsInfo.key_given)
    std::cerr<<"Error: both keys and midP option are given"<<std::endl;
  else if (m_ArgsInfo.midP_flag)
    numberOfKeysGiven=1;
  else
    numberOfKeysGiven=m_ArgsInfo.key_given;

  for (unsigned int i = 0; i < numberOfKeysGiven; i++) {
    std::string entryId(m_ArgsInfo.key_arg[i]  );
    std::string value( m_ArgsInfo.tag_arg[i] );
    for(unsigned int fni = 0; fni<numberOfFilenames; fni++)
      itk::EncapsulateMetaData<std::string>( *((*dictionary)[fni]), entryId, value );
  }

  // Output directory and filenames
  itksys::SystemTools::MakeDirectory( m_ArgsInfo.outputDir_arg ); // create if it doesn't exist
  typedef itk::ImageSeriesWriter<InputImageType, OutputImageType >  SeriesWriterType;
  typename SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();

  seriesWriter->SetInput( volumeReader->GetOutput() );
  seriesWriter->SetImageIO( gdcmIO );
  
  seriesWriter->SetFileNames( filenames_out );
  seriesWriter->SetMetaDataDictionaryArray( dictionary );

  // Write
  try {
    seriesWriter->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error: Exception thrown while writing the series!!" << std::endl;
    std::cerr << excp << std::endl;
  }

}
*/

}//end clitk

#endif //#define clitkWriteDicomSeriesGenericFilter_txx
