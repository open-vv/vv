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

#ifndef CLITKSLICEBYSLICERELATIVEPOSITIONFILTER_H
#define CLITKSLICEBYSLICERELATIVEPOSITIONFILTER_H

// clitk
#include "clitkFilterBase.h"
#include "clitkAddRelativePositionConstraintToLabelImageFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Perform Relative Position filtering in a slice by slice manner. 
  */
  //--------------------------------------------------------------------
  
  template <class ImageType>
  class ITK_EXPORT SliceBySliceRelativePositionFilter:
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<ImageType, ImageType> 
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<ImageType, ImageType>   Superclass;
    typedef SliceBySliceRelativePositionFilter              Self;
    typedef itk::SmartPointer<Self>                         Pointer;
    typedef itk::SmartPointer<const Self>                   ConstPointer;
       
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(SliceBySliceRelativePositionFilter, ImageToImageFilter);
    FILTERBASE_INIT;

    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    typedef itk::Image<float, ImageDimension> FloatImageType;

    /** Some convenient typedefs. */
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   RegionType; 
    typedef typename ImageType::PixelType    PixelType;
    typedef typename ImageType::SpacingType  SpacingType;
    typedef typename ImageType::SizeType     SizeType;
    typedef itk::Image<PixelType, ImageDimension-1> SliceType;
    typedef clitk::AddRelativePositionConstraintToLabelImageFilter<SliceType> RelPosFilterType;
    typedef typename RelPosFilterType::OrientationTypeEnumeration OrientationTypeEnumeration;
    
    /** Input : initial image and object */
    void SetInput(const ImageType * image);
    void SetInputObject(const ImageType * image);
    
    // Options
    itkGetConstMacro(Direction, int);
    itkSetMacro(Direction, int);
    itkGetConstMacro(ObjectBackgroundValue, PixelType);
    itkSetMacro(ObjectBackgroundValue, PixelType);

    itkSetMacro(OrientationType, OrientationTypeEnumeration);
    itkGetConstMacro(OrientationType, OrientationTypeEnumeration);
    itkGetConstMacro(ResampleBeforeRelativePositionFilter, bool);
    itkSetMacro(ResampleBeforeRelativePositionFilter, bool);
    itkBooleanMacro(ResampleBeforeRelativePositionFilter);
    itkGetConstMacro(IntermediateSpacing, double);
    itkSetMacro(IntermediateSpacing, double);
    itkGetConstMacro(FuzzyThreshold, double);
    itkSetMacro(FuzzyThreshold, double);

  protected:
    SliceBySliceRelativePositionFilter();
    virtual ~SliceBySliceRelativePositionFilter() {}
    
    int m_Direction;
    PixelType m_ObjectBackgroundValue;
    OrientationTypeEnumeration m_OrientationType;
    double m_IntermediateSpacing;
    double m_FuzzyThreshold;
    bool m_ResampleBeforeRelativePositionFilter;

    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();

    ImagePointer input;
    ImagePointer object;
    ImagePointer m_working_input;
    ImagePointer m_working_object;

  private:
    SliceBySliceRelativePositionFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkSliceBySliceRelativePositionFilter.txx"
#endif

#endif
