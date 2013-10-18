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
#ifndef clitkImageLaplacianGenericFilter_txx
#define clitkImageLaplacianGenericFilter_txx

/* =================================================
 * @file   clitkImageLaplacianGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

// itk include
#include "itkCastImageFilter.h"
#include "itkLaplacianImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
ImageLaplacianGenericFilter<args_info_type>::ImageLaplacianGenericFilter():
  ImageToImageGenericFilter<Self>("ImageLaplacian")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void ImageLaplacianGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void ImageLaplacianGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
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
  if (mArgsInfo.mask_given) {
    this->AddInputFilename(mArgsInfo.mask_arg);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
ImageLaplacianGenericFilter<args_info_type>::UpdateWithInputImageType()
{
    // Main filter
    typedef typename InputImageType::PixelType InputPixelType;
    typedef itk::Image<float, InputImageType::ImageDimension> FloatImageType;
    typedef itk::Image<unsigned char, FloatImageType::ImageDimension> MaskImageType;

    // Reading input
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

    //Cast input to float
    typedef itk::CastImageFilter< InputImageType, FloatImageType > CastFilterType;
    typename CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(input);
    castFilter->Update();

    typename MaskImageType::Pointer mask = NULL;
    if(mArgsInfo.mask_given) {
        mask = this->template GetInput<MaskImageType>(1);
    }
    else {
        mask = MaskImageType::New();
        mask->SetRegions(input->GetLargestPossibleRegion());
        mask->SetOrigin(input->GetOrigin());
        mask->SetSpacing(input->GetSpacing());
        mask->Allocate();
        mask->FillBuffer(1);
    }


    // Create output image
    typename FloatImageType::Pointer outputImage = FloatImageType::New();
    outputImage->SetRegions(input->GetLargestPossibleRegion());
    outputImage->SetOrigin(input->GetOrigin());
    outputImage->SetSpacing(input->GetSpacing());
    outputImage->Allocate();
    outputImage->FillBuffer(0.0);
    // Set output iterator
    typedef itk::ImageRegionIterator<FloatImageType> IteratorOutputType;
    IteratorOutputType ito = IteratorOutputType(outputImage, outputImage->GetLargestPossibleRegion());

    // Filter
    typedef itk::LaplacianImageFilter<FloatImageType, FloatImageType> LaplacianImageFilterType;
    typename LaplacianImageFilterType::Pointer laplacianFilter=LaplacianImageFilterType::New();
    laplacianFilter->SetInput(castFilter->GetOutput());
    laplacianFilter->Update();
    // Set iterator
    typedef itk::ImageRegionIterator<FloatImageType> IteratorType;
    IteratorType it(laplacianFilter->GetOutput(), laplacianFilter->GetOutput()->GetLargestPossibleRegion());

    // Set mask iterator
    typedef itk::ImageRegionIterator<MaskImageType> IteratorMaskType;
    IteratorMaskType itm(mask, mask->GetLargestPossibleRegion());

    //typedef itk::MinimumMaximumImageCalculator <OutputImageType> ImageCalculatorFilterType;
    //typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();
    //imageCalculatorFilter->SetImage(gradientFilter->GetOutput());
    //imageCalculatorFilter->Compute();
    typedef itk::LabelStatisticsImageFilter< FloatImageType, MaskImageType > LabelStatisticsImageFilterType;
    typename LabelStatisticsImageFilterType::Pointer labelStatisticsImageFilter = LabelStatisticsImageFilterType::New();
    labelStatisticsImageFilter->SetLabelInput( mask );
    labelStatisticsImageFilter->SetInput(laplacianFilter->GetOutput());
    labelStatisticsImageFilter->Update();

    //std::cout << "Number of labels: " << labelStatisticsImageFilter->GetNumberOfLabels() << std::endl;

    float minImg = labelStatisticsImageFilter->GetMinimum(1);
    //std::cout << "minImg: " << minImg << std::endl;
    float maxImg = labelStatisticsImageFilter->GetMaximum(1);
    //std::cout << "maxImg: " << maxImg << std::endl;

    it.GoToBegin();
    ito.GoToBegin();
    itm.GoToBegin();

    while (!ito.IsAtEnd()) {
        if(mArgsInfo.normalize_flag && itm.Get() == 1) {
            ito.Set(((float) it.Get() - minImg)/(maxImg-minImg));
        }
        if (mArgsInfo.normalize_flag == 0 && itm.Get() == 1) {
            ito.Set((float) it.Get());
        }
        ++it;
        ++ito;
        ++itm;
    }

    //typename OutputImageType::Pointer outputImage = gradientFilter->GetOutput();
    this->template SetNextOutput<FloatImageType>(outputImage);
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkImageLaplacianGenericFilter_txx
