/*-------------------------------------------------------------------------

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
-------------------------------------------------------------------------*/

#ifndef clitkGuerreroVentilationGenericFilter_H
#define clitkGuerreroVentilationGenericFilter_H

/**
 -------------------------------------------------------------------
 * @file   clitkGuerreroVentilationGenericFilter.h
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

namespace clitk {
  
  //--------------------------------------------------------------------
  class GuerreroVentilationGenericFilter : public ImageToImageGenericFilter {
  
  public:
	
    // Constructor 
    GuerreroVentilationGenericFilter ();

    // Types
    typedef GuerreroVentilationGenericFilter  Self;
    typedef ImageToImageGenericFilter     Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Set methods
    void SetBloodCorrectionFactor(double f) {blood_mass_factor=f;}
    void SetUseCorrectFormula(bool u) {use_correct_formula=u;}

    // Update
    void Update ();

  protected:  
    //Parameters
    double blood_mass_factor;
    bool use_correct_formula;

    //--------------------------------------------------------------------
    template<unsigned int Dim> void Update_WithDim();
    template<unsigned int Dim, class PixelType> void Update_WithDimAndPixelType();
    //--------------------------------------------------------------------

  }; // end class GuerreroVentilationGenericFilter
//--------------------------------------------------------------------

} // end namespace
//--------------------------------------------------------------------

#endif //#define clitkGuerreroVentilationGenericFilter_H

