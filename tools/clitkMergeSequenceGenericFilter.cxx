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
#ifndef CLITKMERGESEQUENCEGENERICFILTER_CXX
#define CLITKMERGESEQUENCEGENERICFILTER_CXX
#include "clitkMergeSequenceGenericFilter.h"


namespace clitk {

  clitk::MergeSequenceGenericFilter::MergeSequenceGenericFilter()
  {
    m_Verbose=false;
    m_Spacing=1;
  }


  void clitk::MergeSequenceGenericFilter::Update()
  {
    //Get the image Dimension and PixelType
    int Dimension, Components;
    std::string PixelType;
    
    clitk::ReadImageDimensionAndPixelType(m_InputNames[0], Dimension, PixelType, Components);

    if(Dimension==2) UpdateWithDim<2>(PixelType, Components);
    else if(Dimension==3) UpdateWithDim<3>(PixelType, Components);
    else 
      {
	std::cout<<"Error, Only for 2 and 3 Dimensions!!!"<<std::endl ;
	return;
      }
  }
} //end namespace

#endif
