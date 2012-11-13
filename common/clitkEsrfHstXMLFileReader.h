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

#ifndef __clitkEsrfHstXMLFileReader_h
#define __clitkEsrfHstXMLFileReader_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <itkXMLFile.h>
#include <itkMetaDataDictionary.h>

#include <map>

namespace clitk
{

/** \class EsrfHstXMLFileReader
 *
 * Reads the XML-format file written by the software High Speed Tomography
 * at the European Synchrotron Radiation Facility
 */
class EsrfHstXMLFileReader : public itk::XMLReader<itk::MetaDataDictionary>
{
public:
  /** Standard typedefs */
  typedef EsrfHstXMLFileReader                    Self;
  typedef itk::XMLReader<itk::MetaDataDictionary> Superclass;
  typedef itk::SmartPointer<Self>                 Pointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(EsrfHstXMLFileReader, itk::XMLReader);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Determine if a file can be read */
  int CanReadFile(const char* name);

protected:
  EsrfHstXMLFileReader() {m_OutputObject = &m_Dictionary;};
  virtual ~EsrfHstXMLFileReader() {};

  virtual void StartElement(const char * name,const char **atts);

  virtual void EndElement(const char *name);

  void CharacterDataHandler(const char *inData, int inLength);

private:
  EsrfHstXMLFileReader(const Self&); //purposely not implemented
  void operator=(const Self&);       //purposely not implemented

  itk::MetaDataDictionary m_Dictionary;
  std::string             m_CurCharacterData;
};

}
#endif
