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
#ifndef clitkBLUTDIRGenericFilter_h
#define clitkBLUTDIRGenericFilter_h

/* =================================================
 * @file   clitkBLUTDIRGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk
#include "clitkBLUTDIR_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkDifferenceImageFilter.h"
#include "clitkTransformUtilities.h"
#include "clitkLBFGSBOptimizer.h"
#include "clitkBSplineDeformableTransform.h"
#include "clitkGenericOptimizer.h"
#include "clitkGenericInterpolator.h"
#include "clitkGenericMetric.h"
#include "clitkBSplineDeformableTransformInitializer.h"
#include "clitkMultiResolutionPyramidRegionFilter.h"
#include "clitkImageToImageGenericFilter.h"

// itk include
#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkImage.h"
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
#include "itkImageDuplicator.h"
#include "itkExtractImageFilter.h"
#include "itkTransformToDeformationFieldSource.h"

namespace clitk 
{


  class ITK_EXPORT BLUTDIRGenericFilter :public ImageToImageGenericFilter<BLUTDIRGenericFilter>
  {
  public:
    //----------------------------------------
    BLUTDIRGenericFilter();
    //----------------------------------------
    typedef BLUTDIRGenericFilter                   Self;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
    typedef ImageToImageGenericFilterBase   Superclass;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  
    
    void SetArgsInfo(const args_info_clitkBLUTDIR & a);

    // Run-time type information (and related methods)
    
    template<unsigned int Dim>
    void InitializeImageType();


    template<class InputImageType>
    void UpdateWithInputImageType();

    args_info_clitkBLUTDIR m_ArgsInfo;
    bool m_Verbose;
    std::string m_ReferenceFileName;

  };


} // end namespace clitk


#endif // #define clitkBLUTDIRGenericFilter_h
