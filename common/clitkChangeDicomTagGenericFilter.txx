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
#ifndef clitkChangeDicomTagGenericFilter_txx
#define clitkChangeDicomTagGenericFilter_txx

/* =================================================
 * @file   clitkChangeDicomTagGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

// clitk
#include "clitkResampleImageWithOptionsFilter.h"
#if GDCM_MAJOR_VERSION >= 2
#include "gdcmUIDGenerator.h"
#include <gdcmImageHelper.h>
#include <gdcmAttribute.h>
#include <gdcmReader.h>
#include <gdcmWriter.h>
#include <gdcmDataElement.h>
#include <gdcmTag.h>
#else
#include "gdcmFile.h"
#include "gdcmUtil.h"
#endif


namespace clitk
{


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
ChangeDicomTagGenericFilter<args_info_type>::ChangeDicomTagGenericFilter()
{
  m_Verbose=false;
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void ChangeDicomTagGenericFilter<args_info_type>::Update()
{
  UpdateWithDimAndPixelType();
}

//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype read from
// the dicom files. The MHD files may be resampled to match the
// dicom spacing (and number of slices). Rounding errors in resampling
// are handled by removing files when generating the output dicom
// series.
//-------------------------------------------------------------------
template<class args_info_type>
void
ChangeDicomTagGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{
  //Read the dicom file
  gdcm::Reader reader;
  reader.SetFileName( m_ArgsInfo.input_arg );
  reader.Read();
  gdcm::File &fileOutput = reader.GetFile();
  gdcm::DataSet &dsOutput = fileOutput.GetDataSet();
  const unsigned int ptr_len = 42;
  char *ptr = new char[ptr_len];
  memset(ptr,0,ptr_len);

  unsigned int numberOfKeysGiven=m_ArgsInfo.key_given;
  // For all input tag
  for (unsigned int i = 0; i < numberOfKeysGiven; i++) {
    // Split the tag element separated with '|' in 2 parts
    char* copy = (char*)m_ArgsInfo.key_arg[i];
    std::vector<char*> v;
    char* chars_array = strtok(copy, "|");
    while(chars_array) {
      v.push_back(chars_array);
      chars_array = strtok(NULL, "|");
    }
    std::stringstream str0(v[0]), str1(v[1]);
    int first, second;
    str0 >> first;
    str1 >> second;

    //Change the tag element by the corresponding value
    gdcm::DataElement tagDE( gdcm::Tag(first, second) );
    std::string value( m_ArgsInfo.tag_arg[i] );
    tagDE.SetByteValue(value.c_str(), (uint32_t)strlen(value.c_str()));
    dsOutput.Insert(tagDE);
  }

  //Write the output dicom file
  gdcm::Writer w;
  w.SetFile( fileOutput );
  w.SetFileName( m_ArgsInfo.output_arg );
  w.Write();
}

}//end clitk

#endif //#define clitkChangeDicomTagGenericFilter_txx
