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

