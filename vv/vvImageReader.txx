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

#ifndef VVIMAGEREADER_TXX
#define VVIMAGEREADER_TXX

#include <string>
#include <itkImageFileReader.h>
#include <itkImageSeriesReader.h>
#include <itkImageToVTKImageFilter.h>

#include <vtkTransform.h>

#include "clitkCommon.h"
#include "vvFromITK.h"
#include "vvConstants.h"

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
  else if (InputPixelType == "double")
    UpdateWithDimAndInputPixelType<double,VImageDimension>();
  else if (InputPixelType == "float")
    UpdateWithDimAndInputPixelType<float,VImageDimension>();
  else
    std::cerr << "Error, input pixel type : " << InputPixelType << " unknown !" << std::endl;

  if (CLITK_EXPERIMENTAL)
    if (mLastError.size()==0)
      ReadNkiImageTransform();
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
template<class InputPixelType, unsigned int VImageDimension>
void vvImageReader::UpdateWithDimAndInputPixelType()
{

  //  DD(mType);

  if (mType == MERGEDWITHTIME)   // In this case we can load the images
    // one at the time to avoid excessive
    // memory use
  {
    typedef itk::Image< InputPixelType, VImageDimension-1 > InputImageType;
    typedef itk::ImageFileReader<InputImageType> ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    typedef itk::ImageToVTKImageFilter <InputImageType> ConnectorType;
    typename ConnectorType::Pointer connector = ConnectorType::New();
    connector->SetInput(reader->GetOutput());
    mImage=vvImage::New();
    for (std::vector<std::string>::const_iterator i=mInputFilenames.begin(); i!=mInputFilenames.end(); i++) {
      std::cout << (*i) << std::endl;
      reader->SetFileName(*i);
      try {
        reader->Update();
      } catch ( itk::ExceptionObject & err ) {
        std::cerr << "Error while reading " << mInputFilenames[0].c_str()
                  << " " << err << std::endl;
        std::stringstream error;
        error << err;
        mLastError = error.str();
        return;
      }
      try {
        connector->Update();
      } catch ( itk::ExceptionObject & err ) {
        std::cerr << "Error while setting vvImage from ITK (MERGEDWITHTIME)"
                  << " " << err << std::endl;
      }
      vtkImageData *image = vtkImageData::New();
      image->DeepCopy(connector->GetOutput());
      mImage->AddImage(image);
    }
  } else {
    if (mInputFilenames.size() > 1) {
      typedef itk::Image< InputPixelType, VImageDimension > InputImageType;
      typedef itk::ImageSeriesReader<InputImageType> ReaderType;
      typename ReaderType::Pointer reader = ReaderType::New();
      for (std::vector<std::string>::const_iterator i=mInputFilenames.begin(); i!=mInputFilenames.end(); i++)
        std::cout << (*i) << std::endl;
      reader->SetFileNames(mInputFilenames);
      //if (mUseAnObserver) {
      //reader->AddObserver(itk::ProgressEvent(), mObserver);
      //}
      try {
        reader->Update();
      } catch ( itk::ExceptionObject & err ) {
        std::cerr << "Error while reading image series:" << err << std::endl;
        std::stringstream error;
        error << err;
        mLastError = error.str();
        return;
      }

      // DD(reader->GetOutput()->GetImageDimension());
      //           DD(reader->GetOutput()->GetNumberOfComponentsPerPixel());
      //           for(unsigned int i=0; i <reader->GetOutput()->GetImageDimension(); i++) {
      //             DD(reader->GetOutput()->GetSpacing()[i]);
      //           }

      if (mType == IMAGEWITHTIME)
        mImage=vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput(),true);
      else
        mImage=vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput());
    } else {
      typedef itk::Image< InputPixelType, VImageDimension > InputImageType;
      typedef itk::ImageFileReader<InputImageType> ReaderType;
      typename ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(mInputFilenames[0]);
      //if (mUseAnObserver) {
      //reader->AddObserver(itk::ProgressEvent(), mObserver);
      //}
      try {
        reader->Update();
      } catch ( itk::ExceptionObject & err ) {
        std::cerr << "Error while reading " << mInputFilenames[0].c_str()
                  << " " << err << std::endl;
        std::stringstream error;
        error << err;
        mLastError = error.str();
        return;
      }

      // DD(reader->GetOutput()->GetImageDimension());
      //           DD(reader->GetOutput()->GetNumberOfComponentsPerPixel());
      //           for(unsigned int i=0; i <reader->GetOutput()->GetImageDimension(); i++) {
      //             DD(reader->GetOutput()->GetSpacing()[i]);
      //             DD(reader->GetOutput()->GetOrigin()[i]);
      //           }


      if (mType == IMAGEWITHTIME)
        mImage=vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput(),true);
      else
        mImage=vvImageFromITK<VImageDimension,InputPixelType>(reader->GetOutput());
    }
  }
}
//----------------------------------------------------------------------------


#endif /* end #define vvImageReader_TXX */

