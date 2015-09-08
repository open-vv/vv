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

#ifndef CLITKPADIMAGEGENERICFILTER_H
#define CLITKPADIMAGEGENERICFILTER_H

// clitk
#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkPadImage_ggo.h"

//--------------------------------------------------------------------
namespace clitk
{

  class ITK_EXPORT PadImageGenericFilter:
    public ImageToImageGenericFilter<PadImageGenericFilter>
  {
  public:
    enum
    {
      PAD_ERR_SUCCESS = 0,
      PAD_ERR_NOT_SAME_SPACING = -1,
      PAD_ERR_NOT_LIKE_LARGER = -2
    };

    //--------------------------------------------------------------------
    PadImageGenericFilter();

    //--------------------------------------------------------------------
    typedef PadImageGenericFilter   Self;
    typedef ImageToImageGenericFilter<PadImageGenericFilter> Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef args_info_clitkPadImage       args_info_type;

    //--------------------------------------------------------------------
    itkNewMacro(Self);
    itkTypeMacro( PadImageGenericFilter, LightObject );

    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type& a);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class ImageType>
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();

    template <class ImageType, class PadBoundType, unsigned int dim>
    int PadLike(typename ImageType::Pointer input, PadBoundType* padLower, PadBoundType* padUpper);
    
    args_info_type mArgsInfo;

  };// end class
  //--------------------------------------------------------------------
} // end namespace clitk

#endif // #define CLITKPADIMAGEGENERICFILTER_H
