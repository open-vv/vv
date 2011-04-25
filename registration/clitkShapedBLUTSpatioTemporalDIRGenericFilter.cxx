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
#ifndef clitkShapedBLUTSpatioTemporalDIRGenericFilter_cxx
#define clitkShapedBLUTSpatioTemporalDIRGenericFilter_cxx

/* =================================================
 * @file   clitkShapedBLUTSpatioTemporalDIRGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkShapedBLUTSpatioTemporalDIRGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  ShapedBLUTSpatioTemporalDIRGenericFilter::ShapedBLUTSpatioTemporalDIRGenericFilter()
  {
    m_Verbose=false;
    m_ReferenceFileName="";
  }


  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void ShapedBLUTSpatioTemporalDIRGenericFilter::Update()
  {
    // Read the Dimension and PixelType
    int Dimension;
    std::string PixelType;
    ReadImageDimensionAndPixelType(m_ReferenceFileName, Dimension, PixelType);

    //if(Dimension==3) UpdateWithDim<3>(PixelType);
    //else 
    if (Dimension==4)UpdateWithDim<4>(PixelType); 
    else 
      {
	std::cout<<"Error, Only for 4  Dimensions!!!"<<std::endl ;
	return;
      }
  }


} //end clitk

#endif  //#define clitkShapedBLUTSpatioTemporalDIRGenericFilter_cxx
