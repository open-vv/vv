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
#ifndef CLITKVFIMAGEIO_CXX
#define CLITKVFIMAGEIO_CXX

/**
 * @file   clitkVfImageIO.cxx
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Mon Sep 18 10:14:53 2006
 * 
 * @brief  VectorField .vf I/O implementation
 * 
 * 
 */

// clitk include
#include "clitkVfImageIO.h"

// itk include (for itkReadRawBytesAfterSwappingMacro)
#include "itkRawImageIO.h"

//====================================================================
// Read Image Information
void clitk::VfImageIO::ReadImageInformation() 
{
  // open file
  std::ifstream is;
  clitk::openFileForReading(is, m_FileName);  
  // read magic number
  std::string mn; 
  is >> mn; 
  //DD(mn);
  if (mn != "IAMA3DVECTORFIELD") {
	itkExceptionMacro(<<"read magic number '" << mn << "' while expect IAMA3DVECTORFIELD");
  }	
  // read vf file version
  skipComment(is); 
  is >> mn; 
  //DD(mn);
  if (mn != "V2") {
	itkExceptionMacro(<<"read old format '" << mn << "'. TODO");
  }  
	
  // read grid size/spacing
  itk::Vector<unsigned int,3> dim;
  itk::Vector<double,3> spacing;
  itk::Vector<double,3> origin;
  origin.Fill(0.0);
  skipComment(is); 
  is >> dim[0]; 
  is >> dim[1]; 
  is >> dim[2];
  // DD(dim);
  is >> spacing[0];
  is >> spacing[1];
  is >> spacing[2];
  // DD(spacing);
    
  // get header size
  m_HeaderSize = is.tellg();
  m_HeaderSize+=2;

  // set dimension values
  SetNumberOfDimensions(3);
  for(unsigned int i=0; i<3; i++) {
	SetDimensions(i,dim[i]);
	SetSpacing(i,spacing[i]);
	SetOrigin(i,origin[i]);
  }

  // set other information
  SetByteOrderToLittleEndian();
  SetPixelType(itk::ImageIOBase::VECTOR);
  SetNumberOfComponents(3);  
  SetComponentType(itk::ImageIOBase::FLOAT);
} ////

//====================================================================
// Read Image Information
bool clitk::VfImageIO::CanReadFile(const char* FileNameToRead) 
{
  std::string filename(FileNameToRead);
  std::string filenameext = GetExtension(filename);
  if (filenameext != std::string("vf")) return false;
  return true;
} ////

//====================================================================
// Read Image Content
void clitk::VfImageIO::Read(void * buffer) 
{
  // Adapted from itkRawImageIO

  std::ifstream file;
  openFileForReading(file, m_FileName);

  // Offset into file
  unsigned long streamStart = m_HeaderSize;
  file.seekg((long)streamStart, std::ios::beg);
  if ( file.fail() ) {
	itkExceptionMacro(<<"File seek failed (Vf Read)");
  }
	
  float * tmpBuff = new float[GetImageSizeInComponents()];
  if(!this->ReadBufferAsBinary(file, tmpBuff, GetImageSizeInBytes())) {
 	itkExceptionMacro(<<"Read failed: Wanted " 
 					  << GetImageSizeInBytes()
 					  << " bytes, but read " 
 					  << file.gcount() << " bytes.");
  }
  itkDebugMacro(<< "Reading Done");
  
  float *pb = (float *)buffer;
  float *px = tmpBuff;
  float *py = tmpBuff + GetImageSizeInPixels();
  float *pz = tmpBuff + 2 * GetImageSizeInPixels();
  const float *pbe = (float *)buffer + GetImageSizeInComponents();
  while(pb != pbe){
    *pb++ = (*px++)*GetSpacing(0);
    *pb++ = (*py++)*GetSpacing(1);
    *pb++ = (*pz++)*GetSpacing(2);
  }
  delete [] tmpBuff;
  
  typedef itk::ByteSwapper< float > InternalByteSwapperType;
  InternalByteSwapperType::SwapRangeFromSystemToLittleEndian((float *)buffer, GetImageSizeInComponents());
}

//====================================================================
// Write Image Information
void clitk::VfImageIO::WriteImageInformation(bool keepOfStream)
{
  // Check dimension
  if (GetNumberOfDimensions() != 3) {
	itkExceptionMacro(<<"Write failed: only 3D image for Vf file format yet.");
  }

  // Open the file
  clitk::openFileForWriting(file, m_FileName);
  // write magic number
  file << "IAMA3DVECTORFIELD V2 " << std::endl;
  // write grid size/spacing
  file << GetDimensions(0) << ' ' 
	   << GetDimensions(1) << ' ' 
	   << GetDimensions(2) << ' '
	   << GetSpacing(0) << ' ' 
	   << GetSpacing(1) << ' ' 
	   << GetSpacing(2) << ' ' << std::endl;

  // close file
  if (!keepOfStream) file.close();	
}
  
//====================================================================
// Write Image Information
bool clitk::VfImageIO::CanWriteFile(const char* FileNameToWrite)
{
  std::string filename(FileNameToWrite);
  std::string filenameext = GetExtension(filename);
  if (filenameext != std::string("vf")) return false;
  return true;
}

//====================================================================
// Write Image
void clitk::VfImageIO::Write(const void * buffer) 
{
  clitk::VfImageIO::WriteImageInformation(true);
  
  typedef itk::ByteSwapper< float > InternalByteSwapperType;
  std::cout << "GetImageSizeInBytes() " << GetImageSizeInBytes() << std::endl;
  float* tempBuffer = new float[ GetImageSizeInPixels() ];


  for(int i=0 ; i< 3 ; i++){
	float *pb = (float *)buffer;
	pb+=i;
	float *ptb = tempBuffer;
	const float *pbe = (float *)buffer + GetImageSizeInComponents() + i;
	while(pb != pbe){
	  *ptb++ = (*pb)/GetSpacing(i);
	  pb+=3;
	}
	InternalByteSwapperType::SwapRangeFromSystemToLittleEndian(tempBuffer,GetImageSizeInPixels());
	file.write((char*)tempBuffer, GetImageSizeInBytes()/3 );
  }
  delete [] tempBuffer;						
  
  file.close();
} ////

#endif /* end #define CLITKVFIMAGEIO_CXX */

