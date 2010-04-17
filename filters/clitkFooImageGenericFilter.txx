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
#ifndef clitkFooImageGenericFilter_txx
#define clitkFooImageGenericFilter_txx

/* =================================================
 * @file   clitkFooImageGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

// itk include
#include "itkBinaryThresholdImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
FooImageGenericFilter<args_info_type>::FooImageGenericFilter():
        ImageToImageGenericFilter<Self>("FooImage") {
    InitializeImageType<2>();
    InitializeImageType<3>();
    InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void FooImageGenericFilter<args_info_type>::InitializeImageType() {
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
void FooImageGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a) {
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
FooImageGenericFilter<args_info_type>::UpdateWithInputImageType()
{

    // Reading input
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

    // Main filter
    typedef typename InputImageType::PixelType PixelType;
    typedef itk::Image<char, InputImageType::ImageDimension> OutputImageType;

    // Filter
    typedef itk::BinaryThresholdImageFilter<InputImageType, OutputImageType> BinaryThresholdImageFilterType;
    typename BinaryThresholdImageFilterType::Pointer thresholdFilter=BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(input);
    thresholdFilter->SetInsideValue(mArgsInfo.fg_arg);

    if (mArgsInfo.lower_given) thresholdFilter->SetLowerThreshold(static_cast<PixelType>(mArgsInfo.lower_arg));
    if (mArgsInfo.upper_given) thresholdFilter->SetUpperThreshold(static_cast<PixelType>(mArgsInfo.upper_arg));

    if (mArgsInfo.mode_arg == std::string("both")) {
        thresholdFilter->SetOutsideValue(mArgsInfo.bg_arg);
        thresholdFilter->Update();

        typename OutputImageType::Pointer outputImage = thresholdFilter->GetOutput();
        this->template SetNextOutput<OutputImageType>(outputImage);
    }
    else {
        typename InputImageType::Pointer outputImage;
        thresholdFilter->SetOutsideValue(0);
        if (mArgsInfo.mode_arg == std::string("BG")) {
            typedef itk::MaskImageFilter<InputImageType,OutputImageType> maskFilterType;
            typename maskFilterType::Pointer maskFilter = maskFilterType::New();
            maskFilter->SetInput1(input);
            maskFilter->SetInput2(thresholdFilter->GetOutput());
            maskFilter->SetOutsideValue(mArgsInfo.bg_arg);
            maskFilter->Update();
            outputImage = maskFilter->GetOutput();
        }
        else {
            typedef itk::MaskNegatedImageFilter<InputImageType,OutputImageType> maskFilterType;
            typename maskFilterType::Pointer maskFilter = maskFilterType::New();
            maskFilter->SetInput1(input);
            maskFilter->SetInput2(thresholdFilter->GetOutput());
            maskFilter->SetOutsideValue(mArgsInfo.fg_arg);
            maskFilter->Update();
            outputImage = maskFilter->GetOutput();
        }
        // Write/Save results
        this->template SetNextOutput<InputImageType>(outputImage);
    }
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkFooImageGenericFilter_txx
