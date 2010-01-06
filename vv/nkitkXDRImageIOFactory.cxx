#ifndef NKITKXDRIMAGEIOFACTORY_CXX
#define NKITKXDRIMAGEIOFACTORY_CXX

/**
 * @file   nkitkXDRImageIOFactory.cxx
 * @author Simon Rit <simon.rit@gmail.com>
 * @date   Sun Jun  1 22:11:20 2008
 *
 * @brief
 *
 *
 */

#include "nkitkXDRImageIOFactory.h"

//====================================================================
nkitk::XDRImageIOFactory::XDRImageIOFactory()
{
    this->RegisterOverride("itkImageIOBase",
                           "XDRImageIO",
                           "XDR Image IO",
                           1,
                           itk::CreateObjectFunction<XDRImageIO>::New());
}


#endif /* end #define NKITKXDRIMAGEIOFACTORY_CXX */

