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

#ifndef CLITKEXTRACTPATIENTFILTER_H
#define CLITKEXTRACTPATIENTFILTER_H

#include "clitkFilterBase.h"
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"

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
  
  template <class TInputImageType>
  class ITK_EXPORT ExtractPatientFilter: 
    public virtual clitk::FilterBase, 
    public clitk::FilterWithAnatomicalFeatureDatabaseManagement,
    public itk::ImageToImageFilter<TInputImageType, 
                                   itk::Image<uchar, TInputImageType::ImageDimension> > 
  {
  public:
    /** Standard class typedefs. */
    typedef itk::Image<uchar, TInputImageType::ImageDimension>      MaskImageType;
    typedef ExtractPatientFilter                                    Self;
    typedef itk::ImageToImageFilter<TInputImageType, MaskImageType> Superclass;
    typedef itk::SmartPointer<Self>                                 Pointer;
    typedef itk::SmartPointer<const Self>                           ConstPointer;
    
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
        
    typedef typename MaskImageType::ConstPointer MaskImageConstPointer;
    typedef typename MaskImageType::Pointer      MaskImagePointer;
    typedef typename MaskImageType::RegionType   MaskImageRegionType; 
    typedef typename MaskImageType::PixelType    MaskImagePixelType; 
    typedef typename MaskImageType::SizeType     MaskImageSizeType; 
    typedef typename MaskImageType::IndexType    MaskImageIndexType; 

    itkStaticConstMacro(ImageDimension, unsigned int, InputImageType::ImageDimension);
    typedef int InternalPixelType;
    typedef itk::Image<InternalPixelType, ImageDimension> InternalImageType;
    typedef typename InternalImageType::SizeType          InternalImageSizeType;
    
    /** Connect inputs */
    void SetInput(const TInputImageType * image);
    itkSetMacro(OutputPatientFilename, std::string);
    itkGetMacro(OutputPatientFilename, std::string);

    // Step 1
    itkSetMacro(UpperThreshold, InputImagePixelType);
    itkGetMacro(UpperThreshold, InputImagePixelType);

    itkSetMacro(LowerThreshold, InputImagePixelType);
    itkGetMacro(LowerThreshold, InputImagePixelType);
    itkSetMacro(UseLowerThreshold, bool);    
    itkGetConstMacro(UseLowerThreshold, bool);    
    itkBooleanMacro(UseLowerThreshold);

    // Step 2
    itkSetMacro(DecomposeAndReconstructDuringFirstStep, bool);
    itkGetConstMacro(DecomposeAndReconstructDuringFirstStep, bool);
    itkBooleanMacro(DecomposeAndReconstructDuringFirstStep);

    itkSetMacro(Radius1, InternalImageSizeType);
    itkGetConstMacro(Radius1, InternalImageSizeType);

    itkSetMacro(MaximumNumberOfLabels1, int);
    itkGetConstMacro(MaximumNumberOfLabels1, int);

    itkSetMacro(NumberOfNewLabels1, int);
    itkGetConstMacro(NumberOfNewLabels1, int);

    // Step 2
    itkSetMacro(DecomposeAndReconstructDuringSecondStep, bool);
    itkGetConstMacro(DecomposeAndReconstructDuringSecondStep, bool);
    itkBooleanMacro(DecomposeAndReconstructDuringSecondStep);

    itkSetMacro(Radius2, InternalImageSizeType);
    itkGetConstMacro(Radius2, InternalImageSizeType);

    itkSetMacro(MaximumNumberOfLabels2, int);
    itkGetConstMacro(MaximumNumberOfLabels2, int);

    itkSetMacro(NumberOfNewLabels2, int);
    itkGetConstMacro(NumberOfNewLabels2, int);

    // Step 3
    itkSetMacro(FirstKeep, int);
    itkGetConstMacro(FirstKeep, int);

    itkSetMacro(LastKeep, int);
    itkGetConstMacro(LastKeep, int);

    // Step 4
    itkSetMacro(FinalOpenClose, bool);
    itkGetConstMacro(FinalOpenClose, bool);
    itkBooleanMacro(FinalOpenClose);

    // Step 4
    itkSetMacro(AutoCrop, bool);
    itkGetConstMacro(AutoCrop, bool);
    itkBooleanMacro(AutoCrop);

  protected:
    ExtractPatientFilter();
    virtual ~ExtractPatientFilter() {}
    
    itkSetMacro(BackgroundValue, MaskImagePixelType);
    itkSetMacro(ForegroundValue, MaskImagePixelType);
    itkGetConstMacro(BackgroundValue, MaskImagePixelType);
    itkGetConstMacro(ForegroundValue, MaskImagePixelType);
    MaskImagePixelType m_BackgroundValue;
    MaskImagePixelType m_ForegroundValue;

    std::string m_OutputPatientFilename;
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
    MaskImagePointer output;
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
