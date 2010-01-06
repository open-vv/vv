/*------------------------------------------------------------------------=
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkListOfPair.cxx,v $
  Language:  C++
  Date:      $Date: 2010/01/06 13:32:01 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
------------------------------------------------------------------------=*/


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
  std::map<double, double>::const_iterator i;
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

