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
#ifndef CLITKVFIMAGEIOFACTORY_CXX
#define CLITKVFIMAGEIOFACTORY_CXX

/**
 * @file   clitkVfImageIOFactory.cxx
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Mon Sep 18 10:14:25 2006
 *
 * @brief
 *
 *
 */

#include "clitkVfImageIOFactory.h"

//====================================================================
clitk::VfImageIOFactory::VfImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "VfImageIO",
                         "Vf Image IO",
                         1,
                         itk::CreateObjectFunction<VfImageIO>::New());
}


#endif /* end #define CLITKVFIMAGEIOFACTORY_CXX */

