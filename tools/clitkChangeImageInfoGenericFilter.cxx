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
#ifndef clitkChangeImageInfoGenericFilter_cxx
#define clitkChangeImageInfoGenericFilter_cxx

#include "clitkChangeImageInfoGenericFilter.h"

// itk include
#include <itkChangeInformationImageFilter.h>

namespace clitk
{

//--------------------------------------------------------------------
ChangeImageInfoGenericFilter::ChangeImageInfoGenericFilter():
  ImageToImageGenericFilter<Self>("ChangeImageInfo")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim>
void ChangeImageInfoGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void ChangeImageInfoGenericFilter::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  if (mArgsInfo.verbose_given)
    SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_given && mArgsInfo.imagetypes_flag)
    this->PrintAvailableImageTypes();

  if (mArgsInfo.input_given) {
    SetInputFilename(mArgsInfo.input_arg);
  }
  if (mArgsInfo.output_given) {
    SetOutputFilename(mArgsInfo.output_arg);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class InputImageType>
void
ChangeImageInfoGenericFilter::UpdateWithInputImageType()
{
  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  // Main filter
  typedef typename itk::ChangeInformationImageFilter<InputImageType> CIIFType;
  typename CIIFType::Pointer ciif = CIIFType::New();
  const unsigned int Dimension = InputImageType::ImageDimension;
  ciif->SetInput(input);

  // Direction
  ciif->SetChangeDirection(mArgsInfo.direction_given);
  if( ciif->GetChangeDirection() ) {
    if(mArgsInfo.direction_given != Dimension*Dimension) {
      std::cerr << "You must provide " << Dimension*Dimension
                << " values for --direction."
                << std::endl;
      exit(1);
    }
    typename InputImageType::DirectionType dir;
    for(unsigned int i=0; i<Dimension; i++)
      for(unsigned int j=0; j<Dimension; j++)
         dir[i][j] = mArgsInfo.direction_arg[i*Dimension+j];
    ciif->SetOutputDirection(dir);
  }

  // Spacing
  ciif->SetChangeSpacing(mArgsInfo.spacing_given);
  if( ciif->GetChangeSpacing() ) {
    if(mArgsInfo.spacing_given != Dimension) {
      std::cerr << "You must provide " << Dimension
                << " values for --spacing."
                << std::endl;
      exit(1);
    }
    typename InputImageType::SpacingType spacing;
    for(unsigned int i=0; i<Dimension; i++)
       spacing[i] = mArgsInfo.spacing_arg[i];
    ciif->SetOutputSpacing(spacing);
  }

  // Origin
  ciif->SetChangeOrigin(mArgsInfo.origin_given);
  if( ciif->GetChangeOrigin() ) {
    if(mArgsInfo.origin_given != Dimension) {
      std::cerr << "You must provide " << Dimension
                << " values for --origin."
                << std::endl;
      exit(1);
    }
    typename InputImageType::PointType origin;
    for(unsigned int i=0; i<Dimension; i++)
       origin[i] = mArgsInfo.origin_arg[i];
    ciif->SetOutputOrigin(origin);
  }

  ciif->Update();
  this->template SetNextOutput<InputImageType>(ciif->GetOutput());
}
//--------------------------------------------------------------------

}//end clitk

#endif  //#define clitkChangeImageInfoGenericFilter_cxx
