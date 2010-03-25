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
#ifndef CLITKIOCOMMON_TXX
#define CLITKIOCOMMON_TXX
/**
   =================================================
   * @file   clitkIOCommon.txx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   04 Jul 2006 08:34:11
   * 
   * @brief  
   * 
   * 
   =================================================*/

//====================================================================
// To short the code for reading an image
template<typename ImageType>
typename ImageType::Pointer readImage(const std::string & filename, const bool verbose) {
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.c_str());
  if (verbose) {
    std::cout << "Reading " << filename << " ..." << std::endl;
  }
  try {
    reader->Update(); 
  }
  catch( itk::ExceptionObject & err ) {
    std::cerr << "Error while reading " << filename 
	      << " " << err << std::endl;
    exit(0);
  }
  return reader->GetOutput();
}
//====================================================================

//====================================================================
// To short the code for reading an image from several files
template<typename ImageType>
typename ImageType::Pointer readImage(const std::vector<std::string> & filenames, 
				      const bool verbose) {
  if (filenames.size() == 1) return readImage<ImageType>(filenames[0], verbose);
  typedef itk::ImageSeriesReader<ImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileNames(filenames);
  if (verbose) {
    std::cout << "Reading " << filenames[0] << " and others ..." << std::endl;
  }
  try {
    reader->Update(); 
  }
  catch( itk::ExceptionObject & err ) {
    std::cerr << "Error while reading " << filenames[0]
	      << " or other files ..." << err << std::endl;
    exit(0);
  }
  return reader->GetOutput();
}
//====================================================================

//====================================================================
// To short the code for writing an image
template<typename ImageType>
void writeImage(const typename ImageType::Pointer image, 
		const std::string & filename,  
		const bool verbose) {
  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(filename.c_str());
  writer->SetInput(image);
  if (verbose) {
    std::cout << "Writing " << filename << "." << std::endl;
  }
  try { 
    writer->Update(); 
  }
  catch( itk::ExceptionObject & err ) {
    std::cerr << "Error while writing " << filename 
	      << ", the error is : " << err << std::endl;
    exit(0);
  }
}
//====================================================================

#endif /* end #define CLITKIOCOMMON_TXX */

