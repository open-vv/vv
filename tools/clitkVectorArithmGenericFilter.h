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
#ifndef CLITKVectorArithmGENERICFILTER_H
#define CLITKVectorArithmGENERICFILTER_H
/**
 -------------------------------------------------------------------
 * @file   clitkVectorArithmGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008 08:37:53

 * @brief  
 -------------------------------------------------------------------*/

// clitk include
#include "clitkCommon.h"
#include "clitkImageToImageGenericFilter.h"
#include "clitkVectorArithm_ggo.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

//--------------------------------------------------------------------
namespace clitk {
  
  template<class args_info_type>
  class ITK_EXPORT VectorArithmGenericFilter:
    public clitk::ImageToImageGenericFilter<VectorArithmGenericFilter<args_info_type> > {
    
  public:
	
    // Constructor 
    VectorArithmGenericFilter ();

    // Types
    typedef VectorArithmGenericFilter        Self;
    typedef ImageToImageGenericFilterBase   Superclass;
    typedef itk::SmartPointer<Self>         Pointer;
    typedef itk::SmartPointer<const Self>   ConstPointer;

    // New
    itkNewMacro(Self);
    
    //--------------------------------------------------------------------
    void SetArgsInfo(const args_info_type & a);

    // Set methods
    void SetDefaultPixelValue (double value) {  mDefaultPixelValue = value ;}
    void SetTypeOfOperation (int value) {  mTypeOfOperation = value ;}
    void SetScalar (double value) {  mScalar = value ;}
    void EnableOverwriteInputImage(bool b);

    // Get methods
    double GetDefaultPixelValue () { return  mDefaultPixelValue ;} 
    int GetTypeOfOperation () { return  mTypeOfOperation ;} 
    double GetScalar () { return  mScalar ;} 

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:  
    template<unsigned int Dim> void InitializeImageType();
    bool mIsOperationUseASecondImage;
    double mScalar;
    double mDefaultPixelValue;
    int mTypeOfOperation;  
    args_info_type mArgsInfo;
    bool mOverwriteInputImage;
    bool mOutputIsFloat;
    bool mIsOutputScalar;
    
    template<class Iter1, class Iter2>
      void ComputeImage(Iter1 it, Iter2 ito);

    template<class Iter1, class Iter2, class Iter3>
      void ComputeImage(Iter1 it1, Iter2 it2, Iter3 ito);
    
    template<class Iter1, class Iter2>
      void ComputeScalarImage(Iter1 it, Iter2 ito);

    template<class Iter1, class Iter2, class Iter3>
      void ComputeScalarImage(Iter1 it1, Iter2 it2, Iter3 ito);

    //--------------------------------------------------------------------

  }; // end class VectorArithmGenericFilter

  // specializations for itk::Vector<float, 3u>, 3u
} // end namespace
//--------------------------------------------------------------------

  
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkVectorArithmGenericFilter.txx"
#endif

#endif //#define CLITKVectorArithmGENERICFILTER_H

