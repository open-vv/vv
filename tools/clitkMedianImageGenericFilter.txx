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
===========================================================================*/
#ifndef clitkMedianImageGenericFilter_txx
#define clitkMedianImageGenericFilter_txx

/* =================================================
 * @file   clitkMedianImageGenericFilter.txx
 * @author Bharath Navalapakkam <Bharath.Navalpakkam@creatis.insa-lyon.fr>
 * @date   20 March 2010
 *
 * @brief
 *
 ===================================================*/

// itk include
#include "itkMedianImageFilter.h"
#include "itkImage.h"

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
MedianImageGenericFilter<args_info_type>::MedianImageGenericFilter():
  ImageToImageGenericFilter<Self>("MedianImage")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void MedianImageGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void MedianImageGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  this->SetIOVerbose(mArgsInfo.verbose_flag);

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
MedianImageGenericFilter<args_info_type>::UpdateWithInputImageType()
{
  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
  // Typedef
  typedef typename InputImageType::PixelType PixelType;

  // typedef itk::Image<PixelType,InputImageType::ImageDimension> OutputImageType;

  // Filter
  typedef itk::MedianImageFilter<InputImageType, InputImageType> MedianImageFilterType;
  typename MedianImageFilterType::Pointer medianFilter=MedianImageFilterType::New();
  typename MedianImageFilterType::InputSizeType radius;
  radius.Fill(1);
  medianFilter->SetInput(input);

  if(mArgsInfo.radius_given) {
      for (unsigned i = 0; i < InputImageType::ImageDimension; ++i)
          radius[i]=mArgsInfo.radius_arg[i];
  }
  //
  std::cout<<"radius median filter= "<<radius<<std::endl;
  //
  medianFilter->SetRadius( radius );

  medianFilter->Update();
  // Write/Save results
  typename InputImageType::Pointer outputImage = medianFilter->GetOutput();
  this->template SetNextOutput<InputImageType>(outputImage);
}

//--------------------------------------------------------------------

}//end clitk


#endif //#define clitkMedianImageGenericFilter_txx

