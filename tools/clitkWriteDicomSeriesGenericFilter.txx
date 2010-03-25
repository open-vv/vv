/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
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
    else 
      {
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

    if(PixelType == "short"){  
      if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, signed short>(); 
    }
    //    else if(PixelType == "unsigned_short"){  
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
    //     }
    
    else if (PixelType == "unsigned_char"){ 
      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, unsigned char>();
    }
    
    //     else if (PixelType == "char"){ 
    //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
    //       UpdateWithDimAndPixelType<Dimension, signed char>();
    //     }
    else {
      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
      UpdateWithDimAndPixelType<Dimension, float>();
    }
  }


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
    typename   ReaderType::FileNamesContainer filenames = namesGenerator->GetInputFileNames();

    // Output the dicom files
    unsigned int numberOfFilenames =  filenames.size();
    if (m_Verbose) 
      {
	std::cout << numberOfFilenames <<" were read in the directory "<<m_ArgsInfo.inputDir_arg<<"..."<<std::endl<<std::endl; 
	for(unsigned int fni = 0; fni<numberOfFilenames; fni++)
	  {
	    std::cout << "filename # " << fni << " = ";
	    std::cout << filenames[fni] << std::endl;
	  }
      }
    
    // Read the series
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetImageIO( gdcmIO );
    reader->SetFileNames( filenames );
    try
      {
	reader->Update();
      }
    catch (itk::ExceptionObject &excp)
      {
	std::cerr << "Error: Exception thrown while writing the DICOM series!!" << std::endl;
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
    
    for (unsigned int i = 0; i < numberOfKeysGiven; i++)
      {
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
    namesGenerator->SetOutputDirectory( m_ArgsInfo.outputDir_arg  );
    seriesWriter->SetFileNames( namesGenerator->GetOutputFileNames() );
    //seriesWriter->SetMetaDataDictionaryArray( dictionary );
    seriesWriter->SetMetaDataDictionaryArray( dictionary );   
    
    // Write   
    try
      {
	seriesWriter->Update();
      }
    catch( itk::ExceptionObject & excp )
      {
	std::cerr << "Error: Exception thrown while writing the series!!" << std::endl;
	std::cerr << excp << std::endl;
      }

  }


}//end clitk
 
#endif //#define clitkWriteDicomSeriesGenericFilter_txx
