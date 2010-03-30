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
#ifndef CLITKPORTABILITY_H
#define CLITKPORTABILITY_H
#if defined(WIN32)
#  define _USE_MATH_DEFINES //Before math.h include (i.e. cmath)
#endif
#include <cmath>

#if defined(WIN32)
#  define rint(x)  floor(x+0.5)
#  define lrint(x) (long)rint(x) 
#endif

template<typename T>
int IsNormal(const T val) {
#if defined(_MSC_VER)
  return _finite(val);
#else
  return std::isnormal(val);
#endif
}

#endif /* end #define CLITKPORTABILITY_H */
