/*=========================================================================
                                                                                
  Program:   clitk
  Language:  C++
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/


#ifndef CLITKHISIMAGEIOFACTORY_CXX
#define CLITKHISIMAGEIOFACTORY_CXX

/**
 =================================================
 * @file   clitkHisImageIOFactory.cxx
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   16 Feb 2010
 * 
 * @brief  
 * 
 * 
 =================================================*/

#include "clitkHisImageIOFactory.h"

#include <fstream>

//====================================================================
clitk::HisImageIOFactory::HisImageIOFactory() 
{
  this->RegisterOverride("itkImageIOBase",
                         "HisImageIO",
                         "His Image IO",
                         1,
                         itk::CreateObjectFunction<HisImageIO>::New());
}


#endif /* end #define CLITKHISIMAGEIOFACTORY_CXX */

