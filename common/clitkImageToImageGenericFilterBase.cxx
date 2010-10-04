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

// clitk
#include "clitkImageToImageGenericFilterBase.h"

// itk
#include <itkImage.h>

//--------------------------------------------------------------------
clitk::ImageToImageGenericFilterBase::~ImageToImageGenericFilterBase() {}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::ImageToImageGenericFilterBase::ImageToImageGenericFilterBase(std::string n)
  :m_IOVerbose(false)
{
  m_FilterName = n;
  m_FailOnImageTypeError = true;
  m_ReadOnDisk = true;
  // m_LastError = "";
  // StopOnErrorOn();
  SetFilterBase(NULL);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputFilenames(const std::vector<std::string> & filenames)
{
  m_InputFilenames = filenames;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::EnableReadOnDisk(bool b)
{
  m_ReadOnDisk = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputFilename(const std::string & filename)
{
  std::vector<std::string> f;
  f.push_back(filename);
  SetInputFilenames(f);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::AddInputFilename(const std::string & filename)
{
  m_InputFilenames.push_back(filename);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetOutputFilename(const std::string & filename)
{
  m_OutputFilenames.clear();
  m_OutputFilenames.push_back(filename);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::AddOutputFilename(const std::string & filename)
{
  m_OutputFilenames.push_back(filename);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetOutputFilenames(const std::vector<std::string> & filenames)
{
  m_OutputFilenames.clear();
  std::copy(filenames.begin(),filenames.end(),m_OutputFilenames.begin());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string clitk::ImageToImageGenericFilterBase::GetOutputFilename()
{
  assert(m_OutputFilenames.size() == 1);
  return m_OutputFilenames.front();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::GetInputImageDimensionAndPixelType(unsigned int& dim, \
    std::string& pixeltype,unsigned int& components)
{
  if (m_ReadOnDisk && m_InputFilenames.size()) {
    int comp_temp,dim_temp; //clitkCommonImage takes ints
    ReadImageDimensionAndPixelType(m_InputFilenames[0], dim_temp, pixeltype,comp_temp);
    components=comp_temp;
    dim=dim_temp;
  } else {
    if (m_InputVVImages.size()) {
      pixeltype = m_InputVVImages[0]->GetScalarTypeAsITKString();
      dim = m_InputVVImages[0]->GetNumberOfDimensions();
      components = m_InputVVImages[0]->GetNumberOfScalarComponents();
    } else
      assert(false); //No input image, shouldn't happen
  }
  if (m_IOVerbose) {
    std::cout << "Input is " << m_Dim << "D " << m_PixelTypeName << "." << std::endl;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vvImage::Pointer clitk::ImageToImageGenericFilterBase::GetOutputVVImage ()
{
  assert(m_OutputVVImages.size());
  return m_OutputVVImages[0];
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::vector<vvImage::Pointer> clitk::ImageToImageGenericFilterBase::GetOutputVVImages()
{
  return m_OutputVVImages;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputVVImage (vvImage::Pointer input)
{
  m_InputVVImages.clear();
  m_InputVVImages.push_back(input);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::AddInputVVImage (vvImage::Pointer input)
{
  m_InputVVImages.push_back(input);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputVVImages (std::vector<vvImage::Pointer> input)
{
  m_InputVVImages=input;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::PrintAvailableImageTypes()
{
  std::cout << GetAvailableImageTypes();
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::ImageTypeError()
{
  std::cerr << "**Error** The filter <" << m_FilterName << "> is not available for "
            << m_Dim << "D images with pixel="
            << m_PixelTypeName << " and "
            << m_NbOfComponents << " component." << std::endl;
  std::cerr << GetAvailableImageTypes();
  exit(0);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetImageTypeError()
{
  std::cerr << "TODO ! " << std::endl;
  exit(0);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::ImageToImageGenericFilterBase::GetFilterName()
{
  return m_FilterName;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetFilterName(std::string & n)
{
  m_FilterName = n;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetIOVerbose(bool b)
{
  m_IOVerbose = b;
}
//--------------------------------------------------------------------

#define DEF_SetNextOutput_And_GetInput(PixelType, Dim) \
  template \
void clitk::ImageToImageGenericFilterBase::SetNextOutput<itk::Image<PixelType, Dim> >(itk::Image<PixelType,Dim>::Pointer output); \
  template \
   itk::Image<PixelType, Dim>::Pointer clitk::ImageToImageGenericFilterBase::GetInput<itk::Image<PixelType, Dim> >(unsigned int n);

#define DEF_SetNextOutput_And_GetInput_WithCompo(Compo, Dim)   \
  template \
  void clitk::ImageToImageGenericFilterBase::SetNextOutput<itk::Image<itk::Vector<float, Compo>, Dim> >(itk::Image<itk::Vector<float, Compo>,Dim>::Pointer output); \
  template \
  itk::Image<itk::Vector<float,Compo>, Dim>::Pointer clitk::ImageToImageGenericFilterBase::GetInput<itk::Image<itk::Vector<float, Compo>, Dim> >(unsigned int n);

DEF_SetNextOutput_And_GetInput(char, 2);
DEF_SetNextOutput_And_GetInput(unsigned char, 2);
DEF_SetNextOutput_And_GetInput(short, 2);
DEF_SetNextOutput_And_GetInput(unsigned short, 2);
DEF_SetNextOutput_And_GetInput(int, 2);
DEF_SetNextOutput_And_GetInput(float, 2);
DEF_SetNextOutput_And_GetInput(double, 2);

DEF_SetNextOutput_And_GetInput(char, 3);
DEF_SetNextOutput_And_GetInput(unsigned char, 3);
DEF_SetNextOutput_And_GetInput(short, 3);
DEF_SetNextOutput_And_GetInput(unsigned short, 3);
DEF_SetNextOutput_And_GetInput(int, 3);
DEF_SetNextOutput_And_GetInput(float, 3);
DEF_SetNextOutput_And_GetInput(double, 3);

DEF_SetNextOutput_And_GetInput_WithCompo(2, 2);
DEF_SetNextOutput_And_GetInput_WithCompo(2, 3);
DEF_SetNextOutput_And_GetInput_WithCompo(2, 4);
DEF_SetNextOutput_And_GetInput_WithCompo(3, 2);
DEF_SetNextOutput_And_GetInput_WithCompo(3, 3);
DEF_SetNextOutput_And_GetInput_WithCompo(3, 4);
DEF_SetNextOutput_And_GetInput_WithCompo(4, 2);
DEF_SetNextOutput_And_GetInput_WithCompo(4, 3);
DEF_SetNextOutput_And_GetInput_WithCompo(4, 4);

DEF_SetNextOutput_And_GetInput(char, 4);
DEF_SetNextOutput_And_GetInput(unsigned char, 4);
DEF_SetNextOutput_And_GetInput(short, 4);
DEF_SetNextOutput_And_GetInput(unsigned short, 4);
DEF_SetNextOutput_And_GetInput(int, 4);
DEF_SetNextOutput_And_GetInput(float, 4);
DEF_SetNextOutput_And_GetInput(double, 4);


//--------------------------------------------------------------------
template<class ImageType>
void clitk::ImageToImageGenericFilterBase::SetNextOutput(typename ImageType::Pointer output)
{
  if (m_OutputFilenames.size()) {
    clitk::writeImage<ImageType>(output, m_OutputFilenames.front(), m_IOVerbose);
    m_OutputFilenames.pop_front();
  }
  if (m_InputVVImages.size()) //We assume that if a vv image is set as input, we want one as the output
    m_OutputVVImages.push_back(vvImageFromITK<ImageType::ImageDimension,typename ImageType::PixelType>(output));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer clitk::ImageToImageGenericFilterBase::GetInput(unsigned int n)
{
  if (m_ReadOnDisk && m_InputFilenames.size() > n) {
    return clitk::readImage<ImageType>(m_InputFilenames[n], m_IOVerbose);
  } else {
    if (m_InputVVImages.size() > n)
      return typename ImageType::Pointer(const_cast<ImageType*>(vvImageToITK<ImageType>(m_InputVVImages[n]).GetPointer()));
    else {
      assert(false); //No input, this shouldn't happen
      return typename ImageType::Pointer(NULL);
    }
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::MustStop()
// {
//   if (m_FilterBase != NULL) {
//     m_FilterBase->SetMustStop(true);
//   }
// }
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::DeleteLastOutputImage()
{
  if (m_OutputVVImages.size()>1) {
   m_OutputVVImages.pop_back();
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------


