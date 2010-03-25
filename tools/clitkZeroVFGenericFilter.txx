#ifndef __clitkZeroVFGenericFilter_txx
#define __clitkZeroVFGenericFilter_txx
#include "clitkZeroVFGenericFilter.h"


namespace clitk
{

  template<unsigned int Dimension>
  void ZeroVFGenericFilter::UpdateWithDim(std::string PixelType)
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
  void ZeroVFGenericFilter::UpdateWithDimAndPixelType()
  {
    //Define the image type
    typedef itk::Vector<PixelType, Dimension> DisplacementType;
    typedef itk::Image<DisplacementType, Dimension> ImageType;

    //Read the image
    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typename  ImageReaderType::Pointer reader= ImageReaderType::New();
    reader->SetFileName(m_InputName);
    reader->Update(); // not very efficient :-p
    typename ImageType::Pointer image =reader->GetOutput();
    DisplacementType zero;
    zero.Fill(0);
    image->FillBuffer(zero);
    
    //Write the output
    typedef itk::ImageFileWriter<ImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_OutputName);
    writer->SetInput(image);
    writer->Update();
  }


}

#endif
