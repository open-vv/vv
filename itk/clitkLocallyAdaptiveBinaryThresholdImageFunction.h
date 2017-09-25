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
#ifndef __clitkLocallyAdaptiveBinaryThresholdImageFunction_h
#define __clitkLocallyAdaptiveBinaryThresholdImageFunction_h
#include "itkBinaryThresholdImageFunction.h"

namespace clitk
{

template <class TInputImage, class TCoordRep = float >
class ITK_EXPORT LocallyAdaptiveBinaryThresholdImageFunction :
    public itk::BinaryThresholdImageFunction< TInputImage, TCoordRep >
{
public:
  /** Standard class typedefs. */
  typedef LocallyAdaptiveBinaryThresholdImageFunction            Self;
  typedef itk::BinaryThresholdImageFunction<TInputImage,TCoordRep> Superclass;
  typedef itk::SmartPointer<Self>                                  Pointer;
  typedef itk::SmartPointer<const Self>                            ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(LocallyAdaptiveBinaryThresholdImageFunction, BinaryThresholdImageFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** InputImageType typedef support. */
  typedef TInputImage InputImageType;

  /** OutputType typdef support. */
  typedef typename Superclass::OutputType OutputType;

  /** Index typedef support. */
  typedef typename Superclass::IndexType IndexType;
  
  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;

  /** Point typedef support. */
  typedef typename Superclass::PointType PointType;

  /** PixelType typedef support. */
  typedef typename Superclass::PixelType PixelType;

  /** Dimension of the underlying image. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      InputImageType::ImageDimension);

  /** SizeType of the input image */
  typedef typename InputImageType::SizeType InputSizeType;

  /** Set the radius of the neighborhood used in computation. */
  itkSetMacro(Radius, InputSizeType);

  /** Get the radius of the neighborhood used in computation */
  itkGetConstReferenceMacro(Radius, InputSizeType);

  /** Evalulate the function at specified index */
  virtual bool EvaluateAtIndex( const IndexType& index ) const ITK_OVERRIDE;
  
  /** Evaluate the function at non-integer positions */
  virtual bool Evaluate( const PointType& point ) const ITK_OVERRIDE
    { 
    IndexType index;
    this->ConvertPointToNearestIndex( point, index );
    return this->EvaluateAtIndex( index ); 
    }
  virtual bool EvaluateAtContinuousIndex( 
    const ContinuousIndexType& cindex ) const ITK_OVERRIDE
    { 
    IndexType index;
    this->ConvertContinuousIndexToNearestIndex( cindex, index );
    return this->EvaluateAtIndex( index ); 
    }

  // JV
  itkBooleanMacro(LowerBorderIsGiven);
  itkSetMacro( LowerBorderIsGiven, bool);
  itkGetConstReferenceMacro( LowerBorderIsGiven, bool);
  itkBooleanMacro(UpperBorderIsGiven);
  itkSetMacro( UpperBorderIsGiven, bool);
  itkGetConstReferenceMacro( UpperBorderIsGiven, bool);
  itkSetMacro( Multiplier, double);
  itkGetConstMacro( Multiplier, double);
  itkBooleanMacro(MaximumSDIsGiven);
  itkSetMacro( MaximumSDIsGiven, bool);
  itkGetConstReferenceMacro( MaximumSDIsGiven, bool);
  itkSetMacro( MaximumSD, double);
  itkGetConstMacro( MaximumSD, double);
  

  
protected:
  LocallyAdaptiveBinaryThresholdImageFunction();
  ~LocallyAdaptiveBinaryThresholdImageFunction(){};
  void PrintSelf(std::ostream& os, itk::Indent indent) const ITK_OVERRIDE;

private:
  LocallyAdaptiveBinaryThresholdImageFunction( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  InputSizeType m_Radius;

  // JV 
  bool m_LowerBorderIsGiven;
  bool m_UpperBorderIsGiven;
  bool m_MaximumSDIsGiven;
  double m_Multiplier;
  double m_MaximumSD;

};

} // end namespace clitk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_LocallyAdaptiveBinaryThresholdImageFunction(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT LocallyAdaptiveBinaryThresholdImageFunction< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef LocallyAdaptiveBinaryThresholdImageFunction< ITK_TEMPLATE_2 x > \
                                         LocallyAdaptiveBinaryThresholdImageFunction##y; } \
  }

# include "clitkLocallyAdaptiveBinaryThresholdImageFunction.txx"

#endif
