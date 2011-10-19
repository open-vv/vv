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

#ifndef CLITKRELATIVEPOSITIONANALYZERFILTER_H
#define CLITKRELATIVEPOSITIONANALYZERFILTER_H

// clitk
#include "clitkCommon.h"

// itk
#include <itkImageToImageFilter.h>

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    TODO
  */
  //--------------------------------------------------------------------
  
  template <class ImageType>
  class ITK_EXPORT RelativePositionAnalyzerFilter:
    public itk::ImageToImageFilter<ImageType, ImageType> 
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<ImageType, ImageType>      Superclass;
    typedef RelativePositionAnalyzerFilter                     Self;
    typedef itk::SmartPointer<Self>                            Pointer;
    typedef itk::SmartPointer<const Self>                      ConstPointer;
       
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(RelativePositionAnalyzerFilter, ImageToImageFilter);

    /** Some convenient typedefs. */
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   RegionType; 
    typedef typename ImageType::PixelType    PixelType;
    typedef typename ImageType::SpacingType  SpacingType;
    typedef typename ImageType::SizeType     SizeType;
    typedef typename ImageType::IndexType    IndexType;
    typedef typename ImageType::PointType    PointType;
    
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    typedef itk::Image<float, ImageDimension> FloatImageType;

     /** Input : initial image and object */
    void SetInputSupport(const ImageType * image);
    void SetInputObject(const ImageType * image);
    void SetInputTarget(const ImageType * image);
    
    // Options
    itkSetMacro(VerboseFlag, bool);
    itkGetConstMacro(VerboseFlag, bool);
    itkBooleanMacro(VerboseFlag);

    itkGetConstMacro(BackgroundValue, PixelType);
    itkSetMacro(BackgroundValue, PixelType);

    itkGetConstMacro(ForegroundValue, PixelType);
    itkSetMacro(ForegroundValue, PixelType);

    // For debug
    void PrintOptions();

  protected:
    RelativePositionAnalyzerFilter();
    virtual ~RelativePositionAnalyzerFilter() {}
    
    bool m_VerboseFlag;
    PixelType m_BackgroundValue;
    PixelType m_ForegroundValue;
    ImagePointer m_Support;
    ImagePointer m_Object;
    ImagePointer m_Target;

    virtual void GenerateData();

  private:
    RelativePositionAnalyzerFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkRelativePositionAnalyzerFilter.txx"
#endif

#endif
