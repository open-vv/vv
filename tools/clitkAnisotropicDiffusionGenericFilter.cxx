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
#ifndef clitkAnisotropicDiffusionGenericFilter_cxx
#define clitkAnisotropicDiffusionGenericFilter_cxx

#include "clitkAnisotropicDiffusionGenericFilter.h"

// itk include
#include <itkGradientAnisotropicDiffusionImageFilter.h>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>

//#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
AnisotropicDiffusionGenericFilter::AnisotropicDiffusionGenericFilter():
  ImageToImageGenericFilter<Self>("AnisotropicDiffusion")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void AnisotropicDiffusionGenericFilter::InitializeImageType()
{
  ADD_IMAGE_TYPE(Dim, float);
  ADD_IMAGE_TYPE(Dim, double);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void AnisotropicDiffusionGenericFilter::SetArgsInfo(const args_info_type & a)
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
AnisotropicDiffusionGenericFilter::UpdateWithInputImageType()
{
  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  // Filter
  typedef itk::CurvatureAnisotropicDiffusionImageFilter<InputImageType, InputImageType> CADIFType;
  typename CADIFType::Pointer cadFilter = CADIFType::New();
  typedef itk::GradientAnisotropicDiffusionImageFilter<InputImageType, InputImageType> GADIFType;
  typename GADIFType::Pointer gadFilter = GADIFType::New();
  switch(mArgsInfo.type_arg) {
  case type_arg_Gradient:
    gadFilter->SetInput(input);
    gadFilter->SetNumberOfIterations(mArgsInfo.niterations_arg);
    gadFilter->SetTimeStep(mArgsInfo.timestep_arg);
    gadFilter->SetConductanceParameter(mArgsInfo.conductance_arg);
    gadFilter->Update();
    this->template SetNextOutput<InputImageType>(gadFilter->GetOutput());
    break;
  case type_arg_Curvature:
    cadFilter->SetInput(input);
    cadFilter->SetNumberOfIterations(mArgsInfo.niterations_arg);
    cadFilter->SetTimeStep(mArgsInfo.timestep_arg);
    cadFilter->SetConductanceParameter(mArgsInfo.conductance_arg);
    cadFilter->Update();
    this->template SetNextOutput<InputImageType>(cadFilter->GetOutput());
    break;
  default:
    itkGenericExceptionMacro("Unhandled type");
    break;
  }
}
//--------------------------------------------------------------------


}//end clitk

#endif  //#define clitkAnisotropicDiffusionGenericFilter_cxx
