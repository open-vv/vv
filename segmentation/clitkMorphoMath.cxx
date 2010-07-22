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
#ifndef clitkMorphoMath_cxx
#define clitkMorphoMath_cxx
/**
   =================================================
   * @file   clitkMorphoMath.cxx
   * @author Jef Vandemeulebroucke <jefvdmb@gmail.com>
   * @date   5 May 2009
   * 
   * @brief  
   =================================================*/

// clitk include
#include "clitkMorphoMath_ggo.h"
#include "clitkMorphoMathGenericFilter.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"

int main(int argc, char * argv[]) {

  //Init command line
  GGO(clitkMorphoMath,args_info);
  CLITK_INIT;

  //Creation of a generic filter
  clitk::MorphoMathGenericFilter::Pointer genericFilter = clitk::MorphoMathGenericFilter::New();

  //Passing the arguments to the generic filter
  genericFilter->SetArgsInfo(args_info);
 
  //update
  genericFilter->Update();
  return 0;
}

#endif
