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
#ifndef __clitkRecursiveSpatioTemporalMultiResolutionPyramidImageFilter_h
#define __clitkRecursiveSpatioTemporalMultiResolutionPyramidImageFilter_h
#include "clitkSpatioTemporalMultiResolutionPyramidImageFilter.h"
#include "vnl/vnl_matrix.h"

namespace clitk
{

template <
  class TInputImage, 
  class TOutputImage
  >
class ITK_EXPORT RecursiveSpatioTemporalMultiResolutionPyramidImageFilter : 
    public SpatioTemporalMultiResolutionPyramidImageFilter< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef RecursiveSpatioTemporalMultiResolutionPyramidImageFilter  Self;
  typedef SpatioTemporalMultiResolutionPyramidImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef itk::SmartPointer<Self>                          Pointer;
  typedef itk::SmartPointer<const Self>                    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(RecursiveSpatioTemporalMultiResolutionPyramidImageFilter, 
               SpatioTemporalMultiResolutionPyramidImageFilter);

  /** ImageDimension enumeration. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension);

  /** Inherit types from the superclass.. */
  typedef typename Superclass::InputImageType         InputImageType;
  typedef typename Superclass::OutputImageType        OutputImageType;
  typedef typename Superclass::InputImagePointer      InputImagePointer;
  typedef typename Superclass::OutputImagePointer     OutputImagePointer;
  typedef typename Superclass::InputImageConstPointer InputImageConstPointer;

  /** Given one output whose requested region has been set, 
   * this method sets the requtested region for the remaining
   * output images.
   * The original documentation of this method is below.
   * \sa ProcessObject::GenerateOutputRequestedRegion(); */
  virtual void GenerateOutputRequestedRegion(itk::DataObject *output);

  /** RecursiveSpatioTemporalMultiResolutionPyramidImageFilter requires a larger input
   * requested region than the output requested regions to accomdate the
   * shrinkage and smoothing operations.  As such,
   * SpatioTemporalMultiResolutionPyramidImageFilter needs to provide an implementation for
   * GenerateInputRequestedRegion().  The original documentation of this
   * method is below.  \sa ProcessObject::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion();

protected:
  RecursiveSpatioTemporalMultiResolutionPyramidImageFilter();
  ~RecursiveSpatioTemporalMultiResolutionPyramidImageFilter() {};
  void PrintSelf(std::ostream&os, itk::Indent indent) const;

  /** Generate the output data. */
  void GenerateData();

private:
  RecursiveSpatioTemporalMultiResolutionPyramidImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};


} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkRecursiveSpatioTemporalMultiResolutionPyramidImageFilter.txx"
#endif

#endif
