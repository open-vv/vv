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
===========================================================================*/

#ifndef clitkReconstructWithConditionalGrayscaleDilateImageFilter_h
#define clitkReconstructWithConditionalGrayscaleDilateImageFilter_h

// clitk include
#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkConditionalGrayscaleDilateImageFilter.h"

namespace clitk 
{
  /*
    This filter takes as input a multilabel image (and a bg value).
    It performs several greyscale dilatation of radius 1, but only in
    the bg pixels. It means that when two objects (with different
    labels) will dilate inside each other (collistion), the dilatation
    is stopped.
   */

  template <class ImageType>
  class ITK_EXPORT ReconstructWithConditionalGrayscaleDilateImageFilter :
    public itk::ImageToImageFilter<ImageType, ImageType>
  {
  public:
    //----------------------------------------
    // ITK
    //----------------------------------------
    typedef ReconstructWithConditionalGrayscaleDilateImageFilter Self;
    typedef itk::ImageToImageFilter<ImageType, ImageType>        Superclass;
    typedef itk::SmartPointer<Self>                              Pointer;
    typedef itk::SmartPointer<const Self>                        ConstPointer;
   
    // Method for creation through the object factory
    itkNewMacro(Self);  

    // Run-time type information (and related methods)
    itkTypeMacro( ReconstructWithConditionalGrayscaleDilateImageFilter, ImageToImageFilter);

    /** Dimension of the domain space. */
    itkStaticConstMacro(ImageDimension, unsigned int, Superclass::InputImageDimension);

    //----------------------------------------
    // Typedefs
    //----------------------------------------
    typedef typename ImageType::PixelType PixelType;
    typedef typename ImageType::SizeType SizeType;

    //----------------------------------------
    // Set & Get
    //----------------------------------------    
    itkBooleanMacro(Verbose);
    itkSetMacro( Verbose, bool);
    itkGetConstReferenceMacro( Verbose, bool);

    itkSetMacro(IterationNumber, int);
    itkGetConstMacro(IterationNumber, int);

    itkSetMacro(BackgroundValue, PixelType);
    itkGetConstMacro(BackgroundValue, PixelType);

  protected:

    //----------------------------------------  
    // Constructor & Destructor
    //----------------------------------------  
    ReconstructWithConditionalGrayscaleDilateImageFilter();
    ~ReconstructWithConditionalGrayscaleDilateImageFilter() {};

    //----------------------------------------  
    // Update
    //----------------------------------------  
    // Generate Data
    void GenerateData(void);

    //----------------------------------------  
    // Data members
    //----------------------------------------
    bool m_Verbose;
    PixelType m_BackgroundValue;
    int m_IterationNumber;
  };

} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkReconstructWithConditionalGrayscaleDilateImageFilter.txx"
#endif

#endif // #define clitkReconstructWithConditionalGrayscaleDilateImageFilter_h


