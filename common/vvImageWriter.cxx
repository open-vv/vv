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
#ifndef VVIMAGEWRITER_CXX
#define VVIMAGEWRITER_CXX

#include "vvImageWriter.h"
#include "vvImageWriter.txx"

#include <string.h>

//------------------------------------------------------------------------------
vvImageWriter::vvImageWriter()
{
  mImage = NULL;
  mOutputFilename = "";
  mLastError = "";
  mUseAnObserver = false;
  mSaveTransform = false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvImageWriter::~vvImageWriter()
{
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageWriter::Update(int dim,std::string OutputPixelType)
{
  //CALL_FOR_ALL_DIMS(dim,UpdateWithDim,inputPixelType);
  if (dim == 2)
    UpdateWithDim<2>(OutputPixelType);
  else if (dim == 3)
    UpdateWithDim<3>(OutputPixelType);
  else if (dim == 4)
    UpdateWithDim<4>(OutputPixelType);
  else
    std::cerr << "dim not know in Update ! " << std::endl;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageWriter::SetOutputFileName(std::string filename)
{
  mOutputFilename = filename;
}
//------------------------------------------------------------------------------

#endif

