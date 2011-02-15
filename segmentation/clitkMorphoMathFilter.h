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

#ifndef CLITKMORPHOMATHFILTER_H
#define CLITKMORPHOMATHFILTER_H

// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkMorphoMath_ggo.h"
#include "clitkConditionalBinaryErodeImageFilter.h"
#include "clitkConditionalBinaryDilateImageFilter.h"

// itk include
#include <itkLightObject.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryMorphologicalClosingImageFilter.h>
#include <itkBinaryMorphologicalOpeningImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkCastImageFilter.h>

namespace clitk {

  //--------------------------------------------------------------------
  template<class ImageType>
  class ITK_EXPORT MorphoMathFilter: 
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<ImageType, ImageType> 
  {
    
  public: 
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<ImageType, ImageType> Superclass;
    typedef MorphoMathFilter<ImageType>                   Self;
    typedef itk::SmartPointer<Self>                       Pointer;
    typedef itk::SmartPointer<const Self>                 ConstPointer;

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
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, ImageType::ImageDimension> InternalImageType;
    
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    typedef itk::Image<float, ImageDimension> FloatImageType;

    /** Operation types */
    typedef enum { Erode     = 0, Dilate     = 1,
                   Close     = 2, Open       = 3, 
                   CondErode = 4, CondDilate = 5
    } OperationTypeEnumeration;
    
    /** Options */
    itkGetConstMacro(VerboseFlag, bool);
    itkSetMacro(VerboseFlag, bool);
    itkBooleanMacro(VerboseFlag);

    itkGetConstMacro(BackgroundValue, PixelType);
    itkSetMacro(BackgroundValue, PixelType);

    itkGetConstMacro(ForegroundValue, PixelType);
    itkSetMacro(ForegroundValue, PixelType);

    void SetOperationType(int type);
    
    itkGetConstMacro(RadiusInMM, PointType);
    void SetRadiusInMM(PointType & p);

    itkGetConstMacro(Radius, SizeType);
    void SetRadius(SizeType & r);

    itkGetConstMacro(BoundaryToForegroundFlag, bool);
    itkSetMacro(BoundaryToForegroundFlag, bool);
    itkBooleanMacro(BoundaryToForegroundFlag);
    
    itkGetConstMacro(ExtendSupportFlag, bool);
    itkSetMacro(ExtendSupportFlag, bool);
    itkBooleanMacro(ExtendSupportFlag);
    
  protected:
    MorphoMathFilter();
    virtual ~MorphoMathFilter();

    bool m_VerboseFlag;
    PixelType m_BackgroundValue;
    PixelType m_ForegroundValue;
    PointType m_RadiusInMM;
    SizeType  m_Radius;
    bool m_RadiusInMMIsSet;
    bool m_RadiusIsSet;
    OperationTypeEnumeration m_OperationType;
    bool m_BoundaryToForegroundFlag;
    bool m_ExtendSupportFlag;
    typename InternalImageType::Pointer input;

    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();

  private:
    MorphoMathFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
  };
  
} // end namespace clitk
#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkMorphoMathFilter.txx"
#endif
  
#endif //#define CLITKMORPHOMATHFILTER_H
