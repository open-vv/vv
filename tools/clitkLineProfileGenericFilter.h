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
#ifndef clitkLineProfileGenericFilter_h
#define clitkLineProfileGenericFilter_h

/* =================================================
 * @file   clitkLineProfileGenericFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkImageToImageGenericFilter.h"
#include "clitkLineProfile_ggo.h"


namespace clitk
{

  class ITK_EXPORT LineProfileGenericFilter: 
    public ImageToImageGenericFilter<LineProfileGenericFilter>
  {
  public:
    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    LineProfileGenericFilter();
    ~LineProfileGenericFilter() {};

    
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef LineProfileGenericFilter           Self;
    typedef itk::LightObject                   Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( SetSpacingGenericFilter, LightObject );

    //----------------------------------------
    // Typedefs
    //----------------------------------------
    typedef args_info_clitkLineProfile  ArgsInfoType;

    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    void SetArgsInfo(const ArgsInfoType & a)
    {
      m_ArgsInfo=a;
      m_Verbose=m_ArgsInfo.verbose_flag;
      SetIOVerbose(m_Verbose);
      SetInputFilename(m_ArgsInfo.input_arg);
    }
    
    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    template<class InputImageType> void UpdateWithInputImageType();
    
  protected:

    //----------------------------------------  
    // Templated members
    //----------------------------------------  
    //template <unsigned int Dimension>  void UpdateWithDim(std::string PixelType, unsigned int Components);
    template<unsigned int Dim> void InitializeImageType();

    //----------------------------------------  
    // Data members
    //----------------------------------------
    args_info_clitkLineProfile m_ArgsInfo;
    bool m_Verbose;
    
  };
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkLineProfileGenericFilter.txx"
#endif

#endif // clitkLineProfileGenericFilter_h