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

#ifndef CLITKLABELIMAGEOVERLAPMEASUREGENERICFILTER_H
#define CLITKLABELIMAGEOVERLAPMEASUREGENERICFILTER_H

// clitk 
#include "clitkImageToImageGenericFilter.h"
#include "clitkLabelImageOverlapMeasureFilter.h"
#include "clitkBoundingBoxUtils.h"
#include "clitkCropLikeImageFilter.h"

//--------------------------------------------------------------------
namespace clitk 
{

  template<class ArgsInfoType>
  class ITK_EXPORT LabelImageOverlapMeasureGenericFilter:
    public ImageToImageGenericFilter<LabelImageOverlapMeasureGenericFilter<ArgsInfoType> >
  {
  public:
    //--------------------------------------------------------------------
    LabelImageOverlapMeasureGenericFilter();
  
    //--------------------------------------------------------------------
    typedef ImageToImageGenericFilter<LabelImageOverlapMeasureGenericFilter<ArgsInfoType> > Superclass;
    typedef LabelImageOverlapMeasureGenericFilter Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
   
    //--------------------------------------------------------------------
    itkNewMacro(Self);  
    itkTypeMacro(LabelImageOverlapMeasureGenericFilter, LightObject);

    //--------------------------------------------------------------------
    void SetArgsInfo(const ArgsInfoType & a);
    template<class FilterType> 
      void SetOptionsFromArgsInfoToFilter(FilterType * f) ;

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class ImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
      ArgsInfoType mArgsInfo;

  private:
    LabelImageOverlapMeasureGenericFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  };// end class
  //--------------------------------------------------------------------
} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkLabelImageOverlapMeasureGenericFilter.txx"
#endif

#endif // #define CLITKRELATIVEPOSITIONANALYZERGENERICFILTER_H
