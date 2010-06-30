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

#ifndef CLITKEXTRACTLYMPHSTATIONSFILTER_H
#define CLITKEXTRACTLYMPHSTATIONSFILTER_H

#include "clitkFilterBase.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Try to extract the LymphStations part of a thorax CT.
    Inputs : 
    - Patient label image
    - Lungs label image
    - Bones label image
  */
  //--------------------------------------------------------------------
  
  template <class TImageType>
  class ITK_EXPORT ExtractLymphStationsFilter: 
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<TImageType, TImageType> 
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<TImageType, TImageType> Superclass;
    typedef ExtractLymphStationsFilter            Self;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractLymphStationsFilter, InPlaceImageFilter);
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
    void SetInputMediastinumLabelImage(const TImageType * image, ImagePixelType bg=0);
    void SetInputTracheaLabelImage(const TImageType * image, ImagePixelType bg=0);
  
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, TImageType::ImageDimension);

    // Set all options at a time
    template<class ArgsInfoType>
      void SetArgsInfo(ArgsInfoType arg);
   
    // Background / Foreground
    itkSetMacro(BackgroundValueMediastinum, ImagePixelType);
    itkGetConstMacro(BackgroundValueMediastinum, ImagePixelType);
    //GGO_DefineOption(MediastinumBG, SetBackgroundValueMediastinum, ImagePixelType);
    
    itkSetMacro(BackgroundValueTrachea, ImagePixelType);
    itkGetConstMacro(BackgroundValueTrachea, ImagePixelType);
    //GGO_DefineOption(TracheaBG, SetBackgroundValueTrachea, ImagePixelType);

    itkGetConstMacro(BackgroundValue, ImagePixelType);
    itkGetConstMacro(ForegroundValue, ImagePixelType);

    itkSetMacro(CarenaZPositionInMM, double);
    itkGetConstMacro(CarenaZPositionInMM, double);
    GGO_DefineOption(carenaZposition, SetCarenaZPositionInMM, double);

    itkSetMacro(MiddleLobeBronchusZPositionInMM, double);
    itkGetConstMacro(MiddleLobeBronchusZPositionInMM, double);
    GGO_DefineOption(middleLobeBronchusZposition, SetMiddleLobeBronchusZPositionInMM, double);

    itkSetMacro(IntermediateSpacing, double);
    itkGetConstMacro(IntermediateSpacing, double);
    GGO_DefineOption(spacing, SetIntermediateSpacing, double);

    itkSetMacro(FuzzyThreshold1, double);
    itkGetConstMacro(FuzzyThreshold1, double);
    GGO_DefineOption(fuzzy1, SetFuzzyThreshold1, double);



  protected:
    ExtractLymphStationsFilter();
    virtual ~ExtractLymphStationsFilter() {}
    
    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();
       
    itkSetMacro(BackgroundValue, ImagePixelType);
    itkSetMacro(ForegroundValue, ImagePixelType);
    
    ImageConstPointer m_mediastinum;
    ImageConstPointer m_trachea;
    ImagePointer m_working_image;
    ImagePointer m_working_trachea;  
    ImagePointer m_output;  
    
    ImagePixelType m_BackgroundValueMediastinum;
    ImagePixelType m_BackgroundValueTrachea;
    ImagePixelType m_BackgroundValue;
    ImagePixelType m_ForegroundValue;
    
    double m_CarenaZPositionInMM;
    double m_MiddleLobeBronchusZPositionInMM; 
    double m_IntermediateSpacing;
    double m_FuzzyThreshold1;
    
  private:
    ExtractLymphStationsFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractLymphStationsFilter.txx"
#endif

#endif
