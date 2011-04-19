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
#ifndef CLITKLISTOFPAIR_H
#define CLITKLISTOFPAIR_H
/**
   ===================================================================
   * @file   clitkListOfPair.h
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   27 Feb 2007 09:44:18

   * @brief  

   ===================================================================*/

#include "clitkCommon.h"

#include <fstream>

namespace clitk {
  
  //====================================================================
  template<class MapType>
  void ReadMap(const std::string & filename, MapType & list, bool inverse=false);
  
  //====================================================================
  double convertValue(double v, 
                      const std::multimap<double, double> & conversionTable, 
                      bool linear);
  
#include "clitkListOfPair.txx"

} // end namespace

#endif /* end #define CLITKLISTOFPAIR_H */

