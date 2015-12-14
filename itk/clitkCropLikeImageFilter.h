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

#ifndef CLITKCROPLIKEIMAGEFILTER_H
#define CLITKCROPLIKEIMAGEFILTER_H

// clitk
#include "clitkBoundingBoxUtils.h"
#include "clitkImageCommon.h"

// itk
#include <itkImageToImageFilter.h>

namespace clitk {

  //--------------------------------------------------------------------
  /*
    Perform various cropping operation on a image
  */
  //--------------------------------------------------------------------

  template <class ImageType>
  class ITK_EXPORT CropLikeImageFilter: public itk::ImageToImageFilter<ImageType, ImageType> {
  public:
    /** Standard class typedefs. */
    typedef CropLikeImageFilter                            Self;
    typedef itk::ImageToImageFilter<ImageType, ImageType>  Superclass;
    typedef itk::SmartPointer<Self>                        Pointer;
    typedef itk::SmartPointer<const Self>                  ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(CropLikeImageFilter, ImageToImageFilter);

    /** Some convenient typedefs. */
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::PixelType    PixelType;
    typedef typename ImageType::RegionType   RegionType;
    typedef typename ImageType::PointType    PointType;
    typedef typename ImageType::IndexType    IndexType;
    typedef typename ImageType::SizeType     SizeType;

    /** Image filename for Crop Like */
    void SetCropLikeFilename(std::string f);
    void SetCropLikeImage(const itk::ImageBase<ImageType::ImageDimension> * like);
    void SetCropLikeImage(const itk::ImageBase<ImageType::ImageDimension> * like, int axe);

    // Set Background if 'like' is greater than input
    itkSetMacro(BackgroundValue, PixelType);
    itkGetConstMacro(BackgroundValue, PixelType);

    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);

    // I dont want to verify inputs information
    virtual void VerifyInputInformation() { }

  protected:
    CropLikeImageFilter();
    virtual ~CropLikeImageFilter() {}

    virtual void GenerateOutputInformation();
    virtual void GenerateData();
    virtual void GenerateInputRequestedRegion();

    PixelType m_BackgroundValue;
    RegionType m_OutputRegion;
    RegionType m_Region;
    std::string m_LikeFilename;
    bool m_LikeFilenameIsGiven;
    PointType m_Origin;
    const itk::ImageBase<ImageDimension> * m_LikeImage;
    std::vector<bool> m_CropAlongThisDimension;

    PointType m_StartPoint;         // start point in physical world
    IndexType m_StartSourceIndex;   // start index in "source" image
    IndexType m_StartDestIndex;     // start index in "destination" image

    PointType m_StopPoint;         // stop point in physical world
    IndexType m_StopSourceIndex;   // stop index in "source" image
    IndexType m_StopDestIndex;     // stop index in "destination" image

  private:
    CropLikeImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

  }; // end class
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  // Convenient function
  template<class ImageType>
  typename ImageType::Pointer
  ResizeImageLike(const ImageType * input,
                  const itk::ImageBase<ImageType::ImageDimension> * like,
                  typename ImageType::PixelType BG);

  template<class ImageType>
  typename ImageType::Pointer
  ResizeImageLike(const ImageType * input,
                  typename itk::ImageBase<ImageType::ImageDimension>::RegionType * like,
                  typename ImageType::PixelType BG);

  template<class ImageType>
  typename ImageType::Pointer
  ResizeImageLike(const ImageType * input,
                  typename itk::BoundingBox<unsigned long, ImageType::ImageDimension>::Pointer bb,
                  typename ImageType::PixelType BG);


} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkCropLikeImageFilter.txx"
#endif

#endif // # CROPLIKEIMAGEFILTER
