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

#ifndef CLITKEXTRACTPATIENTGENERICFILTER_H
#define CLITKEXTRACTPATIENTGENERICFILTER_H

// clitk 
#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkAddRelativePositionConstraintToLabelImageFilter.h"
#include "clitkSliceBySliceRelativePositionFilter.h"

//--------------------------------------------------------------------
namespace clitk 
{

  template<class ArgsInfoType>
  class ITK_EXPORT RelativePositionGenericFilter:
    public ImageToImageGenericFilter<RelativePositionGenericFilter<ArgsInfoType> >
  {
  public:
    //--------------------------------------------------------------------
    RelativePositionGenericFilter();
  
    //--------------------------------------------------------------------
    typedef ImageToImageGenericFilter<RelativePositionGenericFilter<ArgsInfoType> > Superclass;
    typedef RelativePositionGenericFilter Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
   
    //--------------------------------------------------------------------
    itkNewMacro(Self);  
    itkTypeMacro(RelativePositionGenericFilter, LightObject);

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
    RelativePositionGenericFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  };// end class
  //--------------------------------------------------------------------
} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkRelativePositionGenericFilter.txx"
#endif

#endif // #define CLITKEXTRACTPATIENTGENERICFILTER_H
