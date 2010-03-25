#ifndef CLITKIMAGETOIMAGEGENERICFILTER_H
#define CLITKIMAGETOIMAGEGENERICFILTER_H
#include "clitkImageToImageGenericFilterBase.h"

namespace clitk {
  
//--------------------------------------------------------------------
  template<class FilterType>
  class ImageToImageGenericFilter: public ImageToImageGenericFilterBase {
    
  public: 
    
    typedef ImageToImageGenericFilter<FilterType> Self;

    // constructor - destructor
    ImageToImageGenericFilter(std::string filterName);

    // Main function to call for using the filter. 
    virtual bool Update();
    virtual bool CheckImageType(unsigned int dim,unsigned int ncomp, std::string pixeltype);
    virtual bool CheckImageType();
    virtual std::string GetAvailableImageTypes();

  protected:
    // Object that will manage the list of templatized function for
    // each image type.
    ImageTypesManager<FilterType> mImageTypesManager;
    
  }; // end class clitk::ImageToImageGenericFilter

#include "clitkImageToImageGenericFilter.txx"  

} // end namespace

#endif /* end #define CLITKIMAGETOIMAGEGENERICFILTER_H */

