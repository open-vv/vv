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
#ifndef CLITKXDRIMAGEIOFACTORY_CXX
#define CLITKXDRIMAGEIOFACTORY_CXX

/**
 * @file   clitkXdrImageIOFactory.cxx
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Sun Jun  1 22:11:20 2008
 *
 * @brief
 *
 *
 */

#include "clitkXdrImageIOFactory.h"

//====================================================================
clitk::XdrImageIOFactory::XdrImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "XdrImageIO",
                         "Xdr Image IO",
                         1,
                         itk::CreateObjectFunction<XdrImageIO>::New());
}


#endif /* end #define CLITKXDRIMAGEIOFACTORY_CXX */

