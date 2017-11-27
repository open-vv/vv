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

#ifndef CLITKBOOLEANOPERATORLABELIMAGEFILTER_H
#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_H

#include "itkInPlaceImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Perform boolan operation between two mask images. Like itkAnd and
    others, but take care of:
    - origin of the images (spacing must be equal)
    - in place or not output
    - Binary or Label images as inputs. Label i BG only ; Binary is FG only

    - NO THREAD -> I dont know how (yet) to manage two different inputRegionForThread 

  */
  //--------------------------------------------------------------------
  
  template <class TInputImage1, class TInputImage2=TInputImage1, class TOutputImage=TInputImage1>
  class ITK_EXPORT BooleanOperatorLabelImageFilter: 
    public itk::InPlaceImageFilter<TInputImage1, TOutputImage> {

  public:
    /** Standard class typedefs. */
    typedef BooleanOperatorLabelImageFilter                     Self;
    typedef itk::InPlaceImageFilter<TInputImage1,TOutputImage>  Superclass;
    typedef itk::SmartPointer<Self>                             Pointer;
    typedef itk::SmartPointer<const Self>                       ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(BooleanOperatorLabelImageFilter, InPlaceImageFilter);

    /** Some convenient typedefs. */
    typedef TInputImage1                           Input1ImageType;
    typedef typename Input1ImageType::ConstPointer Input1ImageConstPointer;
    typedef typename Input1ImageType::Pointer      Input1ImagePointer;
    typedef typename Input1ImageType::RegionType   Input1ImageRegionType; 
    typedef typename Input1ImageType::PixelType    Input1ImagePixelType; 
    
    typedef TInputImage2                           Input2ImageType;
    typedef typename Input2ImageType::ConstPointer Input2ImageConstPointer;
    typedef typename Input2ImageType::Pointer      Input2ImagePointer;
    typedef typename Input2ImageType::RegionType   Input2ImageRegionType; 
    typedef typename Input2ImageType::PixelType    Input2ImagePixelType; 
    
    typedef TOutputImage                           OutputImageType;
    typedef typename OutputImageType::Pointer      OutputImagePointer;
    typedef typename OutputImageType::RegionType   OutputImageRegionType;
    typedef typename OutputImageType::PixelType    OutputImagePixelType;
    
    /** Connect one of the operands for pixel-wise addition */
    void SetInput1( const TInputImage1 * image1);
    
    /** Connect one of the operands for pixel-wise addition */
    void SetInput2( const TInputImage2 * image2);
    
    // Set type of operation
    typedef enum {
      And = 0, 
      AndNot = 1, 
      Or = 2
    } OperationTypeEnumeration;
    itkGetMacro(OperationType, OperationTypeEnumeration);
    itkSetMacro(OperationType, OperationTypeEnumeration);

    // LabelImage information (BG and FG)
    void SetBackgroundValue1(Input1ImagePixelType p);
    void SetBackgroundValue2(Input2ImagePixelType p);
    void SetBackgroundValue(OutputImagePixelType p);
    void SetForegroundValue(OutputImagePixelType p);

    /** ImageDimension constants */
    itkStaticConstMacro(InputImage1Dimension, unsigned int, TInputImage1::ImageDimension);
    itkStaticConstMacro(InputImage2Dimension, unsigned int, TInputImage2::ImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

    // I dont want to verify inputs information
    virtual void VerifyInputInformation() ITK_OVERRIDE { }
    
  protected:
    BooleanOperatorLabelImageFilter();
    virtual ~BooleanOperatorLabelImageFilter() {}
    
    virtual void GenerateOutputInformation() ITK_OVERRIDE;
    virtual void GenerateInputRequestedRegion() ITK_OVERRIDE;
    virtual void GenerateData() ITK_OVERRIDE;
    
    // Do not release date to keep input in memory and continue ... 
    virtual void ReleaseInputs() ITK_OVERRIDE { }
    
    Input1ImagePixelType mBackgroundValue1;
    Input2ImagePixelType mBackgroundValue2;
    OutputImagePixelType mBackgroundValue;
    OutputImagePixelType mForegroundValue;
    
    Input1ImageRegionType input1Region;
    Input2ImageRegionType input2Region;
    OutputImageRegionType outputRegion;
    
    OperationTypeEnumeration m_OperationType;
    
    template<class Iter1, class Iter2> void LoopAndNot(Iter1 it1, Iter1 it2, Iter2 ot);
    template<class Iter1, class Iter2> void LoopAnd(Iter1 it1, Iter1 it2, Iter2 ot);
    template<class Iter1, class Iter2> void LoopOr(Iter1 it1, Iter1 it2, Iter2 ot);
    
  private:
    BooleanOperatorLabelImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkBooleanOperatorLabelImageFilter.txx"
#endif

#endif
