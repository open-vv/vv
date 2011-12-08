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
  FILE* handle = fopen(m_FileName.c_str(),"r");
  if (!handle) {
    itkGenericExceptionMacro(<< "Could not open file (for reading): " << m_FileName);
    return;
  }

  GateAsciiHeader header;
  if (!ReadHeader(handle,header)) {
    itkGenericExceptionMacro(<< "Could not read header: " << m_FileName);
    fclose(handle);
    return;
  }
  fclose(handle);

  int real_length = -1;
  double real_spacing = 0;
  for (int kk=0; kk<3; kk++) {
    if (header.resolution[kk]>1) {
      if (real_length>0) {
        itkGenericExceptionMacro(<< "Could not image dimension: " << m_FileName);
        return;
      }
      real_length = header.resolution[kk];
      real_spacing = header.voxel_size[kk];
    }
  }
  assert(real_length == header.nb_value);

  // Set image information
  SetNumberOfDimensions(2);
  SetDimensions(0, real_length);
  SetDimensions(1, 1);
  SetSpacing(0, real_spacing);
  SetSpacing(1, 1);
  SetOrigin(0, 0);
  SetOrigin(1, 0);
  SetComponentType(itk::ImageIOBase::DOUBLE);
}

//--------------------------------------------------------------------
bool clitk::GateAsciiImageIO::CanReadFile(const char* FileNameToRead)
{
  std::string filename(FileNameToRead);

  {
    // check extension
    std::string filenameext = GetExtension(filename);
    if (filenameext != "txt") return false;
  }

  {
    // check header
    FILE* handle = fopen(filename.c_str(),"r");
    if (!handle) return false;

    GateAsciiHeader header;
    if (!ReadHeader(handle,header)) {
      fclose(handle);
      return false;
    }
    fclose(handle);
  }

  return true;
}

//--------------------------------------------------------------------
// Read Line in file
bool clitk::GateAsciiImageIO::ReadLine(FILE* handle, std::string& line)
{
  std::stringstream stream;
  while (true) {
    char item;
    if (ferror(handle)) return false;
    if (fread(&item,1,1,handle) != 1) return false;

    if (item=='\n' || feof(handle)) {
      line = stream.str();
      return true;
    }

    stream << item;
  }
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

bool
clitk::GateAsciiImageIO::FindRegularExpressionNextLine(itksys::RegularExpression &reg, std::string &s, FILE* handle)
{
  std::string line;
  if(!ReadLine(handle,line))  return false;
  if(!reg.compile(s.c_str())) return false;
  return reg.find(line.c_str());
}

//--------------------------------------------------------------------
// Read Image Header
bool clitk::GateAsciiImageIO::ReadHeader(FILE* handle, GateAsciiHeader& header)
{
  assert(handle);

  std::string regexstr[6] = {
    "^#.+$",
    "^# +Matrix *Size *= +\\(([0-9]+\\.?[0-9]*),([0-9]+\\.?[0-9]*),([0-9]+\\.?[0-9]*)\\)$",
    "^# +Resol *= +\\(([0-9]+),([0-9]+),([0-9]+)\\)$",
    "^# +Voxel *Size *= +\\(([0-9]+\\.?[0-9]*),([0-9]+\\.?[0-9]*),([0-9]+\\.?[0-9]*)\\)$",
    "^# +nbVal *= +([0-9]+)$"
  };

  itksys::RegularExpression regex;

  if(!FindRegularExpressionNextLine(regex, regexstr[0], handle)) return false;

  if(!FindRegularExpressionNextLine(regex, regexstr[1], handle)) return false;
  header.matrix_size[0] = ConvertFromString<double>(regex.match(1));
  header.matrix_size[1] = ConvertFromString<double>(regex.match(2));
  header.matrix_size[2] = ConvertFromString<double>(regex.match(3));

  if(!FindRegularExpressionNextLine(regex, regexstr[2], handle)) return false;
  header.resolution[0] = ConvertFromString<int>(regex.match(1));
  header.resolution[1] = ConvertFromString<int>(regex.match(2));
  header.resolution[2] = ConvertFromString<int>(regex.match(3));

  if(!FindRegularExpressionNextLine(regex, regexstr[3], handle)) return false;
  header.voxel_size[0] = ConvertFromString<double>(regex.match(1));
  header.voxel_size[1] = ConvertFromString<double>(regex.match(2));
  header.voxel_size[2] = ConvertFromString<double>(regex.match(3));

  if(!FindRegularExpressionNextLine(regex, regexstr[4], handle)) return false;
  header.nb_value = ConvertFromString<int>(regex.match(1));

  if(!FindRegularExpressionNextLine(regex, regexstr[0], handle)) return false;

  return true;
}

//--------------------------------------------------------------------
// Read Image Content
void clitk::GateAsciiImageIO::Read(void* abstract_buffer)
{
  FILE* handle = fopen(m_FileName.c_str(),"r");
  if (!handle) {
    itkGenericExceptionMacro(<< "Could not open file (for reading): " << m_FileName);
    return;
  }

  GateAsciiHeader header;
  if (!ReadHeader(handle,header)) {
    itkGenericExceptionMacro(<< "Could not read header: " << m_FileName);
    fclose(handle);
    return;
  }

  {
    double* buffer = static_cast<double*>(abstract_buffer);
    int read_count = 0;
    while (true) {
      std::string line;
      if (!ReadLine(handle,line)) break;
      *buffer = ConvertFromString<double>(line);
      read_count++;
      buffer++;
    }
    assert(read_count == header.nb_value);
  }

  fclose(handle);
}

//--------------------------------------------------------------------
bool clitk::GateAsciiImageIO::CanWriteFile(const char* FileNameToWrite)
{
  if (GetExtension(std::string(FileNameToWrite)) != "txt") return false;
  return true;
}

void clitk::GateAsciiImageIO::WriteImageInformation()
{
  cout << GetNumberOfDimensions() << endl;
}

bool clitk::GateAsciiImageIO::SupportsDimension(unsigned long dim)
{
  if (dim==2) return true;
  return false;
}

//--------------------------------------------------------------------
// Write Image
void clitk::GateAsciiImageIO::Write(const void* abstract_buffer)
{
  const unsigned long nb_value = GetDimensions(0)*GetDimensions(1);
  std::stringstream stream;
  stream << "######################" << endl;
  stream << "# Matrix Size= (" << GetSpacing(0)*GetDimensions(0) << "," << GetSpacing(1)*GetDimensions(1) << ",1)" << endl;
  stream << "# Resol      = (" << GetDimensions(0) << "," << GetDimensions(1) << ",1)" << endl;
  stream << "# VoxelSize  = (" << GetSpacing(0) << "," << GetSpacing(1) << ",1)" << endl;
  stream << "# nbVal      = " << nb_value << endl;
  stream << "######################" << endl;

  const double* buffer = static_cast<const double*>(abstract_buffer);
  for (unsigned long kk=0; kk<nb_value; kk++) {
    stream << buffer[kk] << endl;
  }

  FILE* handle = fopen(m_FileName.c_str(),"w");
  if (!handle) {
    itkGenericExceptionMacro(<< "Could not open file (for writing): " << m_FileName);
    return;
  }

  fwrite(stream.str().c_str(),1,stream.str().size(),handle);

  fclose(handle);
}
