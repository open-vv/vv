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
#ifndef __clitkComposeVFGenericFilter_txx
#define __clitkComposeVFGenericFilter_txx
#include "clitkComposeVFGenericFilter.h"


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

    //Define the image type
    typedef itk::Vector<PixelType, Dimension> DisplacementType;
    typedef itk::Image<DisplacementType, Dimension> ImageType;

    //Read the input1
    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typename  ImageReaderType::Pointer reader1= ImageReaderType::New();
    reader1->SetFileName(m_InputName1);
    reader1->Update();
    typename ImageType::Pointer input1 =reader1->GetOutput();
 
    //Read the input2
    typename  ImageReaderType::Pointer reader2= ImageReaderType::New();
    reader2->SetFileName(m_InputName2);
    reader2->Update();
    typename ImageType::Pointer input2=reader2->GetOutput();

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
