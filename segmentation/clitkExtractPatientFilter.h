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

#ifndef CLITKEXTRACTPATIENTFILTER_H
#define CLITKEXTRACTPATIENTFILTER_H

#include "clitkFilterBase.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Try to extract the Patient part of a thorax CT.  

    Prefer high resolution input and resample (NN) output at the end
    (like). Input is binarized using initial thresholds, connected
    components are labeled (firstLabel). The air label (1) is
    removed. The remaining is binarized and relabeled, patient should
    now be the principal label (secondLabel). Two mechanismes are
    provided to influence the label images. Crop to reduce
    connectivity (image is restored to original size), eg for
    SBF. Decomposition through ersion and reconstruction through
    dilation (slow), eg for Pulmo bellows. Choose which labels to keep
    from second Label image. Final mask is cleaned by opening and
    closing.

  */
  //--------------------------------------------------------------------
  
  template <class TInputImageType, class TOutputImageType>
  class ITK_EXPORT ExtractPatientFilter: 
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<TInputImageType, TOutputImageType> 
  {
  public:
    /** Standard class typedefs. */
    typedef ExtractPatientFilter            Self;
    typedef itk::ImageToImageFilter<TInputImageType, TOutputImageType> Superclass;
    typedef itk::SmartPointer<Self>         Pointer;
    typedef itk::SmartPointer<const Self>   ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractPatientFilter, ImageToImageFilter);
    FILTERBASE_INIT;

    /** Some convenient typedefs. */
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
    typedef itk::Image<InternalPixelType, ImageDimension> InternalImageType;
    typedef typename InternalImageType::SizeType          InternalImageSizeType;
    
    /** Connect inputs */
    void SetInput(const TInputImageType * image);

    // Set all options at a time
    template<class ArgsInfoType>
      void SetArgsInfo(ArgsInfoType arg);

    // Step 1
    itkSetMacro(UpperThreshold, InputImagePixelType);
    itkGetMacro(UpperThreshold, InputImagePixelType);
    GGO_DefineOption(upper, SetUpperThreshold, InputImagePixelType);

    itkSetMacro(LowerThreshold, InputImagePixelType);
    itkGetMacro(LowerThreshold, InputImagePixelType);
    itkSetMacro(UseLowerThreshold, bool);    
    itkGetConstMacro(UseLowerThreshold, bool);    
    itkBooleanMacro(UseLowerThreshold);
    GGO_DefineOption_WithTest(lower, SetLowerThreshold, InputImagePixelType, UseLowerThreshold);

    // Step 2
    itkSetMacro(DecomposeAndReconstructDuringFirstStep, bool);
    itkGetConstMacro(DecomposeAndReconstructDuringFirstStep, bool);
    itkBooleanMacro(DecomposeAndReconstructDuringFirstStep);
    GGO_DefineOption_Flag(erode1, SetDecomposeAndReconstructDuringFirstStep);

    itkSetMacro(Radius1, InternalImageSizeType);
    itkGetConstMacro(Radius1, InternalImageSizeType);
    GGO_DefineOption_Vector(radius1, SetRadius1, InternalImageSizeType, ImageDimension, true);

    itkSetMacro(MaximumNumberOfLabels1, int);
    itkGetConstMacro(MaximumNumberOfLabels1, int);
    GGO_DefineOption(max1, SetMaximumNumberOfLabels1, int);

    itkSetMacro(NumberOfNewLabels1, int);
    itkGetConstMacro(NumberOfNewLabels1, int);
    GGO_DefineOption(new1, SetNumberOfNewLabels1, int);

    // Step 2
    itkSetMacro(DecomposeAndReconstructDuringSecondStep, bool);
    itkGetConstMacro(DecomposeAndReconstructDuringSecondStep, bool);
    itkBooleanMacro(DecomposeAndReconstructDuringSecondStep);
    GGO_DefineOption_Flag(erode2, SetDecomposeAndReconstructDuringSecondStep);

    itkSetMacro(Radius2, InternalImageSizeType);
    itkGetConstMacro(Radius2, InternalImageSizeType);
    GGO_DefineOption_Vector(radius2, SetRadius2, InternalImageSizeType, ImageDimension, true)

    itkSetMacro(MaximumNumberOfLabels2, int);
    itkGetConstMacro(MaximumNumberOfLabels2, int);
    GGO_DefineOption(max2, SetMaximumNumberOfLabels2, int);

    itkSetMacro(NumberOfNewLabels2, int);
    itkGetConstMacro(NumberOfNewLabels2, int);
    GGO_DefineOption(new2, SetNumberOfNewLabels2, int);

    // Step 3
    itkSetMacro(FirstKeep, int);
    itkGetConstMacro(FirstKeep, int);
    GGO_DefineOption(firstKeep, SetFirstKeep, int);

    itkSetMacro(LastKeep, int);
    itkGetConstMacro(LastKeep, int);
    GGO_DefineOption(lastKeep, SetLastKeep, int);

    // Step 4
    itkSetMacro(FinalOpenClose, bool);
    itkGetConstMacro(FinalOpenClose, bool);
    itkBooleanMacro(FinalOpenClose);
    GGO_DefineOption_Flag(openClose, SetFinalOpenClose);

    // Step 4
    itkSetMacro(AutoCrop, bool);
    itkGetConstMacro(AutoCrop, bool);
    itkBooleanMacro(AutoCrop);
    GGO_DefineOption_Flag(autoCrop, SetAutoCrop);

  protected:
    ExtractPatientFilter();
    virtual ~ExtractPatientFilter() {}
    
    itkSetMacro(BackgroundValue, OutputImagePixelType);
    itkSetMacro(ForegroundValue, OutputImagePixelType);
    itkGetConstMacro(BackgroundValue, OutputImagePixelType);
    itkGetConstMacro(ForegroundValue, OutputImagePixelType);
    OutputImagePixelType m_BackgroundValue;
    OutputImagePixelType m_ForegroundValue;

    InputImagePixelType m_UpperThreshold;
    InputImagePixelType m_LowerThreshold;
    bool m_UseLowerThreshold;
    bool m_DecomposeAndReconstructDuringFirstStep;
    bool m_DecomposeAndReconstructDuringSecondStep;
    bool m_FinalOpenClose;
    InternalImageSizeType m_Radius1;
    InternalImageSizeType m_Radius2;
    int m_MaximumNumberOfLabels1;
    int m_MaximumNumberOfLabels2;
    int m_NumberOfNewLabels1;
    int m_NumberOfNewLabels2;
    int m_FirstKeep;
    int m_LastKeep;
    bool m_AutoCrop;

    virtual void GenerateOutputInformation();
    virtual void GenerateData();
    
    InputImageConstPointer input;
    OutputImagePointer output;
    typename InternalImageType::Pointer working_image;
        
  private:
    ExtractPatientFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractPatientFilter.txx"
#endif

#endif
