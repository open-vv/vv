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
#ifndef CLITKLISTOFPAIR_CXX
#define CLITKLISTOFPAIR_CXX
/**
   ------------------------------------------------=
   * @file   clitkListOfPair.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   27 Feb 2007 09:55:56
   * 
   * @brief  
   * 
   * 
   ------------------------------------------------=*/

#include "clitkListOfPair.h"

//--------------------------------------------------------------------
double clitk::convertValue(double v, 
                           const std::multimap<double, double> & conversionTable, 
                           bool linear) {
  std::multimap<double, double>::const_iterator i;
  i = conversionTable.lower_bound(v);  
  if (i == conversionTable.end()) {
    std::cerr << "The value " << v << " is out of the table" << std::endl;
    exit(0);
  }
  
  double v2 = i->first;
  double p2 = i->second;
  if (i != conversionTable.begin()) i--;
  double v1 = i->first;
  double p1 = i->second;

  // interpol
  if (!linear) {
    if ((v-v1) > (v2-v)) return p2;
    else return p1;
  }
  else {
    double w = (v-v1)/(v2-v1);
    return p1*(1.0-w)+w*p2;
  }
}
//--------------------------------------------------------------------

#endif /* end #define CLITKLISTOFPAIR_CXX */

