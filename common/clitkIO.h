#ifndef CLITKIO_H
#define CLITKIO_H

/**
   -------------------------------------------------------------------
   * @file   clitkIO.h
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   17 May 2006 07:57:56

   * @brief  

   -------------------------------------------------------------------*/

// std include
#include <iostream>
#include <fstream>

// clitk include
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkVoxImageIO.h"
#include "clitkVoxImageIOFactory.h"
#include "clitkHisImageIO.h"
#include "clitkHisImageIOFactory.h"
#include "clitkVfImageIO.h"
#include "clitkVfImageIOFactory.h"

//--------------------------------------------------------------------
// CLITK_INIT
#define CLITK_INIT					\
  itk::ImageIOFactory::RegisterBuiltInFactories();	\
    clitk::VoxImageIOFactory::RegisterOneFactory();	\
    clitk::VfImageIOFactory::RegisterOneFactory();	\
    clitk::HisImageIOFactory::RegisterOneFactory();

#endif /* end #define CLITKIO_H */

