#ifndef _CLITKVECTORBSPLINEINTERPOLATEIMAGEFUNCTIONWITHLUT_TXX
#define _CLITKVECTORBSPLINEINTERPOLATEIMAGEFUNCTIONWITHLUT_TXX

/* =========================================================================
                                                                                
@file   itkBSplineInterpolateImageFunctionWithLUT.txx
@author jefvdmb@gmail.com

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
namespace clitk
{
  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  VectorBSplineInterpolateImageFunctionWithLUT():Superclass() {  
    // Set default values
    //for(int i=0; i<TImageType::ImageDimension; i++) mOutputSpacing[i] = -1;
    SetLUTSamplingFactor(20);
    SetSplineOrder(3);
    mWeightsAreUpToDate = false;
    // Following need to be pointer beacause values are updated into 
    // "const" function
    //   mIntrinsecError  = new double;
    //   mNumberOfError = new long;
    //   mIntrinsecErrorMax = new double;
    //   mInputIsCoef = false;
  }

  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  SetLUTSamplingFactor(const int& s) {  
    for(int i=0; i<TImageType::ImageDimension; i++) mSamplingFactors[i] = s;
    mWeightsAreUpToDate = false;
  }

  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  SetLUTSamplingFactors(const SizeType& s) {  
    for(int i=0; i<TImageType::ImageDimension; i++) mSamplingFactors[i] = s[i];
    mWeightsAreUpToDate = false;
  }

  //====================================================================

  // //====================================================================
  // template <class TImageType, class TCoordRep, class TCoefficientType>
  // void VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  // SetOutputSpacing(const SpacingType & s) {
  //   for(int i=0; i<TImageType::ImageDimension; i++) 
  //     mOutputSpacing[i] = s[i];
  //   // mWeightsAreUpToDate = false;
  // }
  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
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
  void VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
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
  void VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  SetInputImage(const TImageType * inputData) { 

    //==============================
    //   if (!mInputIsCoef) 
    //     {
    Superclass::SetInputImage(inputData);
    //     }
  
    //==============================
    //   //JV  Don't call superclass (decomposition filter is executeed each time!)
    //   else 
    //     {
    //       this->m_Coefficients = inputData;
    //       if ( this->m_Coefficients.IsNotNull())
    // 	{
    // 	  this->m_DataLength = this->m_Coefficients->GetBufferedRegion().GetSize();
    // 	}
  
    //       //Call super-superclass in case more input arrives      
    //       itk::ImageFunction<TImageType, ITK_TYPENAME itk::NumericTraits<typename TImageType::PixelType>::RealType, TCoefficientType>::SetInputImage(inputData);
    //     }  
  
    if (!inputData) return;
    UpdateWeightsProperties();

  }
  
  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  UpdateWeightsProperties() {
    
    // Compute Memory offset inside coefficients images (for looping over coefficients)
    static const unsigned int d = TImageType::ImageDimension;
    mInputMemoryOffset[0] = 1;
    for(unsigned int l=1; l<d; l++) {
      mInputMemoryOffset[l] = 
	mInputMemoryOffset[l-1]*this->m_Coefficients->GetLargestPossibleRegion().GetSize(l-1);
    }

    //JV Put here?   
    if (!mWeightsAreUpToDate)
      {
	// Compute mSupportOffset according to input size
	mSupportOffset.resize(mSupportSize);
	mSupportIndex.resize(mSupportSize);
	for(unsigned int l=0; l<d; l++) mSupportIndex[0][l] = 0;
	for(unsigned int k=0; k<mSupportSize; k++) {
	  // Get memory offset
	  mSupportOffset[k] = itk::Index2Offset<TImageType::ImageDimension>(mSupportIndex[k], mInputMemoryOffset);
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

	//  // Check  
	//   for(unsigned int l=0; l<d; l++) {
	//     if (mOutputSpacing[l] == -1) {
	//       std::cerr << "Please use SetOutputSpacing before using BLUT interpolator" << std::endl;
	//       exit(0);
	//     }
	//   }

	// Compute BSpline weights if not up to date
	//if (!mWeightsAreUpToDate) 
	UpdatePrecomputedWeights();
      }
    // Initialisation
    //   *mIntrinsecErrorMax = *mIntrinsecError = 0.0;
    //   *mNumberOfError = 0;  
  }
  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  UpdatePrecomputedWeights() {
    //   mLUTTimer.Reset();
    //   mLUTTimer.Start();
    mWeightsCalculator.SetSplineOrders(mSplineOrders);
    mWeightsCalculator.SetSamplingFactors(mSamplingFactors);
    mWeightsCalculator.ComputeTensorProducts();
    mWeightsAreUpToDate = true;
    //DS
    //   coef = new TCoefficientType[mSupportSize];   
    //     mCorrectedSupportIndex.resize(mSupportSize);
    //     mCorrectedSupportOffset.resize(mSupportSize);
    //  mLUTTimer.Stop();
    //   mLUTTimer.Print("LUT      \t");
  }
  //====================================================================

  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  typename VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::IndexType
  VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
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
	//  bool mChange = false;

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

	// Statistics (to be removed)
	/*
	 *mIntrinsecError += fabs(index[l]-t2);		    
	 (*mNumberOfError)++;
	 if (fabs(index[l]-t2)> *mIntrinsecErrorMax) *mIntrinsecErrorMax = fabs(index[l]-t2);
	*/

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
  typename VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::OutputType
  VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  EvaluateAtContinuousIndex(const ContinuousIndexType & x) const {

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
      //    return -1000;
      //std::vector<TCoefficientType> coef(mSupportSize);    
      // DD(EvaluateIndex);
      //std::vector<int> CorrectedSupportOffset;//(mSupportSize);
      std::vector<IndexType> correctedSupportIndex;//(mSupportSize);
      correctedSupportIndex.resize(mSupportSize);
      correctedSupportOffset.resize(mSupportSize);
      for(unsigned int i=0; i<mSupportSize; i++) {
	// DD(mSupportIndex[i]);
	for (unsigned int l=0; l<d; l++) {
	  long a = mSupportIndex[i][l] + evaluateIndex[l];
	  long b = this->m_Coefficients->GetLargestPossibleRegion().GetSize(l);
	  // DD(a);
	  // DD(b);
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
	// DD(correctedSupportIndex[i]);
	correctedSupportOffset[i] = itk::Index2Offset<TImageType::ImageDimension>(correctedSupportIndex[i], mInputMemoryOffset);
      }
      // for (unsigned int l=0; l<d; l++) {
      //       EvaluateIndex[l] = EvaluateIndex[l] + correctedSupportIndex[0][l];
      //     }
      // DD(EvaluateIndex);
      psupport = &correctedSupportOffset[0];
    }
    else {
      psupport = &mSupportOffset[0];
    }

    // Get pointer to first coefficient. Even if in some boundary cases,
    // EvaluateIndex is out of the coefficient image, 
    //JV
    const CoefficientImagePixelType * pcoef = &(this->m_Coefficients->GetPixel(evaluateIndex));

    // Main loop over BSpline support
    CoefficientImagePixelType interpolated = 0.0;
    for (unsigned int p=0; p<mSupportSize; p++) {
      interpolated += pcoef[*psupport] * (*pweights);
      ++psupport;
      ++pweights;
    }

    // Return interpolated value
    return(interpolated);    
  }
  //====================================================================


  //====================================================================
  template <class TImageType, class TCoordRep, class TCoefficientType>
  void
  VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep,TCoefficientType>::
  EvaluateWeightsAtContinuousIndex(const ContinuousIndexType&  x,  const TCoefficientType ** pweights, IndexType & evaluateIndex)const {

    // JV Compute BSpline weights if not up to date! Problem const: pass image as last
    //  if (!mWeightsAreUpToDate) UpdatePrecomputedWeights();

    // For shorter coding
    static const unsigned int d = TImageType::ImageDimension;

    // Compute the index of the first interpolation coefficient in the coefficient image
    //IndexType evaluateIndex;
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
    *pweights = mWeightsCalculator.GetFirstTensorProduct(weightIndex);

  }
  //====================================================================




} //end namespace

#endif //_CLITKVECTORBSPLINEINTERPOLATEIMAGEFUNCTIONWITHLUT_TXX
