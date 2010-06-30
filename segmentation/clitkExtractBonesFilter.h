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
#include "clitkSegmentationFunctions.h"

// itk
#include "itkStatisticsImageFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Extract bony anatomy through thresholding and connected component labelling.
  */
  //--------------------------------------------------------------------
  
  template <class TInputImageType, class TOutputImageType>
  class ITK_EXPORT ExtractBonesFilter: 
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<TInputImageType, TOutputImageType> 
  {
    
  public:
    /** Standard class typedefs. */
    typedef ExtractBonesFilter              Self;
    typedef itk::ImageToImageFilter<TInputImageType, TOutputImageType> Superclass;
    typedef itk::SmartPointer<Self>         Pointer;
    typedef itk::SmartPointer<const Self>   ConstPointer;
    
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
        
    typedef TOutputImageType                       OutputImageType;
    typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
    typedef typename OutputImageType::Pointer      OutputImagePointer;
    typedef typename OutputImageType::RegionType   OutputImageRegionType; 
    typedef typename OutputImageType::PixelType    OutputImagePixelType; 
    typedef typename OutputImageType::SizeType     OutputImageSizeType; 
    typedef typename OutputImageType::IndexType    OutputImageIndexType; 

    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    typedef int InternalPixelType;
    typedef itk::Image<InternalPixelType, InputImageType::ImageDimension> InternalImageType;
    typedef typename InternalImageType::Pointer                           InternalImagePointer;
    typedef typename InternalImageType::IndexType                         InternalIndexType;
    typedef LabelizeParameters<InternalPixelType>                         LabelParamType;
    
    /** Connect inputs */
    void SetInput(const InputImageType * image);
 
    // Set all options at a time
    template<class ArgsInfoType>
      void SetArgsInfo(ArgsInfoType arg);

    // Background / Foreground
    itkGetConstMacro(BackgroundValue, OutputImagePixelType);
    itkGetConstMacro(ForegroundValue, OutputImagePixelType);

    itkSetMacro(MinimalComponentSize, int);
    itkGetConstMacro(MinimalComponentSize, int);
    GGO_DefineOption(minSize, SetMinimalComponentSize, int);

    // Step 1 
    itkSetMacro(UpperThreshold1, InputImagePixelType);
    itkGetMacro(UpperThreshold1, InputImagePixelType);
    GGO_DefineOption(upper1, SetUpperThreshold1, InputImagePixelType);

    itkSetMacro(LowerThreshold1, InputImagePixelType);
    itkGetMacro(LowerThreshold1, InputImagePixelType);
    GGO_DefineOption(lower1, SetLowerThreshold1, InputImagePixelType);

    itkSetMacro(FullConnectivity, bool);
    itkGetConstMacro(FullConnectivity, bool);
    itkBooleanMacro(FullConnectivity);
    GGO_DefineOption_Flag(full, SetFullConnectivity);

    // Step 2 
    itkSetMacro(UpperThreshold2, InputImagePixelType);
    itkGetMacro(UpperThreshold2, InputImagePixelType);
    GGO_DefineOption(upper2, SetUpperThreshold2, InputImagePixelType);

    itkSetMacro(LowerThreshold2, InputImagePixelType);
    itkGetMacro(LowerThreshold2, InputImagePixelType);
    GGO_DefineOption(lower2, SetLowerThreshold2, InputImagePixelType);

    itkSetMacro(Radius2, InputImageSizeType);
    itkGetConstMacro(Radius2, InputImageSizeType);
    GGO_DefineOption_Vector(radius2, SetRadius2, InputImageSizeType, ImageDimension, true);

    itkSetMacro(SampleRate2, int);
    itkGetConstMacro(SampleRate2, int);
    GGO_DefineOption(sampleRate2, SetSampleRate2, int);

    // Final Step
    itkSetMacro(AutoCrop, bool);
    itkGetConstMacro(AutoCrop, bool);
    itkBooleanMacro(AutoCrop);
    GGO_DefineOption_Flag(autoCrop, SetAutoCrop);

  protected:
    ExtractBonesFilter();
    virtual ~ExtractBonesFilter() {}
    
    // Global options
    itkSetMacro(BackgroundValue, OutputImagePixelType);
    itkSetMacro(ForegroundValue, OutputImagePixelType);
    OutputImagePixelType m_BackgroundValue;
    OutputImagePixelType m_ForegroundValue;

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
    
    bool m_AutoCrop;

    virtual void GenerateOutputInformation();
    virtual void GenerateData();

    // Steps
    void RemoveAir();
    void FindTrachea();
    void ExtractBones();
    void RemoveTrachea();
    void BonesSeparation();
    InputImageConstPointer input;
    OutputImageConstPointer patient;
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
