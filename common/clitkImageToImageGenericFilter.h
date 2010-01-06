#ifndef CLITKIMAGETOIMAGEGENERICFILTER_H
#define CLITKIMAGETOIMAGEGENERICFILTER_H

/**
 ===================================================================
 * @file   clitkImageToImageGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   05 May 2008 14:40:51

 * @brief  

 ===================================================================*/

// clitk include
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include <itkImage.h>

#include <vvImage.h>
#include <vvFromITK.h>
#include <vvToITK.h>
#include <list>

namespace clitk {
  
  class ImageToImageGenericFilter: public itk::Object {
    
  public: 
    // constructor - destructor
    ImageToImageGenericFilter();

    // Types
    typedef ImageToImageGenericFilter     Self;
    typedef Object                        Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // Generic IO
    /// Returns the dimension and pixel type of the *first* input
    void GetInputImageDimensionAndPixelType(unsigned int& dim, std::string& pixeltype,unsigned int & components);

    // File IO
    void SetInputFilename(const std::string & filename);
    void AddInputFilename(const std::string & filename);
    void SetInputFilenames(const std::vector<std::string> & filenames);
    void SetOutputFilename(const std::string & filename);
    void AddOutputFilename(const std::string & filename);
    void SetOutputFilenames(const std::vector<std::string> & filenames);
    std::string GetOutputFilename();
    void SetIOVerbose(bool b) { mIOVerbose = b; }

    // VVImage IO
    void SetInputVVImage (vvImage::Pointer input);
    void SetInputVVImages (std::vector<vvImage::Pointer> input);
    void AddInputVVImage (vvImage::Pointer input);
    vvImage::Pointer GetOutputVVImage ();
    std::vector<vvImage::Pointer> GetOutputVVImages ();

    /// Main function to implement
    virtual void Update() = 0;
    
  protected:
    /// Call this function to dispatch an output towards the correct sink
    template<class ImageType> 
        void SetNextOutput(typename ImageType::Pointer output);
    /// Call this function to get the nth itk input image, regardless of input source
    template<class ImageType> 
        typename ImageType::Pointer GetInput(unsigned int n);

    std::vector<std::string> mInputFilenames;
    std::list<std::string> mOutputFilenames;

    bool mIOVerbose;
    unsigned int mDim;
    std::string mPixelTypeName;
    unsigned int mNbOfComponents;

    std::vector<vvImage::Pointer> mInputVVImages;
    std::vector<vvImage::Pointer> mOutputVVImages;

  }; // end class clitk::ImageToImageGenericFilter

} // end namespace

template<class ImageType> 
void clitk::ImageToImageGenericFilter::SetNextOutput(typename ImageType::Pointer output)
{
    if (mOutputFilenames.size())
    {
        clitk::writeImage<ImageType>(output, mOutputFilenames.front(), mIOVerbose);
        mOutputFilenames.pop_front();
    }
    if (mInputVVImages.size()) //We assume that if a vv image is set as input, we want one as the output
        mOutputVVImages.push_back(vvImageFromITK<ImageType::ImageDimension,typename ImageType::PixelType>(output));
}
template<class ImageType> 
typename ImageType::Pointer clitk::ImageToImageGenericFilter::GetInput(unsigned int n)
{
    if (mInputFilenames.size() > n)
        return clitk::readImage<ImageType>(mInputFilenames[n], mIOVerbose);
    else if (mInputVVImages.size() > n)
        return typename ImageType::Pointer(const_cast<ImageType*>(vvImageToITK<ImageType>(mInputVVImages[n]).GetPointer()));
    else
        assert(false); //No input, this shouldn't happen
}
#endif /* end #define CLITKIMAGETOIMAGEGENERICFILTER_H */

