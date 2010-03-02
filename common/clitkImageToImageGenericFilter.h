/*=========================================================================

  Program:   clitk
  Module:    $RCSfile: clitkImageToImageGenericFilter.h,v $
  Language:  C++
  Date:      $Date: 2010/03/02 12:20:50 $
  Version:   $Revision: 1.8 $
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

