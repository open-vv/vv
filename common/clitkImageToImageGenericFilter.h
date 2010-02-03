/*=========================================================================

  Program:   clitk
  Module:    $RCSfile: clitkImageToImageGenericFilter.h,v $
  Language:  C++
  Date:      $Date: 2010/02/03 13:08:52 $
  Version:   $Revision: 1.4 $
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

#ifndef CLITKIMAGETOIMAGEGENERICFILTER_H
#define CLITKIMAGETOIMAGEGENERICFILTER_H

// clitk include
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkCommonGenericFilter.h"
#include <itkImage.h>

// vv include
#include <vvImage.h>
#include <vvFromITK.h>
#include <vvToITK.h>
#include <list>
#include <set>

namespace clitk {
  
  //--------------------------------------------------------------------
  class ImageToImageGenericFilterBase: public itk::Object {
    
  public: 
    // constructor - destructor
    ImageToImageGenericFilterBase(std::string filterName);
    virtual ~ImageToImageGenericFilterBase() {}

    // Types
    typedef ImageToImageGenericFilterBase Self;
    typedef Object                        Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // Filters information
    const std::string & GetFilterName() { return mFilterName; }
    void SetFilterName(std::string & n) { mFilterName = n; }

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

    // Information on available image types
    void PrintAvailableImageTypes();
    std::string GetAvailableImageTypes();
    bool CheckDimension(unsigned int d);
    bool CheckPixelType(std::string pt);

    // Main function to call for using the filter. 
    virtual bool Update() = 0;

    // Use internally only (TO PUT PROTECTED !!!)
    void AddImageType(unsigned int d, std::string p);

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
    std::string mFilterName;

    std::vector<vvImage::Pointer> mInputVVImages;
    std::vector<vvImage::Pointer> mOutputVVImages;

    std::set<std::string> mListOfAllowedPixelTypes;
    std::set<unsigned int> mListOfAllowedDimensions;
    bool CheckImageType();
    bool CheckDimension();
    bool CheckPixelType();
    void ImageTypeError();
    void SetImageTypeError();
    bool mFailOnImageTypeError;

  }; // end class clitk::ImageToImageGenericFilter


  //--------------------------------------------------------------------
  template<class FilterType>
  class ImageToImageGenericFilter: public ImageToImageGenericFilterBase {
    
  public: 
    
    typedef ImageToImageGenericFilter<FilterType> Self;

    // constructor - destructor
    ImageToImageGenericFilter(std::string filterName);
    virtual ~ImageToImageGenericFilter() { delete mImageTypesManager; }

    // Main function to call for using the filter. 
    virtual bool Update();

  protected:
    // Object that will manage the list of templatized function for
    // each image type.
    ImageTypesManager<FilterType> * mImageTypesManager;
    
  }; // end class clitk::ImageToImageGenericFilter

  // #define ADD_IMAGE_DIMENSION(DIM) Initialize<DIM>();

#define ADD_IMAGE_TYPE(DIM, PT) this->mImageTypesManager->template AddNewDimensionAndPixelType<DIM, PT>();


#include "clitkImageToImageGenericFilter.txx"

} // end namespace

#endif /* end #define CLITKIMAGETOIMAGEGENERICFILTER_H */

