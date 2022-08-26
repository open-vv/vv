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
#ifndef __clitkVectorBSplineInterpolateImageFunctionWithLUT_h
#define __clitkVectorBSplineInterpolateImageFunctionWithLUT_h
/* =========================================================================
                                                                                
  @file   clitVectorkBSplineInterpolateImageFunctionWithLUT.h
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

#include "itkBSplineWeightsCalculator.h"
#include "clitkVectorBSplineInterpolateImageFunction.h"

namespace clitk {
  
  template <
    class TImageType, 
    class TCoordRep = double,
    class TCoefficientType = double >
  class ITK_EXPORT VectorBSplineInterpolateImageFunctionWithLUT : 
    public VectorBSplineInterpolateImageFunction<TImageType,TCoordRep,TCoefficientType> {
    
  public: 
    /** Class typedefs */
    typedef VectorBSplineInterpolateImageFunctionWithLUT<TImageType,TCoordRep, TCoefficientType> Self;
    typedef VectorBSplineInterpolateImageFunction<TImageType,TCoordRep, TCoefficientType>  Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef typename Superclass::OutputType OutputType;
    typedef typename Superclass::ContinuousIndexType ContinuousIndexType;
    typedef typename TImageType::IndexType           IndexType;
    typedef typename TImageType::IndexValueType      IndexValueType;
    typedef typename TImageType::SizeType            SizeType;
    typedef typename TImageType::SpacingType         SpacingType;

    typedef TCoefficientType CoefficientDataType;
    typedef typename Superclass::CoefficientImagePixelType CoefficientImagePixelType;
    typedef typename Superclass::CoefficientImageType CoefficientImageType;

    /** New macro for creation of through a Smart Pointer */
    itkNewMacro(Self);
    
    /** Setting LUT sampling (one parameters by dimension or a single
	one for all dim); Default is 20 (for each dim) **/
    void SetLUTSamplingFactor(const int& s);
    void SetLUTSamplingFactors(const SizeType& s);

    /** Get/Sets the Spline Order, supports 0th - 5th order
     *  splines. The default is a 3rd order spline. */
    void SetSplineOrder(const unsigned int & SplineOrder);

    //JV this is added to support different degrees over each dimension
    void SetSplineOrders(const  SizeType & SplineOrders);

    /** Set the input image.  This must be set by the user. */
    virtual void SetInputImage(const TImageType * inputData) ITK_OVERRIDE;
    //void SetOutputSpacing(const SpacingType & s);
    //void SetInputImageIsCoefficient(bool inputIsCoef) { mInputIsCoef = inputIsCoef; }

    /** Evaluate the function at a ContinuousIndex position.
  Overwritten for taking LUT into account (RP: multi-threading-compatible version, 
  the threadID is actually ignored) */  
    virtual OutputType EvaluateAtContinuousIndex(const ContinuousIndexType & index, unsigned int /* threadID */ ) const
    {
      std::cout << "EvaluateAtContinuousIndex" << std::endl;
      return this->EvaluateAtContinuousIndex( index );
    }

    /** Evaluate the function at a ContinuousIndex position.
	Overwritten for taking LUT into account */  
    virtual OutputType EvaluateAtContinuousIndex(const ContinuousIndexType & index ) const;
    void EvaluateWeightsAtContinuousIndex(const ContinuousIndexType &  x,  const TCoefficientType ** pweights, IndexType & evaluateIndex) const;

    /** Static convenient functions to compute BSpline weights for
	various order, dimension, sampling ... **/
    static void ComputeBlendingWeights(int dim, int order, int sampling, TCoefficientType * weights);

  protected:
    VectorBSplineInterpolateImageFunctionWithLUT();
    ~VectorBSplineInterpolateImageFunctionWithLUT(){;}

    SizeType               mSupport;            // nb of coef values used for interpolation (order+1) in 1 dimension
    SizeType               mHalfSupport;        // half size of the previous
    unsigned int           mSupportSize;        // Total support size for all dimension
    std::vector<int>       mSupportOffset;      // Memory pointer offset for going from one coef position to the other (inside the whole support)
    std::vector<IndexType> mSupportIndex;       // nD Index of all support values
    IndexType              mInputMemoryOffset;  // Memory dimension offsets for input image

    /** Sampling factors for LUT weights **/
    SizeType               mSamplingFactors;
    bool                   mWeightsAreUpToDate;
    SizeType mSplineOrders;

    // Filter to compute weights
    itk::BSplineWeightsCalculator<TCoefficientType,TImageType::ImageDimension> mWeightsCalculator;

    // Convenient functions
    void UpdatePrecomputedWeights();
    void UpdateWeightsProperties();
    IndexType GetSampleIndexOfPixelPosition(const ContinuousIndexType & x, IndexType & EvaluateIndex) const;


  }; // end class clitkVectorBSplineInterpolateImageFunctionWithLUT
} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkVectorBSplineInterpolateImageFunctionWithLUT.txx"
#endif

#endif /* end #define CLITKBSPLINEINTERPOLATEIMAGEFUNCTIONWITHLUT_H */
