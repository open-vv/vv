/*=========================================================================

  Program:   vv
  Module:    $RCSfile: clitkImageToImageGenericFilterBase.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/24 10:48:05 $
  Version:   $Revision: 1.3 $
  Author :   Joel Schaerer <joel.schaerer@creatis.insa-lyon.fr>
  David Sarrut <david.sarrut@creatis.insa-lyon.fr>

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

#include "clitkImageToImageGenericFilterBase.h"
#include <itkImage.h>

//--------------------------------------------------------------------
clitk::ImageToImageGenericFilterBase::~ImageToImageGenericFilterBase() {}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
clitk::ImageToImageGenericFilterBase::ImageToImageGenericFilterBase(std::string n)
  :mIOVerbose(false) {
  mFilterName = n;
  mFailOnImageTypeError = true;
  mReadOnDisk = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputFilenames(const std::vector<std::string> & filenames) {
    mInputFilenames=filenames;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::EnableReadOnDisk(bool b) {
  mReadOnDisk = b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputFilename(const std::string & filename) {
  std::vector<std::string> f;
  f.push_back(filename);
  SetInputFilenames(f);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::AddInputFilename(const std::string & filename) {
  mInputFilenames.push_back(filename);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetOutputFilename(const std::string & filename) {
  mOutputFilenames.clear();
  mOutputFilenames.push_back(filename);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::AddOutputFilename(const std::string & filename)
{
  mOutputFilenames.push_back(filename);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetOutputFilenames(const std::vector<std::string> & filenames)
{
    mOutputFilenames.clear();
    std::copy(filenames.begin(),filenames.end(),mOutputFilenames.begin());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string clitk::ImageToImageGenericFilterBase::GetOutputFilename()
{
    assert(mOutputFilenames.size() == 1);
    return mOutputFilenames.front();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::GetInputImageDimensionAndPixelType(unsigned int& dim, \
        std::string& pixeltype,unsigned int& components)
{
  if (mReadOnDisk && mInputFilenames.size()) {
    int comp_temp,dim_temp; //clitkCommonImage takes ints
    ReadImageDimensionAndPixelType(mInputFilenames[0], dim_temp, pixeltype,comp_temp);
    components=comp_temp; dim=dim_temp;
  }
  else {
    if (mInputVVImages.size()) {
      pixeltype=mInputVVImages[0]->GetScalarTypeAsString();
      dim=mInputVVImages[0]->GetNumberOfDimensions();
      components=mInputVVImages[0]->GetNumberOfScalarComponents();
    }
    else
    assert(false); //No input image, shouldn't happen
  }
  if (mIOVerbose) {
    std::cout << "Input is " << mDim << "D " << mPixelTypeName << "." << std::endl;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vvImage::Pointer clitk::ImageToImageGenericFilterBase::GetOutputVVImage ()
{
    assert(mOutputVVImages.size());
    return mOutputVVImages[0];
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::vector<vvImage::Pointer> clitk::ImageToImageGenericFilterBase::GetOutputVVImages()
{
    return mOutputVVImages;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputVVImage (vvImage::Pointer input)
{
    mInputVVImages.clear();
    mInputVVImages.push_back(input);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::AddInputVVImage (vvImage::Pointer input)
{
    mInputVVImages.push_back(input);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputVVImages (std::vector<vvImage::Pointer> input)
{
    mInputVVImages=input;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::PrintAvailableImageTypes() {
  std::cout << GetAvailableImageTypes();
}
//--------------------------------------------------------------------



//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::ImageTypeError() {
  std::cerr << "**Error** The filter <" << mFilterName << "> is not available for " 
            << mDim << "D images with pixel=" 
            << mPixelTypeName << " and "
            << mNbOfComponents << " component." << std::endl;
  std::cerr << GetAvailableImageTypes();
  exit(0);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetImageTypeError() {
  std::cerr << "TODO ! " << std::endl;
  exit(0);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
const std::string & clitk::ImageToImageGenericFilterBase::GetFilterName() { 
  return mFilterName; 
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetFilterName(std::string & n) { 
  mFilterName = n; 
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetIOVerbose(bool b) { 
  mIOVerbose = b; 
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
void clitk::ImageToImageGenericFilterBase::SetNextOutput(typename ImageType::Pointer output) {
  if (mOutputFilenames.size())
    {
      clitk::writeImage<ImageType>(output, mOutputFilenames.front(), mIOVerbose);
      mOutputFilenames.pop_front();
    }
  if (mInputVVImages.size()) //We assume that if a vv image is set as input, we want one as the output
    mOutputVVImages.push_back(vvImageFromITK<ImageType::ImageDimension,typename ImageType::PixelType>(output));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType> 
typename ImageType::Pointer clitk::ImageToImageGenericFilterBase::GetInput(unsigned int n) {
  if (mReadOnDisk && mInputFilenames.size() > n) {
    return clitk::readImage<ImageType>(mInputFilenames[n], mIOVerbose);
  }
  else {
    if (mInputVVImages.size() > n)
      return typename ImageType::Pointer(const_cast<ImageType*>(vvImageToITK<ImageType>(mInputVVImages[n]).GetPointer()));
    else
      {
        assert(false); //No input, this shouldn't happen
        return typename ImageType::Pointer(NULL);
      }
  }
}
//--------------------------------------------------------------------



