/*=========================================================================

 Program:   vv
 Module:    $RCSfile: vvImageReader.cxx,v $
 Language:  C++
 Date:      $Date: 2010/01/26 15:04:33 $
 Version:   $Revision: 1.2 $
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
#ifndef vvImageReader_CXX
#define vvImageReader_CXX

#include <QApplication>
#include <itkImageFileReader.h>
#include "vvImageReader.h"
#include "vvImageReader.txx"


//====================================================================
vvImageReader::vvImageReader()
{
    mImage = NULL;
    mInputFilenames.resize(0);
    mLastError = "";
    mType = UNDEFINEDIMAGETYPE;
}

vvImageReader::~vvImageReader() { }

void vvImageReader::Update()
{
    Update(mType);
}

void vvImageReader::Update(LoadedImageType type)
{
    itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(mInputFilenames[0].c_str(), itk::ImageIOFactory::ReadMode);
    if (!reader) {
        mLastError="Unable to read file.";
    }
    else {
        reader->SetFileName(mInputFilenames[0]);
        reader->ReadImageInformation();
        if (mInputFilenames.size() > 1)
            Update(reader->GetNumberOfDimensions()+1,reader->GetComponentTypeAsString(reader->GetComponentType()),type);
        else
            Update(reader->GetNumberOfDimensions(),reader->GetComponentTypeAsString(reader->GetComponentType()),type);
    }
}

//====================================================================
void vvImageReader::Update(int dim,std::string inputPixelType, LoadedImageType type) {
    //CALL_FOR_ALL_DIMS(dim,UpdateWithDim,inputPixelType);
    mType = type;
    mDim = dim;
    mInputPixelType=inputPixelType;
    this->start(); //Start heavy read operation in a separate thread
    while (this->isRunning())
    {
        qApp->processEvents();
        this->wait(50);
    }
}

void vvImageReader::run()
{
    switch(mDim)
    {
        case 2:
            UpdateWithDim<2>(mInputPixelType);
            break;;
        case 3:
            UpdateWithDim<3>(mInputPixelType);
            break;;
        case 4:
            UpdateWithDim<4>(mInputPixelType);
            break;;
        default:
            std::cerr << "dimension unknown in Update ! " << std::endl;
    }
}
//====================================================================

//====================================================================
/*void vvImageReader::Extract(int dim, std::string inputPixelType, int slice) {
  CALL_FOR_ALL_DIMS(dim, ExtractWithDim, inputPixelType, slice);
}*/
//====================================================================
void vvImageReader::SetInputFilename(const std::string & filename)
{
    mInputFilenames.resize(0);
    mInputFilenames.push_back(filename);
}

//====================================================================
void vvImageReader::SetInputFilenames(const std::vector<std::string> & filenames) {
    mInputFilenames = filenames;
}
//====================================================================

#endif

