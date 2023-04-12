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
#ifndef clitkUpdateVRTagDicomGenericFilter_txx
#define clitkUpdateVRTagDicomGenericFilter_txx

/* =================================================
 * @file   clitkUpdateVRTagDicomGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

#include <sstream>
// clitk
#include "clitkResampleImageWithOptionsFilter.h"
#include "gdcmUIDGenerator.h"
#include <gdcmImageHelper.h>
#include <gdcmAttribute.h>
#include <gdcmReader.h>
#include <gdcmWriter.h>
#include <gdcmDataElement.h>
#include <gdcmTag.h>
#include <gdcmDict.h>
#include <gdcmDicts.h>
#include <gdcmGlobal.h>

#include "itkImageRegionIterator.h"
#include "itkMetaImageIO.h"
#include "itkMetaDataDictionary.h"

namespace clitk
{


//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
UpdateVRTagDicomGenericFilter<args_info_type>::UpdateVRTagDicomGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void UpdateVRTagDicomGenericFilter<args_info_type>::Update()
{
  // Read the Dimension and PixelType
  int Dimension;
  std::string PixelType;
  ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);


  // Call UpdateWithDim
  if(Dimension==2) UpdateWithDim<2>(PixelType);
  else if(Dimension==3) UpdateWithDim<3>(PixelType);
  // else if (Dimension==4)UpdateWithDim<4>(PixelType);
  else {
    std::cout<<"Error, Only for 2 or 3  Dimensions!!!"<<std::endl ;
    return;
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions
//-------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dimension>
void
UpdateVRTagDicomGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

  if(PixelType == "short") {
    if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, signed short>();
  }
  else if(PixelType == "unsigned_short"){
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned short>();
  }

  else if (PixelType == "unsigned_char") {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned char>();
  }

  //     else if (PixelType == "char"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, signed char>();
  //     }
  else if (PixelType == "double") {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and double..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, double>();
  }
  else {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, float>();
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype read from
// the dicom files. The MHD files may be resampled to match the
// dicom spacing (and number of slices). Rounding errors in resampling
// are handled by removing files when generating the output dicom
// series.
//-------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
UpdateVRTagDicomGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{
  UpdateVRMap<Dimension,PixelType>();
  InitVRMap<Dimension,PixelType>();
  char temp[128];
  f.open(m_ArgsInfo.input_arg,std::fstream::binary);
  model.open(m_ArgsInfo.model_arg,std::fstream::binary);
  output.open(m_ArgsInfo.output_arg,std::fstream::binary);

  if(!f.is_open())
  {
    std::cout << "File " << m_ArgsInfo.input_arg << " not found." << std::endl;
    return;
  }
  f.seekg (0);
  model.seekg (0);
  f.read(temp,128); // first 128 bytes in a DICOM file are unused
  model.read(temp,128); // first 128 bytes in a DICOM file are unused
  output.write(temp,128);
  f.read(temp,4);
  // verify that the first 4 bytes are DICM
  if(std::memcmp(temp,"DICM",4))
  {
    std::cout << "Not a valid DICOM file!" << std::endl;
    f.close();
    return;
  }
  model.read(temp,4);
  output.write(temp,4);
  counter = 0;
  size = 0;
  endSQ = 0;

  while(f.peek() != EOF) // process the entire file
  {
    this->ReadDataElement<Dimension,PixelType>(f, model);
  }

  f.close();
  model.close();
  output.close();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
UpdateVRTagDicomGenericFilter<args_info_type>::ReadDataElement(std::ifstream& f, std::ifstream& model)
{

  char temp[8] = {0,0,0,0,0,0,0,0};
  unsigned char SQEndTag[] = {0xFE, 0xFF, 0xDD, 0xE0};

  //Use to convert the output of clitkWriteDicomSeries to the a specific dicom format with the VR tag.
  //Read the input file f and write it in the output.
  //Except for the first 7 lines, we copy the model
  //Except for the 8th line, we remove it.
  //Except for the lines with an unknow VRTag (??)
  //While copy, include the 2 letters of the VRTag
  //For SQ, I had to count the number of byte with size and endSQ

  /*while (f) {
    uint32_t tempChar(0);
    f.read((char*)&tempChar,1);
    std::cout << tempChar << std::endl;
  }
  return;*/

  f.read((char*)&Group,2);
  if (counter <7) model.read((char*)&Group,2);
  f.read((char*)&Element,2);
  if (counter <7) model.read((char*)&Element,2);

  if (counter <7) {
    output.write((char*)&Group,2);
    output.write((char*)&Element,2);
    f.read(temp,2);
    model.read(temp,2);
    output.write(temp,2);
    VR = temp;
  }
  else if (counter == 7) {
    f.read(temp,2);
    VR = temp;
  }
  else {
    std::string VRtag = "(";
    VRtag += int_to_hex(Group);
    std::string tempVR = ",";
    VRtag += tempVR;
    VRtag += int_to_hex(Element);
    tempVR = ")";
    VRtag += tempVR;
    VR = VRMapModel[VRtag];
    if (VR == "??") {
      f.read(temp,4);
      return;
    }
    output.write((char*)&Group,2);
    output.write((char*)&Element,2);
    output.write((char*)&VR[0],1); // Ecrire les 2 lettre separemments en les convertissant en ASCII puis hexa
    output.write((char*)&VR[1],1);
  }
  Length = 0;
  LengthModel = 0;
  switch(VRMap[VR])
  {
    case VR_OB:		// if VR is OB, OW, OF, SQ, UT, or UN, skip two bytes, then read 4 byte length
    case VR_OF:
    case VR_UT:
    case VR_UN:
      f.read(temp,2);
      if (counter <7) model.read(temp,2);
      output.write(temp,2);
      f.read((char*)&Length,4);
      if (counter <7) model.read((char*)&LengthModel,4);
      if (counter <7) output.write((char*)&LengthModel,4); else output.write((char*)&Length,4);
      size += Length + 12;
      break;
    case VR_SQ:
      Value = NULL;
      temp[0] = 0;
      temp[1] = 0;
      output.write(temp,2);
      f.read((char*)&Length,4);
      output.write((char*)&Length,4);
      // this is where we'll be creating new Dicom Objects for SQ data
      // for each Dicom Object in the sequence
      if(Length==0xFFFFFFFF)
      {
        // check the tag before passing to the DicomObj creator
        f.read(temp,4);
        while(std::memcmp(temp,SQEndTag,4))
        {
          f.seekg(-4,std::ios::cur);
          ++endSQ;
          ReadDicomObject<Dimension,PixelType>(f, model);
          --endSQ;
          f.read(temp,4);
        }
        // skip the next four bytes and continue
        output.write(temp,4);
        f.read(temp,4);
        output.write(temp,4);
        size += 20;
      }
      else
      {
        while(size < Length)
        {
          ++endSQ;
          ReadDicomObject<Dimension,PixelType>(f, model);
          --endSQ;
        }
        size += Length + 12;
      }
      if (!endSQ)
        size = 0;
      return;
    case VR_OW:
      f.read(temp,1);
      output.write(temp,1);
      temp[0] = 0;
      temp[1] = 0;
      output.write(temp,2);
      f.read((char*)&Length,4);
      output.write((char*)&Length,4);
      output.write(temp,2);
      size += Length + 12;
    default:
      f.read((char*)&Length,2);
      if (counter <7) model.read((char*)&LengthModel,2);
      if (counter <7) output.write((char*)&LengthModel,2); else if (counter > 7) output.write((char*)&Length,2);
      size += Length + 8;
      break;
  }
  if (counter >=8) {
    f.read(temp,2);
  }
  if(Length)
  {
    Value = new char[Length];
    ValueModel = new char[LengthModel];
    if(Value)
    {
      f.read(Value,Length);
      if (counter <7) model.read(ValueModel,LengthModel);
      if (counter <7) output.write(ValueModel,LengthModel); else if (counter > 7) output.write(Value,Length);
    }
    else
      std::cout << "Error: unable to allocate memory for data element" << std::endl;
  }
  else
    Value = NULL;

  counter++;
  if (!endSQ)
    size = 0;

  return;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
UpdateVRTagDicomGenericFilter<args_info_type>::ReadDicomObject(std::ifstream& f, std::ifstream& model)
{
  char temp[4];
  const unsigned char DicomObjTag[4] = {0xFE,0xFF,0x00,0xE0}; //start of item
  const unsigned char ItemEndTag[4] = {0xFE,0xFF,0x0D,0xE0}; //end of item


  // read tags
  f.read(temp,4);
  output.write(temp,4);
  if(std::memcmp(temp,DicomObjTag,4))
    std::cout << "Nested Dicom Object Tag error\n";
  // read ObjLength
  f.read((char*)&Length,4);
  output.write((char*)&Length,4);

  if(Length == 0xFFFFFFFF)
  {
    // create data elements until an FFFE,E00D tag
    f.read(temp,4);
    while(std::memcmp(temp, ItemEndTag,4))
    {
      f.seekg(-4,std::ios::cur);
      ++endSQ;
      ReadDataElement<Dimension,PixelType>(f, model);
      --endSQ;
      f.read(temp,4);
    }
    output.write(temp,4);
    f.read(temp,4);
    output.write(temp,4);
    size += 16;
  }
  else
  {
    while(size < Length)
    {
      ++endSQ;
      ReadDataElement<Dimension,PixelType>(f, model);
      --endSQ;
    }
    size += Length + 8;
    if(size != Length)
      std::cout << "DicomObj length error\n";
  }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict)
{
  typedef itk::MetaDataDictionary DictionaryType;

  DictionaryType::ConstIterator itr = fromDict.Begin();
  DictionaryType::ConstIterator end = fromDict.End();
  typedef itk::MetaDataObject< std::string > MetaDataStringType;

  while( itr != end )
    {
    itk::MetaDataObjectBase::Pointer  entry = itr->second;

    MetaDataStringType::Pointer entryvalue =
      dynamic_cast<MetaDataStringType *>( entry.GetPointer() ) ;
    if( entryvalue )
      {
      std::string tagkey   = itr->first;
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
      itk::EncapsulateMetaData<std::string>(toDict, tagkey, tagvalue);
      }
    ++itr;
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template <typename T> std::string NumberToString ( T Number )
{
   std::ostringstream ss;
   ss << Number;
   return ss.str();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
std::string string_to_hex( std::string &input )
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template< typename T > std::string int_to_hex( T i )
{
  std::stringstream stream;
  stream << std::setfill ('0') << std::setw(sizeof(T)*2) 
         << std::hex << i;
  return stream.str();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
UpdateVRTagDicomGenericFilter<args_info_type>::InitVRMap()
{
  VRMap["AE"] = VR_AE;
  VRMap["AS"] = VR_AS;
  VRMap["AT"] = VR_AT;
  VRMap["CS"] = VR_CS;
  VRMap["DA"] = VR_DA;
  VRMap["DS"] = VR_DS;
  VRMap["DT"] = VR_DT;
  VRMap["FL"] = VR_FL;
  VRMap["FD"] = VR_FD;
  VRMap["IS"] = VR_IS;
  VRMap["LO"] = VR_LO;
  VRMap["LT"] = VR_LT;
  VRMap["OB"] = VR_OB;
  VRMap["OF"] = VR_OF;
  VRMap["OW"] = VR_OW;
  VRMap["PN"] = VR_PN;
  VRMap["SH"] = VR_SH;
  VRMap["SL"] = VR_SL;
  VRMap["SQ"] = VR_SQ;
  VRMap["SS"] = VR_SS;
  VRMap["ST"] = VR_ST;
  VRMap["TM"] = VR_TM;
  VRMap["UI"] = VR_UI;
  VRMap["UL"] = VR_UL;
  VRMap["UN"] = VR_UN;
  VRMap["US"] = VR_US;
  VRMap["UT"] = VR_UT;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
UpdateVRTagDicomGenericFilter<args_info_type>::UpdateVRMap()
{
  VRMapModel["(0002,0000)"] = "UL";
  VRMapModel["(0002,0001)"] = "OB";
  VRMapModel["(0002,0002)"] = "UI";
  VRMapModel["(0002,0003)"] = "UI";
  VRMapModel["(0002,0010)"] = "UI";
  VRMapModel["(0002,0012)"] = "UI";
  VRMapModel["(0002,0013)"] = "SH";
  VRMapModel["(0008,0005)"] = "CS";
  VRMapModel["(0008,0008)"] = "CS";
  VRMapModel["(0008,0016)"] = "UI";
  VRMapModel["(0008,0018)"] = "UI";
  VRMapModel["(0008,0020)"] = "DA";
  VRMapModel["(0008,0021)"] = "DA";
  VRMapModel["(0008,0022)"] = "DA";
  VRMapModel["(0008,0023)"] = "DA";
  VRMapModel["(0008,002a)"] = "DT";
  VRMapModel["(0008,0030)"] = "TM";
  VRMapModel["(0008,0031)"] = "TM";
  VRMapModel["(0008,0032)"] = "TM";
  VRMapModel["(0008,0033)"] = "TM";
  VRMapModel["(0008,0050)"] = "SH";
  VRMapModel["(0008,0060)"] = "CS";
  VRMapModel["(0008,0070)"] = "LO";
  VRMapModel["(0008,0090)"] = "PN";
  VRMapModel["(0008,1010)"] = "SH";
  VRMapModel["(0008,1030)"] = "LO";
  VRMapModel["(0008,1032)"] = "SQ";
  VRMapModel["(0008,0100)"] = "SH";
  VRMapModel["(0008,0102)"] = "SH";
  VRMapModel["(0008,0103)"] = "SH";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(0008,103e)"] = "LO";
  VRMapModel["(0008,1090)"] = "LO";
  VRMapModel["(0008,1111)"] = "SQ";
  VRMapModel["(0008,1150)"] = "UI";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,1029)"] = "??";
  VRMapModel["(0009,1030)"] = "??";
  VRMapModel["(0009,104d)"] = "??";
  VRMapModel["(0009,104e)"] = "??";
  VRMapModel["(0009,104f)"] = "??";
  VRMapModel["(0009,1050)"] = "??";
  VRMapModel["(0009,1051)"] = "??";
  VRMapModel["(0009,1052)"] = "??";
  VRMapModel["(0009,1053)"] = "??";
  VRMapModel["(0009,1054)"] = "??";
  VRMapModel["(0009,1055)"] = "??";
  VRMapModel["(0009,1056)"] = "??";
  VRMapModel["(0009,1057)"] = "??";
  VRMapModel["(0009,1058)"] = "??";
  VRMapModel["(0009,1059)"] = "??";
  VRMapModel["(0009,105a)"] = "??";
  VRMapModel["(0009,105b)"] = "??";
  VRMapModel["(0009,105c)"] = "??";
  VRMapModel["(0009,105d)"] = "??";
  VRMapModel["(0009,105e)"] = "??";
  VRMapModel["(0009,1069)"] = "??";
  VRMapModel["(0009,106a)"] = "??";
  VRMapModel["(0009,106b)"] = "??";
  VRMapModel["(0009,106c)"] = "??";
  VRMapModel["(0009,106d)"] = "??";
  VRMapModel["(0009,106e)"] = "??";
  VRMapModel["(0009,106f)"] = "??";
  VRMapModel["(0009,1070)"] = "??";
  VRMapModel["(0009,1071)"] = "??";
  VRMapModel["(0009,1087)"] = "??";
  VRMapModel["(0009,1088)"] = "??";
  VRMapModel["(0009,1090)"] = "??";
  VRMapModel["(0009,1091)"] = "??";
  VRMapModel["(0009,1092)"] = "??";
  VRMapModel["(0009,1093)"] = "??";
  VRMapModel["(0009,109a)"] = "??";
  VRMapModel["(0009,10a3)"] = "??";
  VRMapModel["(0009,10a4)"] = "??";
  VRMapModel["(0009,10a9)"] = "??";
  VRMapModel["(0009,10aa)"] = "??";
  VRMapModel["(0009,10ab)"] = "??";
  VRMapModel["(0009,10ac)"] = "??";
  VRMapModel["(0009,10ad)"] = "??";
  VRMapModel["(0009,10ae)"] = "??";
  VRMapModel["(0009,10af)"] = "??";
  VRMapModel["(0009,10b0)"] = "??";
  VRMapModel["(0009,10b1)"] = "??";
  VRMapModel["(0009,10b2)"] = "??";
  VRMapModel["(0009,10ba)"] = "??";
  VRMapModel["(0009,10bb)"] = "SQ";
  VRMapModel["(0009,10bd)"] = "??";
  VRMapModel["(0009,10c2)"] = "??";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,10c4)"] = "OB";
  VRMapModel["(0010,0010)"] = "PN";
  VRMapModel["(0010,0020)"] = "LO";
  VRMapModel["(0010,0030)"] = "DA";
  VRMapModel["(0010,0040)"] = "CS";
  VRMapModel["(0010,1020)"] = "DS";
  VRMapModel["(0010,1030)"] = "DS";
  VRMapModel["(0018,0070)"] = "IS";
  VRMapModel["(0018,0088)"] = "DS";
  VRMapModel["(0018,0071)"] = "CS";
  VRMapModel["(0018,1000)"] = "LO";
  VRMapModel["(0018,1020)"] = "LO";
  VRMapModel["(0018,1030)"] = "LO";
  VRMapModel["(0018,1243)"] = "IS";
  VRMapModel["(0018,5100)"] = "CS";
  VRMapModel["(0020,000d)"] = "UI";
  VRMapModel["(0020,000e)"] = "UI";
  VRMapModel["(0020,0010)"] = "SH";
  VRMapModel["(0020,0011)"] = "IS";
  VRMapModel["(0020,0012)"] = "IS";
  VRMapModel["(0020,0013)"] = "IS";
  VRMapModel["(0020,4000)"] = "LT";
  VRMapModel["(0028,0002)"] = "US";
  VRMapModel["(0028,0004)"] = "CS";
  VRMapModel["(0028,0008)"] = "IS";
  VRMapModel["(0028,0009)"] = "AT";
  VRMapModel["(0028,0010)"] = "US";
  VRMapModel["(0028,0011)"] = "US";
  VRMapModel["(0028,0030)"] = "DS";
  VRMapModel["(0028,0051)"] = "CS";
  VRMapModel["(0028,0100)"] = "US";
  VRMapModel["(0028,0101)"] = "US";
  VRMapModel["(0028,0102)"] = "US";
  VRMapModel["(0028,0103)"] = "US";
  VRMapModel["(0040,0244)"] = "DA";
  VRMapModel["(0040,0245)"] = "TM";
  VRMapModel["(0040,0253)"] = "SH";
  VRMapModel["(0040,0254)"] = "LO";
  VRMapModel["(0040,0260)"] = "SQ";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(0040,0275)"] = "SQ";
  VRMapModel["(0008,0050)"] = "SH";
  VRMapModel["(0020,000d)"] = "UI";
  VRMapModel["(0032,1060)"] = "LO";
  VRMapModel["(0032,1064)"] = "SQ";
  VRMapModel["(0008,0100)"] = "SH";
  VRMapModel["(0008,0102)"] = "SH";
  VRMapModel["(0008,0103)"] = "SH";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(0040,0008)"] = "SQ";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(0040,0009)"] = "SH";
  VRMapModel["(0040,1001)"] = "SH";
  VRMapModel["(0054,0010)"] = "US";
  VRMapModel["(0054,0011)"] = "US";
  VRMapModel["(0054,0012)"] = "SQ";
  VRMapModel["(0054,0013)"] = "SQ";
  VRMapModel["(0054,0014)"] = "DS";
  VRMapModel["(0054,0015)"] = "DS";
  VRMapModel["(0054,0014)"] = "DS";
  VRMapModel["(0054,0015)"] = "DS";
  VRMapModel["(0054,0018)"] = "SH";
  VRMapModel["(0054,0013)"] = "SQ";
  VRMapModel["(0054,0014)"] = "DS";
  VRMapModel["(0054,0015)"] = "DS";
  VRMapModel["(0054,0018)"] = "SH";
  VRMapModel["(0054,0013)"] = "SQ";
  VRMapModel["(0054,0014)"] = "DS";
  VRMapModel["(0054,0015)"] = "DS";
  VRMapModel["(0054,0018)"] = "SH";
  VRMapModel["(0054,0016)"] = "SQ";
  VRMapModel["(0018,0031)"] = "LO";
  VRMapModel["(0018,1071)"] = "DS";
  VRMapModel["(0018,1074)"] = "DS";
  VRMapModel["(0054,0300)"] = "SQ";
  VRMapModel["(0008,0100)"] = "SH";
  VRMapModel["(0008,0102)"] = "SH";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(0054,0302)"] = "SQ";
  VRMapModel["(0008,0100)"] = "SH";
  VRMapModel["(0008,0102)"] = "SH";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(0054,0306)"] = "SQ";
  VRMapModel["(0018,1045)"] = "IS";
  VRMapModel["(0054,0017)"] = "IS";
  VRMapModel["(0054,0308)"] = "US";
  VRMapModel["(0018,1045)"] = "IS";
  VRMapModel["(0054,0017)"] = "IS";
  VRMapModel["(0054,0308)"] = "US";
  VRMapModel["(0018,1045)"] = "IS";
  VRMapModel["(0054,0017)"] = "IS";
  VRMapModel["(0054,0308)"] = "US";
  VRMapModel["(0018,1045)"] = "IS";
  VRMapModel["(0054,0017)"] = "IS";
  VRMapModel["(0054,0308)"] = "US";
  VRMapModel["(0054,0020)"] = "US";
  VRMapModel["(0054,0021)"] = "US";
  VRMapModel["(0054,0022)"] = "SQ";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,1031)"] = "DS";
  VRMapModel["(0009,1032)"] = "SH";
  VRMapModel["(0009,1033)"] = "FD";
  VRMapModel["(0009,1034)"] = "FD";
  VRMapModel["(0009,107c)"] = "FD";
  VRMapModel["(0009,107d)"] = "FD";
  VRMapModel["(0009,107e)"] = "FD";
  VRMapModel["(0009,107f)"] = "FD";
  VRMapModel["(0009,1080)"] = "FD";
  VRMapModel["(0009,1081)"] = "FD";
  VRMapModel["(0009,1082)"] = "FD";
  VRMapModel["(0009,1083)"] = "FD";
  VRMapModel["(0009,1084)"] = "FD";
  VRMapModel["(0009,1085)"] = "FD";
  VRMapModel["(0009,1086)"] = "FD";
  VRMapModel["(0009,10a5)"] = "FD";
  VRMapModel["(0009,10a6)"] = "SL";
  VRMapModel["(0009,10a7)"] = "SL";
  VRMapModel["(0009,10a8)"] = "FD";
  VRMapModel["(0018,1142)"] = "DS";
  VRMapModel["(0018,1147)"] = "CS";
  VRMapModel["(0018,1149)"] = "IS";
  VRMapModel["(0018,1180)"] = "SH";
  VRMapModel["(0018,1181)"] = "CS";
  VRMapModel["(0018,1182)"] = "IS";
  VRMapModel["(0020,0032)"] = "DS";
  VRMapModel["(0020,0037)"] = "DS";
  VRMapModel["(0028,0031)"] = "DS";
  VRMapModel["(0054,0200)"] = "DS";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,1031)"] = "DS";
  VRMapModel["(0009,1032)"] = "SH";
  VRMapModel["(0009,1033)"] = "FD";
  VRMapModel["(0009,1034)"] = "FD";
  VRMapModel["(0009,107c)"] = "FD";
  VRMapModel["(0009,107d)"] = "FD";
  VRMapModel["(0009,107e)"] = "FD";
  VRMapModel["(0009,107f)"] = "FD";
  VRMapModel["(0009,1080)"] = "FD";
  VRMapModel["(0009,1081)"] = "FD";
  VRMapModel["(0009,1082)"] = "FD";
  VRMapModel["(0009,1083)"] = "FD";
  VRMapModel["(0009,1084)"] = "FD";
  VRMapModel["(0009,1085)"] = "FD";
  VRMapModel["(0009,1086)"] = "FD";
  VRMapModel["(0009,10a5)"] = "FD";
  VRMapModel["(0009,10a6)"] = "SL";
  VRMapModel["(0009,10a7)"] = "SL";
  VRMapModel["(0009,10a8)"] = "FD";
  VRMapModel["(0018,1142)"] = "DS";
  VRMapModel["(0018,1147)"] = "CS";
  VRMapModel["(0018,1149)"] = "IS";
  VRMapModel["(0018,1180)"] = "SH";
  VRMapModel["(0018,1181)"] = "CS";
  VRMapModel["(0018,1182)"] = "IS";
  VRMapModel["(0020,0032)"] = "DS";
  VRMapModel["(0020,0037)"] = "DS";
  VRMapModel["(0028,0031)"] = "DS";
  VRMapModel["(0054,0200)"] = "DS";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,1031)"] = "DS";
  VRMapModel["(0009,1032)"] = "SH";
  VRMapModel["(0009,1033)"] = "FD";
  VRMapModel["(0009,1034)"] = "FD";
  VRMapModel["(0009,107c)"] = "FD";
  VRMapModel["(0009,107d)"] = "FD";
  VRMapModel["(0009,107e)"] = "FD";
  VRMapModel["(0009,107f)"] = "FD";
  VRMapModel["(0009,1080)"] = "FD";
  VRMapModel["(0009,1081)"] = "FD";
  VRMapModel["(0009,1082)"] = "FD";
  VRMapModel["(0009,1083)"] = "FD";
  VRMapModel["(0009,1084)"] = "FD";
  VRMapModel["(0009,1085)"] = "FD";
  VRMapModel["(0009,1086)"] = "FD";
  VRMapModel["(0009,10a5)"] = "FD";
  VRMapModel["(0009,10a6)"] = "SL";
  VRMapModel["(0009,10a7)"] = "SL";
  VRMapModel["(0009,10a8)"] = "FD";
  VRMapModel["(0018,1142)"] = "DS";
  VRMapModel["(0018,1147)"] = "CS";
  VRMapModel["(0018,1149)"] = "IS";
  VRMapModel["(0018,1180)"] = "SH";
  VRMapModel["(0018,1181)"] = "CS";
  VRMapModel["(0018,1182)"] = "IS";
  VRMapModel["(0020,0032)"] = "DS";
  VRMapModel["(0020,0037)"] = "DS";
  VRMapModel["(0028,0031)"] = "DS";
  VRMapModel["(0054,0200)"] = "DS";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,1031)"] = "DS";
  VRMapModel["(0009,1032)"] = "SH";
  VRMapModel["(0009,1033)"] = "FD";
  VRMapModel["(0009,1034)"] = "FD";
  VRMapModel["(0009,107c)"] = "FD";
  VRMapModel["(0009,107d)"] = "FD";
  VRMapModel["(0009,107e)"] = "FD";
  VRMapModel["(0009,107f)"] = "FD";
  VRMapModel["(0009,1080)"] = "FD";
  VRMapModel["(0009,1081)"] = "FD";
  VRMapModel["(0009,1082)"] = "FD";
  VRMapModel["(0009,1083)"] = "FD";
  VRMapModel["(0009,1084)"] = "FD";
  VRMapModel["(0009,1085)"] = "FD";
  VRMapModel["(0009,1086)"] = "FD";
  VRMapModel["(0009,10a5)"] = "FD";
  VRMapModel["(0009,10a6)"] = "SL";
  VRMapModel["(0009,10a7)"] = "SL";
  VRMapModel["(0009,10a8)"] = "FD";
  VRMapModel["(0018,1142)"] = "DS";
  VRMapModel["(0018,1147)"] = "CS";
  VRMapModel["(0018,1149)"] = "IS";
  VRMapModel["(0018,1180)"] = "SH";
  VRMapModel["(0018,1181)"] = "CS";
  VRMapModel["(0018,1182)"] = "IS";
  VRMapModel["(0020,0032)"] = "DS";
  VRMapModel["(0020,0037)"] = "DS";
  VRMapModel["(0028,0031)"] = "DS";
  VRMapModel["(0054,0200)"] = "DS";
  VRMapModel["(0054,0050)"] = "US";
  VRMapModel["(0054,0051)"] = "US";
  VRMapModel["(0054,0052)"] = "SQ";
  VRMapModel["(0009,0010)"] = "LO";
  VRMapModel["(0009,1024)"] = "DS";
  VRMapModel["(0009,1025)"] = "DS";
  VRMapModel["(0009,1026)"] = "DS";
  VRMapModel["(0009,1027)"] = "DT";
  VRMapModel["(0009,1028)"] = "DT";
  VRMapModel["(0018,1140)"] = "CS";
  VRMapModel["(0018,1143)"] = "DS";
  VRMapModel["(0018,1144)"] = "DS";
  VRMapModel["(0018,1242)"] = "IS";
  VRMapModel["(0054,0053)"] = "US";
  VRMapModel["(0054,0200)"] = "DS";
  VRMapModel["(0054,0090)"] = "US";
  VRMapModel["(0054,0202)"] = "CS";
  VRMapModel["(0054,0400)"] = "SH";
  VRMapModel["(0054,0410)"] = "SQ";
  VRMapModel["(0008,0100)"] = "SH";
  VRMapModel["(0008,0102)"] = "SH";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(0054,0412)"] = "SQ";
  VRMapModel["(0008,0100)"] = "SH";
  VRMapModel["(0008,0102)"] = "SH";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(0054,0414)"] = "SQ";
  VRMapModel["(0008,0100)"] = "SH";
  VRMapModel["(0008,0102)"] = "SH";
  VRMapModel["(0008,0104)"] = "LO";
  VRMapModel["(7fe0,0010)"] = "OW";
}
//---------------------------------------------------------------------------

}//end clitk

#endif //#define clitkUpdateVRTagDicomGenericFilter_txx
