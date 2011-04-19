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
#ifndef clitkBSplinePyramidGenericFilter_h
#define clitkBSplinePyramidGenericFilter_h

/* =================================================
 * @file   clitkBSplinePyramidGenericFilter.h
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
#include "clitkBSplinePyramid_ggo.h"
#include "clitkVectorImageToImageFilter.h"

//itk include
#include "itkLightObject.h"
#include "itkBSplineDownsampleImageFilter.h"
#include "itkBSplineUpsampleImageFilter.h"
#include "itkCompose3DVectorImageFilter.h"

namespace clitk 
{


  class ITK_EXPORT BSplinePyramidGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef BSplinePyramidGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( BSplinePyramidGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_clitkBSplinePyramid & a)
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
    BSplinePyramidGenericFilter();
    ~BSplinePyramidGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType, unsigned int Components);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndVectorType();


    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_clitkBSplinePyramid m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkBSplinePyramidGenericFilter.txx"
#endif

#endif // #define clitkBSplinePyramidGenericFilter_h
