/*=========================================================================

  Program:   clitk
  Module:    $RCSfile: clitkCommonGenericFilter.h,v $
  Language:  C++
  Date:      $Date: 2010/02/09 14:19:28 $
  Version:   $Revision: 1.3 $
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
    virtual void Execute()= 0;
    FilterType * mFilter;
  };
 
  //--------------------------------------------------------------------
  template<class FilterType, class InputImageType>
  class GenericFilterFunctorWithDimAndPixelType: public GenericFilterFunctorBase<FilterType> {
  public:
    GenericFilterFunctorWithDimAndPixelType(FilterType * f): GenericFilterFunctorBase<FilterType>(f) {}
    virtual void Execute() {       
      GenericFilterFunctorBase<FilterType>::mFilter->template UpdateWithInputImageType<InputImageType>();
    }
  };

  //--------------------------------------------------------------------
  template<class FilterType>
  class ImageTypesManager  {
  public:
    typedef std::map<std::string, GenericFilterFunctorBase<FilterType>*> MapOfPixelTypeToFunctionType;
    typedef std::map<unsigned int, MapOfPixelTypeToFunctionType> MapOfImageComponentsToFunctionType;
    typedef std::map<unsigned int, MapOfImageComponentsToFunctionType> MapOfImageDimensionToFunctionType;
    MapOfImageDimensionToFunctionType mMapOfImageTypeToFunction;

    ImageTypesManager(FilterType * f) { mFilter = f;  }
    virtual void DoIt(int dim, int ncomp, std::string pixelname) {
      // std::cout << "ImageTypesManager DoIt " << dim << " " << pixelname << std::endl;
      if (mMapOfImageTypeToFunction[dim][ncomp][pixelname])
        mMapOfImageTypeToFunction[dim][ncomp][pixelname]->Execute();
    }
    template<unsigned int Dim, unsigned int NComp, class PixelType>
    void AddNewDimensionAndPixelType() {
        typedef itk::Image<itk::Vector<PixelType,NComp>,Dim> InputImageType;
        mMapOfImageTypeToFunction[Dim][NComp][ GetTypeAsString<PixelType>() ] = 
            new GenericFilterFunctorWithDimAndPixelType<FilterType, InputImageType>(mFilter);
    }
    /// Specialization for NComp == 1
    template<unsigned int Dim, class PixelType>
    void AddNewDimensionAndPixelType() {
        typedef itk::Image<PixelType,Dim> InputImageType;
        mMapOfImageTypeToFunction[Dim][1][ GetTypeAsString<PixelType>() ] = 
            new GenericFilterFunctorWithDimAndPixelType<FilterType, InputImageType>(mFilter);
    }
    FilterType * mFilter;
  };
  //--------------------------------------------------------------------

} // end namespace

#endif // end CLITKCOMMONGENERICFILTER_H
