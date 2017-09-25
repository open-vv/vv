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
 * @file   clitkImage2DicomDose.cxx
 * @author Jef Vandemeulebroucke
 * @date   4th of August
 *
 * @brief Write a volume into a dicom with the header of another dicom
 *
 ===================================================*/


// clitk
#include "clitkImage2DicomDose_ggo.h"
#include "clitkIO.h"
#include "clitkImage2DicomDoseGenericFilter.h"
#include "clitkCommon.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkImage2DicomDose, args_info);
  CLITK_INIT;

  // Filter
  typedef clitk::Image2DicomDoseGenericFilter<args_info_clitkImage2DicomDose> FilterType;
  FilterType::Pointer genericFilter = FilterType::New();

  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
