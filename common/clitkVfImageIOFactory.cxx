/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkVfImageIOFactory.cxx,v $
  Language:  C++
  Date:      $Date: 2010/01/06 13:32:01 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/


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

