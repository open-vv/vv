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

#ifndef CLITKCROPIMAGEGENERICFILTER_H
#define CLITKCROPIMAGEGENERICFILTER_H

// clitk 
#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkCropLikeImageFilter.h"
#include "clitkAutoCropFilter.h"

// itk
#include <itkCropImageFilter.h>

//--------------------------------------------------------------------
namespace clitk 
{

  template<class ArgsInfoType>
  class ITK_EXPORT CropImageGenericFilter:
    public ImageToImageGenericFilter<CropImageGenericFilter<ArgsInfoType> >
  {
  public:
    //--------------------------------------------------------------------
    CropImageGenericFilter();
  
    //--------------------------------------------------------------------
    typedef CropImageGenericFilter   Self;
    typedef ImageToImageGenericFilter<CropImageGenericFilter<ArgsInfoType> > Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
   
    //--------------------------------------------------------------------
    itkNewMacro(Self);  
    itkTypeMacro( CropImageGenericFilter, LightObject );

    //--------------------------------------------------------------------
    void SetArgsInfo(const ArgsInfoType & a);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class ImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    ArgsInfoType mArgsInfo;

  };// end class
  //--------------------------------------------------------------------
} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkCropImageGenericFilter.txx"
#endif

#endif // #define CLITKCROPIMAGEGENERICFILTER_H
