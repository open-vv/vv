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
/**
 -------------------------------------------------------------------
 * @file   clitkSplitImageGenericFilter.cxx
 * @author Joël Schaerer
 * @date   20 April 2009

 * @brief
 -------------------------------------------------------------------*/

#include "clitkSplitImageGenericFilter.h"
#include "itkChangeInformationImageFilter.h"
#include <itkIntensityWindowingImageFilter.h>
//--------------------------------------------------------------------
clitk::SplitImageGenericFilter::SplitImageGenericFilter():
  clitk::ImageToImageGenericFilter<Self>("SplitImage")
{
  mSplitDimension = 0;
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::SplitImageGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
  ADD_VEC_IMAGE_TYPE(Dim, 3,float);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
typename clitk::SplitImageGenericFilter::PngConversion<ImageType>::OutputPngImagePointer
clitk::SplitImageGenericFilter::PngConversion<ImageType>::Do(double window,
                                                             double level,
                                                             ImagePointer input)
{
  static const unsigned int PixelDimension = itk::PixelTraits<typename ImageType::PixelType>::Dimension;
  return this->Do(window, level, input, static_cast< PixelDimType<PixelDimension> *>(ITK_NULLPTR) );
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
template<unsigned int Dim>
typename clitk::SplitImageGenericFilter::PngConversion<ImageType>::OutputPngImagePointer
clitk::SplitImageGenericFilter::PngConversion<ImageType>::Do(double window,
                                                             double level,
                                                             ImagePointer input,
                                                             PixelDimType<Dim> *)
{
  clitkExceptionMacro("Png conversion is not implemented for vector fields");
  return ITK_NULLPTR;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
typename clitk::SplitImageGenericFilter::PngConversion<ImageType>::OutputPngImagePointer
clitk::SplitImageGenericFilter::PngConversion<ImageType>::Do(double window,
                                                             double level,
                                                             ImagePointer input,
                                                             PixelDimType<1> *)
{
  typedef itk::IntensityWindowingImageFilter< ImageType, OutputPngImageType > CastFilterType;
  typename CastFilterType::Pointer cast = CastFilterType::New();
  cast->SetWindowLevel(window, level);
  cast->SetInput(input);
  cast->Update();
  return cast->GetOutput();
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
void clitk::SplitImageGenericFilter::UpdateWithInputImageType()
{

  // Read input
  typedef typename ImageType::PixelType PixelType;
  typedef itk::Image<PixelType,ImageType::ImageDimension-1> OutputImageType;
  typename ImageType::Pointer input = this->GetInput<ImageType>(0);
  typedef itk::ExtractImageFilter<ImageType,OutputImageType> FilterType;
  typename FilterType::Pointer filter= FilterType::New();

  filter->SetInput(input);
  typename ImageType::SizeType size=input->GetLargestPossibleRegion().GetSize();
  size[mSplitDimension]=0;
  typename ImageType::RegionType extracted_region;
  extracted_region.SetSize(size);
  filter->SetDirectionCollapseToIdentity();
  filter->SetExtractionRegion(extracted_region);
  filter->Update();

  typedef itk::ChangeInformationImageFilter< OutputImageType > InformationFilterType;

  typename ImageType::IndexType index=input->GetLargestPossibleRegion().GetIndex();
  std::string base_filename=GetOutputFilename();
  unsigned int number_of_output_images=input->GetLargestPossibleRegion().GetSize()[mSplitDimension];
  for (unsigned int i=0; i<number_of_output_images; i++) {
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw((int)(log10(double(number_of_output_images))+1)) << i;
    index[mSplitDimension]=i;
    extracted_region.SetIndex(index);
    filter->SetExtractionRegion(extracted_region);
    filter->Update();

    if (ImageType::ImageDimension == 4 && mSplitDimension == 3) //Copy the transformation matrix if the original image is a 4D image splitted along the time dimension
    {
      typename InformationFilterType::Pointer informationFilter = InformationFilterType::New();
      informationFilter->SetInput( filter->GetOutput() );
      typename OutputImageType::DirectionType directionOutput;
      for (unsigned j=0; j<3; ++j)
        for (unsigned k=0; k<3; ++k)
          directionOutput[j][k] = input->GetDirection()[j][k];
      informationFilter->SetOutputDirection( directionOutput );
      informationFilter->ChangeDirectionOn();
      informationFilter->UpdateOutputInformation();

      if(this->m_Png){
        PngConversion<OutputImageType> png;
        SetOutputFilename(base_filename+"_"+ss.str()+".png");
        typename PngConversion<OutputImageType>::OutputPngImagePointer output;
        output = png.Do(this->m_Window, this->m_Level, informationFilter->GetOutput());
        this->template SetNextOutput<typename PngConversion<OutputImageType>::OutputPngImageType>(output);
      }
      else if(this->m_mha){
          SetOutputFilename(base_filename+"_"+ss.str()+".mha");
          SetNextOutput<OutputImageType>(informationFilter->GetOutput());
      }
      else if(this->m_nii){
          SetOutputFilename(base_filename+"_"+ss.str()+".nii");
          SetNextOutput<OutputImageType>(informationFilter->GetOutput());
      }
      else if(this->m_niigz){
          SetOutputFilename(base_filename+"_"+ss.str()+".nii.gz");
          SetNextOutput<OutputImageType>(informationFilter->GetOutput());
      }
      else {
        SetOutputFilename(base_filename+"_"+ss.str()+".mhd");
        SetNextOutput<OutputImageType>(informationFilter->GetOutput());
      }
    }
    else
    {
      if(this->m_Png){
        PngConversion<OutputImageType> png;
        SetOutputFilename(base_filename+"_"+ss.str()+".png");
        typename PngConversion<OutputImageType>::OutputPngImagePointer output;
        output = png.Do(this->m_Window, this->m_Level, filter->GetOutput());
        this->template SetNextOutput<typename PngConversion<OutputImageType>::OutputPngImageType>(output);
      }
      else if(this->m_mha){
          SetOutputFilename(base_filename+"_"+ss.str()+".mha");
          SetNextOutput<OutputImageType>(filter->GetOutput());
      }
      else if(this->m_nii){
          SetOutputFilename(base_filename+"_"+ss.str()+".nii");
          SetNextOutput<OutputImageType>(filter->GetOutput());
      }
      else if(this->m_niigz){
          SetOutputFilename(base_filename+"_"+ss.str()+".nii.gz");
          SetNextOutput<OutputImageType>(filter->GetOutput());
      }
      else {
        SetOutputFilename(base_filename+"_"+ss.str()+".mhd");
        SetNextOutput<OutputImageType>(filter->GetOutput());
      }
    }
  }
}
//--------------------------------------------------------------------
