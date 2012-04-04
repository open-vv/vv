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

#ifndef CLITKRELATIVEPOSITIONLIST_H
#define CLITKRELATIVEPOSITIONLIST_H

// clitk
#include "clitkSegmentationUtils.h"
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"
#include "clitkRelativePosition_ggo.h"

namespace clitk {
  
  /*--------------------------------------------------------------------
    Manage a list of RelativePosition operations, to be performed on
    the same input image, with different objects and parameters.
    ------------------------------------------------------------------*/
  
  template <class TImageType>
  class ITK_EXPORT RelativePositionList:
    public virtual clitk::FilterBase, 
    public clitk::FilterWithAnatomicalFeatureDatabaseManagement,
    public itk::ImageToImageFilter<TImageType, TImageType>
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<TImageType, TImageType> Superclass;
    typedef RelativePositionList           Self;
    typedef itk::SmartPointer<Self>        Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(RelativePositionList, ImageToImageFilter);

      /** Some convenient typedefs. */
    typedef TImageType                       ImageType;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   ImageRegionType; 
    typedef typename ImageType::PixelType    ImagePixelType; 
    typedef typename ImageType::SizeType     ImageSizeType; 
    typedef typename ImageType::IndexType    ImageIndexType; 
    typedef typename ImageType::PointType    ImagePointType;     
    typedef struct args_info_clitkRelativePosition ArgsInfoType;
    typedef SliceBySliceRelativePositionFilter<ImageType> SliceRelPosFilterType;
    typedef AddRelativePositionConstraintToLabelImageFilter<ImageType> RelPosFilterType;

    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    FILTERBASE_INIT;
   
    itkGetConstMacro(BackgroundValue, ImagePixelType);
    itkGetConstMacro(ForegroundValue, ImagePixelType);
    itkSetMacro(BackgroundValue, ImagePixelType);
    itkSetMacro(ForegroundValue, ImagePixelType);

    itkSetMacro(InputName, std::string);
    itkGetConstMacro(InputName, std::string);

    itkGetConstMacro(ComputeOverlapFlag, bool);
    itkSetMacro(ComputeOverlapFlag, bool);
    itkBooleanMacro(ComputeOverlapFlag);

    void Read(std::string filename);     
    void SetFilterOptions(typename RelPosFilterType::Pointer filter, ArgsInfoType & options);
    void SetReferenceImageForOverlapMeasure(ImagePointer ref);

  protected:
    RelativePositionList();
    virtual ~RelativePositionList() {}

  private:
    RelativePositionList(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
    void GenerateInputRequestedRegion();
    void GenerateOutputInformation();
    void GenerateData();

    std::string m_InputName;
    ImagePixelType m_BackgroundValue;
    ImagePixelType m_ForegroundValue;
    typename SliceRelPosFilterType::Pointer mFilter;
    std::vector<ArgsInfoType> mArgsInfoList;
    ImagePointer m_working_input;
    ImagePointer m_reference;
    bool m_ComputeOverlapFlag;

  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#include "clitkRelativePositionList.txx"

#endif
