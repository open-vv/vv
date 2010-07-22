/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
#ifndef clitkMorphoMathGenericFilter_h
#define clitkMorphoMathGenericFilter_h
/**
   =================================================
   * @file   clitkMorphoMathGenericFilter.h
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   5 May 2009
   * 
   * @brief 
   * 
   =================================================*/

// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkMorphoMath_ggo.h"
#include "clitkConditionalBinaryErodeImageFilter.h"
#include "clitkConditionalBinaryDilateImageFilter.h"

// itk include
#include "itkLightObject.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkCastImageFilter.h"

namespace clitk {
  
  //====================================================================
  class MorphoMathGenericFilter: public itk::LightObject
  {
  public: 

    //================================================
    typedef MorphoMathGenericFilter        Self;
    typedef itk::LightObject  Superclass;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    //================================================
    itkNewMacro(Self);  
    
    //====================================================================
    // Set methods
    void SetArgsInfo(const args_info_clitkMorphoMath a)
    {
      m_ArgsInfo=a;
      m_InputFileName=m_ArgsInfo.input_arg;
      m_Verbose=m_ArgsInfo.verbose_flag;
    }

    //====================================================================
    // Update
    virtual void Update();
    
  protected:
    const char * GetNameOfClass() const { return "MorphoMathGenericFilter"; }

    //====================================================================
    // Constructor & Destructor
    MorphoMathGenericFilter();
    ~MorphoMathGenericFilter(){;}
    
    //====================================================================
    //Protected member functions
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();

  
    args_info_clitkMorphoMath m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };
  
} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkMorphoMathGenericFilter.txx"
#endif
  
#endif //#define clitkMorphoMathGenericFilter_h
