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

#ifndef CLITKEXTRACTSLICEFILTER_H
#define CLITKEXTRACTSLICEFILTER_H

// clitk
#include "clitkFilterBase.h"

// itk
#include "itkPasteImageFilter.h"

// itk ENST
#include "RelativePositionPropImageFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Let A be an initial label image.
    Let B be a label image with an object. 
    Let o be an orientation relatively to the B object (for example RightTo, AntTo, InferiorTo ...)

    This filter removes (=set background) from A all points that are
    not in the wanted o orientation. It uses downsampled version for
    faster processing, and (try to) take into account discretization
    problem. Uses [Bloch 1999].
  */
  //--------------------------------------------------------------------
  
  template <class ImageType>
  class ITK_EXPORT ExtractSliceFilter:
    public clitk::FilterBase, 
    public itk::ImageToImageFilter<ImageType, 
                                   typename itk::Image<typename ImageType::PixelType, ImageType::ImageDimension-1> >
  {

  public:
    /** Some convenient typedefs. */
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   RegionType; 
    typedef typename ImageType::PixelType    PixelType;
    typedef typename ImageType::SpacingType  SpacingType;
    typedef typename ImageType::SizeType     SizeType;
    typedef typename ImageType::IndexType    IndexType;
    
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);

    /** Slice image type **/
    typedef itk::Image<PixelType, ImageDimension-1> SliceType;
    typedef typename SliceType::Pointer             SliceTypePointer;

    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<ImageType, SliceType>   Superclass;
    typedef ExtractSliceFilter                              Self;
    typedef itk::SmartPointer<Self>                         Pointer;
    typedef itk::SmartPointer<const Self>                   ConstPointer;
       
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractSliceFilter, ImageToImageFilter);
    FILTERBASE_INIT;

    /** Input : initial image and object */
    void SetInput(const ImageType * image) ITK_OVERRIDE;
    
    // Options
    itkGetConstMacro(Direction, int);
    itkSetMacro(Direction, int);
    
    // Get results
    void GetOutputSlices(std::vector<typename SliceType::Pointer> & o);

  protected:
    ExtractSliceFilter();
    virtual ~ExtractSliceFilter() {}
    
    int m_Direction;

    virtual void GenerateOutputInformation() ITK_OVERRIDE;
    virtual void GenerateInputRequestedRegion() ITK_OVERRIDE;
    virtual void GenerateData() ITK_OVERRIDE;

    int m_NumberOfSlices;
    ImagePointer input;
    ImagePointer object;
    SliceType *  output;
    
    RegionType m_region;
    SizeType   m_size;
    IndexType  m_index;

  private:
    ExtractSliceFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractSliceFilter.txx"
#endif

#endif
