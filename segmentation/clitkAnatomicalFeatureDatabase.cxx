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

// clitk
#include "clitkAnatomicalFeatureDatabase.h"

// std
#include <iterator>
#include <sstream>

//--------------------------------------------------------------------
clitk::AnatomicalFeatureDatabase::AnatomicalFeatureDatabase() 
{ 
  SetFilename("noname.afdb"); 
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
void clitk::AnatomicalFeatureDatabase::Load() 
{
  // open file
  std::ifstream is;
  openFileForReading(is, GetFilename());
  // load line by line string/string
  while (!is.fail()) {
    std::string tag;
    is >> tag; DD(tag);
    std::string value;
    std::getline(is,value,'\n');
    m_MapOfTag[tag] = value;
  }
}
//--------------------------------------------------------------------

   
//--------------------------------------------------------------------
void clitk::AnatomicalFeatureDatabase::SetPoint3D(std::string tag, PointType3D & p)
{
  ::itk::OStringStream value;
  value << p[0] << " " << p[1] << " " << p[2];
  m_MapOfTag[tag] = value.str();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::AnatomicalFeatureDatabase::GetPoint3D(std::string tag, PointType3D & p)
{
  if (m_MapOfTag.find(tag) == m_MapOfTag.end()) {
    clitkExceptionMacro("Could not find the tag <" << tag << "> of type Point3D in the DB");
  }
  else {
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
      DD(results[i]);
      p[i] = atof(results[i].c_str());
      DD(p[i]);
    }

    /*
    // boost 
    #include <boost/foreach.hpp>
    #include <boost/tokenizer.hpp>
    // parse the string into 3 doubles
    boost::char_separator<char> sep(", ");
    boost::tokenizer<boost::char_separator<char> > tokens(s, sep);
    int i=0;
    BOOST_FOREACH(std::string t, tokens) {
      std::cout << t << "." << std::endl;
      p[i] = atof(t.c_str());
      i++;
    }
    */
  }
}
//--------------------------------------------------------------------

