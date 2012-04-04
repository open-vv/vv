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

#ifndef CLITKSTRUCTURESEXTRACTIONFILTER_H
#define CLITKSTRUCTURESEXTRACTIONFILTER_H

// clitk
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"
#include "clitkFilterBase.h"
#include "clitkRelativePositionList.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    - Convenient class to add some capabilities to a filter :
    FilterBase, FilterWithAnatomicalFeatureDatabaseManagement and
    RelativePositionList
  */
  //--------------------------------------------------------------------
  template <class TImageType>
  class StructuresExtractionFilter: 
    public virtual FilterBase,
    public clitk::FilterWithAnatomicalFeatureDatabaseManagement,
    public itk::ImageToImageFilter<TImageType, itk::Image<uchar, 3> >     
  {
  public:
    // Standard class typedefs
    typedef StructuresExtractionFilter                                 Self;
    typedef itk::ImageToImageFilter<TImageType, itk::Image<uchar, 3> > Superclass;
    typedef itk::SmartPointer<Self>                                    Pointer;
    typedef itk::SmartPointer<const Self>                              ConstPointer;
     
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
   // Run-time type information (and related methods)
    itkTypeMacro(StructuresExtractionFilter, ImageToImageFilter);

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

    typedef itk::Image<MaskImagePixelType, 2>    MaskSliceType;
    typedef typename MaskSliceType::Pointer      MaskSlicePointer;
    typedef typename MaskSliceType::PointType    MaskSlicePointType;
    typedef typename MaskSliceType::RegionType   MaskSliceRegionType; 
    typedef typename MaskSliceType::SizeType     MaskSliceSizeType; 
    typedef typename MaskSliceType::IndexType    MaskSliceIndexType; 
    
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    FILTERBASE_INIT;
   
    itkGetConstMacro(BackgroundValue, MaskImagePixelType);
    itkGetConstMacro(ForegroundValue, MaskImagePixelType);
    itkSetMacro(BackgroundValue, MaskImagePixelType);
    itkSetMacro(ForegroundValue, MaskImagePixelType);
    
    // RelativePositionList management 
    void AddRelativePositionListFilename(std::string s);    
    MaskImagePointer ApplyRelativePositionList(std::string name, MaskImageType * input, bool overlap=false);
    
   protected:
    StructuresExtractionFilter();
    virtual ~StructuresExtractionFilter() {}    
    
    virtual void GenerateData() {}
    
    MaskImagePixelType m_BackgroundValue;
    MaskImagePixelType m_ForegroundValue;
 
    // RelativePositionList
    std::vector<std::string> mListOfRelativePositionListFilename;
    typedef clitk::RelativePositionList<MaskImageType> RelPosListType;
    typedef typename RelPosListType::Pointer RelPosListPointer;
    std::map<std::string, RelPosListPointer> mMapOfRelativePositionList;

  private:
    StructuresExtractionFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#include "clitkStructuresExtractionFilter.txx"

#endif // CLITKSTRUCTURESEXTRACTIONFILTER_H
