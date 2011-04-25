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
#ifndef __clitkMultiResolutionPDEDeformableRegistration_h
#define __clitkMultiResolutionPDEDeformableRegistration_h
#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkPDEDeformableRegistrationFilter.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkVectorResampleImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include <vector>

namespace clitk
{

template <class TFixedImage, class TMovingImage, class TDeformationField, class  TRealType = float>
class ITK_EXPORT MultiResolutionPDEDeformableRegistration :
    public itk::ImageToImageFilter <TDeformationField, TDeformationField>
{
public:
  /** Standard class typedefs */
  typedef MultiResolutionPDEDeformableRegistration Self;
  typedef itk::ImageToImageFilter<TDeformationField, TDeformationField>  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( MultiResolutionPDEDeformableRegistration, 
                ImageToImageFilter );

  /** Fixed image type. */
  typedef TFixedImage FixedImageType;
  typedef typename FixedImageType::Pointer FixedImagePointer;
  typedef typename FixedImageType::ConstPointer FixedImageConstPointer;

  /** Moving image type. */
  typedef TMovingImage MovingImageType;
  typedef typename MovingImageType::Pointer MovingImagePointer;
  typedef typename MovingImageType::ConstPointer MovingImageConstPointer;

  /** Deformation field image type. */
  typedef TDeformationField DeformationFieldType;
  typedef typename DeformationFieldType::Pointer DeformationFieldPointer;

  /** ImageDimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      FixedImageType::ImageDimension);

  /** Internal float image type. */
  typedef itk::Image<TRealType,itkGetStaticConstMacro(ImageDimension)> FloatImageType;

  /** The internal registration type. */
  typedef itk::PDEDeformableRegistrationFilter<
    FloatImageType, FloatImageType, DeformationFieldType > RegistrationType;
  typedef typename RegistrationType::Pointer RegistrationPointer;

  /** The default registration type. */
  typedef itk::DemonsRegistrationFilter<
    FloatImageType, FloatImageType, DeformationFieldType > DefaultRegistrationType;

  /** The fixed multi-resolution image pyramid type. */
  typedef itk::MultiResolutionPyramidImageFilter<
    FixedImageType, FloatImageType > FixedImagePyramidType;
  typedef typename FixedImagePyramidType::Pointer FixedImagePyramidPointer;

  /** The moving multi-resolution image pyramid type. */
typedef itk::MultiResolutionPyramidImageFilter<
    MovingImageType, FloatImageType > MovingImagePyramidType;
  typedef typename MovingImagePyramidType::Pointer MovingImagePyramidPointer;
   
  /** The deformation field expander type. */
typedef itk::VectorResampleImageFilter<
    DeformationFieldType, DeformationFieldType > FieldExpanderType;
  typedef typename FieldExpanderType::Pointer  FieldExpanderPointer;

  /** Set the fixed image. */
  virtual void SetFixedImage( const FixedImageType * ptr );

  /** Get the fixed image. */
  const FixedImageType * GetFixedImage(void) const;

  /** Set the moving image. */
  virtual void SetMovingImage( const MovingImageType * ptr );

  /** Get the moving image. */
  const MovingImageType * GetMovingImage(void) const;

  /** Set initial deformation field to be used as is (no smoothing, no
   *  subsampling at the coarsest level of the pyramid. */
  virtual void SetInitialDeformationField( DeformationFieldType * ptr )
  {
    this->m_InitialDeformationField=ptr;
  }

  /** Set initial deformation field. No assumption is made on the
   *  input. It will therefore be smoothed and resampled to match the
   *  images characteristics at the coarsest level of the pyramid. */
  virtual void SetArbitraryInitialDeformationField( DeformationFieldType * ptr )
  {
    this->SetInput( ptr ); 
  }
  
  /** Get output deformation field. */
  const DeformationFieldType * GetDeformationField(void)
  { return this->GetOutput(); }

  /** Get the number of valid inputs.  For
   * MultiResolutionPDEDeformableRegistration, this checks whether the
   * fixed and moving images have been set. While
   * MultiResolutionPDEDeformableRegistration can take a third input
   * as an initial deformation field, this input is not a required input.
   */
  virtual std::vector<itk::SmartPointer<itk::DataObject> >::size_type GetNumberOfValidRequiredInputs() const;

  /** Set the internal registrator. */
  itkSetObjectMacro( RegistrationFilter, RegistrationType );

  /** Get the internal registrator. */
  itkGetObjectMacro( RegistrationFilter, RegistrationType );
  
  /** Set the fixed image pyramid. */
  itkSetObjectMacro( FixedImagePyramid, FixedImagePyramidType );

  /** Get the fixed image pyramid. */
  itkGetObjectMacro( FixedImagePyramid, FixedImagePyramidType );

  /** Set the moving image pyramid. */
  itkSetObjectMacro( MovingImagePyramid, MovingImagePyramidType );

  /** Get the moving image pyramid. */
  itkGetObjectMacro( MovingImagePyramid, MovingImagePyramidType );

  /** Set number of multi-resolution levels. */
  virtual void SetNumberOfLevels( unsigned int num );

  /** Get number of multi-resolution levels. */
  itkGetConstReferenceMacro( NumberOfLevels, unsigned int );

  /** Get the current resolution level being processed. */
  itkGetConstReferenceMacro( CurrentLevel, unsigned int );

  /** Set number of iterations per multi-resolution levels. */
  itkSetVectorMacro( NumberOfIterations, unsigned int, m_NumberOfLevels );

  /** Set the moving image pyramid. */
  itkSetObjectMacro( FieldExpander, FieldExpanderType );

  /** Get the moving image pyramid. */
  itkGetObjectMacro( FieldExpander, FieldExpanderType );

  /** Get number of iterations per multi-resolution levels. */
  virtual const unsigned int * GetNumberOfIterations() const
  { return &(m_NumberOfIterations[0]); }

  /** Stop the registration after the current iteration. */
  virtual void StopRegistration();

protected:
  MultiResolutionPDEDeformableRegistration();
  ~MultiResolutionPDEDeformableRegistration() {}
void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Generate output data by performing the registration
   * at each resolution level. */
  virtual void GenerateData();

  /** The current implementation of this class does not support
   * streaming. As such it requires the largest possible region
   * for the moving, fixed and input deformation field. */
  virtual void GenerateInputRequestedRegion();

  /** By default, the output deformation field has the same
   * spacing, origin and LargestPossibleRegion as the input/initial
   * deformation field.
   *
   * If the initial deformation field is not set, the output
   * information is copied from the fixed image. */
  virtual void GenerateOutputInformation();

  /** The current implementation of this class does not supprot
   * streaming. As such it produces the output for the largest
   * possible region. */
  virtual void EnlargeOutputRequestedRegion( itk::DataObject *ptr );

  /** This method returns true to indicate that the registration should
   * terminate at the current resolution level. */
  virtual bool Halt();

private:
  MultiResolutionPDEDeformableRegistration(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  RegistrationPointer        m_RegistrationFilter;
  FixedImagePyramidPointer   m_FixedImagePyramid;
  MovingImagePyramidPointer  m_MovingImagePyramid;
  FieldExpanderPointer       m_FieldExpander;
  DeformationFieldPointer    m_InitialDeformationField;

  unsigned int               m_NumberOfLevels;
  unsigned int               m_CurrentLevel;
  std::vector<unsigned int>  m_NumberOfIterations;

  /** Flag to indicate user stop registration request. */
  bool                      m_StopRegistrationFlag;

};


} // end namespace clitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkMultiResolutionPDEDeformableRegistration.txx"
#endif


#endif
