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
#ifndef clitkCombineImageFilter_h
#define clitkCombineImageFilter_h

/* =================================================
 * @file   clitkCombineImageFilter.h
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"

//itk include
#include "itkImageToImageFilter.h"

namespace clitk 
{

  template <class InputImageType>
  class ITK_EXPORT CombineImageFilter :
    public itk::ImageToImageFilter<InputImageType, InputImageType>
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef CombineImageFilter                                                 Self;
    typedef itk::ImageToImageFilter<InputImageType, InputImageType>  Superclass;
    typedef itk::SmartPointer<Self>                                   Pointer;
    typedef itk::SmartPointer<const Self>                             ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( CombineImageFilter, ImageToImageFilter );

    /** Dimension of the domain space. */
    itkStaticConstMacro(InputImageDimension, unsigned int, Superclass::InputImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int, Superclass::OutputImageDimension);
    typedef itk::Image<int, InputImageDimension> MaskImageType;

    //----------------------------------------
    // Typedefs
    //----------------------------------------
    typedef typename InputImageType::RegionType OutputImageRegionType;

    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    itkBooleanMacro(Verbose);
    itkSetMacro( Verbose, bool);
    itkGetConstReferenceMacro( Verbose, bool);

    void SetMask(typename MaskImageType::Pointer m){m_Mask=m;}
    typename MaskImageType::Pointer GetMask(void){return m_Mask;}

 
  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    CombineImageFilter();
    ~CombineImageFilter() {};

    //----------------------------------------  
    // Update
    //----------------------------------------  
    // Generate Data
    void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread, int threadId);

    //----------------------------------------  
    // Data members
    //----------------------------------------
    bool m_Verbose;
    typename MaskImageType::Pointer m_Mask;


  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkCombineImageFilter.txx"
#endif

#endif // #define clitkCombineImageFilter_h


