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

