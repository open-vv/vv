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

#ifndef __clitkComposeVFGenericFilter_h
#define __clitkComposeVFGenericFilter_h

// clitk
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkComposeVFFilter.h"

//itk 
#include "itkLightObject.h"
#include "itkImage.h"
#include "itkVector.h"

namespace clitk
{
  
  class ITK_EXPORT ComposeVFGenericFilter : public itk::LightObject
  
  {
  public:
    typedef ComposeVFGenericFilter     Self;
    typedef itk::LightObject     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  

    /** Run-time type information (and related methods) */
    itkTypeMacro( ComposeVFGenericFilter, ImageToImageFilter );
  
     //Set Methods (inline)
    void SetInput1(const std::string m){m_InputName1=m;}
    void SetInput2(const std::string m){m_InputName2=m;}
    void SetOutput(const std::string m){m_OutputName=m;}
    void SetLikeImage(const std::string like){m_LikeImage=like;}
    void SetInputType(int t){m_Type=t;}
    void SetVerbose(const bool m){m_Verbose=m;}
 

    //Update
    void Update( );

  protected:

    ComposeVFGenericFilter();
    ~ComposeVFGenericFilter() {};

    //Templated members
    template<unsigned int Dimension> void UpdateWithDim(std::string PixelType);
    template<unsigned int Dimension, class PixelType> void UpdateWithDimAndPixelType(); 
    
    std::string m_InputName1;
    std::string m_InputName2;
    std::string m_OutputName;
    std::string m_LikeImage;
    int m_Type;
    
    bool m_Verbose;
  
  };


} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkComposeVFGenericFilter.txx"
#endif

#endif // #define __clitkComposeVFGenericFilter_h
