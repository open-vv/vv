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
#ifndef CLITKCONEBEAMPROJECTIMAGEGENERICFILTER__H
#define CLITKCONEBEAMPROJECTIMAGEGENERICFILTER__H
/**
   =================================================
   * @file   clitkConeBeamProjectImageGenericFilter.h
   * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
   * @date   30 April 2008
   * 
   * @brief Project a 3D image using a ConeBeam geometry
   * 
   =================================================*/

// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkImageCommon.h"
#include "clitkConeBeamProjectImageFilter.h"
#include "clitkConeBeamProjectImage_ggo.h"

// itk include
#include "itkLightObject.h"


namespace clitk {
  
  //====================================================================
  class ConeBeamProjectImageGenericFilter: public itk::LightObject
  {
  public: 

    //Typedefs ITK================================================
    typedef ConeBeamProjectImageGenericFilter        Self;
    typedef itk::LightObject  Superclass;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    //ITK Method for object creation================================================
    itkNewMacro(Self);  
    
    
    //====================================================================
    // Set methods
    void SetArgsInfo(const args_info_clitkConeBeamProjectImage a)
    {
      m_ArgsInfo=a;
      m_InputFileName=m_ArgsInfo.input_arg;
      m_Verbose=m_ArgsInfo.verbose_flag;
    }
    
    //  void SetVerbose(const bool v){mVerbose=v;}
    //     void SetInput(const std::string i)   { mInput = i; }
    //     void SetOutput(const std::string o)   { mOutput = o; }
    //     void SetMask(const std::string m) {mMask = m;}
    //     void SetIsoCenter(const std::vector<double> i) 
    //     {
    //       mIsoCenter.resize(3);
    //       for (unsigned int j=0; j < 3; j++) mIsoCenter[j]=i[j];      
    //     }
    //     void SetSourceToScreen(const double s) {mSourceToScreen = s;}
    //     void SetSourceToAxis(const double s) {mSourceToAxis = s;}
    //     void SetProjectionAngle(const double a) {mProjectionAngle = a;}
    //     void SetRigidTransformMatrix(const itk::Matrix<double,4,4> m) {mRigidTransformMatrix=m;}
    //     void SetEdgePaddingValue(const double m) {mEdgePaddingValue=m;}

    //====================================================================
    // Update
    virtual void Update();
    
  protected:
    const char * GetNameOfClass() const { return "ConeBeamProjectImageGenericFilter"; }
  
    //====================================================================
    // Constructor & Destructor
    ConeBeamProjectImageGenericFilter();
    ~ConeBeamProjectImageGenericFilter(){;}

    //====================================================================
    //Protected member functions
    template <class PixelType> void UpdateWithPixelType();
  
    bool m_Verbose;
    std::string m_InputFileName;
    args_info_clitkConeBeamProjectImage m_ArgsInfo;
    //     std::string mOutput;
    //     std::string mMask;
    //     std::vector<double> mIsoCenter;
    //     double mSourceToScreen;
    //     double mSourceToAxis;
    //     double mProjectionAngle;
    //     itk::Matrix<double,4,4> mRigidTransformMatrix;
    //     double mEdgePaddingValue;

  };
  
} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkConeBeamProjectImageGenericFilter.txx"
#endif
  
#endif //#define CLITKCONEBEAMPROJECTIMAGEGENERICFILTER__H
