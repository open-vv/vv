/*=========================================================================

 Program:   vv
 Module:    $RCSfile: vvImageWriter.txx,v $
 Language:  C++
 Date:      $Date: 2010/01/06 13:31:57 $
 Version:   $Revision: 1.1 $
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
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

