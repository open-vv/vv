#include "clitkImageToImageGenericFilter.h"
#include "clitkImageCommon.h"

//--------------------------------------------------------------------
clitk::ImageToImageGenericFilter::ImageToImageGenericFilter() :
    mIOVerbose(false)
{}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilter::SetInputFilenames(const std::vector<std::string> & filenames) {
  mInputFilenames.resize(filenames.size());
  std::copy(filenames.begin(), filenames.end(), mInputFilenames.begin());
}

void clitk::ImageToImageGenericFilter::SetInputFilename(const std::string & filename) {
  std::vector<std::string> f;
  f.push_back(filename);
  SetInputFilenames(f);
}

void clitk::ImageToImageGenericFilter::AddInputFilename(const std::string & filename) {
  mInputFilenames.push_back(filename);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilter::SetOutputFilename(const std::string & filename) {
  mOutputFilenames.clear();
  mOutputFilenames.push_back(filename);
}
void clitk::ImageToImageGenericFilter::AddOutputFilename(const std::string & filename)
{
  mOutputFilenames.push_back(filename);
}
void clitk::ImageToImageGenericFilter::SetOutputFilenames(const std::vector<std::string> & filenames)
{
    std::copy(filenames.begin(), filenames.end(), mOutputFilenames.begin());
}
std::string clitk::ImageToImageGenericFilter::GetOutputFilename()
{
    assert(mOutputFilenames.size() == 1);
    return mOutputFilenames.front();
}
//--------------------------------------------------------------------
void clitk::ImageToImageGenericFilter::GetInputImageDimensionAndPixelType(unsigned int& dim,\
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
}
vvImage::Pointer clitk::ImageToImageGenericFilter::GetOutputVVImage ()
{
    assert(mOutputVVImages.size());
    return mOutputVVImages[0];
}

std::vector<vvImage::Pointer> clitk::ImageToImageGenericFilter::GetOutputVVImages()
{
    return mOutputVVImages;
}

void clitk::ImageToImageGenericFilter::SetInputVVImage (vvImage::Pointer input)
{
    mInputVVImages.clear();
    mInputVVImages.push_back(input);
}

void clitk::ImageToImageGenericFilter::AddInputVVImage (vvImage::Pointer input)
{
    mInputVVImages.push_back(input);
}

void clitk::ImageToImageGenericFilter::SetInputVVImages (std::vector<vvImage::Pointer> input)
{
    mInputVVImages=input;
}

