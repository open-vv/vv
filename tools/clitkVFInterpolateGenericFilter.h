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
#ifndef CLITKIMAGERESAMPLEGENERICFILTER_H
#define CLITKIMAGERESAMPLEGENERICFILTER_H
/**
 -------------------------------------------------------------------
 * @file   clitkVFInterpolateGenericFilter.h
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
#include "itkInterpolateImageFilter.h"
#include "itkAffineTransform.h"
#include "itkVectorNearestNeighborInterpolateImageFunction.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunctionWithLUT.h"
#include "itkCommand.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class VFInterpolateGenericFilter: 
    public clitk::ImageToImageGenericFilter<VFInterpolateGenericFilter> {
    
  public: 
    // constructor
    VFInterpolateGenericFilter();

    // Types
    typedef VFInterpolateGenericFilter       Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    void SetInputFilename1(const std::string& file) { mInputFilename1 = file; }
    void SetInputFilename2(const std::string& file) { mInputFilename2 = file; }
    void SetInterpolationName(const std::string & inter);
    void SetDistance(double distance) { mDistance = distance; }
    void SetBSplineOrder(int o) { mBSplineOrder = o; }
    void SetBLUTSampling(int b) { mSamplingFactors.resize(1); mSamplingFactors[0] = b; }

   //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:
    template<unsigned int Dim> void InitializeImageType();
    //--------------------------------------------------------------------
    std::string mInputFilename1, mInputFilename2;
    std::string mInterpolatorName;
    int mBSplineOrder;
    std::vector<int> mSamplingFactors;
    double mDistance;

    //--------------------------------------------------------------------
    template<unsigned int Dim, class PixelType, unsigned int DimCompo> 
    void Update_WithDimAndPixelTypeAndComponent();
    template<class ImageType>
    typename ImageType::Pointer ComputeImage(typename ImageType::Pointer inputImage1, typename ImageType::Pointer inputImage2);
    
  }; // end class VFInterpolateGenericFilter
  //--------------------------------------------------------------------
    
} // end namespace
//--------------------------------------------------------------------
    
#endif /* end #define CLITKIMAGERESAMPLEGENERICFILTER_H */

