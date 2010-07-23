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
#ifndef clitkFilterGenericFilter_h
#define clitkFilterGenericFilter_h

/* =================================================
 * @file   clitkFilterGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkFilter_ggo.h"

//itk include
#include "itkLightObject.h"
#include "itkCastImageFilter.h"
#include "itkDerivativeImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkSumProjectionImageFilter.h"
#include "itkMaximumProjectionImageFilter.h"
#include "itkMinimumProjectionImageFilter.h"
#include "itkAccumulateImageFilter.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkBSplineDownsampleImageFilter.h"
#include "itkNormalizeImageFilter.h"
#include "itkAnisotropicDiffusionImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"

namespace clitk 
{


  class ITK_EXPORT FilterGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef FilterGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( FilterGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_clitkFilter & a)
    {
      m_ArgsInfo=a;
      m_Verbose=m_ArgsInfo.verbose_flag;
      m_InputFileName=m_ArgsInfo.input_arg;
    }
    
    
    //----------------------------------------  
    // Update
    //----------------------------------------  
    void Update();

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    FilterGenericFilter();
    ~FilterGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();


    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_clitkFilter m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkFilterGenericFilter.txx"
#endif

#endif // #define clitkFilterGenericFilter_h
