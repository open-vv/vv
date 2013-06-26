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
#ifndef vvImageWriter_TXX
#define vvImageWriter_TXX

#include <itkImageFileWriter.h>
#include <itkChangeInformationImageFilter.h>
#include "vvToITK.h"

//====================================================================
template<unsigned int VImageDimension>
void vvImageWriter::UpdateWithDim(std::string OutputPixelType)
{
  if (OutputPixelType == "short") {
    UpdateWithDimAndOutputPixelType<short,VImageDimension>();
  } else if (OutputPixelType == "unsigned short") {
    UpdateWithDimAndOutputPixelType<unsigned short,VImageDimension>();
  } else if (OutputPixelType == "unsigned_short") {
    UpdateWithDimAndOutputPixelType<unsigned short,VImageDimension>();
  } else if (OutputPixelType == "char") {
    UpdateWithDimAndOutputPixelType<char,VImageDimension>();
  } else if (OutputPixelType == "unsigned_char") {
    UpdateWithDimAndOutputPixelType<unsigned char,VImageDimension>();
  } else if (OutputPixelType == "int") {
    UpdateWithDimAndOutputPixelType<int,VImageDimension>();
  } else if (OutputPixelType == "unsigned_int") {
    UpdateWithDimAndOutputPixelType<unsigned int,VImageDimension>();
  } else if (OutputPixelType == "double") {
    UpdateWithDimAndOutputPixelType<double,VImageDimension>();
  } else if (OutputPixelType == "float") {
    UpdateWithDimAndOutputPixelType<float,VImageDimension>();
  } else {
    std::cerr << "Error, output pixel type : \"" << OutputPixelType << "\" unknown !" << std::endl;
  }
}
//====================================================================

//====================================================================
template<class OutputPixelType, unsigned int VImageDimension>
void vvImageWriter::UpdateWithDimAndOutputPixelType()
{
  // The ITK image
  typedef itk::Image< OutputPixelType, VImageDimension > OutputImageType;
  typename OutputImageType::ConstPointer itkimg = vvImageToITK<OutputImageType>(mImage);

  //Create the writer
  typedef itk::ImageFileWriter<OutputImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(mOutputFilename);

  //Change information if it must transformation must be saved
  typedef itk::ChangeInformationImageFilter<OutputImageType> ChangeInfoType;
  typename ChangeInfoType::Pointer info = ChangeInfoType::New();
  if(mSaveTransform) {
    // Set pipeline
    info->SetInput(itkimg);
    writer->SetInput(info->GetOutput());

    // Inverse vv matrix
    itk::Matrix<double, 4, 4> trans;
    for(int i=0; i<4; i++)
      for(int j=0; j<4; j++)
        // TODO SR and BP: check on the list of transforms and not the first only
        trans[i][j] = mImage->GetTransform()[0]->GetMatrix()->GetElement(i,j);
    trans = trans.GetInverse();

    // Direction
    typename ChangeInfoType::DirectionType direction;
    for(unsigned int i=0; i<VImageDimension; i++)
      for(unsigned int j=0; j<VImageDimension; j++)
        direction[i][j] = trans[i][j];
    info->SetOutputDirection(direction);
    info->ChangeDirectionOn();

    // Origin
    typename ChangeInfoType::PointType origin = itkimg->GetOrigin();
    origin = direction * origin;
    for(unsigned int i=0; i<VImageDimension; i++)
      origin[i] += trans[i][3];
    info->SetOutputOrigin(origin);
    info->ChangeOriginOn();
  }
  else
    writer->SetInput(itkimg);


  if (mUseAnObserver) {
    writer->AddObserver(itk::ProgressEvent(), mObserver);
  }
  try {
    writer->Update();
  } catch ( itk::ExceptionObject & err ) {
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

