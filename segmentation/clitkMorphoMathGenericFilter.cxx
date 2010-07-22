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
#ifndef clitkMorphoMathGenericFilter_cxx
#define clitkMorphoMathGenericFilter_cxx

/**
 * @file   clitkMorphoMathGenericFilter.cxx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   May 5  13:47:57 2009
 * 
 * @brief  
 * 
 * 
 */

#include "clitkMorphoMathGenericFilter.h"

namespace clitk
{

  //====================================================================
  // Constructor
  MorphoMathGenericFilter::MorphoMathGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }

  //====================================================================
  // Update
  void MorphoMathGenericFilter::Update()
  {

    //Read the PixelType and dimension of the input image
    int Dimension;
    std::string PixelType;
    ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);

    if(Dimension==2) UpdateWithDim<2>(PixelType);
    else if(Dimension==3) UpdateWithDim<3>(PixelType);
    else if (Dimension==4)UpdateWithDim<4>(PixelType); 
    else 
      {
	std::cout<<"Error, Only for 2, 3 or 4  Dimensions!!!"<<std::endl ;
	return;
      }
  }


  //====================================================================

} //end namespace

#endif  //#define clitkMorphoMathGenericFilter_cxx
