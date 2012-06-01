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
#ifndef CLITKGATEASCIIIMAGEIO_H
#define CLITKGATEASCIIIMAGEIO_H

// itk include
#include <itkImageIOBase.h>
#include <itksys/RegularExpression.hxx>

#if defined (_MSC_VER) && (_MSC_VER < 1600)
//SR: taken from
//#include "msinttypes/stdint.h"
typedef unsigned int uint32_t;
#else
#include <stdint.h>
#endif

namespace clitk
{

//====================================================================
// Class for reading gate ascii Image file format
class GateAsciiImageIO: public itk::ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef GateAsciiImageIO        Self;
  typedef itk::ImageIOBase        Superclass;
  typedef itk::SmartPointer<Self> Pointer;

  struct GateAsciiHeader {
    double matrix_size[3];
    int resolution[3];
    double voxel_size[3];
    int nb_value;
  };

  GateAsciiImageIO():Superclass() {}

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GateAsciiImageIO, ImageIOBase);

  /*-------- This part of the interface deals with reading data. ------ */
  virtual void ReadImageInformation();
  virtual bool CanReadFile( const char* FileNameToRead );
  virtual void Read(void * buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */
  virtual void WriteImageInformation();
  virtual bool CanWriteFile(const char* filename);
  virtual void Write(const void* buffer);

  virtual bool SupportsDimension(unsigned long dim);

protected:
  static bool ReadHeader(FILE* handle, GateAsciiHeader& header);
  static bool ReadLine(FILE* handle, std::string& line);
  static bool FindRegularExpressionNextLine(itksys::RegularExpression &reg, std::string &s, FILE* handle);

}; // end class GateAsciiImageIO
} // end namespace

// explicit template instantiation
//template class itk::CreateObjectFunction<clitk::GateAsciiImageIO>;

#endif /* end #define CLITKGATEASCIIIMAGEIO_H */

