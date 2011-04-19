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
#ifndef clitkGuerreroVentilationGenericFilter_H
#define clitkGuerreroVentilationGenericFilter_H
/**
 -------------------------------------------------------------------
 * @file   clitkGuerreroVentilationGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008
 -------------------------------------------------------------------*/

// clitk include
#include "clitkImageToImageGenericFilter.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class GuerreroVentilationGenericFilter : 
  public ImageToImageGenericFilter<GuerreroVentilationGenericFilter> {
  public:
	
    // Constructor 
    GuerreroVentilationGenericFilter();
    virtual ~GuerreroVentilationGenericFilter() {}

    // Types
    typedef GuerreroVentilationGenericFilter  Self;
    typedef ImageToImageGenericFilterBase     Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Set methods
    void SetBloodCorrectionFactor(double f) {blood_mass_factor=f;}
    void SetUseCorrectFormula(bool u) {use_correct_formula=u;}

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:  
    template<unsigned int Dim> void InitializeImageType();
    //Parameters
    double blood_mass_factor;
    bool use_correct_formula;

  }; // end class GuerreroVentilationGenericFilter
//--------------------------------------------------------------------

} // end namespace
//--------------------------------------------------------------------

#endif //#define clitkGuerreroVentilationGenericFilter_H

