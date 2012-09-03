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
#ifndef clitkVectorImageToImageFilter_h
#define clitkVectorImageToImageFilter_h

/* =================================================
 * @file   clitkVectorImageToImageFilter.h
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

  template <class InputImageType, class OutputImageType>
  class ITK_EXPORT VectorImageToImageFilter :
    public itk::ImageToImageFilter<InputImageType, OutputImageType>
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef VectorImageToImageFilter                                                 Self;
    typedef itk::ImageToImageFilter<InputImageType, OutputImageType>  Superclass;
    typedef itk::SmartPointer<Self>                                   Pointer;
    typedef itk::SmartPointer<const Self>                             ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( VectorImageToImageFilter, ImageToImageFilter );

    /** Dimension of the domain space. */
    itkStaticConstMacro(InputImageDimension, unsigned int, Superclass::InputImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int, Superclass::OutputImageDimension);

    //----------------------------------------
    // Typedefs
    //----------------------------------------
    typedef typename OutputImageType::RegionType OutputImageRegionType;

    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    itkBooleanMacro(Verbose);
    itkSetMacro( Verbose, bool);
    itkGetConstReferenceMacro( Verbose, bool);
    itkSetMacro(ComponentIndex, unsigned int);
    itkGetConstMacro(ComponentIndex, unsigned int);   

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    VectorImageToImageFilter();
    ~VectorImageToImageFilter() {};

    //----------------------------------------  
    // Update
    //----------------------------------------  
#if ITK_VERSION_MAJOR >= 4
    void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread, itk::ThreadIdType threadId );
#else
    void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread, int threadId );
#endif
   
    //----------------------------------------  
    // Data members
    //----------------------------------------
    bool m_Verbose;
    unsigned int m_ComponentIndex;

  };


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkVectorImageToImageFilter.txx"
#endif

#endif // #define clitkVectorImageToImageFilter_h


