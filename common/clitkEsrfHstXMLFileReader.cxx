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

#include "clitkEsrfHstXMLFileReader.h"
#include "itkMacro.h"

#include <itksys/SystemTools.hxx>
#include <itkMetaDataObject.h>

namespace clitk
{

int
EsrfHstXMLFileReader::
CanReadFile(const char *name)
{
  if(!itksys::SystemTools::FileExists(name) ||
      itksys::SystemTools::FileIsDirectory(name) ||
      itksys::SystemTools::FileLength(name) == 0)
    return 0;
  return 1;
}

void
EsrfHstXMLFileReader::
StartElement(const char * itkNotUsed(name),const char ** itkNotUsed(atts))
{
  m_CurCharacterData = "";
}

void
EsrfHstXMLFileReader::
EndElement(const char *name)
{
#define ENCAPLULATE_META_DATA_DOUBLE(metaName) \
  if(itksys::SystemTools::Strucmp(name, metaName) == 0) { \
    double d = atof(m_CurCharacterData.c_str() ); \
    itk::EncapsulateMetaData<double>(m_Dictionary, metaName, d); \
    }

#define ENCAPLULATE_META_DATA_INT(metaName) \
  if(itksys::SystemTools::Strucmp(name, metaName) == 0) { \
    double d = atof(m_CurCharacterData.c_str() ); \
    itk::EncapsulateMetaData<int>(m_Dictionary, metaName, d); \
    }

#define ENCAPLULATE_META_DATA_STRING(metaName) \
  if(itksys::SystemTools::Strucmp(name, metaName) == 0) { \
    itk::EncapsulateMetaData<std::string>(m_Dictionary, metaName, m_CurCharacterData); \
    }

  ENCAPLULATE_META_DATA_INT("idAc");
  ENCAPLULATE_META_DATA_STRING("SUBVOLUME_NAME");
  ENCAPLULATE_META_DATA_INT("SIZEX");
  ENCAPLULATE_META_DATA_INT("SIZEY");
  ENCAPLULATE_META_DATA_INT("SIZEZ");
  ENCAPLULATE_META_DATA_INT("ORIGINX");
  ENCAPLULATE_META_DATA_INT("ORIGINY");
  ENCAPLULATE_META_DATA_INT("ORIGINZ");
  ENCAPLULATE_META_DATA_INT("DIM_REC");
  ENCAPLULATE_META_DATA_DOUBLE("voxelsize");
  ENCAPLULATE_META_DATA_STRING("BYTE_ORDER");
}

void
EsrfHstXMLFileReader::
CharacterDataHandler(const char *inData, int inLength)
{
  for(int i = 0; i < inLength; i++)
    m_CurCharacterData = m_CurCharacterData + inData[i];
}

}
