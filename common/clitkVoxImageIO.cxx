
/*-------------------------------------------------------------------------
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkVoxImageIO.cxx,v $
  Language:  C++
  Date:      $Date: 2010/01/06 13:32:01 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  -------------------------------------------------------------------------*/


#ifndef CLITKVOXIMAGEIO_CXX
#define CLITKVOXIMAGEIO_CXX

/**
   -------------------------------------------------
   * @file   clitkVoxImageIO.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   17 May 2006 08:03:07
   * 
   * @brief  
   * 
   * 
   -------------------------------------------------*/

// clitk include
#include "clitkVoxImageIO.h"
#include "clitkCommon.h"

// itk include (for itkReadRawBytesAfterSwappingMacro)
#include "itkRawImageIO.h"

//--------------------------------------------------------------------
// Read Image Information
void clitk::VoxImageIO::ReadImageInformation() {
  // open file
  std::ifstream is;
  clitk::openFileForReading(is, m_FileName);  
  // read magic number
  std::string mn; 
  is >> mn; 
  //DD(mn);
  if (mn != "VOX") {
    itkExceptionMacro(<<"read magic number '" << mn << "' while expect VOX");
  }	
  // read vox file version
  skipComment(is); 
  is >> mn; 
  //DD(mn);
  if (mn != "v2") {
    itkExceptionMacro(<<"read old format '" << mn << "'. TODO");
  }  
	
  // ONLY 3D IMAGES YET ...

  // read grid size/spacing
  itk::Vector<unsigned int,3> dim;
  itk::Vector<double,3> spacing;
  itk::Vector<double,3> origin;
  origin.Fill(0.0);
  skipComment(is); 
  is >> dim[0]; 
  is >> dim[1]; 
  is >> dim[2];
  //DD(dim);
  skipComment(is); 
  is >> spacing[0];
  is >> spacing[1];
  is >> spacing[2];
  //DD(spacing);
  skipComment(is);
  int d; 
  is >> d;
  if (d != 3 && d != 2) {
    itkExceptionMacro(<<"could not read no " << d << "D image (only 2D and 3D). TODO");
  }
  // read data type
  skipComment(is);
  std::string dataTypeName; 
  is >> dataTypeName;
  //DD(dataTypeName);

  // get header size
  m_HeaderSize = is.tellg();
  m_HeaderSize++;
  //DD(m_HeaderSize);

  // set dimension values
  SetNumberOfDimensions(d);
  for(int i=0; i<d; i++) {
    SetDimensions(i,dim[i]);
    SetSpacing(i,spacing[i]);
    SetOrigin(i,origin[i]);
  }

  // set other information
  SetByteOrderToLittleEndian();
  SetPixelType(itk::ImageIOBase::SCALAR);
  SetNumberOfComponents(1);  

  if (dataTypeName == "char") SetComponentType(itk::ImageIOBase::CHAR);
  else if (dataTypeName == "schar") SetComponentType(itk::ImageIOBase::CHAR);
  else if (dataTypeName == "uchar" || dataTypeName == "unsigned_char") SetComponentType(itk::ImageIOBase::UCHAR);
  else if (dataTypeName == "sshort") SetComponentType(itk::ImageIOBase::SHORT);
  else if (dataTypeName == "ushort") SetComponentType(itk::ImageIOBase::USHORT);
  else if (dataTypeName == "int") SetComponentType(itk::ImageIOBase::INT);
  else if (dataTypeName == "sint") SetComponentType(itk::ImageIOBase::INT);
  else if (dataTypeName == "uint") SetComponentType(itk::ImageIOBase::UINT);
  else if (dataTypeName == "float") SetComponentType(itk::ImageIOBase::FLOAT);
  else if (dataTypeName == "double") SetComponentType(itk::ImageIOBase::DOUBLE);
  else {
    itkExceptionMacro(<<"Read failed: Wanted pixel type " 
		      << "(char, uchar, short, ushort, int, uint, float, double)" 
		      << " but read " << dataTypeName);
  }

} ////

//--------------------------------------------------------------------
// Read Image Information
bool clitk::VoxImageIO::CanReadFile(const char* FileNameToRead) 
{
  std::string filename(FileNameToRead);
  std::string filenameext = GetExtension(filename);
  if (filenameext != std::string("vox")) return false;
  return true;
} ////

//--------------------------------------------------------------------
// Read Image Content
void clitk::VoxImageIO::Read(void * buffer) 
{
  // Adapted from itkRawImageIO

  std::ifstream file;
  openFileForReading(file, m_FileName);

  // Offset into file
  unsigned long streamStart = m_HeaderSize;
  file.seekg((long)streamStart, std::ios::beg);
  if ( file.fail() ) {
    itkExceptionMacro(<<"File seek failed (Vox Read)");
  }
	
  unsigned long numberOfBytesToBeRead = GetComponentSize();
  for(unsigned int i=0; i<GetNumberOfDimensions(); i++) numberOfBytesToBeRead *= GetDimensions(i);

  //DD(numberOfBytesToBeRead);
	
  if(!this->ReadBufferAsBinary(file, buffer, numberOfBytesToBeRead)) {
    itkExceptionMacro(<<"Read failed: Wanted " 
		      << numberOfBytesToBeRead
		      << " bytes, but read " 
		      << file.gcount() << " bytes.");
  }
  itkDebugMacro(<< "Reading Done");

  {
    using namespace itk;
    // Swap bytes if necessary
    if itkReadRawBytesAfterSwappingMacro( unsigned short, USHORT ) 
      else if itkReadRawBytesAfterSwappingMacro( short, SHORT ) 
	else if itkReadRawBytesAfterSwappingMacro( char, CHAR ) 
	  else if itkReadRawBytesAfterSwappingMacro( unsigned char, UCHAR ) 
	    else if itkReadRawBytesAfterSwappingMacro( unsigned int, UINT ) 
	      else if itkReadRawBytesAfterSwappingMacro( int, INT ) 
		else if itkReadRawBytesAfterSwappingMacro( float, FLOAT ) 
		  else if itkReadRawBytesAfterSwappingMacro( double, DOUBLE );
  }
}

//--------------------------------------------------------------------
// Write Image Information
void clitk::VoxImageIO::WriteImageInformation(bool keepOfStream)
{
  // Check dimension
  if (GetNumberOfDimensions() != 3 &&  GetNumberOfDimensions() != 2) {
    itkExceptionMacro(<<"Write failed: only 3D and 2D image for Vox file format yet.");
  }

  // Open the file
  clitk::openFileForWriting(file, m_FileName);
  // write magic number
  file << "VOX v2" << std::endl;
  // write grid size/spacing
  file << "# Size" << std::endl;
  file << GetDimensions(0) << " " 
       << GetDimensions(1) << " " 
       << GetDimensions(2) << std::endl;
  file << "# Spacing" << std::endl;
  file.precision(40);  
  file << GetSpacing(0) << " " 
       << GetSpacing(1) << " " 
       << GetSpacing(2) << std::endl;
  file << "# Image dim" << std::endl << "3" << std::endl;
  file << "# Image type" << std::endl;

  std::string dataTypeName;
  if (GetComponentType() == itk::ImageIOBase::CHAR) dataTypeName = "schar";
  else if (GetComponentType() == itk::ImageIOBase::UCHAR) dataTypeName = "uchar";
  else if (GetComponentType() == itk::ImageIOBase::SHORT) dataTypeName = "sshort";
  else if (GetComponentType() == itk::ImageIOBase::USHORT) dataTypeName = "ushort";
  else if (GetComponentType() == itk::ImageIOBase::INT) dataTypeName = "int";
  else if (GetComponentType() == itk::ImageIOBase::UINT) dataTypeName = "uint";
  else if (GetComponentType() == itk::ImageIOBase::FLOAT) dataTypeName = "float";
  else if (GetComponentType() == itk::ImageIOBase::DOUBLE) dataTypeName = "double";
  else {
    itkExceptionMacro(<<"Write failed: Wanted pixel type " 
		      << "(char, uchar, short, ushort, int, uint, float, double)" 
		      << " but Vox is : " << dataTypeName);
  }
  file << dataTypeName << std::endl;

  // close file
  if (!keepOfStream) file.close();	
}
  
//--------------------------------------------------------------------
// Write Image Information
bool clitk::VoxImageIO::CanWriteFile(const char* FileNameToWrite)
{
  std::string filename(FileNameToWrite);
  std::string filenameext = GetExtension(filename);
  if (filenameext != std::string("vox")) return false;
  return true;
}

//--------------------------------------------------------------------
// Write Image
void clitk::VoxImageIO::Write(const void * buffer) 
{
  clitk::VoxImageIO::WriteImageInformation(true);
  SetByteOrderToLittleEndian();

  //-------------------------------------------
  // Cut & Paste from itkRawImageIO
  // (warning BigEndian / LittleEndian)
  const unsigned long numberOfBytes      = this->GetImageSizeInBytes();
  const unsigned long numberOfComponents = this->GetImageSizeInComponents();
  // Swap bytes if necessary
  using namespace itk;
  if itkWriteRawBytesAfterSwappingMacro( unsigned short, USHORT ) 
    else if itkWriteRawBytesAfterSwappingMacro( short, SHORT ) 
      else if itkWriteRawBytesAfterSwappingMacro( char, CHAR ) 
	else if itkWriteRawBytesAfterSwappingMacro( unsigned char, UCHAR ) 
	  else if itkWriteRawBytesAfterSwappingMacro( unsigned int, UINT ) 
	    else if itkWriteRawBytesAfterSwappingMacro( int, INT ) 
	      else if itkWriteRawBytesAfterSwappingMacro( float, FLOAT ) 
		else if itkWriteRawBytesAfterSwappingMacro( double, DOUBLE ) ;
  //-------------------------------------------
	  
  file.close();
} ////

#endif /* end #define CLITKVOXIMAGEIO_CXX */

