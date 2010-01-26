/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joël Schaerer (joel.schaerer@insa-lyon.fr)


Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
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
