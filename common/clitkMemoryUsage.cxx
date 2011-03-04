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

// clitk include
#include "clitkCommon.h"
#include "clitkMemoryUsage.h"

// itk include
#include "itkMemoryUsageObserver.h"

void clitk::PrintMemory(bool verbose, std::string s) 
{
#if CLITK_MEMORY_INFO == 1
  if (verbose) {
    static double previous=0;
    double mem = GetMemoryUsageInMb();
    if (s != "") std::cout << "==> " << s << ": ";
    std::cout << mem << "MB (" << mem-previous << "MB)" << std::endl;
    previous = mem;
  }
#endif
}

double clitk::GetMemoryUsageInMb() 
{
  itk::MemoryUsageObserver memUsage;
  return memUsage.GetMemoryUsage() / 1024.;
}

