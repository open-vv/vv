#ifndef CLITKIMAGERESAMPLEGENERICFILTER_H
#define CLITKIMAGERESAMPLEGENERICFILTER_H

/**
 -------------------------------------------------------------------
 * @file   clitkVFResampleGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008 08:37:53

 * @brief  
 -------------------------------------------------------------------*/

// clitk include
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkImageToImageGenericFilter.h"

// itk include
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkFixedArray.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkVectorResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkVectorNearestNeighborInterpolateImageFunction.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunctionWithLUT.h"
#include "itkCommand.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class VFResampleGenericFilter: 
    public clitk::ImageToImageGenericFilter<VFResampleGenericFilter> {
    
  public: 
    // constructor
    VFResampleGenericFilter();

    // Types
    typedef VFResampleGenericFilter       Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    void SetOutputSize(const std::vector<int> & size);
    void SetOutputSpacing(const std::vector<double> & spacing);
    void SetGaussianSigma(const std::vector<double> & sigma);
    void SetInterpolationName(const std::string & inter);
    void SetDefaultPixelValue(double dpv) { mDefaultPixelValue = dpv;}
    void SetBSplineOrder(int o) { mBSplineOrder = o; }
    void SetBLUTSampling(int b) { mSamplingFactors.resize(1); mSamplingFactors[0] = b; }

   //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    //--------------------------------------------------------------------
    std::string mInterpolatorName;
    std::vector<int> mOutputSize;
    std::vector<double> mOutputSpacing;
    std::vector<double> mOutputOrigin;
    double mDefaultPixelValue;
    bool mApplyGaussianFilterBefore;
    std::vector<double> mSigma;
    int mBSplineOrder;
    std::vector<int> mSamplingFactors;

    //--------------------------------------------------------------------
    template<unsigned int Dim, class PixelType, unsigned int DimCompo> 
    void Update_WithDimAndPixelTypeAndComponent();
    template<class ImageType>
    typename ImageType::Pointer ComputeImage(typename ImageType::Pointer inputImage);
    
  }; // end class VFResampleGenericFilter
  //--------------------------------------------------------------------
    
#include "clitkVFResampleGenericFilter.txx"

} // end namespace
//--------------------------------------------------------------------
    
#endif /* end #define CLITKIMAGERESAMPLEGENERICFILTER_H */

