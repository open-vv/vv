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
#ifndef clitkMatrixTransformToVFGenericFilter_cxx
#define clitkMatrixTransformToVFGenericFilter_cxx

/* =================================================
 * @file   clitkMatrixTransformToVFGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkMatrixTransformToVFGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  MatrixTransformToVFGenericFilter::MatrixTransformToVFGenericFilter()
  {
    m_Verbose=false;
   
  }


  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void MatrixTransformToVFGenericFilter::Update()
  {
    // Read the Dimension and PixelType
    int Dimension=m_ArgsInfo.dim_arg;
    //std::string PixelType;
    //ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);
    
    // Call UpdateWithDim
    if(Dimension==2) UpdateWithDim<2>();
    else if(Dimension==3) UpdateWithDim<3>();
    // else if (Dimension==4)UpdateWithDim<4>(PixelType); 
    else 
      {
	std::cout<<"Error, Only for 2 or 3  Dimensions!!!"<<std::endl ;
	return;
      }
  }


} //end clitk

#endif  //#define clitkMatrixTransformToVFGenericFilter_cxx
