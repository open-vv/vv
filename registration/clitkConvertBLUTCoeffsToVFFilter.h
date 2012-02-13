
#ifndef clitkConvertBLUTCoeffsToVFFilter_h
#define clitkConvertBLUTCoeffsToVFFilter_h

#include "clitkIO.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkBSplineDeformableTransform.h"
#include "itkBSplineDeformableTransform.h"
#if ITK_VERSION_MAJOR >= 4
#include "itkTransformToDisplacementFieldSource.h"
#else
#include "itkTransformToDeformationFieldSource.h"
#endif

namespace clitk 
{
  template <class TDVFType>
  class ConvertBLUTCoeffsToVFFilter : 
    public itk::ImageSource<TDVFType>
  {
  public:

    /** Standard class typedefs. */
    typedef ConvertBLUTCoeffsToVFFilter       Self;
    typedef itk::ImageSource<TDVFType>         Superclass;
    typedef itk::SmartPointer<Self>                      Pointer;
    typedef itk::SmartPointer<const Self>                ConstPointer;
    
     // ImageTypes
    typedef itk::ImageIOBase LikeImageType;
    typedef typename LikeImageType::Pointer LikeImagePointer;

    typedef TDVFType  OutputImageType;
    typedef typename OutputImageType::Pointer       OutputImagePointer;
    typedef typename OutputImageType::ConstPointer  OutputImageConstPointer;
    typedef typename OutputImageType::RegionType    OutputImageRegionType;
    typedef typename OutputImageType::SizeType    OutputImageSizeType;
    typedef typename OutputImageType::PointType    OutputImagePointType;
    typedef typename OutputImageType::SpacingType    OutputImageSpacingType;

    typedef clitk::BSplineDeformableTransform<double, TDVFType::ImageDimension, TDVFType::ImageDimension> BLUTTransformType;
    typedef typename BLUTTransformType::CoefficientImageType BLUTCoefficientImageType;
    typedef itk::BSplineDeformableTransform<double, TDVFType::ImageDimension, TDVFType::ImageDimension> ITKTransformType;

    typedef itk::Transform< double, TDVFType::ImageDimension, TDVFType::ImageDimension> GenericTransformType;
    
#if ITK_VERSION_MAJOR >= 4
    typedef itk::TransformToDisplacementFieldSource<OutputImageType, double> ConvertorType;
#else
    typedef itk::TransformToDeformationFieldSource<OutputImageType, double> ConvertorType;
#endif

    itkNewMacro(Self);
    
    itkSetMacro(Verbose, bool);
    itkSetMacro(InputFileName, std::string);
    itkSetMacro(LikeFileName, std::string);
    itkSetMacro(OutputOrigin, OutputImagePointType);
    itkSetMacro(OutputSpacing, OutputImageSpacingType);
    itkSetMacro(OutputSize, OutputImageSizeType);
    itkSetMacro(TransformType, unsigned int);
    itkSetMacro(MaskFileName, std::string);
    itkSetMacro(BLUTSplineOrders, typename BLUTCoefficientImageType::SizeType);
    
    /** DeformationFieldImageFilter produces a vector image. */
    virtual void GenerateOutputInformation( void );

  private:
    
    ConvertBLUTCoeffsToVFFilter();
    ~ConvertBLUTCoeffsToVFFilter() {};
    
    virtual void GenerateData();

    bool m_Verbose;
    unsigned int m_TransformType;
    typename BLUTTransformType::Pointer m_BLUTTransform;
    typename ITKTransformType::Pointer m_ITKTransform;
    typename GenericTransformType::Pointer m_GenericTransform;
    
    std::string m_InputFileName, m_LikeFileName, m_MaskFileName;
    OutputImageSizeType m_OutputSize;
    OutputImageSpacingType m_OutputSpacing;
    OutputImagePointType m_OutputOrigin;
    typename BLUTCoefficientImageType::SizeType m_BLUTSplineOrders;
    typename ConvertorType::Pointer m_Filter;
  };
}

#include "clitkConvertBLUTCoeffsToVFFilter.txx"

#endif // clitkConvertBLUTCoeffsToVFFilter_h