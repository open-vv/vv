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
 * @file   clitkImage2Dicom.cxx
 * @author Thomas B
 * @date   2018
 *
 * @brief Write a volume into a series with the header of another series
 *
 ===================================================*/


// clitk
#include "clitkImage2Dicom_ggo.h"
#include "clitkIO.h"
#include "clitkImage2DicomGenericFilter.h"
#include "clitkCommon.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkImage2Dicom, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::Image2DicomGenericFilter<args_info_clitkImage2Dicom> FilterType;
  FilterType::Pointer genericFilter = FilterType::New();

  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
