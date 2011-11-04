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

#ifndef CLITKADDRELATIVEPOSITIONCONSTRAINTTOLABELIMAGEFILTER_H
#define CLITKADDRELATIVEPOSITIONCONSTRAINTTOLABELIMAGEFILTER_H

// clitk
#include "clitkFilterBase.h"
#include "clitkCropLikeImageFilter.h"

// itk
#include <itkPasteImageFilter.h>

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
    typedef typename ImageType::IndexType    IndexType;
    typedef typename ImageType::PointType    PointType;
    
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
    void AddOrientationType(OrientationTypeEnumeration orientation);
    void AddOrientationTypeString(std::string s);
    void ClearOrientationType();
    void AddAnglesInRad(double a, double b);
    void AddAnglesInDeg(double a, double b);
    double GetAngle1InRad(int i) { return m_Angle1[i]; }
    double GetAngle2InRad(int i) { return m_Angle2[i]; }
    int GetNumberOfAngles();
    std::string GetOrientationTypeString(int i) { return m_OrientationTypeString[i]; }
    std::vector<std::string> & GetOrientationTypeString() { return m_OrientationTypeString; }

    itkGetConstMacro(IntermediateSpacingFlag, bool);
    itkSetMacro(IntermediateSpacingFlag, bool);
    itkBooleanMacro(IntermediateSpacingFlag);

    itkGetConstMacro(IntermediateSpacing, double);
    itkSetMacro(IntermediateSpacing, double);

    itkGetConstMacro(FuzzyThreshold, double);
    itkSetMacro(FuzzyThreshold, double);

    itkGetConstMacro(BackgroundValue, PixelType);
    itkSetMacro(BackgroundValue, PixelType);

    itkGetConstMacro(ObjectBackgroundValue, PixelType);
    itkSetMacro(ObjectBackgroundValue, PixelType);

    itkGetConstMacro(AutoCropFlag, bool);
    itkSetMacro(AutoCropFlag, bool);
    itkBooleanMacro(AutoCropFlag);

    itkGetConstMacro(InverseOrientationFlag, bool);
    itkSetMacro(InverseOrientationFlag, bool);
    itkBooleanMacro(InverseOrientationFlag);

    itkGetConstMacro(RemoveObjectFlag, bool);
    itkSetMacro(RemoveObjectFlag, bool);
    itkBooleanMacro(RemoveObjectFlag);

    itkGetConstMacro(CombineWithOrFlag, bool);
    itkSetMacro(CombineWithOrFlag, bool);
    itkBooleanMacro(CombineWithOrFlag);

    itkGetConstMacro(FuzzyMapOnlyFlag, bool);
    itkSetMacro(FuzzyMapOnlyFlag, bool);
    itkBooleanMacro(FuzzyMapOnlyFlag);

    typename FloatImageType::Pointer GetFuzzyMap() { return m_FuzzyMap; }

    // I dont want to verify inputs information
    virtual void VerifyInputInformation() { }
    
    // For debug
    void PrintOptions();

  protected:
    AddRelativePositionConstraintToLabelImageFilter();
    virtual ~AddRelativePositionConstraintToLabelImageFilter() {}
    
    std::vector<OrientationTypeEnumeration> m_OrientationType;
    std::vector<std::string> m_OrientationTypeString;
    double m_IntermediateSpacing;
    double m_FuzzyThreshold;
    PixelType m_BackgroundValue;
    PixelType m_ObjectBackgroundValue;
    std::vector<double> m_Angle1;
    std::vector<double> m_Angle2;
    bool m_IntermediateSpacingFlag;
    bool m_AutoCropFlag;
    bool m_InverseOrientationFlag;
    bool m_RemoveObjectFlag;
    bool m_CombineWithOrFlag;
    bool m_FuzzyMapOnlyFlag;

    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();

    typedef itk::PasteImageFilter<ImageType,ImageType> PasteFilterType;
    typename ImageType::Pointer working_image;
    typename ImageType::Pointer object_resampled;
    typename FloatImageType::Pointer relPos;
    typename FloatImageType::Pointer m_FuzzyMap;
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
