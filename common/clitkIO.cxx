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

