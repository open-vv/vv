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

#ifndef CLITKAFFINEREGISTRATIONGENERICFILTER_H
#define CLITKAFFINEREGISTRATIONGENERICFILTER_H

// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkAffineRegistration_ggo.h"
#include "clitkImageArithm_ggo.h"
#include "clitkCorrelationRatioImageToImageMetric.h"
#include "clitkTransformUtilities.h"
#include "clitkGenericMetric.h"
#include "clitkGenericOptimizer.h"
#include "clitkGenericInterpolator.h"
#include "clitkGenericAffineTransform.h"
#include "clitkImageToImageGenericFilter.h"


//itk include
#include <itkMultiResolutionImageRegistrationMethod.h>
#include <itkMultiResolutionPyramidImageFilter.h>
#include <itkImageToImageMetric.h>
#include <itkEuler2DTransform.h>
#include <itkCenteredEuler3DTransform.h>
#include <itkImage.h>
#include <itkResampleImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkNormalizeImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkImageMaskSpatialObject.h>
#include <itkCommand.h>
#include <itkCheckerBoardImageFilter.h>
#include <itkSubtractImageFilter.h>
#include <itkLightObject.h>
#include <itkImageMomentsCalculator.h>
#include <itkThresholdImageFilter.h>

// other includes
#include <time.h>
#include <iostream>
#include <iomanip>

namespace clitk
{

//====================================================================
template<class args_info_clitkAffineRegistration>
class ITK_EXPORT AffineRegistrationGenericFilter:
    public ImageToImageGenericFilter<AffineRegistrationGenericFilter<args_info_clitkAffineRegistration> >
{
public:

  AffineRegistrationGenericFilter();

  //================================================
  typedef AffineRegistrationGenericFilter        Self;
  
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef ImageToImageGenericFilterBase   Superclass;
  //================================================
  itkNewMacro(Self);
  //itkTypeMacro(AffineRegistrationGenericFilter, LightObject);


  //====================================================================
  // Set methods
       void SetArgsInfo(const args_info_clitkAffineRegistration & a);

  //====================================================================
  template<unsigned int Dim>
  void InitializeImageType();

  bool m_Verbose;
  args_info_clitkAffineRegistration m_ArgsInfo;

  template<class InputImageType>
  void UpdateWithInputImageType();
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkAffineRegistrationGenericFilter.txx"
#endif

#endif //#define CLITKAFFINEREGISTRATIONGENERICFILTER__H
