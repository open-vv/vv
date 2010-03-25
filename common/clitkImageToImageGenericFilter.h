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

