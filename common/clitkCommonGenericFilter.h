/*=========================================================================

  Program:   clitk
  Module:    $RCSfile: clitkCommonGenericFilter.h,v $
  Language:  C++
  Date:      $Date: 2010/01/29 08:48:42 $
  Version:   $Revision: 1.1 $
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

#ifndef CLITKCOMMONGENERICFILTER_H
#define CLITKCOMMONGENERICFILTER_H

// clitk include
#include "clitkCommon.h"

/*--------------------------------------------------------------------
  DISCLAIMER : I obviously know how to make this mess much clearer and
  shorter, but I fear its being too long to put in this margin ...
  D.S.
  -------------------------------------------------------------------- */

namespace clitk {
  
  //--------------------------------------------------------------------
  template<class FilterType>
  class GenericFilterFunctorBase {
  public:
    GenericFilterFunctorBase(FilterType * f) { mFilter = f; }
    virtual void DoIt()= 0;
    FilterType * mFilter;
  };
 
  //--------------------------------------------------------------------
  template<class FilterType, unsigned int Dim, class PixelType>
  class GenericFilterFunctorWithDimAndPixelType: public GenericFilterFunctorBase<FilterType> {
  public:
    GenericFilterFunctorWithDimAndPixelType(FilterType * f): GenericFilterFunctorBase<FilterType>(f) {}
    virtual void DoIt() {       
      //GenericFilterFunctorBase<FilterType>::mFilter->template UpdateWithDimAndPixelType<Dim,PixelType>();
      typedef itk::Image<PixelType,Dim> InputImageType;
      GenericFilterFunctorBase<FilterType>::mFilter->template UpdateWithInputImageType<InputImageType>();
    }
  };

  //--------------------------------------------------------------------
  template<class FilterType>
  class ImageTypesManager  {
  public:
    typedef std::map<std::string, GenericFilterFunctorBase<FilterType>*> MapOfPixelTypeToFunctionType;
    std::map<int, MapOfPixelTypeToFunctionType> mMapOfImageTypeToFunction;

    ImageTypesManager(FilterType * f) { mFilter = f;  }
    virtual void DoIt(int dim, std::string pixelname) {
      // std::cout << "ImageTypesManager DoIt " << dim << " " << pixelname << std::endl;
      if (mMapOfImageTypeToFunction[dim][pixelname])
        mMapOfImageTypeToFunction[dim][pixelname]->DoIt();
    }
    template<unsigned int Dim, class Pixeltype>
    void AddNewDimensionAndPixelType() {
      // std::cout << "Adding Dim=" << Dim << " and PT = " << GetTypeAsString<Pixeltype>() << std::endl;
      mFilter->AddImageType(Dim, GetTypeAsString<Pixeltype>());
      mMapOfImageTypeToFunction[Dim][ GetTypeAsString<Pixeltype>() ] = 
        new GenericFilterFunctorWithDimAndPixelType<FilterType, Dim, Pixeltype>(mFilter);
    }
    FilterType * mFilter;
  };
  //--------------------------------------------------------------------

} // end namespace

#endif // end CLITKCOMMONGENERICFILTER_H
