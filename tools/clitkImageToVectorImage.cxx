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

/* =================================================
 * @file   clitkImageToVectorImage.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk
#include "clitkImageToVectorImage_ggo.h"
#include "clitkIO.h"
#include "clitkImageToVectorImageGenericFilter.h"


//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkImageToVectorImage, args_info);
  CLITK_INIT;

  // Filter
  clitk::ImageToVectorImageGenericFilter::Pointer genericFilter=clitk::ImageToVectorImageGenericFilter::New();
  
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
