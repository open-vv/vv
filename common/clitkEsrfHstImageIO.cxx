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

#include "clitkEsrfHstImageIO.h"
#include "clitkEsrfHstXMLFileReader.h"
#include "clitkDD.h"

// itk include (for itkReadRawBytesAfterSwappingMacro)
#include <itkRawImageIO.h>
#include <itksys/SystemTools.hxx>
#include <itkMetaDataObject.h>

//--------------------------------------------------------------------
// Read Image Information
void clitk::EsrfHstImageIO::ReadImageInformation()
{
  clitk::EsrfHstXMLFileReader::Pointer hstXmlReader;
  hstXmlReader = clitk::EsrfHstXMLFileReader::New();
  hstXmlReader->SetFilename(m_XmlFileName);
  hstXmlReader->GenerateOutputInformation();
  itk::MetaDataDictionary &dic = *(hstXmlReader->GetOutputObject() );

  typedef itk::MetaDataObject< double > MetaDataDoubleType;
  typedef itk::MetaDataObject< std::string > MetaDataStringType;
  typedef itk::MetaDataObject< int > MetaDataIntType;
  SetComponentType(itk::ImageIOBase::FLOAT);
  SetNumberOfDimensions(3);
  SetDimensions(0, dynamic_cast<MetaDataIntType *>(dic["SIZEX"].GetPointer() )->GetMetaDataObjectValue());
  SetDimensions(1, dynamic_cast<MetaDataIntType *>(dic["SIZEY"].GetPointer() )->GetMetaDataObjectValue());
  SetDimensions(2, dynamic_cast<MetaDataIntType *>(dic["SIZEZ"].GetPointer() )->GetMetaDataObjectValue());
  SetSpacing(0, dynamic_cast<MetaDataDoubleType *>(dic["voxelsize"].GetPointer() )->GetMetaDataObjectValue());
  SetSpacing(1, GetSpacing(0));
  SetSpacing(2, GetSpacing(0));
  SetOrigin(0, GetSpacing(0)*(dynamic_cast<MetaDataIntType *>(dic["ORIGINX"].GetPointer() )->GetMetaDataObjectValue()-1));
  SetOrigin(1, GetSpacing(0)*(dynamic_cast<MetaDataIntType *>(dic["ORIGINY"].GetPointer() )->GetMetaDataObjectValue()-1));
  SetOrigin(2, GetSpacing(0)*(dynamic_cast<MetaDataIntType *>(dic["ORIGINZ"].GetPointer() )->GetMetaDataObjectValue()-1));
  if(std::string("LOWBYTEFIRST") == dynamic_cast<MetaDataStringType*>(dic["SUBVOLUME_NAME"].GetPointer() )->GetMetaDataObjectValue())
    this->SetByteOrder(BigEndian);
  else
    this->SetByteOrder(LittleEndian);

  // Prepare raw file name
  m_RawFileName = itksys::SystemTools::GetFilenamePath(m_XmlFileName);
  if(m_RawFileName != "")
    m_RawFileName += std::string("/");
  m_RawFileName += dynamic_cast<MetaDataStringType*>(dic["SUBVOLUME_NAME"].GetPointer() )->GetMetaDataObjectValue();
} ////

//--------------------------------------------------------------------
// Read Image Information
bool clitk::EsrfHstImageIO::CanReadFile(const char* FileNameToRead)
{
  std::string ext = itksys::SystemTools::GetFilenameLastExtension(FileNameToRead);
  if( ext!=std::string(".xml") && ext!=std::string(".vol") )
    return false;

  m_XmlFileName = std::string(FileNameToRead);
  if( ext==std::string(".vol") )
    m_XmlFileName += std::string(".xml");

  std::ifstream is(m_XmlFileName.c_str());
  if(! is.is_open() )
    return false;

  std::string firstLine;
  std::getline(is, firstLine);
  if(firstLine != std::string("<!-- PyHST VOLUME XML FILE -->") )
    return false;

  return true;
} ////

//--------------------------------------------------------------------
// Read Image Content
void clitk::EsrfHstImageIO::Read(void * buffer)
{
  // Adapted from itkRawImageIO
  std::ifstream is(m_RawFileName.c_str(), std::ios::binary);
  if(! is.is_open() )
    itkExceptionMacro(<<"Could not open file " << m_RawFileName);

  unsigned long numberOfBytesToBeRead = GetComponentSize();
  for(unsigned int i=0; i<GetNumberOfDimensions(); i++) numberOfBytesToBeRead *= GetDimensions(i);

  if(!this->ReadBufferAsBinary(is, buffer, numberOfBytesToBeRead)) {
    itkExceptionMacro(<<"Read failed: Wanted "
                      << numberOfBytesToBeRead
                      << " bytes, but read "
                      << is.gcount() << " bytes.");
  }
  itkDebugMacro(<< "Reading Done");

  // Adapted from itkRawImageIO
  {
    using namespace itk;
    // Swap bytes if necessary
    if itkReadRawBytesAfterSwappingMacro( unsigned short, USHORT )
      else if itkReadRawBytesAfterSwappingMacro( short, SHORT )
        else if itkReadRawBytesAfterSwappingMacro( char, CHAR )
          else if itkReadRawBytesAfterSwappingMacro( unsigned char, UCHAR )
            else if itkReadRawBytesAfterSwappingMacro( unsigned int, UINT )
              else if itkReadRawBytesAfterSwappingMacro( int, INT )
                else if itkReadRawBytesAfterSwappingMacro( unsigned int, ULONG )
                  else if itkReadRawBytesAfterSwappingMacro( int, LONG )
                    else if itkReadRawBytesAfterSwappingMacro( float, FLOAT )
                      else if itkReadRawBytesAfterSwappingMacro( double, DOUBLE );
  }
}

//--------------------------------------------------------------------
// Write Image Information
void clitk::EsrfHstImageIO::WriteImageInformation(bool keepOfStream)
{
}

//--------------------------------------------------------------------
// Write Image Information
bool clitk::EsrfHstImageIO::CanWriteFile(const char* FileNameToWrite)
{
  return false;
}

//--------------------------------------------------------------------
// Write Image
void clitk::EsrfHstImageIO::Write(const void * buffer)
{
} ////
