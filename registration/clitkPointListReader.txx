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
#ifndef __clitkPointListReader_txx
#define __clitkPointListReader_txx
#include "clitkPointListReader.h"

namespace clitk
{

  template<unsigned int Dimension>
  PointListReader< Dimension >
  ::PointListReader()
  {
    m_Verbose=false;
  }

  template<unsigned int Dimension>
  typename PointListReader< Dimension >::PointListType
  PointListReader< Dimension >
  ::Read(const std::string& filename)
  {
    std::ifstream is;
    openFileForReading(is, filename);
    PointListType list;
    PointType point;
    double x;

    if(m_Verbose)std::cout<<"Reading "<<filename<<"..."<<std::endl;
    for (unsigned int dim=0; dim<Dimension; dim++)
      {
	skipComment(is);
	is >> x;
	point[dim]=x;
      }
    while (!is.eof()) 
      {
	list.push_back(point);
	for (unsigned int dim=0; dim<Dimension; dim++)
	  {
	    skipComment(is);
	    is >> x;
	    point[dim]=x;
	  }
      }
    return list;
  }
    
  template<unsigned int Dimension>
  typename PointListReader< Dimension >::PointListsType
  PointListReader< Dimension >
  ::Read(char** filename, const unsigned int& n)
  {
    PointListsType lists;
    for (unsigned int number=0; number<n; number++)
      lists.push_back(Read(filename[number]) );
    return lists;
  }

}// clitk
#endif // PointListReader
