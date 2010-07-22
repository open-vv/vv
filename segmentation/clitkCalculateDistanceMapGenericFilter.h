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
#ifndef clitkCalculateDistanceMapGenericFilter_h
#define clitkCalculateDistanceMapGenericFilter_h

/* =================================================
 * @file   clitkCalculateDistanceMapGenericFilter.h
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkCalculateDistanceMap_ggo.h"

//itk include
#include "itkLightObject.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"

namespace clitk 
{


  class ITK_EXPORT CalculateDistanceMapGenericFilter : public itk::LightObject
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef CalculateDistanceMapGenericFilter                   Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( CalculateDistanceMapGenericFilter, LightObject );


    //----------------------------------------
    // Typedefs
    //----------------------------------------


    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const args_info_clitkCalculateDistanceMap & a)
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
    CalculateDistanceMapGenericFilter();
    ~CalculateDistanceMapGenericFilter() {};

    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType);
    template <unsigned int Dimension, class PixelType>  void UpdateWithDimAndPixelType();


    //----------------------------------------  
    // Data members
    //----------------------------------------
    bool m_Verbose;
    args_info_clitkCalculateDistanceMap m_ArgsInfo;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkCalculateDistanceMapGenericFilter.txx"
#endif

#endif // #define clitkCalculateDistanceMapGenericFilter_h
