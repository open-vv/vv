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
#include "vvImageReader.h"
#include "vvImageWriter.h"
#include "itkFlipImageFilter.h"
#include "itkGDCMImageIO.h"
#include <itkChangeInformationImageFilter.h>

#include "gdcmReader.h"
#include "gdcmAttribute.h"
#include "gdcmPrinter.h"
#include "gdcmDict.h"
#include "gdcmStringFilter.h"

//--------------------------------------------------------------------
clitk::ImageConvertGenericFilter::ImageConvertGenericFilter():
  clitk::ImageToImageGenericFilter<Self>("ImageConvert")
{
  mOutputPixelTypeName = "NotSpecified";
  mDisplayWarning = true;
  mWarning = "";
  mWarningOccur = false;
  SetCorrectNegativeSpacingFlag(false);

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

  if(mVV) {
    if (mOutputPixelTypeName != "NotSpecified" || m_WriteCompression) {
      std::cerr << "--vv is not compatible with --compression and --type options." << std::endl;
      exit(-1);
    }

    vvImageReader::Pointer reader = vvImageReader::New();
    reader->SetInputFilenames(m_InputFilenames);
    reader->Update(vvImageReader::IMAGE);

    vvImageWriter::Pointer writer = vvImageWriter::New();
    writer->SetOutputFileName(m_OutputFilenames.front());
    writer->SetSaveTransform(true);
    writer->SetInput(reader->GetOutput());
    writer->Update();
    return;
  }
  else if ((m_PixelTypeName == mOutputPixelTypeName) || (mOutputPixelTypeName == "NotSpecified")) {

    // Get input image
    typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

    if (mCorrectNegativeSpacingFlag) {
      // Read dicom
      gdcm::Reader reader;
      reader.SetFileName(m_InputFilenames[0].c_str());
      // if (!reader.CanReadFile(m_InputFilenames[0])) {
      //   std::cout << "Error: " << m_InputFilenames[0] << " is not a dicom file. Abort." << std::endl;
      //   exit(0);
      // }
      reader.Read();

      // the dataset is the the set of element we are interested in:
      gdcm::DataSet & ds = reader.GetFile().GetDataSet();

      // Read the attribute SpacingBetweenSlices, check if negative and replace
      gdcm::Attribute<0x0018,0x0088> SpacingBetweenSlices;
      SpacingBetweenSlices.SetFromDataSet(ds);
      double s = SpacingBetweenSlices.GetValue();
      if (s >=0) {
        std::cout << "Error: no negative spacing found SpacingBetweenSlices = " << s << " Abort. " << std::endl;
        exit(0);
      }
      s = -s;

      // Set spacing
      typename InputImageType::SpacingType spacing = input->GetSpacing();
      spacing[2] = s;
      input->SetSpacing(spacing);

      // Flip
      typedef itk::FlipImageFilter< InputImageType >  FilterType;
      typename FilterType::Pointer filter = FilterType::New();
      typedef typename FilterType::FlipAxesArrayType     FlipAxesArrayType;
      FlipAxesArrayType flipArray;
      flipArray[0] = false;
      flipArray[1] = false;
      flipArray[2] = true;
      filter->SetFlipAxes(flipArray);
      filter->SetInput(input);
      filter->Update();

      // Read the attribute  Image Position (Patient)
      gdcm::Tag  DetectorInformationSequenceTag(0x0054,0x0022);
      const gdcm::DataElement & DIS = ds.GetDataElement(DetectorInformationSequenceTag);
      gdcm::SmartPointer<gdcm::SequenceOfItems> sqf = DIS.GetValueAsSQ();
      gdcm::Item & item = sqf->GetItem(1);
      gdcm::DataSet & ds_position = item.GetNestedDataSet();
      gdcm::Attribute<0x0020,0x0032> ImagePositionPatient;
      ImagePositionPatient.SetFromDataSet(ds_position);
      double x = ImagePositionPatient.GetValue(0);
      double y = ImagePositionPatient.GetValue(1);
      double z = ImagePositionPatient.GetValue(2);

      // Set offset
      typename InputImageType::PointType origin = input->GetOrigin();
      origin[0] = x;
      origin[1] = y;
      origin[2] = z;
      input->SetOrigin(origin);

      // Orientation
      typename InputImageType::DirectionType direction = input->GetDirection();
      direction[2][2] = -1;
      input->SetDirection(direction);

      // Empty meta info
      itk::MetaDataDictionary dict;// = new itk::MetaDataDictionary;
      input->SetMetaDataDictionary(dict);
    }

    typedef itk::ChangeInformationImageFilter<InputImageType> CIType;
    typename CIType::Pointer changeInfo = CIType::New();
    if(mNoNiiMeta) {
      changeInfo->SetInput(input);
      typename CIType::PointType o = input->GetOrigin();
      o[0] *= -1.;
      o[1] *= -1.;
      typename CIType::DirectionType d = input->GetDirection();
      d[0][0] *= -1.;
      d[1][1] *= -1.;
      changeInfo->ChangeDirectionOn();
      changeInfo->ChangeOriginOn();
      changeInfo->SetOutputOrigin(o);
      changeInfo->SetOutputDirection(d);
      changeInfo->Update();
      input = changeInfo->GetOutput();
    }

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
