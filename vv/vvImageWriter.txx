#ifndef vvImageWriter_TXX
#define vvImageWriter_TXX
#include <itkImageFileWriter.h>
#include "vvToITK.h"

//====================================================================
template<unsigned int VImageDimension>
void vvImageWriter::UpdateWithDim(std::string OutputPixelType)
{
    if (OutputPixelType == "short")
    {
        UpdateWithDimAndOutputPixelType<short,VImageDimension>();
    }
    else if (OutputPixelType == "unsigned short")
    {
        UpdateWithDimAndOutputPixelType<unsigned short,VImageDimension>();
    }
    else if (OutputPixelType == "unsigned_short")
    {
        UpdateWithDimAndOutputPixelType<unsigned short,VImageDimension>();
    }
    else if (OutputPixelType == "char")
    {
        UpdateWithDimAndOutputPixelType<char,VImageDimension>();
    }
    else if (OutputPixelType == "unsigned_char")
    {
        UpdateWithDimAndOutputPixelType<unsigned char,VImageDimension>();
    }
    else if (OutputPixelType == "int")
    {
        UpdateWithDimAndOutputPixelType<int,VImageDimension>();
    }
    else if (OutputPixelType == "double")
    {
        UpdateWithDimAndOutputPixelType<double,VImageDimension>();
    }
    else if (OutputPixelType == "float")
    {
        UpdateWithDimAndOutputPixelType<float,VImageDimension>();
    }
    else
    {
        std::cerr << "Error, output pixel type : \"" << OutputPixelType << "\" unknown !" << std::endl;
    }
}
//====================================================================

//====================================================================
template<class OutputPixelType, unsigned int VImageDimension>
void vvImageWriter::UpdateWithDimAndOutputPixelType()
{
    //Create the writer
    typedef itk::Image< OutputPixelType, VImageDimension > OutputImageType;
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(mOutputFilename);
    writer->SetInput(vvImageToITK<OutputImageType>(mImage));
    if (mUseAnObserver) {
        writer->AddObserver(itk::ProgressEvent(), mObserver);
    }
    try {
        writer->Update();
    }
    catch ( itk::ExceptionObject & err ) {
        std::cerr << "Error while reading " << mOutputFilename.c_str()
                  << " " << err << std::endl;
        std::stringstream error;
        error << err;
        mLastError = error.str();
        return;
    }
}
//====================================================================

#endif /* end #define vvImageWriter_TXX */

