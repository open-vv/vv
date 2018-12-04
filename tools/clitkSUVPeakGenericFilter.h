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
#ifndef CLITKSUVPEAKGENERICFILTER_H
#define CLITKSUVPEAKGENERICFILTER_H
/**
 -------------------------------------------------------------------
 * @file   clitkSUVPeakGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008 08:37:53

 * @brief
 -------------------------------------------------------------------*/

// clitk include
#include "clitkCommon.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkSUVPeak_ggo.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

//--------------------------------------------------------------------
namespace clitk {

  template<class args_info_type>
  class ITK_EXPORT SUVPeakGenericFilter:
    public clitk::ImageToImageGenericFilter<SUVPeakGenericFilter<args_info_type> > {

  public:

    // Constructor
    SUVPeakGenericFilter ();

    // Types
    typedef SUVPeakGenericFilter        Self;
    typedef ImageToImageGenericFilterBase   Superclass;
    typedef itk::SmartPointer<Self>         Pointer;
    typedef itk::SmartPointer<const Self>   ConstPointer;

    // New
    itkNewMacro(Self);



    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type & a);

    //SUVPeak
    template<class ImageType> typename ImageType::Pointer ComputeMeanFilterKernel(const typename ImageType::SpacingType & spacing, double radius);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    args_info_type mArgsInfo;

  }; // end class SUVPeakGenericFilter

} // end namespace
//--------------------------------------------------------------------


#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkSUVPeakGenericFilter.txx"
#endif

#endif //#define CLITKSUVPEAKGENERICFILTER_H

