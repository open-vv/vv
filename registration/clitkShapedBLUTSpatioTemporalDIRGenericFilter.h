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
#ifndef clitkShapedBLUTSpatioTemporalDIRGenericFilter_h
#define clitkShapedBLUTSpatioTemporalDIRGenericFilter_h

/* =================================================
 * @file   clitkShapedBLUTSpatioTemporalDIRGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkShapedBLUTSpatioTemporalDIR_ggo.h"
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkGenericOptimizer.h"
#include "clitkGenericMetric.h"
#include "clitkGenericInterpolator.h"
#include "clitkShapedBLUTSpatioTemporalDeformableTransform.h"
#include "clitkShapedBLUTSpatioTemporalDeformableTransformInitializer.h"
#include "clitkSpatioTemporalMultiResolutionImageRegistrationMethod.h"
#include "clitkSpatioTemporalMultiResolutionPyramidImageFilter.h"
#include "clitkDifferenceImageFilter.h"
#include "clitkMultiResolutionPyramidRegionFilter.h"

//itk include
#include "itkLightObject.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImageToImageMetric.h"
#include "itkInterpolateImageFunction.h"
#include "itkWarpImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkExtractImageFilter.h"
#include "itkCropImageFilter.h"

namespace clitk 
{


  class ITK_EXPORT ShapedBLUTSpatioTemporalDIRGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef ShapedBLUTSpatioTemporalDIRGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( ShapedBLUTSpatioTemporalDIRGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_clitkShapedBLUTSpatioTemporalDIR & a)
    {
      m_ArgsInfo=a;
      m_Verbose=m_ArgsInfo.verbose_flag;
      m_ReferenceFileName=m_ArgsInfo.reference_arg;
    }
    
    
    //----------------------------------------  
    // Update
    //----------------------------------------  
    void Update();

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    ShapedBLUTSpatioTemporalDIRGenericFilter();
    ~ShapedBLUTSpatioTemporalDIRGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();


    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_clitkShapedBLUTSpatioTemporalDIR m_ArgsInfo;
    bool m_Verbose;
    std::string m_ReferenceFileName;
  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkShapedBLUTSpatioTemporalDIRGenericFilter.txx"
#endif

#endif // #define clitkShapedBLUTSpatioTemporalDIRGenericFilter_h
