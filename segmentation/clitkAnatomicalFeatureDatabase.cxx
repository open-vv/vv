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

// clitk
#include "clitkAnatomicalFeatureDatabase.h"

// std
#include <iterator>
#include <sstream>
#include <cctype>
#include <functional>

//--------------------------------------------------------------------
clitk::AnatomicalFeatureDatabase::AnatomicalFeatureDatabase() 
{ 
  SetFilename("default.afdb"); 
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::AnatomicalFeatureDatabase::Write() 
{
  // open file
  std::ofstream os;
  openFileForWriting(os, GetFilename());
  MapTagType::const_iterator iter = m_MapOfTag.begin();
  while (iter != m_MapOfTag.end()) {
    os << iter->first << " " << iter->second << std::endl;
    iter++;
  }
  os.close();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start
static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}
// trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}
// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::AnatomicalFeatureDatabase::Load() 
{
  m_MapOfTag.clear();
  // open file
  std::ifstream is;
  openFileForReading(is, GetFilename());
  // load line by line string/string
  while (!is.fail()) {
    std::string tag;
    is >> tag; 
    if (tag != "") {
      std::string value;
      std::getline(is,value,'\n');
      ltrim(value); // remove leading space
      m_MapOfTag[tag] = value;
    }
  }
  is.close();
}
//--------------------------------------------------------------------

   
//--------------------------------------------------------------------
void clitk::AnatomicalFeatureDatabase::SetPoint3D(std::string tag, PointType3D & p)
{
#if ITK_VERSION_MAJOR > 3
  std::ostringstream value;
#else
  ::itk::OStringStream value;
#endif
  value << p[0] << " " << p[1] << " " << p[2];
  m_MapOfTag[tag] = value.str();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double clitk::AnatomicalFeatureDatabase::GetPoint3D(std::string tag, int dim)
{
  PointType3D p;
  GetPoint3D(tag, p);
  return p[dim];
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::AnatomicalFeatureDatabase::GetPoint3D(std::string tag, PointType3D & p)
{
  if (!TagExist(tag)) {
    clitkExceptionMacro("Could not find the tag <" << tag << "> of type Point3D in the DB");
    return;
  }

  std::string s = m_MapOfTag[tag];
    
  // construct a stream from the string
  std::stringstream strstr(s);

  // use stream iterators to copy the stream to the vector as
  // whitespace separated strings
  std::istream_iterator<std::string> it(strstr);
  std::istream_iterator<std::string> end;
  std::vector<std::string> results(it, end);

  // parse the string into 3 doubles
  for(int i=0; i<3; i++) {

    if (!clitk::fromString<double>(p[i], results[i].c_str())) {
      clitkExceptionMacro("Error while reading Point3D, could not convert '" 
                          << results[i].c_str() << "' into double.");
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::AnatomicalFeatureDatabase::SetImageFilename(std::string tag, std::string f)
{
  m_MapOfTag[tag] = f;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool clitk::AnatomicalFeatureDatabase::TagExist(std::string tag)
{
  return (m_MapOfTag.find(tag) != m_MapOfTag.end());
}
//--------------------------------------------------------------------

//-------------------------------------------------------------------- 
void clitk::AnatomicalFeatureDatabase::SetDouble(std::string tag, double value)
{
  m_MapOfTag[tag] = clitk::toString(value);
}
//-------------------------------------------------------------------- 

//-------------------------------------------------------------------- 
double clitk::AnatomicalFeatureDatabase::GetDouble(std::string tag)
{
  if (!TagExist(tag)) {
    clitkExceptionMacro("Could not find the tag <" << tag << "> of type Double in the DB");
    return -1;
  }

  double a;
  if (!clitk::fromString<double>(a, m_MapOfTag[tag])) {
    clitkExceptionMacro("Error while reading Double (tag='" << tag << "'), could not convert '" 
                        << m_MapOfTag[tag] << "' into double.");
  }
  return a;  
}
//-------------------------------------------------------------------- 
