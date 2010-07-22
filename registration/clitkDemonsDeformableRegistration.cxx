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
/**
 * @file   clitkDemonsDeformableRegistration.cxx
 * @author Jef Vandemeulebroucke <jefvdmb@gmail.com>
 * @date   April 24  10:14:53 2009
 * 
 * @brief  Demons deformable registration 
 * 
 */


// clitk include
//#include "clitkDemonsDeformableRegistration_ggo.h"
#include "clitkIO.h"
#include "clitkDemonsDeformableRegistrationGenericFilter.h"
//#include "clitkImageCommon.h"


int main( int argc, char *argv[] )
{
  
  GGO(clitkDemonsDeformableRegistration,args_info);
  CLITK_INIT;
  
  //generic Filter
  clitk::DemonsDeformableRegistrationGenericFilter::Pointer genericFilter = clitk::DemonsDeformableRegistrationGenericFilter::New();
  genericFilter->SetArgsInfo(args_info);
  genericFilter->Update();
  
  return EXIT_SUCCESS; 
}
