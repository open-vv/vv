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
#ifndef CLITKVOXIMAGEIOFACTORY_CXX
#define CLITKVOXIMAGEIOFACTORY_CXX
/**
 =================================================
 * @file   clitkVoxImageIOFactory.cxx
 * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
 * @date   03 Jul 2006 11:29:10
 *
 * @brief
 *
 *
 =================================================*/

#include "clitkVoxImageIOFactory.h"

#include <fstream>

//====================================================================
clitk::VoxImageIOFactory::VoxImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "VoxImageIO",
                         "Vox Image IO",
                         1,
                         itk::CreateObjectFunction<VoxImageIO>::New());
}


#endif /* end #define CLITKVOXIMAGEIOFACTORY_CXX */

