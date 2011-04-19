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

#ifndef CLITKIRESAMPLEIMAGEGENERICFILTER_H
#define CLITKIRESAMPLEIMAGEGENERICFILTER_H

// clitk include
#include "clitkResampleImage_ggo.h"
#include "clitkImageToImageGenericFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class ITK_EXPORT ResampleImageGenericFilter: 
    public ImageToImageGenericFilter<ResampleImageGenericFilter> {
    
  public: 
    // Constructor
    ResampleImageGenericFilter();

    // Types
    typedef ResampleImageGenericFilter    Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef args_info_clitkResampleImage  ArgsInfoType;

    // New
    itkNewMacro(Self);

    // Args
    void SetArgsInfo(const ArgsInfoType & a);
    
    // Main function
    template<class InputImageType> void UpdateWithInputImageType();

  protected:
    ArgsInfoType mArgsInfo;
    template<unsigned int Dim> void InitializeImageType();
     
  }; // end class ResampleImageGenericFilter
  //--------------------------------------------------------------------
    
} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkResampleImageGenericFilter.txx"
#endif

#endif /* end #define CLITKIRESAMPLEIMAGEGENERICFILTER_H */

