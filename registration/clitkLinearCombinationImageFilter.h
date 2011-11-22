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
#ifndef __itkLinearCombinationImageFilter_h
#define __itkLinearCombinationImageFilter_h
#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"


namespace clitk
{
  

  template <class TInputImage, class TOutputImage>
  class ITK_EXPORT LinearCombinationImageFilter :
    public    itk::ImageToImageFilter<TInputImage, TOutputImage >
  {
  public:
    //-----------------------------------
    /** Standard class typedefs. */
    //-----------------------------------
    typedef LinearCombinationImageFilter                      Self;
    typedef itk::ImageToImageFilter<TInputImage,TOutputImage> Superclass;
    typedef itk::SmartPointer<Self>                           Pointer;
    typedef itk::SmartPointer<const Self>                     ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Runtime information support. */
    itkTypeMacro(LinearCombinationImageFilter, ImageToImageFilter);
    
    //-----------------------------------
    //Typedefs
    //-----------------------------------
    typedef typename TInputImage::Pointer InputPointer;
    typedef typename TInputImage::ConstPointer InputConstPointer;
    typedef typename TInputImage::PixelType InputPixelType;
    
    typedef typename TOutputImage::Pointer OutputPointer;
    typedef typename TOutputImage::PixelType OutputPixelType;
    typedef typename TOutputImage::RegionType OutputImageRegionType;
    //-----------------------------------
    //Set/Get
    //----------------------------------
    void SetFirstInput (const InputPointer i);
    void SetFirstInput ( const InputConstPointer i);
    void SetSecondInput (const InputPointer i);
    void SetSecondInput (const InputConstPointer i);

    itkSetMacro(A, double);
    itkGetMacro(A, double);
    itkSetMacro(B, double);
    itkGetMacro(B, double);
    itkSetMacro(C, double);
    itkGetMacro(C, double);

  protected:
    LinearCombinationImageFilter();
    virtual ~LinearCombinationImageFilter(){};

    virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, itk::ThreadIdType threadId);
    
  private:
    LinearCombinationImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
    double m_A;
    double m_B;
    double m_C;
    
  };
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkLinearCombinationImageFilter.txx"
#endif

#endif
