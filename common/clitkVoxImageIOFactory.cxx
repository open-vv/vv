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

