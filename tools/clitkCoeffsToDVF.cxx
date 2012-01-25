#include "clitkCoeffsToDVF_ggo.h"
#include "clitkCoeffsToDVF.h"
#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageIOFactory.h"
#include <string>

template <class DisplacementFieldType> 
void
Write(typename DisplacementFieldType::Pointer dvf, std::string fileName)
{
  typedef itk::ImageFileWriter<DisplacementFieldType> ImageWriterType;
  typename ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetFileName(fileName);
  writer->SetInput(dvf);
  writer->Update();
}

int main(int argc, char** argv) 
{
  GGO(clitkCoeffsToDVF, args_info);
  CLITK_INIT;

  typename itk::ImageIOBase::Pointer image_io = itk::ImageIOFactory::CreateImageIO(args_info.input_arg, itk::ImageIOFactory::ReadMode);
  image_io->SetFileName(args_info.input_arg);
  image_io->ReadImageInformation();
  
  unsigned int ndim = image_io->GetNumberOfDimensions();
  switch (ndim) {
    case 2:
    {
      unsigned const dim = 2;
      typedef itk::Vector<double, dim>  PixelType;
      typedef itk::Image<PixelType, dim> DVFType;
      typename DVFType::Pointer dvf = clitk::BLUTCoeffsToDVF<DVFType>(args_info.input_arg, args_info.like_arg);
      Write<DVFType>(dvf, args_info.output_arg);
    }
    break;
    
    case 3:
    {
      unsigned const dim = 3;
      typedef itk::Vector<double, dim>  PixelType;
      typedef itk::Image<PixelType, dim> DVFType;
      typename DVFType::Pointer dvf = clitk::BLUTCoeffsToDVF<DVFType>(args_info.input_arg, args_info.like_arg);
      Write<DVFType>(dvf, args_info.output_arg);
    }
    break;
    
    default:
      std::cerr << "Unsupported image dimension (either 2 or 3)" << std::endl;
      return -1;
  }
  
  return 0;
}