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

/* =================================================
 * @file   clitkFooImageGenericFilter.txx
 * @author xxx <xxx@creatis.insa-lyon.fr>
 * @date   29 June 2029
 *
 * @brief FooImage an image
 *
 ===================================================*/

// clitk
#include "clitkFooImage_ggo.h"
#include "clitkFooImageGenericFilter.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

    // Init command line
    GGO(clitkFooImage, args_info);
    CLITK_INIT;

    // Filter
    typedef clitk::FooImageGenericFilter<args_info_clitkFooImage> FilterType;
    FilterType::Pointer filter = FilterType::New();

    filter->SetArgsInfo(args_info);
    filter->Update();

    return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
