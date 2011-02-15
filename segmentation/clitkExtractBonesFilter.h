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

#ifndef CLITKEXTRACTBONESSFILTER_H
#define CLITKEXTRACTBONESSFILTER_H

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
    Extract bony anatomy through thresholding and connected component
    labelling.
  */
  //--------------------------------------------------------------------
  
  template <class TInputImageType>
  class ITK_EXPORT ExtractBonesFilter: 
    public virtual clitk::FilterBase, 
    public clitk::FilterWithAnatomicalFeatureDatabaseManagement,
    public itk::ImageToImageFilter<TInputImageType, 
                                   itk::Image<uchar, TInputImageType::ImageDimension> > 
  {
    
  public:
    /** Standard class typedefs. */
    typedef itk::Image<uchar, TInputImageType::ImageDimension>      MaskImageType;
    typedef ExtractBonesFilter                                      Self;
    typedef itk::ImageToImageFilter<TInputImageType, MaskImageType> Superclass;
    typedef itk::SmartPointer<Self>                                 Pointer;
    typedef itk::SmartPointer<const Self>                           ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractBonesFilter, ImageToImageFilter);
    FILTERBASE_INIT;

    /** Some convenient typedefs */
    typedef TInputImageType                       InputImageType;
    typedef typename InputImageType::ConstPointer InputImageConstPointer;
    typedef typename InputImageType::Pointer      InputImagePointer;
    typedef typename InputImageType::RegionType   InputImageRegionType; 
    typedef typename InputImageType::PixelType    InputImagePixelType; 
    typedef typename InputImageType::SizeType     InputImageSizeType; 
    typedef typename InputImageType::IndexType    InputImageIndexType; 
        
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
 
    // Background / Foreground
    itkGetConstMacro(BackgroundValue, MaskImagePixelType);
    itkGetConstMacro(ForegroundValue, MaskImagePixelType);

    itkSetMacro(MinimalComponentSize, int);
    itkGetConstMacro(MinimalComponentSize, int);
    
    // Output filename  (for AFBD)
    itkSetMacro(OutputBonesFilename, std::string);
    itkGetMacro(OutputBonesFilename, std::string);

    // Step 0
    itkBooleanMacro(InitialSmoothing);
    itkSetMacro(InitialSmoothing, bool);
    itkGetMacro(InitialSmoothing, bool);

    itkSetMacro(SmoothingConductanceParameter, double);
    itkGetConstMacro(SmoothingConductanceParameter, double);
    
    itkSetMacro(SmoothingNumberOfIterations, int);
    itkGetConstMacro(SmoothingNumberOfIterations, int);

    itkSetMacro(SmoothingTimeStep, double);
    itkGetConstMacro(SmoothingTimeStep, double);

    itkSetMacro(SmoothingUseImageSpacing, bool);
    itkGetConstMacro(SmoothingUseImageSpacing, bool);
    itkBooleanMacro(SmoothingUseImageSpacing);

    // Step 1 
    itkSetMacro(UpperThreshold1, InputImagePixelType);
    itkGetMacro(UpperThreshold1, InputImagePixelType);

    itkSetMacro(LowerThreshold1, InputImagePixelType);
    itkGetMacro(LowerThreshold1, InputImagePixelType);

    itkSetMacro(FullConnectivity, bool);
    itkGetConstMacro(FullConnectivity, bool);
    itkBooleanMacro(FullConnectivity);

    // Step 2 
    itkSetMacro(UpperThreshold2, InputImagePixelType);
    itkGetMacro(UpperThreshold2, InputImagePixelType);

    itkSetMacro(LowerThreshold2, InputImagePixelType);
    itkGetMacro(LowerThreshold2, InputImagePixelType);

    itkSetMacro(Radius2, InputImageSizeType);
    itkGetConstMacro(Radius2, InputImageSizeType);

    itkSetMacro(SampleRate2, int);
    itkGetConstMacro(SampleRate2, int);

    // Step fill holes
    itkSetMacro(FillHoles, bool);
    itkGetConstMacro(FillHoles, bool);
    itkBooleanMacro(FillHoles);

    // Step Auto Crop
    itkSetMacro(AutoCrop, bool);
    itkGetConstMacro(AutoCrop, bool);
    itkBooleanMacro(AutoCrop);

  protected:
    ExtractBonesFilter();
    virtual ~ExtractBonesFilter() {}
    
    // Global options
    itkSetMacro(BackgroundValue, MaskImagePixelType);
    itkSetMacro(ForegroundValue, MaskImagePixelType);
    MaskImagePixelType m_BackgroundValue;
    MaskImagePixelType m_ForegroundValue;
    bool m_AutoCrop;

    // Step 0 : Initial Filtering
    bool m_InitialSmoothing;
    double m_SmoothingConductanceParameter;
    int m_SmoothingNumberOfIterations;
    double m_SmoothingTimeStep; 
    bool m_SmoothingUseImageSpacing;

    // Step 1
    InputImagePixelType m_UpperThreshold1;
    InputImagePixelType m_LowerThreshold1;
    int m_MinimalComponentSize;
    bool m_FullConnectivity;

    // Step 2
    InputImagePixelType m_UpperThreshold2;
    InputImagePixelType m_LowerThreshold2;
    InputImageSizeType m_Radius2;
    int m_SampleRate2;
    
    // Step 
    bool m_FillHoles;    
    InputImageSizeType m_FillHolesDirections;

    virtual void GenerateOutputInformation();
    virtual void GenerateData();

    // Steps
    void RemoveAir();
    void FindTrachea();
    void ExtractBones();
    void RemoveTrachea();
    void BonesSeparation();
    std::string m_OutputBonesFilename;
    InputImageConstPointer input;
    InputImagePointer filtered_input;
    MaskImageConstPointer patient;
    InputImagePointer working_input;
    typename InternalImageType::Pointer working_image;  
    typename InternalImageType::Pointer trachea;
    typename InternalImageType::Pointer output;
        
  private:
    ExtractBonesFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractBonesFilter.txx"
#endif

#endif
