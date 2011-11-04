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

#ifndef CLITKRELATIVEPOSITIONANALYZERFILTER_H
#define CLITKRELATIVEPOSITIONANALYZERFILTER_H

// clitk
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"
#include "clitkFilterBase.h"
#include "clitkSliceBySliceRelativePositionFilter.h"
#include "clitkRelativePositionDataBase.h"

// itk
#include <itkImageToImageFilter.h>
#include <itkLabelStatisticsImageFilter.h>

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Analyze the relative position of a Target (mask image) according
    to some Object (mask image), in a given Support (mask
    image). Compute the optimal threshold allowing to remove the
    maximal area from the Support without removing area belonging to
    the Target.
  */
  //--------------------------------------------------------------------
  
  template <class ImageType>
  class RelativePositionAnalyzerFilter:
    public itk::ImageToImageFilter<ImageType, ImageType>
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<ImageType, ImageType>      Superclass;
    typedef RelativePositionAnalyzerFilter<ImageType>          Self;
    typedef itk::SmartPointer<Self>                            Pointer;
    typedef itk::SmartPointer<const Self>                      ConstPointer;
       
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(RelativePositionAnalyzerFilter, ImageToImageFilter);

    /** Some convenient typedefs. */
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   RegionType; 
    typedef typename ImageType::PixelType    PixelType;
    typedef typename ImageType::SpacingType  SpacingType;
    typedef typename ImageType::SizeType     SizeType;
    typedef typename ImageType::IndexType    IndexType;
    typedef typename ImageType::PointType    PointType;
    
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    FILTERBASE_INIT;
    typedef itk::Image<float, ImageDimension> FloatImageType;

     /** Input : initial image and object */
    void SetInputSupport(const ImageType * image);
    void SetInputObject(const ImageType * image);
    void SetInputTarget(const ImageType * image);
    
    // Input
    // supportname, objectname multiple targetname
    
    // Options
    itkGetConstMacro(BackgroundValue, PixelType);
    itkSetMacro(BackgroundValue, PixelType);

    itkGetConstMacro(ForegroundValue, PixelType);
    itkSetMacro(ForegroundValue, PixelType);

    clitk::RelativePositionDirectionType & GetDirection() { return m_Direction; }
    void SetDirection(clitk::RelativePositionDirectionType & d) { m_Direction = d; }

    itkGetConstMacro(NumberOfBins, int);
    itkSetMacro(NumberOfBins, int);

    itkGetConstMacro(AreaLossTolerance, double);
    itkSetMacro(AreaLossTolerance, double);

    itkGetConstMacro(SupportSize, int);
    itkGetConstMacro(TargetSize, int);
    itkGetConstMacro(SizeWithThreshold, int);
    itkGetConstMacro(SizeWithReverseThreshold, int);

    itkGetConstMacro(Info, clitk::RelativePositionInformationType);
    itkGetConstMacro(InfoReverse, clitk::RelativePositionInformationType);

    // For debug
    void PrintOptions();
    
    // Print output
    void Print(std::ostream & os=std::cout);

    // I dont want to verify inputs information
    virtual void VerifyInputInformation() { }
    
   protected:
    RelativePositionAnalyzerFilter();
    virtual ~RelativePositionAnalyzerFilter() {}
    
    itkSetMacro(SupportSize, int);
    itkSetMacro(TargetSize, int);
    itkSetMacro(SizeWithThreshold, int);
    itkSetMacro(SizeWithReverseThreshold, int);

    PixelType m_BackgroundValue;
    PixelType m_ForegroundValue;
    ImagePointer m_Support;
    ImagePointer m_Object;
    ImagePointer m_Target;
    int m_NumberOfBins;
    double m_AreaLossTolerance;
    int m_SupportSize;
    int m_TargetSize;
    int m_SizeWithReverseThreshold;
    int m_SizeWithThreshold;
    clitk::RelativePositionDirectionType m_Direction;
    clitk::RelativePositionInformationType m_Info;
    clitk::RelativePositionInformationType m_InfoReverse;
    
    virtual void GenerateOutputInformation();
    virtual void GenerateData();

    typename FloatImageType::Pointer
    ComputeFuzzyMap(ImageType * object, ImageType * target, ImageType * support, double angle);
    
    void
    ComputeOptimalThresholds(FloatImageType * map, ImageType * target, int bins, double tolerance, 
                             double & threshold, double & reverseThreshold);
  private:
    RelativePositionAnalyzerFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#include "clitkRelativePositionAnalyzerFilter.txx"

#endif
