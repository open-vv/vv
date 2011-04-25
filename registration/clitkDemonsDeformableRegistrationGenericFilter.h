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
#ifndef _clitkDemonsDeformableRegistrationGenericFilter_h
#define _clitkDemonsDeformableRegistrationGenericFilter_h
/**
   =================================================
   * @file   clitkDemonsDeformableRegistrationGenericFilter.h
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   14 March 2009
   * 
   * @brief 
   * 
   =================================================*/

// clitk
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkDemonsDeformableRegistration_ggo.h"
#include "clitkDifferenceImageFilter.h"
#include "clitkMultiResolutionPDEDeformableRegistration.h"

// itk include
#include "itkMultiResolutionPDEDeformableRegistration.h"
#include "itkPDEDeformableRegistrationFilter.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkSymmetricForcesDemonsRegistrationFilter.h"
#include "itkFastSymmetricForcesDemonsRegistrationFilter.h"
#include "itkDiffeomorphicDemonsRegistrationFilter.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkWarpImageFilter.h"
#include "itkLightObject.h"


namespace clitk
{

  class ITK_EXPORT DemonsDeformableRegistrationGenericFilter : public itk::LightObject
  
  {
  public:
    typedef DemonsDeformableRegistrationGenericFilter  Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
    
    /** Run-time type information (and related methods) */
    itkTypeMacro( DemonsDeformableRegistrationGenericFilter, LightObject );
    
    
    //====================================================================
    // Set methods
    void SetArgsInfo(const args_info_clitkDemonsDeformableRegistration a)
    {
      m_ArgsInfo=a;
      m_ReferenceFileName=m_ArgsInfo.reference_arg;
      m_Verbose=m_ArgsInfo.verbose_flag;
    }
    
    //====================================================================
    // Update
    virtual void Update();
    
  protected:
    //const char * GetNameOfClass() const { return "DemonsDeformableRegistrationGenericFilter"; }
    
    //====================================================================
    // Constructor & Destructor
    DemonsDeformableRegistrationGenericFilter();
    ~DemonsDeformableRegistrationGenericFilter(){;}
    
    //====================================================================
    //Protected member functions
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();
    
    args_info_clitkDemonsDeformableRegistration m_ArgsInfo;
    bool m_Verbose;
    std::string m_ReferenceFileName;
    
  };
  
} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkDemonsDeformableRegistrationGenericFilter.txx"
#endif
  
#endif //#define _clitkDemonsDeformableRegistrationGenericFilter_h




