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

#ifndef CLITKMEMORYUSAGE_H
#define CLITKMEMORYUSAGE_H

// clitk include
#include "clitkConfiguration.h"

// statgrab include
#if CLITK_MEMORY_INFO == 1
#include <statgrab.h>
#endif

//--------------------------------------------------------------------
namespace clitk {

  void PrintMemory(bool verbose=true, std::string s="") ;
  double GetMemoryUsageInMb() ;

}  // end namespace
//--------------------------------------------------------------------

#endif /* end #define CLITKMEMORYUSAGE_H */

