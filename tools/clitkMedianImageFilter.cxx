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
/*------------------------------------------------------------------------

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  ------------------------------------------------------------------------*/

/* =================================================
 * @file   clitkMedianImageGenericFilter.txx
 * @author Bharath Navalpakkam <Bharath.Navalpakkam@creatis.insa-lyon.fr>
 * @date   18 March 2010
 *
 * @brief Apply Median Filter to an Image
 *
 ===================================================*/

// clitk
#include "clitkMedianImageFilter_ggo.h"
#include "clitkMedianImageGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkMedianImageFilter, args_info);
  CLITK_INIT;

  // Filter
  clitk::MedianImageGenericFilter<args_info_clitkMedianImageFilter>::Pointer filter =
    clitk::MedianImageGenericFilter<args_info_clitkMedianImageFilter>::New();


  filter->SetOutputFilename(args_info.output_arg);

  filter->SetArgsInfo(args_info);

  filter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
