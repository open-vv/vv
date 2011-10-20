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

// std include
#include <stdio.h>

// clitk include
#include "clitkGateAsciiImageIO.h"
#include "clitkCommon.h"

// itk include
#include <itkMetaDataObject.h>

//--------------------------------------------------------------------
// Read Image Information
void clitk::GateAsciiImageIO::ReadImageInformation()
{
    itkGenericExceptionMacro(<< "Could not open file (for reading): " << m_FileName);


  /* Convert hnd to ITK image information */
  SetNumberOfDimensions(2);
  //SetDimensions(0, hnd.SizeX);
  //SetDimensions(1, hnd.SizeY);
  //SetSpacing(0, hnd.dIDUResolutionX);
  //SetSpacing(1, hnd.dIDUResolutionY);
  //SetOrigin(0, -0.5*(hnd.SizeX-1)*hnd.dIDUResolutionX); //SR: assumed centered
  //SetOrigin(1, -0.5*(hnd.SizeY-1)*hnd.dIDUResolutionY); //SR: assumed centered
  //SetComponentType(itk::ImageIOBase::UINT);

  /* Store important meta information in the meta data dictionary */
  //itk::EncapsulateMetaData<double>(this->GetMetaDataDictionary(), "dCTProjectionAngle", hnd.dCTProjectionAngle);
  //itk::ExposeMetaData<double>( this->GetMetaDataDictionary(), &(hnd.dCTProjectionAngle), "dCTProjectionAngle");
}

//--------------------------------------------------------------------
bool clitk::GateAsciiImageIO::CanReadFile(const char* FileNameToRead)
{
  std::string filename(FileNameToRead);
  std::string filenameext = GetExtension(filename);
  if (filenameext != std::string("hnd")) return false;
  return true;
}

//--------------------------------------------------------------------
// Read Image Content
void clitk::GateAsciiImageIO::Read(void * buffer)
{
  FILE *fp;

  uint32_t* buf = (uint32_t*)buffer;
  unsigned char *pt_lut;
  uint32_t a;
  float b;
  unsigned char v;
  int lut_idx, lut_off;
  size_t num_read;
  char dc;
  short ds;
  long dl, diff=0;
  uint32_t i;

  fp = fopen (m_FileName.c_str(), "rb");
  if (fp == NULL)
    itkGenericExceptionMacro(<< "Could not open file (for reading): " << m_FileName);

  pt_lut = (unsigned char*) malloc (sizeof (unsigned char) * GetDimensions(0) * GetDimensions(1));

  /* Read LUT */
  fseek (fp, 1024, SEEK_SET);
  fread (pt_lut, sizeof(unsigned char), (GetDimensions(1)-1)*GetDimensions(0) / 4, fp);

  /* Read first row */
  for (i = 0; i < GetDimensions(0); i++) {
    fread (&a, sizeof(uint32_t), 1, fp);
    buf[i] = a;
    b = a;
  }

  /* Read first pixel of second row */
  fread (&a, sizeof(uint32_t), 1, fp);
  buf[i++] = a;
  b = a;

  /* Decompress the rest */
  lut_idx = 0;
  lut_off = 0;
  while (i < GetDimensions(0) * GetDimensions(1)) {
    uint32_t r11, r12, r21;

    r11 = buf[i-GetDimensions(0)-1];
    r12 = buf[i-GetDimensions(0)];
    r21 = buf[i-1];
    v = pt_lut[lut_idx];
    switch (lut_off) {
    case 0:
      v = v & 0x03;
      lut_off ++;
      break;
    case 1:
      v = (v & 0x0C) >> 2;
      lut_off ++;
      break;
    case 2:
      v = (v & 0x30) >> 4;
      lut_off ++;
      break;
    case 3:
      v = (v & 0xC0) >> 6;
      lut_off = 0;
      lut_idx ++;
      break;
    }
    switch (v) {
    case 0:
      num_read = fread (&dc, sizeof(unsigned char), 1, fp);
      if (num_read != 1) goto read_error;
      diff = dc;
      break;
    case 1:
      num_read = fread (&ds, sizeof(unsigned short), 1, fp);
      if (num_read != 1) goto read_error;
      diff = ds;
      break;
    case 2:
      num_read = fread (&dl, sizeof(uint32_t), 1, fp);
      if (num_read != 1) goto read_error;
      diff = dl;
      break;
    }

    buf[i] = r21 + r12 + diff - r11;
    b = buf[i];
    i++;
  }

  /* Clean up */
  free (pt_lut);
  fclose (fp);
  return;

read_error:

  itkGenericExceptionMacro(<< "Error reading gate ascii file");
  free (pt_lut);
  fclose (fp);
  return;
}

//--------------------------------------------------------------------
bool clitk::GateAsciiImageIO::CanWriteFile(const char* FileNameToWrite)
{
  return false;
}

//--------------------------------------------------------------------
// Write Image
void clitk::GateAsciiImageIO::Write(const void* buffer)
{
}
