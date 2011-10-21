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
#include <regex.h>
#include <cstdio>
#include <sstream>
#include <iostream>
using std::cout;
using std::endl;

// clitk include
#include "clitkGateAsciiImageIO.h"
#include "clitkCommon.h"

// itk include
#include <itkMetaDataObject.h>

std::ostream& operator<<(std::ostream& os, const clitk::GateAsciiImageIO::GateAsciiHeader& header)
{
    os << "matrix_size=[" << header.matrix_size[0] << "," << header.matrix_size[1] << "," << header.matrix_size[2] << "]" << endl;
    os << "resolution=[" << header.resolution[0] << "," << header.resolution[1] << "," << header.resolution[2] << "]" << endl;
    os << "voxel_size=[" << header.voxel_size[0] << "," << header.voxel_size[1] << "," << header.voxel_size[2] << "]" << endl;
    os << "nb_value=" << header.nb_value << endl;
    return os;
}

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

    { // check extension
	std::string filenameext = GetExtension(filename);
	if (filenameext != std::string("txt")) return false;
    }

    { // check header
	FILE* handle = fopen(filename.c_str(),"r");
	if (!handle) return false;

	GateAsciiHeader header;
	if (!ReadHeader(handle,header)) { fclose(handle); return false; }
	fclose(handle);
    }

    return true;
}

//--------------------------------------------------------------------
// Read Line in file
bool clitk::GateAsciiImageIO::ReadLine(FILE* handle, std::string& line)
{
    std::stringstream stream;
    while (true)
    {
	char item;
	if (ferror(handle)) return false;
	if (fread(&item,1,1,handle) != 1) return false;

	if (item=='\n' or feof(handle)) {
	    line = stream.str();
	    return true;
	}

	stream << item;
    }
}

std::string ExtractMatch(const std::string& base, const regmatch_t& match) 
{
    return base.substr(match.rm_so,match.rm_eo-match.rm_so);
}

template <typename T>
T ConvertFromString(const std::string& value)
{
    std::stringstream stream;
    stream << value;
    T converted;
    stream >> converted;
    return converted;
}

//--------------------------------------------------------------------
// Read Image Header
bool clitk::GateAsciiImageIO::ReadHeader(FILE* handle, GateAsciiHeader& header)
{
    assert(handle);
    std::string line;

    regex_t re_comment;
    regex_t re_matrix_size;
    regex_t re_resol;
    regex_t re_voxel_size;
    regex_t re_nb_value;
    regmatch_t matches[4];

    { // build regex
	int ret = 0;
	ret = regcomp(&re_comment,"^#.+$",REG_EXTENDED|REG_NEWLINE);
	assert(ret);
	ret = regcomp(&re_matrix_size,"^# +Matrix *Size *= +\\(([0-9]+\\.?[0-9]*),([0-9]+\\.?[0-9]*),([0-9]+\\.?[0-9]*)\\)$",REG_EXTENDED|REG_NEWLINE);
	assert(ret);
	ret = regcomp(&re_resol,"^# +Resol *= +\\(([0-9]+),([0-9]+),([0-9]+)\\)$",REG_EXTENDED|REG_NEWLINE);
	assert(ret);
	ret = regcomp(&re_voxel_size,"^# +Voxel *Size *= +\\(([0-9]+\\.?[0-9]*),([0-9]+\\.?[0-9]*),([0-9]+\\.?[0-9]*)\\)$",REG_EXTENDED|REG_NEWLINE);
	assert(ret);
	ret = regcomp(&re_nb_value,"^# +nbVal *= +([0-9]+)$",REG_EXTENDED|REG_NEWLINE);
	assert(ret);
    }

    if (!ReadLine(handle,line)) return false;
    if (regexec(&re_comment,line.c_str(),1,matches,0) == REG_NOMATCH) return false;

    if (!ReadLine(handle,line)) return false;
    if (regexec(&re_matrix_size,line.c_str(),4,matches,0) == REG_NOMATCH) return false;
    header.matrix_size[0] = ConvertFromString<double>(ExtractMatch(line,matches[1]));
    header.matrix_size[1] = ConvertFromString<double>(ExtractMatch(line,matches[2]));
    header.matrix_size[2] = ConvertFromString<double>(ExtractMatch(line,matches[3]));

    if (!ReadLine(handle,line)) return false;
    if (regexec(&re_resol,line.c_str(),4,matches,0) == REG_NOMATCH) return false;
    header.resolution[0] = ConvertFromString<int>(ExtractMatch(line,matches[1]));
    header.resolution[1] = ConvertFromString<int>(ExtractMatch(line,matches[2]));
    header.resolution[2] = ConvertFromString<int>(ExtractMatch(line,matches[3]));

    if (!ReadLine(handle,line)) return false;
    if (regexec(&re_voxel_size,line.c_str(),4,matches,0) == REG_NOMATCH) return false;
    header.voxel_size[0] = ConvertFromString<double>(ExtractMatch(line,matches[1]));
    header.voxel_size[1] = ConvertFromString<double>(ExtractMatch(line,matches[2]));
    header.voxel_size[2] = ConvertFromString<double>(ExtractMatch(line,matches[3]));

    if (!ReadLine(handle,line)) return false;
    if (regexec(&re_nb_value,line.c_str(),2,matches,0) == REG_NOMATCH) return false;
    header.nb_value = ConvertFromString<int>(ExtractMatch(line,matches[1]));

    if (!ReadLine(handle,line)) return false;
    if (regexec(&re_comment,line.c_str(),1,matches,0) == REG_NOMATCH) return false;

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
