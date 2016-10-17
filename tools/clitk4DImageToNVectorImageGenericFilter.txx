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
#ifndef clitk4DImageToNVectorImageGenericFilter_txx
#define clitk4DImageToNVectorImageGenericFilter_txx

/* =================================================
 * @file   clitk4DImageToNVectorImageGenericFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "itkComposeImageFilter.h"
#include "itkExtractImageFilter.h"

namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<unsigned int Dimension>
  void 
  FourDImageToNVectorImageGenericFilter::UpdateWithDim(std::string PixelType, unsigned int Components)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

    if (PixelType == "short") {
      UpdateWithDimAndPixelType<Dimension, short>();
    } else if (PixelType == "unsigned short") {
      UpdateWithDimAndPixelType<Dimension, unsigned short>();
    } else if (PixelType == "unsigned_short") {
      UpdateWithDimAndPixelType<Dimension, unsigned short>();
    } else if (PixelType == "char") {
      UpdateWithDimAndPixelType<Dimension, char>();
    } else if (PixelType == "unsigned_char") {
      UpdateWithDimAndPixelType<Dimension, unsigned char>();
    } else if (PixelType == "int") {
      UpdateWithDimAndPixelType<Dimension, int>();
    } else if (PixelType == "unsigned_int") {
      UpdateWithDimAndPixelType<Dimension, unsigned int>();
    } else if (PixelType == "double") {
      UpdateWithDimAndPixelType<Dimension, double>();
    } else if (PixelType == "float") {
      UpdateWithDimAndPixelType<Dimension, float>();
    } else {
      std::cerr << "Error, pixel type : \"" << PixelType << "\" unknown !" << std::endl;
    }
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  FourDImageToNVectorImageGenericFilter::UpdateWithDimAndPixelType()
  {
    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<PixelType, Dimension-1> MedianImageType;
    typedef itk::VectorImage<PixelType, Dimension-1> OutputImageType;

    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName(m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();
    std::string fileName=m_ArgsInfo.output_arg;

    //Filter
    typedef itk::ComposeImageFilter<MedianImageType> ImageToVectorImageFilterType;
    typename ImageToVectorImageFilterType::Pointer imageToVectorImageFilter = ImageToVectorImageFilterType::New();
    typedef itk::ExtractImageFilter<InputImageType, MedianImageType> ExtractFilterType;
    typename ExtractFilterType::Pointer extractFilter = ExtractFilterType::New();
    extractFilter->SetDirectionCollapseToSubmatrix();
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();

    extractFilter->SetInput(input);
    typename InputImageType::SizeType size;
    for (unsigned int nbDimension=0; nbDimension<Dimension-1; ++nbDimension)
    {
        size[nbDimension] = input->GetLargestPossibleRegion().GetSize(nbDimension);
    }
    size[Dimension-1] = 0;
    typename MedianImageType::Pointer tempImage = MedianImageType::New();
    
    //Extract All "time" slices
    for (unsigned int nbSlice=0; nbSlice<input->GetLargestPossibleRegion().GetSize(Dimension-1); ++nbSlice)
    {
      typename InputImageType::RegionType desiredRegion;
      typename InputImageType::IndexType index;
      index.Fill(0);
      index[Dimension-1]=nbSlice;
      desiredRegion.SetSize(size);
      extractFilter->SetInput(input);
      desiredRegion.SetIndex(index);
      extractFilter->SetExtractionRegion(desiredRegion);
      extractFilter->Update();
      tempImage = extractFilter->GetOutput();
      tempImage->DisconnectPipeline();
      extractFilter->Update();
      imageToVectorImageFilter->SetInput(nbSlice, tempImage);
    }

    imageToVectorImageFilter->Update();

    // Output
    writer->SetInput(imageToVectorImageFilter->GetOutput());
    writer->SetFileName(fileName);
    writer->Update();
  }
}//end clitk
 
#endif //#define clitk4DImageToNVectorImageGenericFilter_txx
