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
#ifndef CLITKLISTOFPAIR_TXX
#define CLITKLISTOFPAIR_TXX
/**
 =================================================
 * @file   clitkListOfPair.txx
 * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
 * @date   27 Feb 2007 09:44:48
 * 
 * @brief  
 * 
 * 
 =================================================*/

//====================================================================
template<class MapType>
void ReadMap(const std::string & filename, MapType & list, bool inverse) {
  std::ifstream is;
  clitk::openFileForReading(is, filename);
  clitk::skipComment(is);
  typedef typename MapType::key_type KType;
  KType v1;
  typedef typename MapType::mapped_type MType;
  MType v2;
  while (is) {
	is >> v1;
	is >> v2;
	if (is) {
	  if (!inverse) list.insert(std::pair<KType, MType>(v1,v2));
	  else list.insert(std::pair<KType, MType>(v2,v1));	  
	}
	clitk::skipComment(is);
  }
  is.close();
}
//====================================================================

#endif /* end #define CLITKLISTOFPAIR_TXX */

