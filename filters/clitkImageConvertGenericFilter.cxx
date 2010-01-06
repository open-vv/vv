#ifndef CLITKIMAGECONVERTGENERICFILTER_CXX
#define CLITKIMAGECONVERTGENERICFILTER_CXX

/**
 -------------------------------------------------
 * @file   clitkImageConvertGenericFilter.cxx
 * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
 * @date   05 May 2008 10:57:19
 * 
 * @brief  
 * 
 * 
 -------------------------------------------------*/

#include "clitkImageConvertGenericFilter.h"

//--------------------------------------------------------------------
clitk::ImageConvertGenericFilter::ImageConvertGenericFilter():ImageToImageGenericFilter() {
  mOutputPixelTypeName = "NotSpecified";
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::ImageConvertGenericFilter::Update() {  
  // Load image header
  itk::ImageIOBase::Pointer header = clitk::readImageHeader(mInputFilenames[0]);

  // Determine dim, pixel type, number of components
  mDim = header->GetNumberOfDimensions();
  mPixelTypeName = header->GetComponentTypeAsString(header->GetComponentType());  
  mNbOfComponents = header->GetNumberOfComponents();

  // Verbose stuff
  if (mIOVerbose) {
    if (mInputFilenames.size() == 1) {
      std::cout << "Input image <" << mInputFilenames[0] << "> is ";
      printImageHeader(header, std::cout);
      std::cout << std::endl;
    }
    else {
      for(unsigned int i=0; i<mInputFilenames.size(); i++) {
	std::cout << "Input image " << i << " <" << mInputFilenames[i] << "> is ";
	itk::ImageIOBase::Pointer h = clitk::readImageHeader(mInputFilenames[i]);	
	printImageHeader(h, std::cout);
	std::cout << std::endl;
      }
    }
  }

  // Switch by dimension
  if (mInputFilenames.size() > 1) mDim++;
  if (mDim == 2) { Update_WithDim<2>(); return; }
  if (mDim == 3) { Update_WithDim<3>(); return; }
  if (mDim == 4) { Update_WithDim<4>(); return; }

  std::cerr << "Error, dimension of input image is " << mDim << ", but I only work with 2,3,4." << std::endl;
  exit(0);
}
//--------------------------------------------------------------------

#endif /* end #define CLITKIMAGECONVERTGENERICFILTER_CXX */

