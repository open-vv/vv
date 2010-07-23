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
#ifndef clitkBSplineCoefficientsToValuesGenericFilter_cxx
#define clitkBSplineCoefficientsToValuesGenericFilter_cxx

/* =================================================
 * @file   clitkBSplineCoefficientsToValuesGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkBSplineCoefficientsToValuesGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  BSplineCoefficientsToValuesGenericFilter::BSplineCoefficientsToValuesGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }


  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void BSplineCoefficientsToValuesGenericFilter::Update()
  {
   // Read the Dimension and PixelType
    int Dimension, Components;
    std::string PixelType;
    ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType, Components);

    
    // Call UpdateWithDim
    // if(Dimension==2) UpdateWithDim<2>(PixelType, Components);
    if(Dimension==3) UpdateWithDim<3>(PixelType, Components);
    //else if (Dimension==4)UpdateWithDim<4>(PixelType, Components); 
    else 
      {
	std::cout<<"Error, Only for 2, 3 or 4  Dimensions!!!"<<std::endl ;
	return;
      }
  }


} //end clitk

#endif  //#define clitkBSplineCoefficientsToValuesGenericFilter_cxx
