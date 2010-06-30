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

#ifndef CLITKEXTRACTLUNGSFILTER_H
#define CLITKEXTRACTLUNGSFILTER_H

// clitk 
#include "clitkFilterBase.h"
#include "clitkDecomposeAndReconstructImageFilter.h"
#include "clitkExplosionControlledThresholdConnectedImageFilter.h"
#include "clitkSegmentationFunctions.h"

// itk
#include "itkStatisticsImageFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Try to extract the Lung part of a thorax CT. Inspired by
    Rikxoort2009, Section IIA, MedPhys.

    - First, all air besides lungs and thrachea is removed, by
    removing the second largest label of the firstLabelImage and
    setting the remainder to 0HU . This modified input is optimally
    thresholded (Otsu1979).

    - Trachea and bronchi are grown from seeds in the top of the image
    by explosion controlled region growing, slightly dilated and
    removed from the second label image.

    - Left and right lung are separated (if necessary) by erosion and
    reconstructed by conditional dilation. 

    - TRACHEA is available at the end

    TODO ********** Remaining holes can be       filled afterwards (clitkFillMask).

  */
  //--------------------------------------------------------------------
  
  template <class TInputImageType, class TMaskImageType>
  class ITK_EXPORT ExtractLungFilter: 
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<TInputImageType, TMaskImageType> 
  {
    
  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<TInputImageType, TMaskImageType> Superclass;
    typedef ExtractLungFilter              Self;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractLungFilter, ImageToImageFilter);
    FILTERBASE_INIT;

    /** Some convenient typedefs */
    typedef TInputImageType                       InputImageType;
    typedef typename InputImageType::ConstPointer InputImageConstPointer;
    typedef typename InputImageType::Pointer      InputImagePointer;
    typedef typename InputImageType::RegionType   InputImageRegionType; 
    typedef typename InputImageType::PixelType    InputImagePixelType; 
    typedef typename InputImageType::SizeType     InputImageSizeType; 
    typedef typename InputImageType::IndexType    InputImageIndexType; 
        
    typedef TMaskImageType                       MaskImageType;
    typedef typename MaskImageType::ConstPointer MaskImageConstPointer;
    typedef typename MaskImageType::Pointer      MaskImagePointer;
    typedef typename MaskImageType::RegionType   MaskImageRegionType; 
    typedef typename MaskImageType::PixelType    MaskImagePixelType; 
    typedef typename MaskImageType::SizeType     MaskImageSizeType; 
    typedef typename MaskImageType::IndexType    MaskImageIndexType; 

    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    typedef int InternalPixelType;
    typedef itk::Image<InternalPixelType, InputImageType::ImageDimension> InternalImageType;
    typedef typename InternalImageType::Pointer                           InternalImagePointer;
    typedef typename InternalImageType::IndexType                         InternalIndexType;
    typedef LabelizeParameters<InternalPixelType>                         LabelParamType;
    
    /** Connect inputs */
    void SetInput(const InputImageType * image);
    void SetInputPatientMask(MaskImageType * mask, MaskImagePixelType BG);
    itkSetMacro(PatientMaskBackgroundValue, MaskImagePixelType);
    itkGetConstMacro(PatientMaskBackgroundValue, MaskImagePixelType);
    GGO_DefineOption(patientBG, SetPatientMaskBackgroundValue, MaskImagePixelType);

    // Set all options at a time
    template<class ArgsInfoType>
      void SetArgsInfo(ArgsInfoType arg);

    // Get output (only availabe after update !)
    typename MaskImageType::Pointer GetTracheaImage() { return trachea; }

    // Background / Foreground
    itkGetConstMacro(BackgroundValue, MaskImagePixelType);
    itkGetConstMacro(ForegroundValue, MaskImagePixelType);

    // For common segmentation processes
    itkSetMacro(MinimalComponentSize, int);
    itkGetConstMacro(MinimalComponentSize, int);
    GGO_DefineOption(minSize, SetMinimalComponentSize, int);

    // Step 1 options RemoveAir
    itkSetMacro(UpperThreshold, InputImagePixelType);
    itkGetConstMacro(UpperThreshold, InputImagePixelType);
    GGO_DefineOption(upper, SetUpperThreshold, InputImagePixelType);

    itkSetMacro(LowerThreshold, InputImagePixelType);
    itkGetConstMacro(LowerThreshold, InputImagePixelType);
    itkSetMacro(UseLowerThreshold, bool);
    itkGetConstMacro(UseLowerThreshold, bool);
    itkBooleanMacro(UseLowerThreshold);
    GGO_DefineOption_WithTest(lower, SetLowerThreshold, InputImagePixelType, UseLowerThreshold);

    void SetLabelizeParameters1(LabelParamType * a) { m_LabelizeParameters1 = a; }
    itkGetConstMacro(LabelizeParameters1, LabelParamType*);
    GGO_DefineOption_LabelParam(1, SetLabelizeParameters1, LabelParamType);

    // Step 2 options FindTrachea
    itkSetMacro(UpperThresholdForTrachea, InputImagePixelType);
    itkGetConstMacro(UpperThresholdForTrachea, InputImagePixelType);
    GGO_DefineOption(upperThresholdForTrachea, SetUpperThresholdForTrachea, InputImagePixelType);

    itkSetMacro(MultiplierForTrachea, double);
    itkGetConstMacro(MultiplierForTrachea, double);
    GGO_DefineOption(multiplierForTrachea, SetMultiplierForTrachea, double);

    itkSetMacro(ThresholdStepSizeForTrachea, InputImagePixelType);
    itkGetConstMacro(ThresholdStepSizeForTrachea, InputImagePixelType);
    GGO_DefineOption(thresholdStepSizeForTrachea, SetThresholdStepSizeForTrachea, InputImagePixelType);

    void AddSeed(InternalIndexType s);
    std::vector<InternalIndexType> & GetSeeds() { return  m_Seeds; }
    GGO_DefineOption_Vector(seed, AddSeed, InternalIndexType, InputImageType::ImageDimension, true);

    // Step 3 options ExtractLung
    itkSetMacro(NumberOfHistogramBins, int);
    itkGetConstMacro(NumberOfHistogramBins, int);
    GGO_DefineOption(bins, SetNumberOfHistogramBins, int);

    void SetLabelizeParameters2(LabelParamType* a) { m_LabelizeParameters2 = a; }
    itkGetConstMacro(LabelizeParameters2, LabelParamType*);
    GGO_DefineOption_LabelParam(2, SetLabelizeParameters2, LabelParamType);

    // Step 4 options RemoveTrachea
    itkSetMacro(RadiusForTrachea, int);
    itkGetConstMacro(RadiusForTrachea, int);
    GGO_DefineOption(radius, SetRadiusForTrachea, int);
    
    void SetLabelizeParameters3(LabelParamType * a) { m_LabelizeParameters3 = a; }
    itkGetConstMacro(LabelizeParameters3, LabelParamType*);
    GGO_DefineOption_LabelParam(3, SetLabelizeParameters3, LabelParamType);

    // Step 5 options LungSeparation
    //     itkSetMacro(FinalOpenClose, bool);
    //     itkGetConstMacro(FinalOpenClose, bool);
    //     itkBooleanMacro(FinalOpenClose);

 //    virtual void Update();

  protected:
    ExtractLungFilter();
    virtual ~ExtractLungFilter() {}
    
    // Global options
    itkSetMacro(BackgroundValue, MaskImagePixelType);
    itkSetMacro(ForegroundValue, MaskImagePixelType);
    MaskImagePixelType m_PatientMaskBackgroundValue;
    MaskImagePixelType m_BackgroundValue;
    MaskImagePixelType m_ForegroundValue;
    int m_MinimalComponentSize;

    // Step 1
    InputImagePixelType m_UpperThreshold;
    InputImagePixelType m_LowerThreshold;
    bool m_UseLowerThreshold;
    LabelParamType* m_LabelizeParameters1;

    // Step 2
    InputImagePixelType m_UpperThresholdForTrachea;
    InputImagePixelType m_ThresholdStepSizeForTrachea;
    double m_MultiplierForTrachea;
    std::vector<InternalIndexType> m_Seeds;

    // Step 3
    int m_NumberOfHistogramBins;
    LabelParamType* m_LabelizeParameters2;

    // Step 4
    int m_RadiusForTrachea;
    LabelParamType* m_LabelizeParameters3;

    // Step 5
    //     bool m_FinalOpenClose;
    
    virtual void GenerateOutputInformation();
    virtual void GenerateData();

    // Steps
    void RemoveAir();
    void FindTrachea();
    void ExtractLung();
    void RemoveTrachea();
    void LungSeparation();
    InputImageConstPointer input;
    MaskImageConstPointer patient;
    InputImagePointer working_input;
    typename InternalImageType::Pointer working_image;  
    typename InternalImageType::Pointer trachea_tmp;
    MaskImagePointer trachea;
        
  private:
    ExtractLungFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractLungFilter.txx"
#endif

#endif
