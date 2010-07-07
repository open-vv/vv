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

#ifndef CLITKAUTOCROPFILTER_H
#define CLITKAUTOCROPFILTER_H

#include <itkImageToImageFilter.h>

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Perform auto crop on a Label Image (with Background defined)
  */
  //--------------------------------------------------------------------
  
  template <class ImageType>
  class ITK_EXPORT AutoCropFilter: 
    public itk::ImageToImageFilter<ImageType, ImageType> {

  public:
    /** Standard class typedefs. */
    typedef AutoCropFilter                         Self;
    typedef itk::ImageToImageFilter<ImageType, ImageType>  Superclass;
    typedef itk::SmartPointer<Self>                          Pointer;
    typedef itk::SmartPointer<const Self>                    ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(AutoCropFilter, ImageToImageFilter);

    /** Some convenient typedefs. */
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::PixelType    ImagePixelType;
    typedef typename ImageType::RegionType   ImageRegionType;
    typedef long LabelType;

    /** Connect one of the operands for pixel-wise addition */
    void SetInput( const ImageType * image);
    
    // LabelImage information (BG)
    void SetBackgroundValue(ImagePixelType p);

    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    
  protected:
    AutoCropFilter();
    virtual ~AutoCropFilter() {}
    
    virtual void GenerateOutputInformation();
    virtual void GenerateData();
    
    ImagePixelType m_BackgroundValue;
    ImageRegionType m_Region;
    ImagePointer m_labeImage;
    
  private:
    AutoCropFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkAutoCropFilter.txx"
#endif

#endif
