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

