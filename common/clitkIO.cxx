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
#include <itkBMPImageIOFactory.h>
#include <itkGDCMImageIOFactory.h>
#include <itkGiplImageIOFactory.h>
#include <itkJPEGImageIOFactory.h>
#include <itkMetaImageIOFactory.h>
#include <itkPNGImageIOFactory.h>
#include <itkStimulateImageIOFactory.h>
#include <itkTIFFImageIOFactory.h>
#include <itkVTKImageIOFactory.h>
#include <itkNiftiImageIO.h>

//--------------------------------------------------------------------
// Register factories
void clitk::RegisterClitkFactories()
{
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
#if CLITK_PRIVATE_FEATURES
  clitk::UsfImageIOFactory::RegisterOneFactory();
  clitk::USVoxImageIOFactory::RegisterOneFactory();
  clitk::SvlImageIOFactory::RegisterOneFactory();
#endif
  clitk::GateAsciiImageIOFactory::RegisterOneFactory();
  clitk::DicomRTDoseIOFactory::RegisterOneFactory();
  clitk::VoxImageIOFactory::RegisterOneFactory();
  clitk::VfImageIOFactory::RegisterOneFactory();
  clitk::XdrImageIOFactory::RegisterOneFactory();
  rtk::HisImageIOFactory::RegisterOneFactory();
  rtk::HndImageIOFactory::RegisterOneFactory();
  rtk::EdfImageIOFactory::RegisterOneFactory();
  rtk::ImagXImageIOFactory::RegisterOneFactory();
  rtk::XRadImageIOFactory::RegisterOneFactory();
  clitk::EsrfHstImageIOFactory::RegisterOneFactory();
  itk::BMPImageIOFactory::RegisterOneFactory();
  itk::GDCMImageIOFactory::RegisterOneFactory();
  itk::GiplImageIOFactory::RegisterOneFactory();
  itk::JPEGImageIOFactory::RegisterOneFactory();
  itk::MetaImageIOFactory::RegisterOneFactory();
  itk::PNGImageIOFactory::RegisterOneFactory();
  itk::StimulateImageIOFactory::RegisterOneFactory();
  itk::TIFFImageIOFactory::RegisterOneFactory();
  itk::VTKImageIOFactory::RegisterOneFactory();
} ////

