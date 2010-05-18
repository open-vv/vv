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
#ifndef CLITKHISIMAGEIO_CXX
#define CLITKHISIMAGEIO_CXX
#define HEADER_INFO_SIZE 68
/**
   -------------------------------------------------
   * @file   clitkHisImageIO.cxx
   * @author Simon Rit <simon.rit@gmail.com>
   * @date   16 Feb 2010
   *
   * @brief
   *
   *
   -------------------------------------------------*/

// Based on a true story by the Nederlands Kanker Instituut (AVS_HEIMANN.CPP from the 20090608)

// std include
#include <fstream>

// clitk include
#include "clitkHisImageIO.h"
#include "clitkCommon.h"

//--------------------------------------------------------------------
// Read Image Information
void clitk::HisImageIO::ReadImageInformation()
{
  // open file
  std::ifstream file(m_FileName.c_str(), std::ios::in | std::ios::binary);
  if ( file.fail() )
    itkGenericExceptionMacro(<< "Could not open file (for reading): " << m_FileName);

  // read header
  char header[HEADER_INFO_SIZE];
  file.read(header, HEADER_INFO_SIZE);

  if (header[0]!=0 || header[1]!=112 || header[2]!=68 || header[3]!=0) {
    itkExceptionMacro(<< "clitk::HisImageIO::ReadImageInformation: file " << m_FileName << " not in Heimann HIS format version 100");
    return;
  }

  int nrframes, type, ulx, uly, brx, bry;
  m_HeaderSize  = header[10] + (header[11]<<8);
  ulx      = header[12] + (header[13]<<8);
  uly      = header[14] + (header[15]<<8);
  brx      = header[16] + (header[17]<<8);
  bry      = header[18] + (header[19]<<8);
  nrframes = header[20] + (header[21]<<8);
  type     = header[32] + (header[34]<<8);

  switch(type) {
  case  4:
    SetComponentType(itk::ImageIOBase::USHORT);
    break;
//    case  8: SetComponentType(itk::ImageIOBase::INT);   break;
//    case 16: SetComponentType(itk::ImageIOBase::FLOAT); break;
//    case 32: SetComponentType(itk::ImageIOBase::INT);   break;
  default:
    SetComponentType(itk::ImageIOBase::USHORT);
    break;
  }

  switch(nrframes) {
  case 1:
    SetNumberOfDimensions(2);
    break;
  default:
    SetNumberOfDimensions(3);
    break;
  }

  SetDimensions(0, bry-uly+1);
  SetDimensions(1, brx-ulx+1);
  if (nrframes>1)
    SetDimensions(2, nrframes);
} ////

//--------------------------------------------------------------------
// Read Image Information
bool clitk::HisImageIO::CanReadFile(const char* FileNameToRead)
{
  std::string filename(FileNameToRead);
  std::string filenameext = GetExtension(filename);
  if (filenameext != std::string("his")) return false;
  return true;
} ////

//--------------------------------------------------------------------
// Read Image Content
void clitk::HisImageIO::Read(void * buffer)
{
  // open file
  std::ifstream file(m_FileName.c_str(), std::ios::in | std::ios::binary);
  if ( file.fail() )
    itkGenericExceptionMacro(<< "Could not open file (for reading): " << m_FileName);


  file.seekg(m_HeaderSize+HEADER_INFO_SIZE, std::ios::beg);
  if ( file.fail() )
    itkExceptionMacro(<<"File seek failed (His Read)");


  file.read((char*)buffer, GetImageSizeInBytes());
  if ( file.fail() )
    itkExceptionMacro(<<"Read failed: Wanted "
                      << GetImageSizeInBytes()
                      << " bytes, but read "
                      << file.gcount() << " bytes. The current state is: "
                      << file.rdstate());
}

//--------------------------------------------------------------------
bool clitk::HisImageIO::CanWriteFile(const char* FileNameToWrite)
{
  std::string filename(FileNameToWrite);
  std::string filenameext = GetExtension(filename);
  if (filenameext != std::string("his")) return false;
  return true;
}

//--------------------------------------------------------------------
// Write Image
void clitk::HisImageIO::Write(const void* buffer)
{
  std::ofstream file(m_FileName.c_str(), std::ios::out | std::ios::binary);
  if ( file.fail() )
    itkGenericExceptionMacro(<< "Could not open file (for writing): " << m_FileName);

  m_HeaderSize = HEADER_INFO_SIZE + 32;
  char szHeader[HEADER_INFO_SIZE + 32] = {
    0x00, 0x70, 0x44, 0x00, 0x64, 0x00, 0x64, 0x00, 0x20, 0x00, 0x20, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x00, 0x04, 0x00, 0x04, 0x01, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6A, 0x18, 0x41,
    0x04, 0x00, 0x40, 0x5F, 0x48, 0x01, 0x40, 0x00, 0x86, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x63, 0x13, 0x00, 0xE8, 0x51, 0x13, 0x00, 0x5C, 0xE7, 0x12, 0x00,
    0xFE, 0x2A, 0x49, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
  };

  /* Fill into the header the essentials
     The 'iheader' in previous module is fixed to 0x20, and is included in szHeader.
     The 'ulx' and 'uly' are fixed to 0x01, so that 'brx' and 'bry' reflect the dimensions of
     the image.
  */
  const unsigned int ndim = GetNumberOfDimensions();
  if ((ndim < 2) || (ndim > 3))
    itkExceptionMacro( <<"Only 2D or 3D support");

  szHeader[16] = (char)(GetDimensions(0) % 256);	// X-size	lsb
  szHeader[17] = (char)(GetDimensions(0) / 256);	// X-size	msb
  szHeader[18] = (char)(GetDimensions(1) % 256);	// Y-size	lsb
  szHeader[19] = (char)(GetDimensions(1) / 256);	// Y-size	msb
  if (ndim == 3) {
    szHeader[20] = (char)(GetDimensions(0) % 256);	// NbFrames	lsb
    szHeader[21] = (char)(GetDimensions(0) / 256);	// NbFrames	msb
  }

  switch (GetComponentType()) {
  case itk::ImageIOBase::USHORT:
    szHeader[32] = 4;
    break;
    //case AVS_TYPE_INTEGER:
    //  szHeader[32] = 8;
    //  break;
    //case AVS_TYPE_REAL:
    //  szHeader[32] = 16;
    //  break;
  default:
    itkExceptionMacro(<< "Unsupported field type");
    break;
  }

  file.write(szHeader, m_HeaderSize);
  file.write((const char *)buffer, GetImageSizeInBytes());
  file.close();
} ////

#endif /* end #define CLITKHISIMAGEIO_CXX */

