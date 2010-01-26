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

