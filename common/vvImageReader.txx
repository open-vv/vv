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
#include <itkFlexibleVectorCastImageFilter.h>

#include <vtkTransform.h>

#include "clitkCommon.h"
#include "clitkConfiguration.h"
#include "vvFromITK.h"
//----------------------------------------------------------------------------
template<unsigned int VImageDimension>
void vvImageReader::UpdateWithDim(std::string InputPixelType)
{
  if (mType == VECTORFIELD || mType == VECTORFIELDWITHTIME)
  {
    if (VImageDimension == 4)
      UpdateWithDimAndInputVectorPixelType<itk::Vector<float,3>,VImageDimension>();
    else
      UpdateWithDimAndInputVectorPixelType<itk::Vector<float,VImageDimension>,VImageDimension>();
  }
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

  if (mLastError.size()==0) {
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
        mImage->ComputeScalarRangeBase<InputPixelType, VImageDimension-1>(reader->GetOutput());
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
#if ITK_VERSION_MAJOR == 4
    filter->SetDirectionCollapseToSubmatrix();
#endif
    try {
      mImage->AddItkImage<SlicedImageType>(filter->GetOutput());
      mImage->ComputeScalarRangeBase<InputPixelType, VImageDimension-1>(filter->GetOutput());
    } catch ( itk::ExceptionObject & err ) {
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
        {
          std::cerr << "We should never come here:" << std::endl
            << "  Calling vvImageReader with multiple images and IMAGEWITHTIME is undefined." << std::endl
            << "  You are probably looking for MERGEDWITHTIME Type." << std::endl;
          return;
        }
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
        mImage = vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput(), mType == IMAGEWITHTIME || mType == VECTORFIELDWITHTIME);
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

//----------------------------------------------------------------------------
template<class InputPixelType, unsigned int VImageDimension>
void vvImageReader::UpdateWithDimAndInputVectorPixelType()
{
  typedef itk::Image< InputPixelType, VImageDimension > InputImageType;
  typename InputImageType::Pointer input;

  if (mInputFilenames.size() > 1) {
    typedef itk::ImageSeriesReader<InputImageType> ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileNames(mInputFilenames);
    reader->ReleaseDataFlagOn();
    try {
      reader->Update();
      input = reader->GetOutput();
    } catch ( itk::ExceptionObject & err ) {
      std::cerr << "Error while reading image series:" << err << std::endl;
      std::stringstream error;
      error << err;
      mLastError = error.str();
      return;
    }
  } else {
    typedef itk::ImageFileReader<InputImageType> ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(mInputFilenames[0]);
    reader->ReleaseDataFlagOn();
    try {
      reader->Update();
      input = reader->GetOutput();
    } catch ( itk::ExceptionObject & err ) {
      std::cerr << "Error while reading " << mInputFilenames[0].c_str()
        << " " << err << std::endl;
      std::stringstream error;
      error << err;
      mLastError = error.str();
      return;
    }
  }
  
  typedef itk::Image< itk::Vector<float , 3>, VImageDimension > VectorImageType;
  typedef itk::FlexibleVectorCastImageFilter<InputImageType, VectorImageType> CasterType;
  typename VectorImageType::Pointer casted_input;
  typename CasterType::Pointer caster = CasterType::New();
  caster->SetInput(input);
  caster->Update();
  casted_input = caster->GetOutput();
  
  mImage = vvImageFromITK<VImageDimension, itk::Vector<float, 3> >(casted_input, mType == IMAGEWITHTIME || mType == VECTORFIELDWITHTIME);
}
//----------------------------------------------------------------------------

#endif /* end #define vvImageReader_TXX */

