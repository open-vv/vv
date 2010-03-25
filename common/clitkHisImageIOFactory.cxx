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

