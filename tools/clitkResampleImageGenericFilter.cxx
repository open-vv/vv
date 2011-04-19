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

// clitk
#include "clitkResampleImageGenericFilter.h"
#include "clitkResampleImageWithOptionsFilter.h"

//--------------------------------------------------------------------
clitk::ResampleImageGenericFilter::ResampleImageGenericFilter():
  ImageToImageGenericFilter<Self>("Resample") 
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::ResampleImageGenericFilter::InitializeImageType() 
{      
  ADD_DEFAULT_IMAGE_TYPES(Dim);
  ADD_IMAGE_TYPE(Dim, short);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
void clitk::ResampleImageGenericFilter::SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.input_given) {
    SetInputFilename(mArgsInfo.input_arg);
  }
  if (mArgsInfo.output_given) {
    SetOutputFilename(mArgsInfo.output_arg);
  }
}
//--------------------------------------------------------------------

