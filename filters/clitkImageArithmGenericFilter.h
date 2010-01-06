/*-------------------------------------------------------------------------

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
-------------------------------------------------------------------------*/

#ifndef CLITKIMAGEARITHMGENERICFILTER_H
#define CLITKIMAGEARITHMGENERICFILTER_H

/**
 -------------------------------------------------------------------
 * @file   clitkImageArithmGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008 08:37:53

 * @brief  
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

namespace clitk {
  
  //--------------------------------------------------------------------
  // Main class for an Image Resample Generic Filter 
  // (multiple dimensions, multiple pixel types)
  class ImageArithmGenericFilter : public clitk::ImageToImageGenericFilter {
  
  public:
	
    // Constructor 
    ImageArithmGenericFilter ();

    // Types
    typedef ImageArithmGenericFilter      Self;
    typedef ImageToImageGenericFilter     Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Set methods
    void SetDefaultPixelValue (double value) {  mDefaultPixelValue = value ;}
    void SetTypeOfOperation (int value) {  mTypeOfOperation = value ;}
    void SetScalar (double value) {  mScalar = value ;}

    // Get methods
    double GetDefaultPixelValue () { return  mDefaultPixelValue ;} 
    int GetTypeOfOperation () { return  mTypeOfOperation ;} 
    double GetScalar () { return  mScalar ;} 

    // Update
    void Update ();

  protected:  
    bool mIsOperationUseASecondImage;
    double mScalar;
    double mDefaultPixelValue;
    int mTypeOfOperation;  

    //--------------------------------------------------------------------
    template<unsigned int Dim> void Update_WithDim();
    template<unsigned int Dim, class PixelType> void Update_WithDimAndPixelType();

    template<class ImageType>
    typename ImageType::Pointer ComputeImage(typename ImageType::Pointer inputImage);

    template<class ImageType1, class ImageType2>
    typename ImageType1::Pointer
    ComputeImage(typename ImageType1::Pointer inputImage1, 
		 typename ImageType2::Pointer inputImage2);
    //--------------------------------------------------------------------

  }; // end class ImageArithmGenericFilter
//--------------------------------------------------------------------

#include "clitkImageArithmGenericFilter.txx"

} // end namespace
//--------------------------------------------------------------------

#endif //#define CLITKIMAGEARITHMGENERICFILTER_H

