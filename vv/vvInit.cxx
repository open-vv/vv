#include "nkitkXDRImageIO.h"
#include "nkitkXDRImageIOFactory.h"
#include "clitkVoxImageIO.h"
#include "clitkVoxImageIOFactory.h"
#include "clitkVfImageIO.h"
#include "clitkVfImageIOFactory.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkByteSwapper.h"


void initialize_IO()
{
    itk::ImageIOFactory::RegisterBuiltInFactories();
    clitk::VoxImageIOFactory::RegisterOneFactory();
    clitk::VfImageIOFactory::RegisterOneFactory();
    nkitk::XDRImageIOFactory::RegisterOneFactory();
}
