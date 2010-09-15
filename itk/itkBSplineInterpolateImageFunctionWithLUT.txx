#ifndef _ITKINTERPOLATEIMAGEFUNCTIONWITHLUT_TXX
#define _ITKINTERPOLATEIMAGEFUNCTIONWITHLUT_TXX

/* =========================================================================
                                                                                
@file   itkBSplineInterpolateImageFunctionWithLUT.txx
@author David Sarrut <david.sarrut@creatis.insa-lyon.fr>

Copyright (c) 
* CREATIS (Centre de Recherche et d'Applications en Traitement de l'Image). 
All rights reserved. See Doc/License.txt or
http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
* Léon Bérard cancer center, 28 rue Laënnec, 69373 Lyon cedex 08, France
* http://www.creatis.insa-lyon.fr/rio
                                                                                
This software is distributed WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
                                                                             
========================================================================= */
namespace itk
{
  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  BSplineInterpolateImageFunctionWithLUT():Superclass() {  
    
    // Set default values
    SetLUTSamplingFactor(20);
    SetSplineOrder(3);
    mWeightsAreUpToDate = false;
  }

  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  SetLUTSamplingFactor(const int& s) {  
    for(int i=0; i<TImageType::ImageDimension; i++) mSamplingFactors[i] = s;
    mWeightsAreUpToDate = false;
  }

  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  SetLUTSamplingFactors(const SizeType& s) {  
    for(int i=0; i<TImageType::ImageDimension; i++) mSamplingFactors[i] = s[i];
    mWeightsAreUpToDate = false;
  }

  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  SetSplineOrder(const unsigned int& SplineOrder) {
    Superclass::SetSplineOrder(SplineOrder);
    // Compute support and half size
    static const int d = TImageType::ImageDimension;
    for(int l=0; l<d; l++) {
      mSplineOrders[l]= SplineOrder;
      mSupport[l] = SplineOrder+1;
      if (mSupport[l] % 2 == 0) { // support is even
	mHalfSupport[l] = mSupport[l]/2-1;
      }
      else mHalfSupport[l] = mSupport[l]/2; // support is odd (like cubic spline)
    }
    mSupportSize = 1;
    for(int l=0; l<d; l++) {
      mSupportSize *= mSupport[l];
    }
    mWeightsAreUpToDate = false;
  }
  //====================================================================

  //JV
  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  SetSplineOrders(const SizeType& SplineOrders) {
    mSplineOrders=SplineOrders;

    // Compute support and half size
    static const int d = TImageType::ImageDimension;
    for(int l=0; l<d; l++) {
      mSupport[l] = mSplineOrders[l]+1;
      if (mSupport[l] % 2 == 0) { // support is even
	mHalfSupport[l] = mSupport[l]/2-1;
      }
      else mHalfSupport[l] = mSupport[l]/2; // support is odd (like cubic spline)
    }
    mSupportSize = 1;
    for(int l=0; l<d; l++) {
      mSupportSize *= mSupport[l];
    }
    mWeightsAreUpToDate = false;
  }
  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  SetInputImage(const TImageType * inputData) { 

    // JV  Call superclass (decomposition filter is executeed each time!)
    // JV Should call itkBSplineDecompositionFilterWithOBD to allow different order by dimension
    Superclass::SetInputImage(inputData);
    
    // Update the weightproperties
    if (!inputData) return;
    UpdateWeightsProperties();
  }
  
  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  UpdateWeightsProperties() {
    
    // Compute Memory offset inside coefficients images (for looping over coefficients)
    static const unsigned int d = TImageType::ImageDimension;
    mInputMemoryOffset[0] = 1;
    for(unsigned int l=1; l<d; l++) {
      mInputMemoryOffset[l] = 
	mInputMemoryOffset[l-1]*this->m_Coefficients->GetLargestPossibleRegion().GetSize(l-1);
    }
    
    // Compute mSupportOffset according to input size
    mSupportOffset.resize(mSupportSize);
    mSupportIndex.resize(mSupportSize);
    for(unsigned int l=0; l<d; l++) mSupportIndex[0][l] = 0;
    for(unsigned int k=0; k<mSupportSize; k++) {
      // Get memory offset
      mSupportOffset[k] = Index2Offset<TImageType::ImageDimension>(mSupportIndex[k], mInputMemoryOffset);
      // next coefficient index
      if (k != mSupportSize-1) {
	for(unsigned int l=0; l<d; l++) mSupportIndex[k+1][l] = mSupportIndex[k][l];
	int l=0;
	bool stop = false;
	while (!stop) {
	  mSupportIndex[k+1][l]++;
	  if (static_cast<unsigned int>(mSupportIndex[k+1][l]) == mSupport[l]) {
	    mSupportIndex[k+1][l] = 0;
	    l++;
	  }
	      else stop = true;
	}
      }
    }
    
    // Compute BSpline weights if not up to date
    if (!mWeightsAreUpToDate) 
      UpdatePrecomputedWeights();
  }
  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  UpdatePrecomputedWeights() {

    mWeightsCalculator.SetSplineOrders(mSplineOrders);
    mWeightsCalculator.SetSamplingFactors(mSamplingFactors);
    mWeightsCalculator.ComputeTensorProducts();
    mWeightsAreUpToDate = true;
  }
  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  typename BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::IndexType
  BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  GetSampleIndexOfPixelPosition(const ContinuousIndexType & x, IndexType & EvaluateIndex) const {

    /*
      WARNING : sometimes, a floating number x could not really be
      represented in memory. In this case, the difference between x and
      floor(x) can be almost 1 (instead of 0).  So we take into account
      such special case, otherwise it could lead to annoying results.
    */
    //  static const TCoefficientType tiny = 1.0e-7;
    IndexType index;

    for(int l=0; l<TImageType::ImageDimension; l++) 
      {
	// Compute t1 = distance to floor 
	TCoefficientType t1 = x[l]- vcl_floor(x[l]);
    
	// Compute index in precomputed weights table
	TCoefficientType t2 = mSamplingFactors[l]*t1;
	index[l] = (IndexValueType)lrint(t2);

	// For even order : test if too close to 0.5 (but lower). In this
	// case : take the next coefficient
	if (!(mSplineOrders[l] & 1)) {
	  if (t1<0.5) {
	    if (mSamplingFactors[l] & 1) {
	      if (index[l] ==  (int) mSamplingFactors[l]/2+1) EvaluateIndex[l] = EvaluateIndex[l]+1;
	    }

	    else if (index[l] == (int) mSamplingFactors[l]/2) EvaluateIndex[l] = EvaluateIndex[l]+1;
	  }
	}

	// When to close to 1, take the next coefficient for odd order, but
	// only change index for odd
	if (index[l] == (int)mSamplingFactors[l]) {
	  index[l] = 0;
	  if (mSplineOrders[l] & 1) EvaluateIndex[l] = EvaluateIndex[l]+1;
	}
      }

    // The end
    return index;
  }


  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  typename BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::OutputType
  BSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  EvaluateAtContinuousIndex(const ContinuousIndexType & x) const {

    // JV Compute BSpline weights if not up to date! Problem const: pass image as last
    //  if (!mWeightsAreUpToDate) UpdatePrecomputedWeights();

    // For shorter coding
    static const unsigned int d = TImageType::ImageDimension;

    // Compute the index of the first interpolation coefficient in the coefficient image
    IndexType evaluateIndex;
    long indx;
    for (unsigned int l=0; l<d; l++)  {
      if (mSplineOrders[l] & 1) {  // Use this index calculation for odd splineOrder (like cubic)
	indx = (long)vcl_floor(x[l]) - mSplineOrders[l] / 2 ; //this->m_SplineOrder / 2;
	evaluateIndex[l] = indx;
      }
      else { // Use this index calculation for even splineOrder
	if (mSplineOrders[l] == 0) evaluateIndex[l] = (long)rint(x[l]);
	else {
	  indx = (long)vcl_floor((x[l]+ 0.5)) - mSplineOrders[l] / 2; //this->m_SplineOrder / 2;
	  evaluateIndex[l] = indx;
	}
      }
    }
  
    // Compute index of precomputed weights and get pointer to first weights
    const IndexType weightIndex = GetSampleIndexOfPixelPosition(x, evaluateIndex);
    const TCoefficientType * pweights = mWeightsCalculator.GetFirstTensorProduct(weightIndex);

    // Check boundaries
    bool boundaryCase = false;
    for (unsigned int l=0; l<d; l++) {
      if ((evaluateIndex[l] < 0) ||
	  (evaluateIndex[l]+mSupport[l]) >= this->m_Coefficients->GetLargestPossibleRegion().GetSize(l)) {
	boundaryCase = true;
      }
    }

    // Pointer to support offset
    const int * psupport;
    
    // Special case for boundary (to be changed ....)
    std::vector<int> correctedSupportOffset;
    if (boundaryCase) {
    
      std::vector<IndexType> correctedSupportIndex;//(mSupportSize);
      correctedSupportIndex.resize(mSupportSize);
      correctedSupportOffset.resize(mSupportSize);
      for(unsigned int i=0; i<mSupportSize; i++) {
	for (unsigned int l=0; l<d; l++) {
	  long a = mSupportIndex[i][l] + evaluateIndex[l];
	  long b = this->m_Coefficients->GetLargestPossibleRegion().GetSize(l);
	  long d2 = 2 * b - 2;
	  if (a < 0) {
	    correctedSupportIndex[i][l] = -a - d2*(-a/d2) - evaluateIndex[l];//mSupportIndex[i][l]-a;
	  }
	  else {
	    if (a>=b) {
	      correctedSupportIndex[i][l] = d2 - a - evaluateIndex[l];
	    }
	    else {
	      correctedSupportIndex[i][l] = mSupportIndex[i][l]; //a - d2*(a/d2) - EvaluateIndex[l];
	    }
	    /*
	      if (a>=b) {
	      correctedSupportIndex[i][l] = d2 - a - EvaluateIndex[l];//mSupportIndex[i][l] - (a-(b-1));
	      }
	      else {
	      correctedSupportIndex[i][l] = mSupportIndex[i][l];
	      }
	    */
	  }
	}
	correctedSupportOffset[i] = itk::Index2Offset<TImageType::ImageDimension>(correctedSupportIndex[i], mInputMemoryOffset);
      }
      psupport = &correctedSupportOffset[0];
    }
    else {
      psupport = &mSupportOffset[0];
    }

    // Get pointer to first coefficient. Even if in some boundary cases,
    // EvaluateIndex is out of the coefficient image, 
    const TCoefficientType * pcoef = &(this->m_Coefficients->GetPixel(evaluateIndex));

    // Main loop over BSpline support
    TCoefficientType interpolated = itk::NumericTraits<TCoefficientType>::Zero;
    for (unsigned int p=0; p<mSupportSize; p++) {
      interpolated += pcoef[*psupport] * (*pweights);
      ++psupport;
      ++pweights;
    }

    // Return interpolated value
    return(interpolated);    
  }
  //====================================================================

}
#endif //_ITKINTERPOLATEIMAGEFUNCTIONWITHLUT_TXX
