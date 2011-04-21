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

// clitk
#include "clitkFilterBase.h"
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"

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
    public virtual clitk::FilterBase, 
    public clitk::FilterWithAnatomicalFeatureDatabaseManagement,
    public itk::ImageToImageFilter<TImageType, itk::Image<uchar, 3> >
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<TImageType, itk::Image<uchar, 3> > Superclass;
    typedef ExtractLymphStationsFilter          Self;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractLymphStationsFilter, InPlaceImageFilter);

    /** Some convenient typedefs. */
    typedef TImageType                       ImageType;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   ImageRegionType; 
    typedef typename ImageType::PixelType    ImagePixelType; 
    typedef typename ImageType::SizeType     ImageSizeType; 
    typedef typename ImageType::IndexType    ImageIndexType; 
    typedef typename ImageType::PointType    ImagePointType; 
        
    typedef uchar MaskImagePixelType;
    typedef itk::Image<MaskImagePixelType, 3> MaskImageType;  
    typedef typename MaskImageType::Pointer   MaskImagePointer;

    /** Connect inputs */
    //    void SetInput(const TImageType * image);
  
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, TImageType::ImageDimension);
    FILTERBASE_INIT;
   
    itkGetConstMacro(BackgroundValue, ImagePixelType);
    itkGetConstMacro(ForegroundValue, ImagePixelType);
    itkSetMacro(BackgroundValue, ImagePixelType);
    itkSetMacro(ForegroundValue, ImagePixelType);
    
    itkSetMacro(FuzzyThreshold, double);
    itkGetConstMacro(FuzzyThreshold, double);

  protected:
    ExtractLymphStationsFilter();
    virtual ~ExtractLymphStationsFilter() {}
    
    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();
       
    MaskImagePointer m_mediastinum;
    MaskImagePointer m_trachea;
    MaskImagePointer m_working_mediastinum;
    MaskImagePointer m_working_trachea;  
    MaskImagePointer m_output;  
    
    ImagePixelType m_BackgroundValue;
    ImagePixelType m_ForegroundValue;
    double m_CarinaZPositionInMM;
    bool   m_CarinaZPositionInMMIsSet;
    double m_MiddleLobeBronchusZPositionInMM; 

    double m_IntermediateSpacing;
    double m_FuzzyThreshold;
    
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
