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
===========================================================================*/
#include "clitkLineProfileGenericFilter.h"

namespace clitk
{
  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  LineProfileGenericFilter::LineProfileGenericFilter()
    : ImageToImageGenericFilter<Self>("Resample")
  {
    m_Verbose=false;

    InitializeImageType<2>();
    InitializeImageType<3>();
    //InitializeImageType<4>();
  }
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  template<unsigned int Dim>
  void LineProfileGenericFilter::InitializeImageType() 
  {      
    ADD_DEFAULT_IMAGE_TYPES(Dim);
    //ADD_IMAGE_TYPE(Dim, short);
  }
  //--------------------------------------------------------------------
    
}