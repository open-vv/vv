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

#ifndef CLITKEXTRACTAIRWAYTREEINFOSFILTER_H
#define CLITKEXTRACTAIRWAYTREEINFOSFILTER_H

// clitk 
#include "clitkFilterBase.h"
#include "clitkDecomposeAndReconstructImageFilter.h"
#include "clitkExplosionControlledThresholdConnectedImageFilter.h"
#include "clitkSegmentationUtils.h"

// itk
#include "itkStatisticsImageFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    From a trachea binary image, compute the skeleton and track the
    path to find the carena.
  */
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template<class IndexType, class PixelType>
  class Bifurcation
  {
  public:
    Bifurcation(IndexType _index, PixelType _l, PixelType _l1, PixelType _l2) {
      index = _index;
      _l = l;
      _l1 = l1;
      _l2 = l2;
    }
    IndexType index;
    PixelType l;
    PixelType l1;
    PixelType l2;
  };
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TImageType>
  class ITK_EXPORT ExtractAirwayTreeInfoFilter: 
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<TImageType, TImageType> 
  {
    
  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<TImageType, TImageType> Superclass;
    typedef ExtractAirwayTreeInfoFilter         Self;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);  
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractAirwayTreeInfoFilter, ImageToImageFilter);
    FILTERBASE_INIT;

    /** Some convenient typedefs */
    typedef TImageType                       ImageType;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   ImageRegionType; 
    typedef typename ImageType::PixelType    ImagePixelType; 
    typedef typename ImageType::SizeType     ImageSizeType; 
    typedef typename ImageType::IndexType    ImageIndexType; 
    typedef typename ImageType::PointType    ImagePointType; 
        
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    typedef int InternalPixelType;
    typedef itk::Image<InternalPixelType, ImageType::ImageDimension> InternalImageType;
    typedef typename InternalImageType::Pointer                      InternalImagePointer;
    typedef typename InternalImageType::IndexType                    InternalIndexType;
    typedef LabelizeParameters<InternalPixelType>                    LabelParamType;
    
    /** Connect inputs */
    void SetInput(const ImageType * image);

    // Set all options at a time
    template<class ArgsInfoType>
      void SetArgsInfo(ArgsInfoType arg);

    // Background / Foreground
    itkGetConstMacro(BackgroundValue, ImagePixelType);
    itkGetConstMacro(ForegroundValue, ImagePixelType);
    
    // Get results
    itkGetConstMacro(FirstTracheaPoint, ImagePointType);
    itkGetConstMacro(CarinaPoint, ImagePointType);

  protected:
    ExtractAirwayTreeInfoFilter();
    virtual ~ExtractAirwayTreeInfoFilter() {}

    // Main members
    ImageConstPointer input;
    ImagePointer skeleton;
    ImagePointer working_input;

    // Global options
    itkSetMacro(BackgroundValue, ImagePixelType);
    itkSetMacro(ForegroundValue, ImagePixelType);
    ImagePixelType m_BackgroundValue;
    ImagePixelType m_ForegroundValue;

    // Results
    ImagePointType m_FirstTracheaPoint;
    ImagePointType m_CarinaPoint;
    
    virtual void GenerateOutputInformation();
    virtual void GenerateData();

    typedef Bifurcation<ImageIndexType,ImagePixelType> BifurcationType;
    void TrackFromThisIndex(std::vector<BifurcationType> & listOfBifurcations, 
                            ImagePointer skeleton, 
                            ImageIndexType index,
                            ImagePixelType label);
  private:
    ExtractAirwayTreeInfoFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractAirwayTreeInfoFilter.txx"
#endif

#endif
