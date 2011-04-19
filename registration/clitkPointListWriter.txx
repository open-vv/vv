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
#ifndef __clitkPointListWriter_txx
#define __clitkPointListWriter_txx
#include "clitkPointListWriter.h"

namespace clitk
{

  template<unsigned int Dimension>
  PointListWriter< Dimension >
  ::PointListWriter()
  {
    
  }

  template<unsigned int Dimension>
  void
  PointListWriter< Dimension >
  ::Write(const PointListType& list, const std::string& filename )
  {
    std::ofstream os;
    openFileForWriting(os, filename);
    
    for (unsigned int pointIndex=0; pointIndex<list.size(); pointIndex++)
      {	
	os << list[pointIndex][0];
	for (unsigned int dim=1; dim<Dimension; dim++)
	  os <<"\t"<< list[pointIndex][dim];
	os<<std::endl;
      }
  }
  
  
  template<unsigned int Dimension>
  void
  PointListWriter< Dimension >
  ::Write(const DisplacementListType& list, const std::string& filename )
  {
    std::ofstream os;
    openFileForWriting(os, filename);
    
    for (unsigned int pointIndex=0; pointIndex<list.size(); pointIndex++)
      {	
	os << list[pointIndex][0];
	for (unsigned int dim=1; dim<Dimension; dim++)
	  os <<"\t"<< list[pointIndex][dim];
	os<<std::endl;
      }
  }
  
  
  template<unsigned int Dimension>
  void 
  PointListWriter< Dimension >
  ::Write(const PointListsType& lists, char** filename)
  {
    for (unsigned int number =0; number<lists.size(); number++)
      Write(lists[number],filename[number]);
  }


  template<unsigned int Dimension>
  void 
  PointListWriter< Dimension >
  ::Write(const DisplacementListsType& lists, char** filename)
  {
    for (unsigned int number =0; number<lists.size(); number++)
      Write(lists[number],filename[number]);
  }


}// clitk

#endif // PointListWriter
