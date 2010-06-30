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

#ifndef CLITKEXTRACTMEDIASTINUMFILTER_H
#define CLITKEXTRACTMEDIASTINUMFILTER_H

#include "clitkFilterBase.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Try to extract the mediastinum part of a thorax CT.
    Inputs : 
    - Patient label image
    - Lungs label image
    - Bones label image
  */
  //--------------------------------------------------------------------
  
  template <class TImageType>
  class ITK_EXPORT ExtractMediastinumFilter: 
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<TImageType, TImageType> 
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<TImageType, TImageType> Superclass;
    typedef ExtractMediastinumFilter            Self;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractMediastinumFilter, InPlaceImageFilter);
    FILTERBASE_INIT;

    /** Some convenient typedefs. */
    typedef TImageType                       ImageType;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   ImageRegionType; 
    typedef typename ImageType::PixelType    ImagePixelType; 
    typedef typename ImageType::SizeType     ImageSizeType; 
    typedef typename ImageType::IndexType    ImageIndexType; 
        
    /** Connect inputs */
    void SetInputPatientLabelImage(const TImageType * image, ImagePixelType bg=0);
    void SetInputLungLabelImage(const TImageType * image, ImagePixelType bg=0, 
                                 ImagePixelType fgLeftLung=1, ImagePixelType fgRightLung=2);
    void SetInputBonesLabelImage(const TImageType * image, ImagePixelType bg=0);
   
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, TImageType::ImageDimension);

    // Set all options at a time
    template<class ArgsInfoType>
      void SetArgsInfo(ArgsInfoType arg);
   
    // Background / Foreground
    itkSetMacro(BackgroundValuePatient, ImagePixelType);
    itkGetConstMacro(BackgroundValuePatient, ImagePixelType);
    GGO_DefineOption(patientBG, SetBackgroundValuePatient, ImagePixelType);
    
    itkSetMacro(BackgroundValueLung, ImagePixelType);
    itkGetConstMacro(BackgroundValueLung, ImagePixelType);
    GGO_DefineOption(lungBG, SetBackgroundValueLung, ImagePixelType);
    
    itkSetMacro(BackgroundValueBones, ImagePixelType);
    itkGetConstMacro(BackgroundValueBones, ImagePixelType);
    GGO_DefineOption(bonesBG, SetBackgroundValueBones, ImagePixelType);
    
    itkGetConstMacro(BackgroundValue, ImagePixelType);
    itkGetConstMacro(ForegroundValue, ImagePixelType);

    itkSetMacro(ForegroundValueLeftLung, ImagePixelType);
    itkGetConstMacro(ForegroundValueLeftLung, ImagePixelType);
    GGO_DefineOption(lungLeft, SetForegroundValueLeftLung, ImagePixelType);
    
    itkSetMacro(ForegroundValueRightLung, ImagePixelType);
    itkGetConstMacro(ForegroundValueRightLung, ImagePixelType);
    GGO_DefineOption(lungRight, SetForegroundValueRightLung, ImagePixelType);
    
    itkSetMacro(IntermediateSpacing, double);
    itkGetConstMacro(IntermediateSpacing, double);
    GGO_DefineOption(spacing, SetIntermediateSpacing, double);

    itkSetMacro(FuzzyThreshold1, double);
    itkGetConstMacro(FuzzyThreshold1, double);
    GGO_DefineOption(fuzzy1, SetFuzzyThreshold1, double);

    itkSetMacro(FuzzyThreshold2, double);
    itkGetConstMacro(FuzzyThreshold2, double);
    GGO_DefineOption(fuzzy2, SetFuzzyThreshold2, double);

  protected:
    ExtractMediastinumFilter();
    virtual ~ExtractMediastinumFilter() {}
    
    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();
       
    itkSetMacro(BackgroundValue, ImagePixelType);
    itkSetMacro(ForegroundValue, ImagePixelType);
    
    ImagePixelType m_BackgroundValuePatient;
    ImagePixelType m_BackgroundValueLung;
    ImagePixelType m_BackgroundValueBones;
    ImagePixelType m_ForegroundValueLeftLung;
    ImagePixelType m_ForegroundValueRightLung;

    ImagePixelType m_BackgroundValue;
    ImagePixelType m_ForegroundValue;
    
    double m_IntermediateSpacing;
    double m_FuzzyThreshold1;
    double m_FuzzyThreshold2;
    
  private:
    ExtractMediastinumFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractMediastinumFilter.txx"
#endif

#endif
