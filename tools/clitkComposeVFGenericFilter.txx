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
#ifndef __clitkComposeVFGenericFilter_txx
#define __clitkComposeVFGenericFilter_txx
#include "clitkComposeVFGenericFilter.h"

#include "clitkConvertBLUTCoeffsToVFFilter.h"

namespace clitk
{

  template<unsigned int Dimension>
  void ComposeVFGenericFilter::UpdateWithDim(std::string PixelType)
  {
    if (PixelType=="double")
      {
	UpdateWithDimAndPixelType<Dimension, double>();
      }     
    else
      {
	UpdateWithDimAndPixelType<Dimension, float>();
      }
  }

  
  template<unsigned int Dimension, class PixelType>
  void ComposeVFGenericFilter::UpdateWithDimAndPixelType()
  {
    typedef itk::Vector<PixelType, Dimension> DisplacementType;
    typedef itk::Image<DisplacementType, Dimension> ImageType;
    typename ImageType::Pointer input1, input2;

    //Define the image type
    if (m_Type == 1) {
      typedef ConvertBLUTCoeffsToVFFilter<ImageType> VFFilterType;
      typename VFFilterType::Pointer vf_filter = VFFilterType::New();
      vf_filter->SetInputFileName(m_InputName1);
      vf_filter->SetLikeFileName(m_LikeImage);
      vf_filter->SetVerbose(m_Verbose);
      vf_filter->Update();
      input1 = vf_filter->GetOutput();

      vf_filter->SetInputFileName(m_InputName2);
      vf_filter->Update();
      input2 = vf_filter->GetOutput();
    }
    else {
      //Read the input1
      typedef itk::ImageFileReader<ImageType> ImageReaderType;
      typename  ImageReaderType::Pointer reader1= ImageReaderType::New();
      reader1->SetFileName(m_InputName1);
      reader1->Update();
      input1 =reader1->GetOutput();

      //Read the input2
      typename  ImageReaderType::Pointer reader2= ImageReaderType::New();
      reader2->SetFileName(m_InputName2);
      reader2->Update();
      input2=reader2->GetOutput();
    }

      //Create the ComposeVFFilter
      typedef clitk::ComposeVFFilter<ImageType,ImageType> FilterType;
      typename FilterType::Pointer filter =FilterType::New();
      filter->SetInput1(input1);
      filter->SetInput2(input2);
      filter->SetVerbose(m_Verbose);
      filter->Update();

      //Write the output
      typedef itk::ImageFileWriter<ImageType> WriterType;
      typename WriterType::Pointer writer = WriterType::New();
      writer->SetFileName(m_OutputName);
      writer->SetInput(filter->GetOutput());
      writer->Update();
    
  }
}

#endif
