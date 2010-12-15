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
    Need a set of Anatomical Features (AFDB)
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
    itkTypeMacro(ExtractLymphStationsFilter, ImageToImageFilter);

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
    typedef itk::Image<MaskImagePixelType, 3>    MaskImageType;  
    typedef typename MaskImageType::Pointer      MaskImagePointer;
    typedef typename MaskImageType::RegionType   MaskImageRegionType; 
    typedef typename MaskImageType::SizeType     MaskImageSizeType; 
    typedef typename MaskImageType::IndexType    MaskImageIndexType; 
    typedef typename MaskImageType::PointType    MaskImagePointType; 

    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    FILTERBASE_INIT;
   
    /** Main options (from ggo) */
    template <class ArgsInfoType>
    void SetArgsInfo(ArgsInfoType & argsinfo);

    itkGetConstMacro(BackgroundValue, MaskImagePixelType);
    itkGetConstMacro(ForegroundValue, MaskImagePixelType);
    itkSetMacro(BackgroundValue, MaskImagePixelType);
    itkSetMacro(ForegroundValue, MaskImagePixelType);
    
    // Station 7
    itkSetMacro(FuzzyThreshold, double);
    itkGetConstMacro(FuzzyThreshold, double);
    itkSetMacro(Station7Filename, std::string);
    itkGetConstMacro(Station7Filename, std::string);

  protected:
    ExtractLymphStationsFilter();
    virtual ~ExtractLymphStationsFilter() {}
    
    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();
    
    ImageConstPointer  m_Input;
    MaskImagePointer   m_Support;
    MaskImagePointer   m_Working_Support;
    MaskImagePointer   m_Output;
    MaskImagePixelType m_BackgroundValue;
    MaskImagePixelType m_ForegroundValue;    

    // Common 
    MaskImagePointer m_Trachea;

    // Station 7
    void ExtractStation_7();
    void ExtractStation_7_SI_Limits();
    void ExtractStation_7_RL_Limits();
    void ExtractStation_7_Posterior_Limits();       
    std::string      m_Station7Filename;
    MaskImagePointer m_working_trachea;
    double           m_FuzzyThreshold;
    MaskImagePointer m_LeftBronchus;
    MaskImagePointer m_RightBronchus;
    MaskImagePointer m_Station7;

    // Station 4RL
    void ExtractStation_4RL();
    void ExtractStation_4RL_SI_Limits();
    void ExtractStation_4RL_LR_Limits();
    MaskImagePointer m_Station4RL;

  private:
    ExtractLymphStationsFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractLymphStationsFilter.txx"
#include "clitkExtractLymphStation_7.txx"
#include "clitkExtractLymphStation_4RL.txx"
#endif

#endif
