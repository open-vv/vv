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
#ifndef clitkCalculateTREGenericFilter_h
#define clitkCalculateTREGenericFilter_h

/* =================================================
 * @file   clitkCalculateTREGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkCalculateTRE_ggo.h"
#include "clitkGenericVectorInterpolator.h"
#include "clitkPointListWriter.h"
#include "clitkPointListReader.h"
#include "clitkDeformationListStatisticsFilter.h"
#include "clitkList.h"

//itk include
#include "itkLightObject.h"
#include "itkExtractImageFilter.h"

//general
#include <iomanip>

namespace clitk 
{


  class ITK_EXPORT CalculateTREGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef CalculateTREGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( CalculateTREGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_clitkCalculateTRE& a)
    {
      m_ArgsInfo=a;
      m_Verbose=m_ArgsInfo.verbose_flag;
      m_InputFileName=m_ArgsInfo.vf_arg[0];
    }
    
    
    //----------------------------------------  
    // Update
    //----------------------------------------  
    void Update();

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    CalculateTREGenericFilter();
    ~CalculateTREGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension,unsigned int Components> void ReadVectorFields(void);
    template <unsigned int Dimension,unsigned int Components> void ProcessVectorFields(std::vector< typename itk::Image<itk::Vector<float, Components>, Dimension>::Pointer > dvfs, char** filenames);
    template <unsigned int Dimension> void UpdateWithDim( std::vector<typename itk::Image<itk::Vector<float, Dimension>, Dimension>::Pointer > dvfs,  std::vector<std::string> filenames);
    

    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_clitkCalculateTRE m_ArgsInfo;
    bool m_Verbose;
    std::string m_InputFileName;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkCalculateTREGenericFilter.txx"
#endif

#endif // #define clitkCalculateTREGenericFilter_h
