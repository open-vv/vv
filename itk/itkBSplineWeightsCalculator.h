/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - LÃ©on BÃ©rard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
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

#ifndef ITKBSPLINEWEIGHTSCALCULATOR_H
#define ITKBSPLINEWEIGHTSCALCULATOR_H

#include <vector>
#include <itkIndex.h>
#include <itkContinuousIndex.h>

namespace itk {
  
  //====================================================================
  template<int VDimension>
  typename Index<VDimension>::IndexValueType Index2Offset(const Index<VDimension> & index, 
							  const Index<VDimension> & offsetTable);

  //====================================================================
  template<class TCoefficientType, int VDimension>
  class BSplineWeightsCalculator {
    
  public: 
    // Some typedef
    typedef Index<VDimension> IndexType;
    typedef Size<VDimension>  SizeType;
    typedef ContinuousIndex<double, VDimension>         ContinuousIndexType;
    typedef typename IndexType::IndexValueType          IndexValueType;
    typedef typename SizeType::SizeValueType            SizeValueType;
    typedef std::vector<std::vector<TCoefficientType> > InitialWeightsType;

    // Constructor
    BSplineWeightsCalculator();

    // Set order of the spline (could be different in each dimension)
    void SetSplineOrder(int splineOrder);
    void SetSplineOrders(const SizeType & splineOrder);

    // Set the sampling factor (could be different in each dimension)
    void SetSamplingFactor(int sampling);
    void SetSamplingFactors(const SizeType & sampling);
    
    // Main function : compute the tensor product at sampling positions
    void ComputeTensorProducts();

    // Must be used only after ComputeTensorProducts !
    const TCoefficientType * GetFirstTensorProduct(const IndexType & index) const;

  protected:
    SizeType      mSplineOrders;
    SizeType      mSplineSupport;
    SizeType      mSamplingFactors;

    // Number of points in the BSpline support
    int                    mSupportSize;

    // nD Index of all points in the BSpline support
    std::vector<IndexType> mSupportIndex;

    // mBasisFunctionCoefficientsMatrix : map of 2D matrix for basic function of arbitrary order
    // Could be computed once for all if needed.
    //       index map = BSpline order 
    //               X = support size
    //               Y = support size
    std::map<int, InitialWeightsType> mBasisFunctionCoefficientsMatrix;

    // mWeights : 3D array with  :
    //            X = dimension number
    //            Y = support position 1D index
    //            Z = sampling position 1D index
    std::vector<std::vector<std::vector<TCoefficientType> > > mWeights;
    
    // mTensorProducts : 2D array with :
    //            X = sampling position 1D index
    //            Y = support position 1D index
    std::vector<std::vector<TCoefficientType> > mTensorProducts;


    IndexType mTensorProductMemoryOffset;
    
    bool                mWeightsAreUpToDate;
    std::map<int, bool> mBasisFunctionCoefficientsMatrixAreUpToDate;
    bool                mTensorProductsAreUpToDate;

    void ComputeSampledWeights();
    void ComputeSampledWeights1D(std::vector<std::vector<TCoefficientType> > & w, int order, int sampling);
    TCoefficientType BSplineEvaluate(int order, int b, double e);
    InitialWeightsType & GetInitialWeights(int order);
    void ComputeBasisFunctionCoefficientsMatrix(int order);
    double BinomialCoefficient(int i, int j);

  }; // end class itkBSplineWeightsCalculator

#include "itkBSplineWeightsCalculator.txx"

} // end namespace

#endif /* end #define ITKBSPLINEWEIGHTSCALCULATOR_H */

