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

#ifndef CLITKADDRELATIVEPOSITIONCONSTRAINTTOLABELIMAGEFILTER_H
#define CLITKADDRELATIVEPOSITIONCONSTRAINTTOLABELIMAGEFILTER_H

// clitk
#include "clitkFilterBase.h"

// itk
#include "itkPasteImageFilter.h"

// itk ENST
#include "RelativePositionPropImageFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Let A be an initial label image.
    Let B be a label image with an object. 
    Let o be an orientation relatively to the B object (for example RightTo, AntTo, InferiorTo ...)

    This filter removes (=set background) from A all points that are
    not in the wanted o orientation. It uses downsampled version for
    faster processing, and (try to) take into account discretization
    problem. Uses [Bloch 1999].
  */
  //--------------------------------------------------------------------
  
  template <class ImageType>
  class ITK_EXPORT AddRelativePositionConstraintToLabelImageFilter:
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<ImageType, ImageType> 
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<ImageType, ImageType>      Superclass;
    typedef AddRelativePositionConstraintToLabelImageFilter    Self;
    typedef itk::SmartPointer<Self>                            Pointer;
    typedef itk::SmartPointer<const Self>                      ConstPointer;
       
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(AddRelativePositionConstraintToLabelImageFilter, ImageToImageFilter);
    FILTERBASE_INIT;

    /** Some convenient typedefs. */
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   RegionType; 
    typedef typename ImageType::PixelType    PixelType;
    typedef typename ImageType::SpacingType  SpacingType;
    typedef typename ImageType::SizeType     SizeType;
    
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    typedef itk::Image<float, ImageDimension> FloatImageType;

    /** Orientation types */
    typedef enum { RightTo = 0, LeftTo = 1,
                   AntTo = 2,   PostTo = 3, 
                   InfTo = 4,   SupTo = 5, Angle = 6
    } OrientationTypeEnumeration;

    /** Input : initial image and object */
    void SetInput(const ImageType * image);
    void SetInputObject(const ImageType * image);
    
    // Options
    void SetOrientationType(OrientationTypeEnumeration orientation);
    itkGetConstMacro(OrientationType, OrientationTypeEnumeration);

    void SetAngle1(double a);
    void SetAngle2(double a);
    itkGetConstMacro(Angle1, double);
    itkGetConstMacro(Angle2, double);

    itkGetConstMacro(ResampleBeforeRelativePositionFilter, bool);
    itkSetMacro(ResampleBeforeRelativePositionFilter, bool);
    itkBooleanMacro(ResampleBeforeRelativePositionFilter);

    itkGetConstMacro(IntermediateSpacing, double);
    itkSetMacro(IntermediateSpacing, double);

    itkGetConstMacro(FuzzyThreshold, double);
    itkSetMacro(FuzzyThreshold, double);

    itkGetConstMacro(BackgroundValue, PixelType);
    itkSetMacro(BackgroundValue, PixelType);

    itkGetConstMacro(ObjectBackgroundValue, PixelType);
    itkSetMacro(ObjectBackgroundValue, PixelType);

    itkGetConstMacro(AutoCrop, bool);
    itkSetMacro(AutoCrop, bool);
    itkBooleanMacro(AutoCrop);

  protected:
    AddRelativePositionConstraintToLabelImageFilter();
    virtual ~AddRelativePositionConstraintToLabelImageFilter() {}
    
    OrientationTypeEnumeration m_OrientationType;
    double m_IntermediateSpacing;
    double m_FuzzyThreshold;
    PixelType m_BackgroundValue;
    PixelType m_ObjectBackgroundValue;
    double m_Angle1;
    double m_Angle2;
    bool m_ResampleBeforeRelativePositionFilter;
    bool m_AutoCrop;

    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();

    typedef itk::PasteImageFilter<ImageType,ImageType> PadFilterType;
    typename ImageType::Pointer working_image;
    typename ImageType::Pointer object_resampled;
    typename FloatImageType::Pointer relPos;
    ImagePointer input;
    ImagePointer object;

  private:
    AddRelativePositionConstraintToLabelImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkAddRelativePositionConstraintToLabelImageFilter.txx"
#endif

#endif
