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
#ifndef __clitkSpatioTemporalMultiResolutionPyramidImageFilter_h
#define __clitkSpatioTemporalMultiResolutionPyramidImageFilter_h
#include "itkImageToImageFilter.h"
#include "itkArray2D.h"

namespace clitk
{

template <  class TInputImage,  class TOutputImage  >
class ITK_EXPORT SpatioTemporalMultiResolutionPyramidImageFilter : 
    public itk::ImageToImageFilter< TInputImage, TOutputImage >
{

public:
  /** Standard class typedefs. */
  typedef SpatioTemporalMultiResolutionPyramidImageFilter             Self;
  typedef itk::ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef itk::SmartPointer<Self>                            Pointer;
  typedef itk::SmartPointer<const Self>                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SpatioTemporalMultiResolutionPyramidImageFilter, ImageToImageFilter);

  /** ScheduleType typedef support. */
  typedef itk::Array2D<unsigned int>  ScheduleType;

  /** ImageDimension enumeration. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Inherit types from Superclass. */
  typedef typename Superclass::InputImageType         InputImageType;
  typedef typename Superclass::OutputImageType        OutputImageType;
  typedef typename Superclass::InputImagePointer      InputImagePointer;
  typedef typename Superclass::OutputImagePointer     OutputImagePointer;
  typedef typename Superclass::InputImageConstPointer InputImageConstPointer;

  /** Set the number of multi-resolution levels. The matrix containing the
   * schedule will be resized accordingly.  The schedule is populated with
   * default values.  At the coarset (0) level, the shrink factors are set
   * 2^(nlevel - 1) for all dimension. These shrink factors are halved for
   * subsequent levels.  The number of levels is clamped to a minimum value
   * of 1.  All shrink factors are also clamped to a minimum value of 1. */
  virtual void SetNumberOfLevels(unsigned int num);

  /** Get the number of multi-resolution levels. */
  itkGetConstMacro(NumberOfLevels, unsigned int);

  /** Set a multi-resolution schedule.  The input schedule must have only
   * ImageDimension number of columns and NumberOfLevels number of rows.  For
   * each dimension, the shrink factor must be non-increasing with respect to
   * subsequent levels. This function will clamp shrink factors to satisify
   * this condition.  All shrink factors less than one will also be clamped
   * to the value of 1. */
  virtual void SetSchedule( const ScheduleType& schedule );

  /** Get the multi-resolution schedule. */
  itkGetConstReferenceMacro(Schedule, ScheduleType);

  /** Set the starting shrink factor for the coarset (0) resolution
   * level. The schedule is then populated with defaults values obtained by
   * halving the factors at the previous level.  All shrink factors are
   * clamped to a minimum value of 1. */
  virtual void SetStartingShrinkFactors( unsigned int factor );
  virtual void SetStartingShrinkFactors( unsigned int* factors );

  /** Get the starting shrink factors */
  const unsigned int * GetStartingShrinkFactors() const;

  /** Test if the schedule is downward divisible. This method returns true if
   * at every level, the shrink factors are divisble by the shrink factors at
   * the next level. */
  static bool IsScheduleDownwardDivisible( const ScheduleType& schedule );

  /** MultiResolutionPyramidImageFilter produces images which are of
   * different resolution and different pixel spacing than its input image.
   * As such, MultiResolutionPyramidImageFilter needs to provide an
   * implementation for GenerateOutputInformation() in order to inform the
   * pipeline execution model.  The original documentation of this method is
   * below.  \sa ProcessObject::GenerateOutputInformaton() */
  virtual void GenerateOutputInformation();

  /** Given one output whose requested region has been set, this method sets
   * the requested region for the remaining output images.  The original
   * documentation of this method is below.  \sa
   * ProcessObject::GenerateOutputRequestedRegion(); */
  virtual void GenerateOutputRequestedRegion(itk::DataObject *output);

  /** MultiResolutionPyramidImageFilter requires a larger input requested
   * region than the output requested regions to accomdate the shrinkage and
   * smoothing operations. As such, MultiResolutionPyramidImageFilter needs
   * to provide an implementation for GenerateInputRequestedRegion().  The
   * original documentation of this method is below.  \sa
   * ProcessObject::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion();

  itkSetMacro(MaximumError,double);
  itkGetConstReferenceMacro(MaximumError,double);

  itkSetMacro(UseShrinkImageFilter,bool);
  itkGetConstMacro(UseShrinkImageFilter,bool);
  itkBooleanMacro(UseShrinkImageFilter);
  

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(SameDimensionCheck,
    (itk::Concept::SameDimension<ImageDimension, OutputImageDimension>));
  itkConceptMacro(OutputHasNumericTraitsCheck,
    (itk::Concept::HasNumericTraits<typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  SpatioTemporalMultiResolutionPyramidImageFilter();
  ~SpatioTemporalMultiResolutionPyramidImageFilter() {};
  void PrintSelf(std::ostream&os, itk::Indent indent) const;

  /** Generate the output data. */
  void GenerateData();

  double          m_MaximumError; 
  unsigned int    m_NumberOfLevels;
  ScheduleType    m_Schedule;
  bool            m_UseShrinkImageFilter;

private:
  SpatioTemporalMultiResolutionPyramidImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
};


} // namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkSpatioTemporalMultiResolutionPyramidImageFilter.txx"
#endif

#endif
