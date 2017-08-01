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
#ifndef clitkBlurImageGenericFilter_txx
#define clitkBlurImageGenericFilter_txx

/* =================================================
 * @file   clitkBlurImageGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

// itk include
#include "itkDiscreteGaussianImageFilter.h"
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
BlurImageGenericFilter<args_info_type>::BlurImageGenericFilter():
  ImageToImageGenericFilter<Self>("BlurImage")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  //InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void BlurImageGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void BlurImageGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

  if (mArgsInfo.input_given) {
    this->SetInputFilename(mArgsInfo.input_arg);
  }
  if (mArgsInfo.output_given) {
    this->SetOutputFilename(mArgsInfo.output_arg);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
BlurImageGenericFilter<args_info_type>::UpdateWithInputImageType()
{
  const unsigned int dim = InputImageType::ImageDimension;
  double varianceArray[dim];

  //std::cout<<"mArgsInfo.variance_given"<<mArgsInfo.variance_given<<std::endl;


  //if (mArgsInfo.variance_given != dim) {
  //  std::cerr << "The number of variance parameters must be equal to the image dimension." << std::endl;
  //  return;
  //}

  if (mArgsInfo.variance_given && mArgsInfo.variance_given == dim) {
    for (unsigned int i = 0; i < dim; i++) {
      //std::cout<<"mArgsInfo.variance_arg[i]"<<mArgsInfo.variance_arg[i]<<std::endl;
      varianceArray[i] = mArgsInfo.variance_arg[i];
    }
  } else if (mArgsInfo.variance_given && mArgsInfo.variance_given == 1) {
      for (unsigned int i = 0; i < dim; i++) {
          varianceArray[i] = mArgsInfo.variance_arg[0];
      }
  } else {
      for (unsigned int i = 0; i < dim; i++) {
          varianceArray[i] = 1.0;
      }
  }
  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  // Main filter
  typedef typename InputImageType::PixelType PixelType;
  typedef itk::Image<float, InputImageType::ImageDimension> OutputImageType;

  // Filter
  typedef itk::DiscreteGaussianImageFilter<InputImageType, OutputImageType> DiscreteGaussianImageFilterType;
  typename DiscreteGaussianImageFilterType::Pointer gaussianFilter=DiscreteGaussianImageFilterType::New();
  gaussianFilter->SetInput(input);
  gaussianFilter->SetVariance(varianceArray);
  gaussianFilter->Update();

  //std::cout<<"variance value="<<gaussianFilter->GetVariance()<<std::endl;

  // Write/Save results
  this->template SetNextOutput<OutputImageType>(gaussianFilter->GetOutput());
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkBlurImageGenericFilter_txx
