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

  // ImageTypes
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;

  // Read the dicom directory
  typedef itk::ImageFileReader< InputImageType >     ReaderType;
  typedef itk::GDCMImageIO ImageIOType;
  typedef itk::GDCMSeriesFileNames NamesGeneratorType;

  ImageIOType::Pointer gdcmIO = ImageIOType::New();
  std::string filename_out = m_ArgsInfo.outputFilename_arg;

  // Output the dicom files
  unsigned int numberOfFilenames =  1;
  if (m_Verbose) {
    std::cout << numberOfFilenames <<" were read in the directory "<<m_ArgsInfo.inputModelFilename_arg<<"..."<<std::endl<<std::endl;
  }

  // Read the series
  typename ReaderType::Pointer reader = ReaderType::New();
#if GDCM_MAJOR_VERSION >= 2
  gdcmIO->LoadPrivateTagsOn();
  gdcmIO->KeepOriginalUIDOn();
#endif
  reader->SetImageIO( gdcmIO );
  reader->SetFileName( m_ArgsInfo.inputModelFilename_arg );
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
  if (input->GetLargestPossibleRegion().GetSize() != reader->GetOutput()->GetLargestPossibleRegion().GetSize()) {
        
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

#if GDCM_MAJOR_VERSION < 2
  gdcmIO->SetKeepOriginalUID(true);
#endif


  //Read the dicom file to find the energy, the head & the steps (TODO: do it on the mhd filetext)



/*
  // update output dicom keys/tags
  //std::string seriesUIDkey = "0020|000e";
  for (unsigned int i = 0; i < numberOfKeysGiven; i++) {
    std::string entryId(m_ArgsInfo.key_arg[i]  );
    std::string value( m_ArgsInfo.tag_arg[i] );

    itk::EncapsulateMetaData<std::string>(reader->GetMetaDataDictionary(), entryId, value );
  }
*/
  // Output directory and filenames
  //itksys::SystemTools::MakeDirectory( m_ArgsInfo.outputFilename_arg ); // create if it doesn't exist
  typedef itk::ImageFileWriter<OutputImageType>  SeriesWriterType;
  typename SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();

  seriesWriter->SetInput( input );
  seriesWriter->SetImageIO( gdcmIO );
  
  seriesWriter->SetFileName( filename_out );
  seriesWriter->SetMetaDataDictionary(reader->GetMetaDataDictionary());

  // Write
  try {
    if (m_ArgsInfo.verbose_flag)
      std::cout << seriesWriter << std::endl;
    seriesWriter->Update();
  } catch( itk::ExceptionObject & excp ) {
    std::cerr << "Error: Exception thrown while writing the series!!" << std::endl;
    std::cerr << excp << std::endl;
  }

}

}//end clitk

#endif //#define clitkGateSimulation2DicomGenericFilter_txx
