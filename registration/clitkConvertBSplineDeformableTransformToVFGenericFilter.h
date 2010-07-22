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
#ifndef clitkConvertBSplineDeformableTransformToVFGenericFilter_h
#define clitkConvertBSplineDeformableTransformToVFGenericFilter_h

/* =================================================
 * @file   clitkConvertBSplineDeformableTransformToVFGenericFilter.h
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
#include "clitkConvertBSplineDeformableTransformToVF_ggo.h"
#include "clitkBSplineDeformableTransform.h"
#include "clitkShapedBLUTSpatioTemporalDeformableTransform.h"
#include "clitkTransformToDeformationFieldSource.h"

//itk include
#include "itkLightObject.h"
#include "itkImageMaskSpatialObject.h"
#include "itkTransformToDeformationFieldSource.h"


namespace clitk 
{

  class ITK_EXPORT ConvertBSplineDeformableTransformToVFGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef ConvertBSplineDeformableTransformToVFGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( ConvertBSplineDeformableTransformToVFGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_clitkConvertBSplineDeformableTransformToVF & a)
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
    ConvertBSplineDeformableTransformToVFGenericFilter();
    ~ConvertBSplineDeformableTransformToVFGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType, int Components);
    // template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();


    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_clitkConvertBSplineDeformableTransformToVF m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk


#endif // #define clitkConvertBSplineDeformableTransformToVFGenericFilter_h
