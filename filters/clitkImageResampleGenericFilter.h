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

#ifndef CLITKIMAGERESAMPLEGENERICFILTER_H
#define CLITKIMAGERESAMPLEGENERICFILTER_H

// clitk include
#include "clitkImageToImageGenericFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class ImageResampleGenericFilter: 
    public ImageToImageGenericFilter<ImageResampleGenericFilter> {
    
  public: 
    // constructor
    ImageResampleGenericFilter();

    // Types
    typedef ImageResampleGenericFilter    Self;
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
    template<class InputImageType> void UpdateWithInputImageType();

  protected:
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
    template<unsigned int Dim> void InitializeImageTypeWithDim();
     
  }; // end class ImageResampleGenericFilter
  //--------------------------------------------------------------------
    
  //#include "clitkImageResampleGenericFilter.txx"

} // end namespace
//--------------------------------------------------------------------
    
#endif /* end #define CLITKIMAGERESAMPLEGENERICFILTER_H */

