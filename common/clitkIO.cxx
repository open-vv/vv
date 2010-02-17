
/*-------------------------------------------------------------------------
                                                                                
  Program:   clitk
  Language:  C++
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  -------------------------------------------------------------------------*/

/**
   -------------------------------------------------
   * @file   clitkHisImageIO.cxx
   * @author Simon Rit <simon.rit@gmail.com>
   * @date   16 Feb 2010
   * 
   * @brief  
   * 
   * 
   -------------------------------------------------*/

// std include
#include <iostream>
#include <fstream>

// itk include
#include "itkExceptionObject.h"

// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkVoxImageIO.h"
#include "clitkVoxImageIOFactory.h"
#include "clitkHisImageIO.h"
#include "clitkHisImageIOFactory.h"
#include "clitkVfImageIO.h"
#include "clitkVfImageIOFactory.h"
#include "clitkXdrImageIO.h"
#include "clitkXdrImageIOFactory.h"

//--------------------------------------------------------------------
// Register factories
void clitk::RegisterClitkFactories()
{
    itk::ImageIOFactory::RegisterBuiltInFactories();
    clitk::VoxImageIOFactory::RegisterOneFactory();
    clitk::VfImageIOFactory::RegisterOneFactory();
    clitk::HisImageIOFactory::RegisterOneFactory();
    clitk::XdrImageIOFactory::RegisterOneFactory();
} ////

