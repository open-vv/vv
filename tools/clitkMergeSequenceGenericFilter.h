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
#ifndef __clitkMergeSequenceGenericFilter_h
#define __clitkMergeSequenceGenericFilter_h
#include "clitkImageCommon.h"
#include "clitkImageCommon.h"

//itk include
#include "itkLightObject.h"
#include "itkImage.h"
#include "itkImageSeriesReader.h"


namespace clitk
{
  
  class ITK_EXPORT MergeSequenceGenericFilter : public itk::LightObject
  
  {
  public:
    typedef MergeSequenceGenericFilter     Self;
    typedef itk::LightObject     Superclass;
    typedef itk::SmartPointer<Self>            Pointer;
    typedef itk::SmartPointer<const Self>      ConstPointer;
   
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  

    //Set Methods (inline)
    void SetInputs(const std::vector<std::string> m){m_InputNames=m;}
    void SetSpacing(const double m){m_Spacing=m;}
    void SetOutput(const std::string m){m_OutputName=m;}
    void SetVerbose(const bool m){m_Verbose=m;}

    //Update
    void Update( );


  protected:

    MergeSequenceGenericFilter();
    ~MergeSequenceGenericFilter() {};

    //Templated members
    template<unsigned int Dimension> void UpdateWithDim(std::string PixelType, unsigned int Components);
    template<unsigned int Dimension, class PixelType> void UpdateWithDimAndPixelType(); 

    std::vector<std::string> m_InputNames;
    double  m_Spacing;
    std::string m_OutputName;
    bool m_Verbose;
  
};


} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkMergeSequenceGenericFilter.txx"
#endif

#endif // #define __clitkMergeSequenceGenericFilter_h
