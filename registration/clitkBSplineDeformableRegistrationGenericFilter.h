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
#ifndef _clitkBSplineDeformableRegistrationGenericFilter_h
#define _clitkBSplineDeformableRegistrationGenericFilter_h
/**
   =================================================
   * @file   clitkBSplineDeformableRegistrationGenericFilter.h
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   14 March 2009
   * 
   * @brief 
   * 
   =================================================*/

// clitk
#include "clitkBSplineDeformableRegistration_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkDifferenceImageFilter.h"
#include "clitkTransformUtilities.h"
#include "clitkLBFGSBOptimizer.h"
#include "clitkBSplineDeformableTransform.h"
#include "clitkGenericOptimizer.h"
#include "clitkGenericInterpolator.h"
#include "clitkGenericMetric.h"

// itk include
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkImage.h"
#include "itkBSplineDeformableTransform.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCommand.h"
#include "itkImageMaskSpatialObject.h"
#include "itkEuler3DTransform.h"
#include "itkWarpImageFilter.h"
#include "itkLightObject.h"
#include "itkImageToImageMetric.h"
#include "itkInterpolateImageFunction.h"
#include "itkLabelStatisticsImageFilter.h"


namespace clitk
{

  class ITK_EXPORT BSplineDeformableRegistrationGenericFilter : public itk::LightObject
  
  {
  public:
    typedef BSplineDeformableRegistrationGenericFilter  Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
    
    /** Run-time type information (and related methods) */
    itkTypeMacro( BSplineDeformableRegistrationGenericFilter, LightObject );
    
    
    //====================================================================
    // Set methods
    void SetArgsInfo(const args_info_clitkBSplineDeformableRegistration& a)
    {
      m_ArgsInfo=a;
      m_ReferenceFileName=m_ArgsInfo.reference_arg;
      m_Verbose=m_ArgsInfo.verbose_flag;
    }
    
    //====================================================================
    // Update
    virtual void Update();
    
  protected:
    //const char * GetNameOfClass() const { return "BSplineDeformableRegistrationGenericFilter"; }
    
    //====================================================================
    // Constructor & Destructor
    BSplineDeformableRegistrationGenericFilter();
    ~BSplineDeformableRegistrationGenericFilter(){;}
    
    //====================================================================
    //Protected member functions
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();
    
    args_info_clitkBSplineDeformableRegistration m_ArgsInfo;
    bool m_Verbose;
    std::string m_ReferenceFileName;
    
  };
  
} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkBSplineDeformableRegistrationGenericFilter.txx"
#endif
  
#endif //#define _clitkBSplineDeformableRegistrationGenericFilter_h




