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
#ifndef CLITKIMAGECONVERTGENERICFILTER_CXX
#define CLITKIMAGECONVERTGENERICFILTER_CXX

#include "clitkImageConvertGenericFilter.h"

//--------------------------------------------------------------------
clitk::ImageConvertGenericFilter::ImageConvertGenericFilter():
  clitk::ImageToImageGenericFilter<Self>("ImageConvert")
{
  mOutputPixelTypeName = "NotSpecified";
  mWarningOccur = false;
  mWarning = "";
  mDisplayWarning = true;
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::ImageConvertGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class InputImageType>
void clitk::ImageConvertGenericFilter::UpdateWithInputImageType()
{

  // Verbose stuff
  if (m_IOVerbose) {
    if (m_InputFilenames.size() == 1) {
      std::cout << "Input image <" << m_InputFilenames[0] << "> is ";
      itk::ImageIOBase::Pointer header = clitk::readImageHeader(m_InputFilenames[0]);
      printImageHeader(header, std::cout);
      std::cout << std::endl;
    } else {
      for(unsigned int i=0; i<m_InputFilenames.size(); i++) {
        std::cout << "Input image " << i << " <" << m_InputFilenames[i] << "> is ";
        itk::ImageIOBase::Pointer h = clitk::readImageHeader(m_InputFilenames[i]);
        printImageHeader(h, std::cout);
        std::cout << std::endl;
      }
    }
  }


  if ((m_PixelTypeName == mOutputPixelTypeName) || (mOutputPixelTypeName == "NotSpecified")) {
    //    typename InputImageType::Pointer input = clitk::readImage<InputImageType>(m_InputFilenames);
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
    //clitk::writeImage<InputImageType>(input, mOutputFilename, m_IOVerbose);
    this->SetNextOutput<InputImageType>(input);
  } else {
#define TRY_TYPE(TYPE)							\
    if (IsSameType<TYPE>(mOutputPixelTypeName)) { UpdateWithOutputType<InputImageType, TYPE>(); return; }
    TRY_TYPE(char);
    //    TRY_TYPE(signed char);
    TRY_TYPE(uchar);
    TRY_TYPE(short);
    TRY_TYPE(ushort);
    TRY_TYPE(int); // no uint ...
    TRY_TYPE(float);
    TRY_TYPE(double);
#undef TRY_TYPE

    std::string list = CreateListOfTypes<char, uchar, short, ushort, int, float, double>();
    std::cerr << "Error, I don't know the output type '" << mOutputPixelTypeName
              << "'. " << std::endl << "Known types are " << list << "." << std::endl;
    exit(0);
  }
}
//====================================================================

//====================================================================
template<class InputImageType, class OutputPixelType>
void clitk::ImageConvertGenericFilter::UpdateWithOutputType()
{
  // Read
  typename InputImageType::Pointer input =this->template GetInput<InputImageType>(0);

  // Typedef
  typedef typename InputImageType::PixelType PixelType;

  // Warning
  std::ostringstream osstream;
  if (std::numeric_limits<PixelType>::is_signed) {
    if (!std::numeric_limits<OutputPixelType>::is_signed) {
      osstream << "Warning, input type is signed (" << m_PixelTypeName << ") while output type is not ("
               << mOutputPixelTypeName << "), use at your own responsability." << std::endl;
      mWarningOccur = true;
    }
  }
  if (!std::numeric_limits<PixelType>::is_integer) {
    if (std::numeric_limits<OutputPixelType>::is_integer) {
      osstream << "Warning, input type is not integer (" << m_PixelTypeName << ") while output type is ("
               << mOutputPixelTypeName << "), use at your own responsability." << std::endl;
      mWarningOccur = true;
    }
  }
  //  DD(std::numeric_limits<PixelType>::digits10);
  // DD(std::numeric_limits<OutputPixelType>::digits10);
  if (!std::numeric_limits<PixelType>::is_integer) {
    if (std::numeric_limits<OutputPixelType>::is_integer) {
      osstream << "Warning, input type is not integer (" << m_PixelTypeName << ") while output type is ("
               << mOutputPixelTypeName << "), use at your own responsability." << std::endl;
      mWarningOccur = true;
    }
  }
  if (std::numeric_limits<PixelType>::digits10 > std::numeric_limits<OutputPixelType>::digits10) {
    osstream << "Warning, possible loss of precision : input type is (" << m_PixelTypeName << ") while output type is ("
             << mOutputPixelTypeName << "), use at your own responsability." << std::endl;
    mWarningOccur = true;
  }

  mWarning = osstream.str();
  if (mDisplayWarning) {
    std::cerr << mWarning;
  }

  // Cast
  typedef itk::Image<OutputPixelType,InputImageType::ImageDimension> OutputImageType;
  typedef itk::CastImageFilter<InputImageType, OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input);
  filter->Update();

  // Write
  SetNextOutput<OutputImageType>(filter->GetOutput());
  //clitk::writeImage<OutputImageType>(filter->GetOutput(), mOutputFilename, m_IOVerbose);
}
//====================================================================


#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_CXX */

