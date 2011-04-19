/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/

#ifndef CLITKEXTRACTLUNGSGENERICFILTER_H
#define CLITKEXTRACTLUNGSGENERICFILTER_H

// clitk
#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkExtractLungFilter.h"

//--------------------------------------------------------------------
namespace clitk 
{
  template<class ArgsInfoType>
  class ITK_EXPORT ExtractLungGenericFilter: 
    public ImageToImageGenericFilter<ExtractLungGenericFilter<ArgsInfoType> >
  {
    
  public:
    //--------------------------------------------------------------------
    ExtractLungGenericFilter();

    //--------------------------------------------------------------------
    typedef ExtractLungGenericFilter      Self;
    typedef ImageToImageGenericFilter<ExtractLungGenericFilter<ArgsInfoType> > Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    //--------------------------------------------------------------------
    itkNewMacro(Self);  
    itkTypeMacro(ExtractLungGenericFilter, LightObject);

    //--------------------------------------------------------------------
    void SetArgsInfo(const ArgsInfoType & a);
    template<class FilterType>
    void SetOptionsFromArgsInfoToFilter(FilterType * f);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class ImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    ArgsInfoType mArgsInfo;
    
  }; // end class
  //--------------------------------------------------------------------
    
} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractLungGenericFilter.txx"
#endif

#endif // #define CLITKEXTRACTLUNGSGENERICFILTER_H
