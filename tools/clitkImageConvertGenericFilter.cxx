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
  mDisplayWarning = true;
  mWarning = "";
  mWarningOccur = false;
  
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
  ADD_VEC_IMAGE_TYPE(Dim, 2, float);
  ADD_VEC_IMAGE_TYPE(Dim, 3, float);
  ADD_VEC_IMAGE_TYPE(Dim, 2, double);
  ADD_VEC_IMAGE_TYPE(Dim, 3, double);
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
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
    this->SetNextOutput<InputImageType>(input);
  } else {
    // "trick" to call independent versions of update according to the 
    // pixel type (vector or scalar), using partial specializations
    if (!UpdateWithSelectiveOutputType<InputImageType, ImageConvertTraits<typename InputImageType::PixelType>::IS_VECTOR>::Run(*this, mOutputPixelTypeName))    
      exit(-1);
  }
}
//====================================================================

//====================================================================

template<class PixelType, class OutputPixelType>
void clitk::ImageConvertGenericFilter::CheckTypes(
  std::string inType, std::string outType
)
{
  std::ostringstream osstream;
  if (std::numeric_limits<PixelType>::is_signed) {
    if (!std::numeric_limits<OutputPixelType>::is_signed) {
      osstream << "Warning, input type is signed (";
    }
  }
  if (!std::numeric_limits<PixelType>::is_integer) {
    if (std::numeric_limits<OutputPixelType>::is_integer) {
      osstream << "Warning, input type is not integer (";
    }
  }
  //  DD(std::numeric_limits<PixelType>::digits10);
  // DD(std::numeric_limits<OutputPixelType>::digits10);
  if (!std::numeric_limits<PixelType>::is_integer) {
    if (std::numeric_limits<OutputPixelType>::is_integer) {
      osstream << "Warning, input type is not integer (";
    }
  }
  if (std::numeric_limits<PixelType>::digits10 > std::numeric_limits<OutputPixelType>::digits10) {
    osstream << "Warning, possible loss of precision : input type is (" ;
  }

  if (!osstream.str().empty())
  {
    mWarningOccur = true;
    osstream << inType << ") while output type is (" << outType << "), use at your own responsability." << std::endl;
    mWarning = osstream.str();
    if (mDisplayWarning) {
      std::cerr << mWarning;
    }
  }
}


#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_CXX */

