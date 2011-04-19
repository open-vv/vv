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

#ifndef VVIMAGEREADER_TXX
#define VVIMAGEREADER_TXX

#include <string>
#include <itkImageFileReader.h>
#include <itkImageSeriesReader.h>
#include <itkImageToVTKImageFilter.h>

#include <vtkTransform.h>

#include "clitkCommon.h"
#include "clitkConfiguration.h"
#include "vvFromITK.h"
//----------------------------------------------------------------------------
template<unsigned int VImageDimension>
void vvImageReader::UpdateWithDim(std::string InputPixelType)
{
  if (mType == VECTORFIELD)
    UpdateWithDimAndInputPixelType<itk::Vector<float,3>,VImageDimension>();
  else if (InputPixelType == "short")
    UpdateWithDimAndInputPixelType<short,VImageDimension>();
  else if (InputPixelType == "unsigned_short")
    UpdateWithDimAndInputPixelType<unsigned short,VImageDimension>();
  else if (InputPixelType == "char")
    UpdateWithDimAndInputPixelType<char,VImageDimension>();
  else if (InputPixelType == "unsigned_char")
    UpdateWithDimAndInputPixelType<unsigned char,VImageDimension>();
  else if (InputPixelType == "int")
    UpdateWithDimAndInputPixelType<int,VImageDimension>();
  else if (InputPixelType == "unsigned_int")
    UpdateWithDimAndInputPixelType<unsigned int,VImageDimension>();
  else if (InputPixelType == "double")
    UpdateWithDimAndInputPixelType<double,VImageDimension>();
  else if (InputPixelType == "float")
    UpdateWithDimAndInputPixelType<float,VImageDimension>();
  else
    std::cerr << "Error, input pixel type : " << InputPixelType << " unknown !" << std::endl;

  if (CLITK_EXPERIMENTAL && mLastError.size()==0) {
    //ReadNkiImageTransform();
    ReadMatImageTransform();
  }
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
template<class InputPixelType, unsigned int VImageDimension>
void vvImageReader::UpdateWithDimAndInputPixelType()
{
  if (mType == MERGEDWITHTIME)   // In this case we can load the images
    // one at the time to avoid excessive
    // memory use
  {
    mImage=vvImage::New();

    for (std::vector<std::string>::const_iterator i=mInputFilenames.begin(); i!=mInputFilenames.end(); i++) {
      typedef itk::Image< InputPixelType, VImageDimension-1 > InputImageType;
      typedef itk::ImageFileReader<InputImageType> ReaderType;
      typename ReaderType::Pointer reader = ReaderType::New();
      reader->ReleaseDataFlagOn();
      reader->SetFileName(*i);
      try {
        mImage->AddItkImage<InputImageType>(reader->GetOutput());
      } catch ( itk::ExceptionObject & err ) {
        std::cerr << "Error while reading " << mInputFilenames[0].c_str()
                  << " " << err << std::endl;
        std::stringstream error;
        error << err;
        mLastError = error.str();
        return;
      }
    }
  } else if (mType == SLICED) {
    mImage=vvImage::New();
    typedef itk::Image< InputPixelType, VImageDimension > InputImageType;
    typedef itk::ImageFileReader<InputImageType> ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(mInputFilenames[0]);
    reader->UpdateOutputInformation();

    typedef itk::Image< InputPixelType, VImageDimension-1 > SlicedImageType;
    typedef itk::ExtractImageFilter<InputImageType, SlicedImageType> FilterType;

    typename InputImageType::RegionType inputRegion = reader->GetOutput()->GetLargestPossibleRegion();
    typename InputImageType::SizeType inputSize = inputRegion.GetSize();
    typename InputImageType::IndexType start = inputRegion.GetIndex();
    typename InputImageType::SizeType extractedRegionSize = inputSize;
    typename InputImageType::RegionType extractedRegion;
    extractedRegionSize[VImageDimension - 1] = 0;
    extractedRegion.SetSize(extractedRegionSize);
    start[VImageDimension - 1] = mSlice;
    extractedRegion.SetIndex(start);

    typename FilterType::Pointer filter = FilterType::New();
    filter->SetExtractionRegion(extractedRegion);
    filter->SetInput(reader->GetOutput());
    filter->ReleaseDataFlagOn();
    try {
      mImage->AddItkImage<SlicedImageType>(filter->GetOutput());
    }
    catch ( itk::ExceptionObject & err ) {
      std::cerr << "Error while slicing " << mInputFilenames[0].c_str()
                << "(slice #" << mSlice << ") " << err << std::endl;
      return;
    }
  } else {
    if (mInputFilenames.size() > 1) {
      typedef itk::Image< InputPixelType, VImageDimension > InputImageType;
      typedef itk::ImageSeriesReader<InputImageType> ReaderType;
      typename ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileNames(mInputFilenames);
      reader->ReleaseDataFlagOn();

      try {
        if (mType == IMAGEWITHTIME)
          mImage=vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput(),true);
        else
          mImage=vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput());
      } catch ( itk::ExceptionObject & err ) {
        std::cerr << "Error while reading image series:" << err << std::endl;
        std::stringstream error;
        error << err;
        mLastError = error.str();
        return;
      }
    } else {
      typedef itk::Image< InputPixelType, VImageDimension > InputImageType;
      typedef itk::ImageFileReader<InputImageType> ReaderType;
      typename ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(mInputFilenames[0]);
      reader->ReleaseDataFlagOn();

      try {
        if (mType == IMAGEWITHTIME)
          mImage=vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput(),true);
        else
          mImage=vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput());
      } catch ( itk::ExceptionObject & err ) {
        std::cerr << "Error while reading " << mInputFilenames[0].c_str()
                  << " " << err << std::endl;
        std::stringstream error;
        error << err;
        mLastError = error.str();
        return;
      }
    }
  }
}
//----------------------------------------------------------------------------


#endif /* end #define vvImageReader_TXX */

