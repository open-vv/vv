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

#ifndef CLITKEXTRACTLUNGSFILTER_H
#define CLITKEXTRACTLUNGSFILTER_H

// clitk 
#include "clitkFilterBase.h"
#include "clitkDecomposeAndReconstructImageFilter.h"
#include "clitkExplosionControlledThresholdConnectedImageFilter.h"
#include "clitkSegmentationUtils.h"
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"

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
  
  //--------------------------------------------------------------------
  template <class TImageType>
  class ITK_EXPORT ExtractLungFilter: 
    public virtual clitk::FilterBase, 
    public clitk::FilterWithAnatomicalFeatureDatabaseManagement,
    public itk::ImageToImageFilter<TImageType, itk::Image<uchar, TImageType::ImageDimension> > 
  {
    
  public:
    /** Standard class typedefs. */
    typedef itk::Image<uchar, TImageType::ImageDimension> MaskImageType;
    typedef itk::ImageToImageFilter<TImageType, MaskImageType> Superclass;
    typedef ExtractLungFilter              Self;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractLungFilter, ImageToImageFilter);
    FILTERBASE_INIT;

    /** Some convenient typedefs */
    typedef TImageType                       ImageType;
    typedef typename ImageType::ConstPointer InputImageConstPointer;
    typedef typename ImageType::Pointer      InputImagePointer;
    typedef typename ImageType::RegionType   InputImageRegionType; 
    typedef typename ImageType::PixelType    InputImagePixelType; 
    typedef typename ImageType::SizeType     InputImageSizeType; 
    typedef typename ImageType::IndexType    InputImageIndexType; 
    typedef typename ImageType::PointType    InputImagePointType; 
        
    typedef typename MaskImageType::ConstPointer MaskImageConstPointer;
    typedef typename MaskImageType::Pointer      MaskImagePointer;
    typedef typename MaskImageType::RegionType   MaskImageRegionType; 
    typedef typename MaskImageType::PixelType    MaskImagePixelType; 
    typedef typename MaskImageType::SizeType     MaskImageSizeType; 
    typedef typename MaskImageType::IndexType    MaskImageIndexType; 
    typedef typename MaskImageType::PointType    MaskImagePointType; 

    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    //    typedef int InternalPixelType;
    typedef uchar InternalPixelType;
    typedef itk::Image<InternalPixelType, ImageType::ImageDimension> InternalImageType;
    typedef typename InternalImageType::Pointer                      InternalImagePointer;
    typedef typename InternalImageType::IndexType                    InternalIndexType;
    typedef LabelizeParameters<InternalPixelType>                    LabelParamType;
    
    /** Connect inputs */
    void SetInput(const ImageType * image);
    itkSetMacro(PatientMaskBackgroundValue, MaskImagePixelType);
    itkGetConstMacro(PatientMaskBackgroundValue, MaskImagePixelType);

    // Output filename  (for AFBD)
    itkSetMacro(OutputLungFilename, std::string);
    itkGetMacro(OutputLungFilename, std::string);

    itkSetMacro(OutputTracheaFilename, std::string);
    itkGetMacro(OutputTracheaFilename, std::string);

    // Get output (only availabe after update !)
    typename MaskImageType::Pointer GetTracheaImage() { return trachea; }

    // Background / Foreground
    itkGetConstMacro(BackgroundValue, MaskImagePixelType);
    itkGetConstMacro(ForegroundValue, MaskImagePixelType);

    // For common segmentation processes
    itkSetMacro(MinimalComponentSize, int);
    itkGetConstMacro(MinimalComponentSize, int);

    // Step 1 options RemoveAir
    itkSetMacro(UpperThreshold, InputImagePixelType);
    itkGetConstMacro(UpperThreshold, InputImagePixelType);

    itkSetMacro(NumberOfSlicesToSkipBeforeSearchingSeed, int);
    itkGetConstMacro(NumberOfSlicesToSkipBeforeSearchingSeed, int);
    
    itkSetMacro(LowerThreshold, InputImagePixelType);
    itkGetConstMacro(LowerThreshold, InputImagePixelType);
    itkSetMacro(UseLowerThreshold, bool);
    itkGetConstMacro(UseLowerThreshold, bool);
    itkBooleanMacro(UseLowerThreshold);

    void SetLabelizeParameters1(LabelParamType * a) { m_LabelizeParameters1 = a; }
    itkGetConstMacro(LabelizeParameters1, LabelParamType*);

    // Step 2 options FindTrachea
    itkSetMacro(UpperThresholdForTrachea, InputImagePixelType);
    itkGetConstMacro(UpperThresholdForTrachea, InputImagePixelType);

    itkSetMacro(MultiplierForTrachea, double);
    itkGetConstMacro(MultiplierForTrachea, double);

    itkSetMacro(ThresholdStepSizeForTrachea, InputImagePixelType);
    itkGetConstMacro(ThresholdStepSizeForTrachea, InputImagePixelType);

    void AddSeed(InternalIndexType s);
    std::vector<InternalIndexType> & GetSeeds() { return  m_Seeds; }

    itkSetMacro(TracheaVolumeMustBeCheckedFlag, bool);
    itkGetConstMacro(TracheaVolumeMustBeCheckedFlag, bool);
    itkBooleanMacro(TracheaVolumeMustBeCheckedFlag);    

    itkSetMacro(VerboseRegionGrowingFlag, bool);
    itkGetConstMacro(VerboseRegionGrowingFlag, bool);
    itkBooleanMacro(VerboseRegionGrowingFlag);    

    // Step 3 options ExtractLung
    itkSetMacro(NumberOfHistogramBins, int);
    itkGetConstMacro(NumberOfHistogramBins, int);

    void SetLabelizeParameters2(LabelParamType* a) { m_LabelizeParameters2 = a; }
    itkGetConstMacro(LabelizeParameters2, LabelParamType*);

    // Step 4 options RemoveTrachea
    itkSetMacro(RadiusForTrachea, int);
    itkGetConstMacro(RadiusForTrachea, int);
    
    void SetLabelizeParameters3(LabelParamType * a) { m_LabelizeParameters3 = a; }
    itkGetConstMacro(LabelizeParameters3, LabelParamType*);

    // Step 5 final openclose
    itkSetMacro(OpenCloseFlag, bool);
    itkGetConstMacro(OpenCloseFlag, bool);
    itkBooleanMacro(OpenCloseFlag);

    itkSetMacro(OpenCloseRadius, int);
    itkGetConstMacro(OpenCloseRadius, int);
    
    // Step 6 fill holes
    itkSetMacro(FillHolesFlag, bool);
    itkGetConstMacro(FillHolesFlag, bool);
    itkBooleanMacro(FillHolesFlag);

    // Separate lungs
    itkSetMacro(SeparateLungsFlag, bool);
    itkGetConstMacro(SeparateLungsFlag, bool);
    itkBooleanMacro(SeparateLungsFlag);

    // Step Auto Crop
    itkSetMacro(AutoCrop, bool);
    itkGetConstMacro(AutoCrop, bool);
    itkBooleanMacro(AutoCrop);

  protected:
    ExtractLungFilter();
    virtual ~ExtractLungFilter() {}

    // Main members
    InputImageConstPointer input;
    MaskImagePointer patient;
    InputImagePointer working_input;
    std::string m_OutputLungFilename;
    std::string m_OutputTracheaFilename;
    MaskImagePointer working_mask;  
    MaskImagePointer trachea;
    unsigned int m_MaxSeedNumber;

    // Global options
    itkSetMacro(BackgroundValue, MaskImagePixelType);
    itkSetMacro(ForegroundValue, MaskImagePixelType);
    MaskImagePixelType m_PatientMaskBackgroundValue;
    MaskImagePixelType m_BackgroundValue;
    MaskImagePixelType m_ForegroundValue;
    int m_MinimalComponentSize;
    bool m_AutoCrop;

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
    int m_NumberOfSlicesToSkipBeforeSearchingSeed;
    bool m_TracheaVolumeMustBeCheckedFlag;
    bool m_VerboseRegionGrowingFlag;

    // Step 3
    int m_NumberOfHistogramBins;
    LabelParamType* m_LabelizeParameters2;

    // Step 4
    int m_RadiusForTrachea;
    LabelParamType* m_LabelizeParameters3;

    // Step 5
    bool m_OpenCloseFlag;    
    int m_OpenCloseRadius;

    // Step 6
    bool m_FillHolesFlag;    
    InputImageSizeType m_FillHolesDirections;

    bool m_SeparateLungsFlag;
    
    // Main functions
    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();
    
    // Functions for trachea extraction
    bool SearchForTracheaSeed(int skip);
    bool SearchForTracheaSeed2(int numberOfSlices);
    void SearchForTrachea();
    void TracheaRegionGrowing();
    double ComputeTracheaVolume();

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
