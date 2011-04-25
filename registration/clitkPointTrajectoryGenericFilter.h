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
#ifndef clitkPointTrajectoryGenericFilter_h
#define clitkPointTrajectoryGenericFilter_h

/* =================================================
 * @file   clitkPointTrajectoryGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkPointTrajectory_ggo.h"
#include "clitkPointListTransform.h"
#include "clitkDeformationFieldTransform.h"
#include "clitkShapedBLUTSpatioTemporalDeformableTransform.h"
#include "clitkLists.h"
#include "clitkGenericVectorInterpolator.h"


//itk include
#include "itkLightObject.h"
#include "itkTransform.h"
#include "itkImageMaskSpatialObject.h"

namespace clitk 
{


  class ITK_EXPORT PointTrajectoryGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef PointTrajectoryGenericFilter       Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( PointTrajectoryGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_clitkPointTrajectory & a)
    {
      m_ArgsInfo=a;
      m_Verbose=m_ArgsInfo.verbose_flag;
    }
    
    
    //----------------------------------------  
    // Update
    //----------------------------------------  
    void Update();

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    PointTrajectoryGenericFilter();
    ~PointTrajectoryGenericFilter() {};

    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_clitkPointTrajectory m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk


#endif // #define clitkPointTrajectoryGenericFilter_h
