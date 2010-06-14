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
#include "clitkCorrelationRatioImageToImageMetric.h"
#include "clitkTransformUtilities.h"
#include "clitkGenericMetric.h"
#include "clitkGenericOptimizer.h"
#include "clitkGenericInterpolator.h"
#include "clitkGenericAffineTransform.h"

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
class AffineRegistrationGenericFilter: public itk::LightObject
{
public:

  //================================================
  typedef AffineRegistrationGenericFilter        Self;
  typedef itk::LightObject  Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  //================================================
  itkNewMacro(Self);

  //====================================================================
  // Set methods
  void SetArgsInfo(const args_info_clitkAffineRegistration args_info) {
    m_ArgsInfo=args_info;
    m_Verbose=m_ArgsInfo.verbose_flag;
  }

  //====================================================================
  // Update
  void Update();

protected:
  const char * GetNameOfClass() const {
    return "AffineRegistrationGenericFilter";
  }

  //====================================================================
  // Constructor & Destructor
  AffineRegistrationGenericFilter();
  ~AffineRegistrationGenericFilter() {
    ;
  }

  //====================================================================
  //Templated member functions
  template <unsigned int Dimension> void UpdateWithDim(std::string PixelType);
  template <unsigned int Dimension, class PixelType> void UpdateWithDimAndPixelType();

  //====================================================================
  //Member Data
public:
  bool m_Verbose;
  args_info_clitkAffineRegistration m_ArgsInfo;
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkAffineRegistrationGenericFilter.txx"
#endif


#endif //#define CLITKAFFINEREGISTRATIONGENERICFILTER__H
