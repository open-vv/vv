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
#ifndef __clitkMergeSequenceGenericFilter_txx
#define __clitkMergeSequenceGenericFilter_txx
#include "clitkMergeSequenceGenericFilter.h"


namespace clitk
{
  template<unsigned int Dimension>
  void MergeSequenceGenericFilter::UpdateWithDim(std::string PixelType, unsigned int Components)
  {

    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<<Components<<" component(s) of "<<  PixelType<<"..."<<std::endl;

    if (Components==1) {
      if(PixelType == "short") {
        if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, signed short>();
      } else if(PixelType == "unsigned_short") {
         if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
         UpdateWithDimAndPixelType<Dimension, unsigned short>();
      } else if (PixelType == "unsigned_char") {
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, unsigned char>();
      } else if (PixelType == "char"){
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, signed char>();
      } else if(PixelType == "double"){
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and double..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, double>();
      } else {
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, float>();
      }
    } else if (Components==3) {
      if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and 3D float (DVF)" << std::endl;
      UpdateWithDimAndPixelType<Dimension, itk::Vector<float, 3> >();
    } else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;
  }


  template<unsigned int Dimension, class PixelType >
  void MergeSequenceGenericFilter::UpdateWithDimAndPixelType()
  {

    //Define the input and output image type
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<PixelType, Dimension+1> OutputImageType;

    //Read the input image series
    typedef itk::ImageSeriesReader<OutputImageType> ImageReaderType;
    typename  ImageReaderType::Pointer reader= ImageReaderType::New();
    reader->SetFileNames(m_InputNames);
    reader->Update();
    typename OutputImageType::Pointer image =reader->GetOutput();

    //Set the spacing
    typename OutputImageType::SpacingType spacing=image->GetSpacing();
    spacing[Dimension]=m_Spacing;
    image->SetSpacing(spacing);

    //Write the output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_OutputName);
    writer->SetInput(reader->GetOutput());
    writer->Update();

  }


}

#endif

