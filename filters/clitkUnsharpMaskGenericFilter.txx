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
#ifndef clitkUnsharpMaskGenericFilter_txx
#define clitkUnsharpMaskGenericFilter_txx

/* =================================================
 * @file   clitkUnsharpMaskGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

// itk include
#include "itkRecursiveGaussianImageFilter.h"
#include "itkSubtractImageFilter.h"
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
UnsharpMaskGenericFilter<args_info_type>::UnsharpMaskGenericFilter():
        ImageToImageGenericFilter<Self>("UnsharpMask") {
    InitializeImageType<2>();
    InitializeImageType<3>();
    //InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void UnsharpMaskGenericFilter<args_info_type>::InitializeImageType() {
    ADD_IMAGE_TYPE(Dim, char);
    ADD_IMAGE_TYPE(Dim, uchar);
    ADD_IMAGE_TYPE(Dim, short);
    ADD_IMAGE_TYPE(Dim, ushort);
    ADD_IMAGE_TYPE(Dim, int);
    ADD_IMAGE_TYPE(Dim, float);
    ADD_IMAGE_TYPE(Dim, double);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void UnsharpMaskGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a) {
    mArgsInfo=a;
    SetIOVerbose(mArgsInfo.verbose_flag);
    if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

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
template<class args_info_type>
template<class InputImageType>
void
UnsharpMaskGenericFilter<args_info_type>::UpdateWithInputImageType()
{

    // Reading input
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

    // Main filter
    typedef typename InputImageType::PixelType PixelType;
    typedef itk::Image<float, InputImageType::ImageDimension> OutputImageType;

    // Filter
    typedef itk::RecursiveGaussianImageFilter<InputImageType, OutputImageType> RecursiveGaussianImageFilterType;
    typename RecursiveGaussianImageFilterType::Pointer gaussianFilter=RecursiveGaussianImageFilterType::New();
    gaussianFilter->SetInput(input);
    gaussianFilter->SetSigma(mArgsInfo.sigma_arg);

    typedef itk::SubtractImageFilter<InputImageType, OutputImageType, OutputImageType> SubtractFilterType;
    typename SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();
    subtractFilter->SetInput1(input);
    subtractFilter->SetInput2(gaussianFilter->GetOutput());
    subtractFilter->Update();

    this->template SetNextOutput<OutputImageType>(subtractFilter->GetOutput());
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkUnsharpMaskGenericFilter_txx
