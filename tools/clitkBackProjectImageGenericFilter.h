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
#ifndef CLITKBACKPROJECTIMAGEGENERICFILTER__H
#define CLITKBACKPROJECTIMAGEGENERICFILTER__H
/**
   =================================================
   * @file   clitkBackProjectImageGenericFilter.h
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   30 April 2008
   * 
   * @brief Project a 3D image using a ConeBeam geometry
   * 
   =================================================*/

// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkBackProjectImageFilter.h"
#include "clitkBackProjectImage_ggo.h"

// itk include
#include "itkLightObject.h"


namespace clitk {
  
  //====================================================================
  class BackProjectImageGenericFilter: public itk::LightObject
  {
  public: 

    //================================================
    typedef BackProjectImageGenericFilter        Self;
    typedef itk::LightObject  Superclass;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    //================================================
    itkNewMacro(Self);  
    
    //====================================================================
    // Set methods
    void SetArgsInfo(const args_info_clitkBackProjectImage a)
    {
      m_ArgsInfo=a;
      m_InputFileName=m_ArgsInfo.input_arg;
      m_Verbose=m_ArgsInfo.verbose_flag;
    }

    //====================================================================
    // Update
    virtual void Update();
    
  protected:
    const char * GetNameOfClass() const { return "BackProjectImageGenericFilter"; }

    //====================================================================
    // Constructor & Destructor
    BackProjectImageGenericFilter();
    ~BackProjectImageGenericFilter(){;}
    
    //====================================================================
    //Protected member functions
    template <class PixelType>  void UpdateWithPixelType();
  
    args_info_clitkBackProjectImage m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };
  
} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkBackProjectImageGenericFilter.txx"
#endif
  
#endif //#define CLITKBACKPROJECTIMAGEGENERICFILTER__H
