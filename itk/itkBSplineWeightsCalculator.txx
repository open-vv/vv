/* =========================================================================
                                                                                
  @file   itkBSplineWeightsCalculator.h
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

#ifndef ITKBSPLINEWEIGHTSCALCULATOR_TXX
#define ITKBSPLINEWEIGHTSCALCULATOR_TXX

//====================================================================
template<int VDimension>
typename Index<VDimension>::IndexValueType Index2Offset(const Index<VDimension> & index, const Index<VDimension> & offsetTable) {
  long v = index[0];
  for(int l=1; l<VDimension; l++) {
    v += index[l] * offsetTable[l];
  }
  return v;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
BSplineWeightsCalculator<TCoefficientType, VDimension>::
BSplineWeightsCalculator() {
  mWeightsAreUpToDate = false;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
void BSplineWeightsCalculator<TCoefficientType, VDimension>::
SetSplineOrder(int splineOrder) {
  SizeType temp;
  for(int l=0; l<VDimension; l++) temp[l] = splineOrder;
  SetSplineOrders(temp);
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
void BSplineWeightsCalculator<TCoefficientType, VDimension>::
SetSplineOrders(const SizeType & splineOrder) {
  // Compute support size
  mSupportSize = 1;
  for(int l=0; l<VDimension; l++) {
    mSplineOrders[l] = splineOrder[l];
    mSplineSupport[l] = mSplineOrders[l]+1;
    mSupportSize *= mSplineSupport[l];
  }

  // Compute indexes of points in support
  mSupportIndex.resize(mSupportSize);
  for(int l=0; l<VDimension; l++) mSupportIndex[0][l] = 0;
  for(int k=0; k<mSupportSize; k++) {
    if (k != mSupportSize-1) {
      for(int l=0; l<VDimension; l++) mSupportIndex[k+1][l] = mSupportIndex[k][l];
      int l=0;
      bool stop = false;
      while (!stop) {
	mSupportIndex[k+1][l]++;
	if (mSupportIndex[k+1][l] == (int)mSplineSupport[l]) { //Ds supportindex=int support=uint
	  mSupportIndex[k+1][l] = 0;
	  l++;
	}
	else stop = true;
      }
    }
  }
  mWeightsAreUpToDate = false;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
void BSplineWeightsCalculator<TCoefficientType, VDimension>::
SetSamplingFactor(int sampling) {
  for(int l=0; l<VDimension; l++) mSamplingFactors[l] = sampling;
  mWeightsAreUpToDate = false;
}
//====================================================================

///====================================================================
template<class TCoefficientType, int VDimension>
void BSplineWeightsCalculator<TCoefficientType, VDimension>::
SetSamplingFactors(const SizeType & sampling) {
  for(int l=0; l<VDimension; l++) mSamplingFactors[l] = sampling[l];
  mWeightsAreUpToDate = false;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
typename BSplineWeightsCalculator<TCoefficientType, VDimension>::InitialWeightsType & 
BSplineWeightsCalculator<TCoefficientType, VDimension>::
GetInitialWeights(int order) {
  if (!mBasisFunctionCoefficientsMatrixAreUpToDate[order]) ComputeBasisFunctionCoefficientsMatrix(order);
  return mBasisFunctionCoefficientsMatrix[order];
}
//====================================================================

//====================================================================
template<class T> inline T factorial(T rhs) {
  T lhs = (T)1;  
  for(T x=(T)1; x<=rhs; ++x) {
    lhs *= x;
  }  
  return lhs;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
double BSplineWeightsCalculator<TCoefficientType, VDimension>::
BinomialCoefficient(int i, int j) {
  double f = (factorial(i))/(factorial(j) * factorial(i-j));
  return f;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
void BSplineWeightsCalculator<TCoefficientType, VDimension>::
ComputeBasisFunctionCoefficientsMatrix(int order) {
  // Compute the sxs matrix of coefficients used to build the different
  // polynomials. With s the support is order+1.
  int s = order+1;
  mBasisFunctionCoefficientsMatrix[order] = InitialWeightsType();
  mBasisFunctionCoefficientsMatrix[order].resize(s);
  for(int i=0; i<s; i++) {
    mBasisFunctionCoefficientsMatrix[order][i].resize(s);
    for(int j=0; j<s; j++) {
      mBasisFunctionCoefficientsMatrix[order][i][j] = 0.0;
      for(int m=j; m<s; m++) {
	double a = pow((double)(s-(m+1)),i) * pow((double)-1,m-j) * BinomialCoefficient(s, m-j);
	mBasisFunctionCoefficientsMatrix[order][i][j] += a;
      }	  
      mBasisFunctionCoefficientsMatrix[order][i][j] *= BinomialCoefficient(s-1, i);
    }
  }
  int f = factorial(order);
  for(int i=0; i<s; i++) {
    for(int j=0; j<s; j++) {
      mBasisFunctionCoefficientsMatrix[order][i][j] /= f;
    }
  }
  mBasisFunctionCoefficientsMatrixAreUpToDate[order] = true;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
TCoefficientType BSplineWeightsCalculator<TCoefficientType, VDimension>::
BSplineEvaluate(int order, int k, double e) {
  // Evaluate a BSpline 
  int s=order+1;
  TCoefficientType v = 0.0;
  for(int p=0; p<s; p++) {
    v += pow(e,order-p)*GetInitialWeights(order)[p][k];
  }
  return v;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
void BSplineWeightsCalculator<TCoefficientType, VDimension>::
ComputeSampledWeights1D(std::vector<std::vector<TCoefficientType> > & w, int order, int sampling) {
  int s = order+1;
  w.resize(s);
  for(int k=0; k<s; k++) w[k].resize(sampling);
  double offset = 1.0/sampling;
  double e=0.0;
  // For odd order (like cubic) start with e=0
  if (order & 1) e = 0;
  // or even order (like quadratic), shift by 0.5
  else  e = +0.5;
  for(int a=0; a<sampling; a++) {   // loop over positions
    // std::cout << a << " = " << e << std::endl;
    for(int k=0; k<s; k++) {        // loop over spline support
      w[k][a] = BSplineEvaluate(order, k, e);
    }
    e += offset;
    if (fabs(1.0-e)<=1e-6) e = 1.0; // (for even order)
    if (e>=1.0) e = e-1.0; 
  }
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
void BSplineWeightsCalculator<TCoefficientType, VDimension>::
ComputeSampledWeights() {
  mWeights.resize(VDimension);
  // Loop over dimension to compute weights
  for(int l=0; l<VDimension; l++) { 
    ComputeSampledWeights1D(mWeights[l], mSplineOrders[l], mSamplingFactors[l]);
  }
  mWeightsAreUpToDate = true;
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
void BSplineWeightsCalculator<TCoefficientType, VDimension>::
ComputeTensorProducts() {
  // Initial BSpline samples weights
  ComputeSampledWeights();
  
  // tensor product memory offsets
  mTensorProductMemoryOffset[0] = 1;
  for(int l=1; l<VDimension; l++) {    
    mTensorProductMemoryOffset[l] = mTensorProductMemoryOffset[l-1]*mSamplingFactors[l-1];
  }

  // Tensor product initialisation
  int nbPositions = 1;
  for(int l=0; l<VDimension; l++) nbPositions *= mSamplingFactors[l];
  // int nb = mSupportSize * nbPositions;
  mTensorProducts.resize(nbPositions);
  IndexType mPositionIndex;
  for(int l=0; l<VDimension; l++) mPositionIndex[l] = 0;

  // Tensor product
  for(int a=0; a<nbPositions; a++) { // Loop over sampling positions
    mTensorProducts[a].resize(mSupportSize);

    for(int k=0; k<mSupportSize; k++) { // Loop over support positions
      TCoefficientType B = 1.0;

      for(int l=0; l<VDimension; l++) { // loop for tensor product	
	B *= mWeights[l][mSupportIndex[k][l]][mPositionIndex[l]];
      }
      mTensorProducts[a][k] = B;
    }
    
    // Next sample Position index
    int l=0;
    bool stop = false;
    while (!stop) {
      mPositionIndex[l]++;
      if (mPositionIndex[l] == (int)mSamplingFactors[l]) {
	mPositionIndex[l] = 0;
	l++;
      }
      else stop = true;
    }      
  }
}
//====================================================================

//====================================================================
template<class TCoefficientType, int VDimension>
const TCoefficientType * BSplineWeightsCalculator<TCoefficientType, VDimension>::
GetFirstTensorProduct(const IndexType & index) const {
  int i = Index2Offset<VDimension>(index, mTensorProductMemoryOffset);
  return &(mTensorProducts[i][0]);
}
//====================================================================

#endif /* end #define ITKBSPLINEWEIGHTSCALCULATOR_TXX */

