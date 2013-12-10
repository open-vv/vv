/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/

// std include
#include <iostream>
#include <fstream>

// itk include
#include "itkMacro.h"

// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkDicomRTDoseIOFactory.h"
#include "clitkVoxImageIOFactory.h"
#include "clitkVfImageIOFactory.h"
#include "clitkXdrImageIOFactory.h"
#include "rtkHisImageIOFactory.h"
#include "rtkHndImageIOFactory.h"
#include "rtkEdfImageIOFactory.h"
#include "rtkImagXImageIOFactory.h"
#include "rtkXRadImageIOFactory.h"
#include "clitkEsrfHstImageIOFactory.h"
#include "clitkGateAsciiImageIOFactory.h"
#include "clitkConfiguration.h"
#if CLITK_PRIVATE_FEATURES
  #include "clitkUsfImageIOFactory.h"
  #include "clitkUSVoxImageIOFactory.h"
  #include "clitkSvlImageIOFactory.h"
#endif
#if ITK_VERSION_MAJOR >= 4
  #include "itkGDCMImageIOFactory.h"
  #include "itkPNGImageIOFactory.h"
#endif

//--------------------------------------------------------------------
// Register factories
void clitk::RegisterClitkFactories()
{
#if ITK_VERSION_MAJOR >= 4
  std::list< itk::ObjectFactoryBase * > fl = itk::GDCMImageIOFactory::GetRegisteredFactories();
  for (std::list< itk::ObjectFactoryBase * >::iterator it = fl.begin(); it != fl.end(); ++it)
    if (dynamic_cast<itk::GDCMImageIOFactory *>(*it))
    {
      itk::GDCMImageIOFactory::UnRegisterFactory(*it);
      break;
    }

  std::list< itk::ObjectFactoryBase * > flpng = itk::PNGImageIOFactory::GetRegisteredFactories();
  for (std::list< itk::ObjectFactoryBase * >::iterator it = flpng.begin(); it != flpng.end(); ++it)
    if (dynamic_cast<itk::PNGImageIOFactory *>(*it))
    {
      itk::PNGImageIOFactory::UnRegisterFactory(*it);
      break;
    }
#endif
#if CLITK_PRIVATE_FEATURES
  clitk::UsfImageIOFactory::RegisterOneFactory();
  clitk::USVoxImageIOFactory::RegisterOneFactory();
  clitk::SvlImageIOFactory::RegisterOneFactory();
#endif
  clitk::GateAsciiImageIOFactory::RegisterOneFactory();
  clitk::DicomRTDoseIOFactory::RegisterOneFactory();
#if ITK_VERSION_MAJOR <= 3
  itk::ImageIOFactory::RegisterBuiltInFactories();
#endif
  clitk::VoxImageIOFactory::RegisterOneFactory();
  clitk::VfImageIOFactory::RegisterOneFactory();
  clitk::XdrImageIOFactory::RegisterOneFactory();
  rtk::HisImageIOFactory::RegisterOneFactory();
  rtk::HndImageIOFactory::RegisterOneFactory();
  rtk::EdfImageIOFactory::RegisterOneFactory();
  rtk::ImagXImageIOFactory::RegisterOneFactory();
  rtk::XRadImageIOFactory::RegisterOneFactory();
  clitk::EsrfHstImageIOFactory::RegisterOneFactory();
#if ITK_VERSION_MAJOR >= 4
  itk::GDCMImageIOFactory::RegisterOneFactory();
  itk::PNGImageIOFactory::RegisterOneFactory();
#endif
} ////

