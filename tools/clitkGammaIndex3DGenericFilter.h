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
#ifndef CLITKGAMMAINDEX3DGENERICFILTER_H
#define CLITKGAMMAINDEX3DGENERICFILTER_H
/**
 -------------------------------------------------------------------
 * @file   clitkGammaIndex3DGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008 08:37:53

 * @brief  
 -------------------------------------------------------------------*/

// clitk include
#include "clitkCommon.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkGammaIndex3D_ggo.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

//--------------------------------------------------------------------
namespace clitk {
  
  template<class args_info_type>
  class ITK_EXPORT GammaIndex3DGenericFilter:
    public clitk::ImageToImageGenericFilter<GammaIndex3DGenericFilter<args_info_type> > {
    
  public:
	
    // Constructor 
    GammaIndex3DGenericFilter ();

    // Types
    typedef GammaIndex3DGenericFilter        Self;
    typedef ImageToImageGenericFilterBase   Superclass;
    typedef itk::SmartPointer<Self>         Pointer;
    typedef itk::SmartPointer<const Self>   ConstPointer;

    // New
    itkNewMacro(Self);
    
    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type & a);

    // Set methods
    void SetDistance (double distance) {  mDistance = distance ;}
    void SetDose (double dose) {  mDose = dose ;}

    // Get methods
    double GetDistance () { return  mDistance ;}
    double GetDose () { return  mDose ;}

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:  
    template<unsigned int Dim> void InitializeImageType();
    double mDistance;
    double mDose;
    args_info_type mArgsInfo;

    //--------------------------------------------------------------------

  }; // end class GammaIndex3DGenericFilter

} // end namespace
//--------------------------------------------------------------------

  
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkGammaIndex3DGenericFilter.txx"
#endif

#endif //#define CLITKGAMMAINDEX3DMGENERICFILTER_H

