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
#include "nkitkXDRImageIO.h"
#include "nkitkXDRImageIOFactory.h"
#include "clitkVoxImageIO.h"
#include "clitkVoxImageIOFactory.h"
#include "clitkVfImageIO.h"
#include "clitkVfImageIOFactory.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkByteSwapper.h"


void initialize_IO()
{
  itk::ImageIOFactory::RegisterBuiltInFactories();
  clitk::VoxImageIOFactory::RegisterOneFactory();
  clitk::VfImageIOFactory::RegisterOneFactory();
  nkitk::XDRImageIOFactory::RegisterOneFactory();
}
