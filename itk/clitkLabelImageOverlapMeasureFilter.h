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

#ifndef CLITKLABELIMAGEOVERLAPMEASUREFILTER_H
#define CLITKLABELIMAGEOVERLAPMEASUREFILTER_H

// clitk
#include "clitkFilterBase.h"
#include "clitkCropLikeImageFilter.h"
#include "clitkSegmentationUtils.h"
// #include "itkLabelOverlapMeasuresImageFilter.h"

// itk
#include <itkImageToImageFilter.h>
#include <itkLabelStatisticsImageFilter.h>

#include <iomanip>

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Analyze the relative position of a Target (mask image) according
    to some Object, in a given Support. Indicate the main important
    position of this Target according the Object. 
  */
  //--------------------------------------------------------------------
  
  template <class ImageType>
  class LabelImageOverlapMeasureFilter:
    public virtual FilterBase,
    public itk::ImageToImageFilter<ImageType, ImageType>
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<ImageType, ImageType>      Superclass;
    typedef LabelImageOverlapMeasureFilter<ImageType>          Self;
    typedef itk::SmartPointer<Self>                            Pointer;
    typedef itk::SmartPointer<const Self>                      ConstPointer;
       
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(LabelImageOverlapMeasureFilter, ImageToImageFilter);

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
    FILTERBASE_INIT;
 
    // Options
    itkGetConstMacro(BackgroundValue, PixelType);
    itkSetMacro(BackgroundValue, PixelType);

    itkGetConstMacro(Label1, PixelType);
    itkSetMacro(Label1, PixelType);

    itkGetConstMacro(Label2, PixelType);
    itkSetMacro(Label2, PixelType);

    // I dont want to verify inputs information
    virtual void VerifyInputInformation() { }
    
   protected:
    LabelImageOverlapMeasureFilter();
    virtual ~LabelImageOverlapMeasureFilter() {}
    
    PixelType m_BackgroundValue;
    PixelType m_Label1;
    PixelType m_Label2;
    ImagePointer m_Input1;
    ImagePointer m_Input2;

    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();

  private:
    LabelImageOverlapMeasureFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#include "clitkLabelImageOverlapMeasureFilter.txx"

#endif
