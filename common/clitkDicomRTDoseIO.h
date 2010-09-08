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
#ifndef clitkDicomRTDoseIO_h
#define clitkDicomRTDoseIO_h

// clitk include
#include "clitkCommon.h"

// itk include
#include <itkImageIOBase.h>
#include <gdcmFile.h>

// std include
#include <fstream>

namespace clitk
{

//====================================================================
// Class for reading Vox Image file format
class DicomRTDoseIO: public itk::ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef DicomRTDoseIO              Self;
  typedef itk::ImageIOBase        Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef signed short int        PixelType;

  DicomRTDoseIO():Superclass() {
    mustWriteHeader = false;
    m_GdcmFile=NULL;
  }

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DicomRTDoseIO, ImageIOBase);

  /*-------- This part of the interface deals with reading data. ------ */
  virtual void ReadImageInformation();
  virtual bool CanReadFile( const char* FileNameToRead );
  virtual void Read(void * buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */
  virtual void WriteImageInformation(bool keepOfStream);
  virtual void WriteImageInformation() {
    WriteImageInformation(false);
  }
  virtual bool CanWriteFile(const char* filename);
  virtual void Write(const void* buffer);

protected:
  template <class T> void dose_copy_raw (float *img_out, T *img_in, int nvox, float scale);

  bool mustWriteHeader;
  int m_HeaderSize;
  std::ofstream file;
  gdcm::File *m_GdcmFile;
  float m_DoseScaling;
}; // end class DicomRTDoseIO

} // end namespace

#endif /* end #define clitkDicomRTDoseIO_h */

