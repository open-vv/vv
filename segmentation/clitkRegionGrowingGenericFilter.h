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

#ifndef CLITKREGIONGROWINGGENERICFILTER_H
#define CLITKREGIONGROWINGGENERICFILTER_H

// clitk include
#include "clitkIO.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkLocallyAdaptiveThresholdConnectedImageFilter.h"
#include "clitkExplosionControlledThresholdConnectedImageFilter.h"

//itk include
#include "itkLightObject.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkConfidenceConnectedImageFilter.h"

//--------------------------------------------------------------------
namespace clitk 
{
  template<class ArgsInfoType>
  class ITK_EXPORT RegionGrowingGenericFilter: 
    public ImageToImageGenericFilter<RegionGrowingGenericFilter<ArgsInfoType> >
  {

  public:
    //----------------------------------------
    RegionGrowingGenericFilter();

    //----------------------------------------
    typedef RegionGrowingGenericFilter         Self;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    //----------------------------------------
    itkNewMacro(Self);  
    itkTypeMacro( RegionGrowingGenericFilter, LightObject );

    //--------------------------------------------------------------------
    void SetArgsInfo(const ArgsInfoType & a);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class ImageType>  
    void UpdateWithInputImageType();

  protected:
    void Modified() {} // Need for using itkMacros
    template<unsigned int Dim> void InitializeImageType();
    ArgsInfoType mArgsInfo;

  }; // end class
} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkRegionGrowingGenericFilter.txx"
#endif

#endif // #define CLITKREGIONGROWINGGENERICFILTER_H
