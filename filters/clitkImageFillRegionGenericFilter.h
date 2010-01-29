/*-------------------------------------------------------------------------

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
-------------------------------------------------------------------------*/

#ifndef CLITKIMAGEFILLREGIONGENERICFILTER_H
#define CLITKIMAGEFILLREGIONGENERICFILTER_H

/**
 -------------------------------------------------------------------
 * @file   clitkImageFillRegionGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008
 -------------------------------------------------------------------*/

// clitk include
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkImageToImageGenericFilter.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class ImageFillRegionGenericFilter: 
    public clitk::ImageToImageGenericFilter<ImageFillRegionGenericFilter> {
  
  public:
	
    // Constructor 
    ImageFillRegionGenericFilter ();
    ~ImageFillRegionGenericFilter (){;}

    // Types
    typedef ImageFillRegionGenericFilter  Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Set methods
    void SetFillPixelValue (double value) { mPixelValue = value; }
    void SetRegion(int * size, int * start) { mSize = size; mStart = start ; }
    void SetSphericRegion(std::vector<double> & radius, std::vector<double> & center);
    void SetSphericRegion(std::vector<double> & radius);

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:  
    template<unsigned int Dim> void InitializeImageType();
    double mPixelValue;
    int * mSize;
    int * mStart;
    std::vector<double> mCenter;
    std::vector<double> mRadius;
    bool mSphericRegion;
    bool m_IsCentered;

    template<unsigned int Dim, class PixelType> void Update_WithDimAndPixelType_SphericRegion();

  }; // end class ImageFillRegionGenericFilter
//--------------------------------------------------------------------

#include "clitkImageFillRegionGenericFilter.txx"

} // end namespace
//--------------------------------------------------------------------

#endif //#define CLITKIMAGEFILLREGIONGENERICFILTER_H

