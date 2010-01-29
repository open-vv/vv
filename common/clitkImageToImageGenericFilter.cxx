#include "clitkImageToImageGenericFilter.h"
#include "clitkImageCommon.h"

//--------------------------------------------------------------------
clitk::ImageToImageGenericFilterBase::ImageToImageGenericFilterBase(std::string n)
  :mIOVerbose(false) {
  mFilterName = n;
  mListOfAllowedDimension.clear();
  mListOfAllowedPixelType.clear();
  mFailOnImageTypeError = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::AddImageType(unsigned int d, std::string p) {
  // Search for dimension (to not duplicate)
  std::vector<unsigned int>::const_iterator it = 
    std::find (mListOfAllowedDimension.begin(), 
               mListOfAllowedDimension.end(), d);
  if (it == mListOfAllowedDimension.end()) mListOfAllowedDimension.push_back(d);
  // Search for PixelType (to not duplicate)
  std::vector<std::string>::const_iterator itt = 
    std::find (mListOfAllowedPixelType.begin(), 
               mListOfAllowedPixelType.end(), p);
  if (itt == mListOfAllowedPixelType.end()) mListOfAllowedPixelType.push_back(p);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetInputFilenames(const std::vector<std::string> & filenames) {
  mInputFilenames.resize(filenames.size());
  std::copy(filenames.begin(), filenames.end(), mInputFilenames.begin());
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
    std::copy(filenames.begin(), filenames.end(), mOutputFilenames.begin());
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
  if (mInputFilenames.size())
    {
      int comp_temp,dim_temp; //clitkCommonImage takes ints
      ReadImageDimensionAndPixelType(mInputFilenames[0], dim_temp, pixeltype,comp_temp);
      components=comp_temp; dim=dim_temp;
    }
  else if (mInputVVImages.size())
    {
      pixeltype=mInputVVImages[0]->GetScalarTypeAsString();
      dim=mInputVVImages[0]->GetNumberOfDimensions();
      components=mInputVVImages[0]->GetNumberOfScalarComponents();
    }
  else
    assert(false); //No input image, shouldn't happen
  
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
bool clitk::ImageToImageGenericFilterBase::CheckImageType() {
  return (CheckDimension() && CheckPixelType());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool clitk::ImageToImageGenericFilterBase::CheckDimension(unsigned int d) {
  bool b = false;
  unsigned int i=0;
  while ((!b) && (i<mListOfAllowedDimension.size())) {
    b = (mListOfAllowedDimension[i] == d);
    i++;
  }
  return b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool clitk::ImageToImageGenericFilterBase::CheckPixelType(std::string pt) {
  bool b = false;
  unsigned int i=0;
  while ((!b) && (i<mListOfAllowedPixelType.size())) {
    b = (mListOfAllowedPixelType[i] == pt);
    i++;
  }
  return b;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool clitk::ImageToImageGenericFilterBase::CheckDimension() {
  return CheckDimension(mDim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool clitk::ImageToImageGenericFilterBase::CheckPixelType() {
  return CheckPixelType(mPixelTypeName);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::PrintAvailableImageTypes() {
  std::cout << "The filter <" << mFilterName << "> manages ";
  for(unsigned int i=0; i<mListOfAllowedDimension.size(); i++) {
    std::cout << mListOfAllowedDimension[i] << "D ";
  }
  std::cout << "images, with pixel types: ";
  for(unsigned int i=0; i<mListOfAllowedPixelType.size(); i++) {
    std::cout << mListOfAllowedPixelType[i] << " ";
  }
  std::cout << std::endl;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::ImageTypeError() {
  std::cerr << "**Error** The filter <" << mFilterName << "> is not available for " 
            << mDim << "D images with pixel=" 
            << mPixelTypeName << "." << std::endl;
  std::cerr << "**Error** Allowed image dim: \t";
  for(unsigned int i=0; i<mListOfAllowedDimension.size(); i++) {
    std::cerr << mListOfAllowedDimension[i] << " ";
  }
  std::cerr << std::endl << "**Error** Allowed pixel types: \t";
  for(unsigned int i=0; i<mListOfAllowedPixelType.size(); i++) {
    std::cerr << mListOfAllowedPixelType[i] << " ";
  }
  std::cerr << std::endl;
  exit(0);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilterBase::SetImageTypeError() {
  std::cerr << "TODO ! " << std::endl;
  exit(0);
}
//--------------------------------------------------------------------
