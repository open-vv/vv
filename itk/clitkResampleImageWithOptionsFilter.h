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

#ifndef CLITKRESAMPLEIMAGEWITHOPTIONSFILTER_H
#define CLITKRESAMPLEIMAGEWITHOPTIONSFILTER_H

#include "itkImageToImageFilter.h"
#include "itkAffineTransform.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Image resampling with several interpolations and Gaussian filtering included.
  */
  //--------------------------------------------------------------------
  
  template <class TInputImage, class TOutputImage=TInputImage>
  class ITK_EXPORT ResampleImageWithOptionsFilter: 
    public itk::ImageToImageFilter<TInputImage, TOutputImage> {

  public:
    /** Standard class typedefs. */
    typedef ResampleImageWithOptionsFilter                     Self;
    typedef itk::ImageToImageFilter<TInputImage, TOutputImage> Superclass;
    typedef itk::SmartPointer<Self>                            Pointer;
    typedef itk::SmartPointer<const Self>                      ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ResampleImageWithOptionsFilter, ImageToImageFilter);

    /** Some convenient typedefs. */
    typedef TInputImage                           InputImageType;
    typedef typename InputImageType::ConstPointer InputImageConstPointer;
    typedef typename InputImageType::Pointer      InputImagePointer;
    typedef typename InputImageType::RegionType   InputImageRegionType; 
    typedef typename InputImageType::PixelType    InputImagePixelType;
    typedef typename InputImageType::SpacingType  InputImageSpacingType;
    typedef typename InputImageType::SizeType     InputImageSizeType;
    
    typedef TOutputImage                           OutputImageType;
    typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
    typedef typename OutputImageType::Pointer      OutputImagePointer;
    typedef typename OutputImageType::RegionType   OutputImageRegionType; 
    typedef typename OutputImageType::PixelType    OutputImagePixelType;
    typedef typename OutputImageType::SpacingType  OutputImageSpacingType;
    typedef typename OutputImageType::SizeType     OutputImageSizeType;
    
    typedef itk::AffineTransform<double, InputImageType::ImageDimension> TransformType;
    typedef typename InputImageType::SpacingType                         GaussianSigmaType;

    /** ImageDimension constants */
    itkStaticConstMacro(InputImageDimension, unsigned int,
                        TInputImage::ImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int,
                        TOutputImage::ImageDimension);
    itkConceptMacro(SameDimensionCheck,
                    (itk::Concept::SameDimension<InputImageDimension, OutputImageDimension>));

    /** Interpolation types */
    typedef enum {
      NearestNeighbor = 0,
      Linear = 1, 
      BSpline = 2, 
      B_LUT = 3
    } InterpolationTypeEnumeration;

    /** Input : image to resample */
    void SetInput(const InputImageType * image);
    
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);

    // Options
    itkGetMacro(LastDimensionIsTime, bool);
    itkSetMacro(LastDimensionIsTime, bool);
    itkSetMacro(OutputIsoSpacing, double);
    itkGetMacro(OutputIsoSpacing, double);
    itkSetMacro(OutputSpacing, OutputImageSpacingType);
    itkGetMacro(OutputSpacing, OutputImageSpacingType);
    itkSetMacro(OutputSize, OutputImageSizeType);
    itkGetMacro(OutputSize, OutputImageSizeType);
    itkGetMacro(InterpolationType, InterpolationTypeEnumeration);
    itkSetMacro(InterpolationType, InterpolationTypeEnumeration);    
    itkGetMacro(GaussianFilteringEnabled, bool);
    itkSetMacro(GaussianFilteringEnabled, bool);
    itkGetMacro(BSplineOrder, int);
    itkSetMacro(BSplineOrder, int);
    itkGetMacro(BLUTSamplingFactor, int);
    itkSetMacro(BLUTSamplingFactor, int);
    itkGetMacro(Transform, typename TransformType::Pointer);
    itkSetMacro(Transform, typename TransformType::Pointer);
    itkGetMacro(GaussianSigma, GaussianSigmaType);
    itkSetMacro(GaussianSigma, GaussianSigmaType);
    itkGetMacro(DefaultPixelValue, OutputImagePixelType);
    itkSetMacro(DefaultPixelValue, OutputImagePixelType);
    itkGetMacro(VerboseOptions, bool);
    itkSetMacro(VerboseOptions, bool);
    
  protected:
    ResampleImageWithOptionsFilter();
    virtual ~ResampleImageWithOptionsFilter() {}
    
    bool m_LastDimensionIsTime;
    double m_OutputIsoSpacing;
    InterpolationTypeEnumeration m_InterpolationType;
    bool m_GaussianFilteringEnabled;
    int m_BSplineOrder;
    int m_BLUTSamplingFactor;    
    OutputImageSizeType m_OutputSize;
    OutputImageSpacingType m_OutputSpacing;  
    typename TransformType::Pointer m_Transform;
    GaussianSigmaType m_GaussianSigma;
    OutputImagePixelType m_DefaultPixelValue;
    bool m_VerboseOptions;
    OutputImageRegionType m_OutputRegion;

    virtual void GenerateInputRequestedRegion();
    virtual void GenerateOutputInformation();
    virtual void GenerateData();
    
  private:
    ResampleImageWithOptionsFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkResampleImageWithOptionsFilter.txx"
#endif

#endif
