#include "clitkImageToImageGenericFilter.h"
#include "clitkImageCommon.h"

// //--------------------------------------------------------------------
// clitk::ImageToImageGenericFilterBase::ImageToImageGenericFilterBase(std::string n)
//   :mIOVerbose(false) {
//   mFilterName = n;
//   mFailOnImageTypeError = true;
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::SetInputFilenames(const std::vector<std::string> & filenames) {
//     mInputFilenames=filenames;
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::SetInputFilename(const std::string & filename) {
//   std::vector<std::string> f;
//   f.push_back(filename);
//   SetInputFilenames(f);
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::AddInputFilename(const std::string & filename) {
//   mInputFilenames.push_back(filename);
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::SetOutputFilename(const std::string & filename) {
//   mOutputFilenames.clear();
//   mOutputFilenames.push_back(filename);
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::AddOutputFilename(const std::string & filename)
// {
//   mOutputFilenames.push_back(filename);
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::SetOutputFilenames(const std::vector<std::string> & filenames)
// {
//     mOutputFilenames.clear();
//     std::copy(filenames.begin(),filenames.end(),mOutputFilenames.begin());
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// std::string clitk::ImageToImageGenericFilterBase::GetOutputFilename()
// {
//     assert(mOutputFilenames.size() == 1);
//     return mOutputFilenames.front();
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::GetInputImageDimensionAndPixelType(unsigned int& dim, \
//         std::string& pixeltype,unsigned int& components)
// {
//   if (mInputFilenames.size())
//     {
//       int comp_temp,dim_temp; //clitkCommonImage takes ints
//       ReadImageDimensionAndPixelType(mInputFilenames[0], dim_temp, pixeltype,comp_temp);
//       components=comp_temp; dim=dim_temp;
//     }
//   else if (mInputVVImages.size())
//     {
//       pixeltype=mInputVVImages[0]->GetScalarTypeAsString();
//       dim=mInputVVImages[0]->GetNumberOfDimensions();
//       components=mInputVVImages[0]->GetNumberOfScalarComponents();
//     }
//   else
//     assert(false); //No input image, shouldn't happen
  
//   if (mIOVerbose) {
//     std::cout << "Input is " << mDim << "D " << mPixelTypeName << "." << std::endl;
//   }
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// vvImage::Pointer clitk::ImageToImageGenericFilterBase::GetOutputVVImage ()
// {
//     assert(mOutputVVImages.size());
//     return mOutputVVImages[0];
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// std::vector<vvImage::Pointer> clitk::ImageToImageGenericFilterBase::GetOutputVVImages()
// {
//     return mOutputVVImages;
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::SetInputVVImage (vvImage::Pointer input)
// {
//     mInputVVImages.clear();
//     mInputVVImages.push_back(input);
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::AddInputVVImage (vvImage::Pointer input)
// {
//     mInputVVImages.push_back(input);
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::SetInputVVImages (std::vector<vvImage::Pointer> input)
// {
//     mInputVVImages=input;
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::PrintAvailableImageTypes() {
//   std::cout << GetAvailableImageTypes();
// }
// //--------------------------------------------------------------------



// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::ImageTypeError() {
//   std::cerr << "**Error** The filter <" << mFilterName << "> is not available for " 
//             << mDim << "D images with pixel=" 
//             << mPixelTypeName << " and "
//             << mNbOfComponents << " component." << std::endl;
//   std::cerr << GetAvailableImageTypes();
//   exit(0);
// }
// //--------------------------------------------------------------------


// //--------------------------------------------------------------------
// void clitk::ImageToImageGenericFilterBase::SetImageTypeError() {
//   std::cerr << "TODO ! " << std::endl;
//   exit(0);
// }
// //--------------------------------------------------------------------
