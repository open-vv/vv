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

#ifndef CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX

#include "clitkCommon.h"
#include "clitkBooleanOperatorLabelImageFilter.h"
#include "clitkSegmentationUtils.h"

namespace clitk {

  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::
  BooleanOperatorLabelImageFilter():itk::InPlaceImageFilter<TInputImage1, TOutputImage>() {
    this->SetNumberOfRequiredInputs( 2 );
    this->InPlaceOn();    
    mBackgroundValue1 = 0;
    mBackgroundValue2 = 0;
    mBackgroundValue  = 0;
    mForegroundValue = 1;
    m_OperationType = AndNot;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::
  SetInput1(const TInputImage1 * image1) {
    // Process object is not const-correct so the const casting is required.
    this->SetNthInput(0, const_cast<TInputImage1 *>( image1 ));
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::  
  SetBackgroundValue1(Input1ImagePixelType p) {
    mBackgroundValue1 = p;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::  
  SetBackgroundValue2(Input2ImagePixelType p) {
    mBackgroundValue2 = p;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::  
  SetBackgroundValue(OutputImagePixelType p) {
    mBackgroundValue = p;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::  
  SetForegroundValue(OutputImagePixelType p) {
    mForegroundValue = p;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::
  SetInput2(const TInputImage2 * image2) {
    // Process object is not const-correct so the const casting is required.
    this->SetNthInput(1, const_cast<TInputImage1 *>( image2 ));
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::
  GenerateOutputInformation() { 

    // Get input pointers
    Input1ImagePointer input1 = dynamic_cast<TInputImage1*>(itk::ProcessObject::GetInput(0));
    Input2ImagePointer input2 = dynamic_cast<TInputImage2*>(itk::ProcessObject::GetInput(1));
     
    // Check spacing
    static const unsigned int Dim = Input1ImageType::ImageDimension;
    for(unsigned int i=0; i<Dim; i++) {
      if (input1->GetSpacing()[i] != input2->GetSpacing()[i]) {
        itkExceptionMacro(<< "Input 1&2 must have the same spacing. " << std::endl
                          << "\t input1 =  " << input1->GetSpacing() << std::endl
                          << "\t input2 =  " << input2->GetSpacing() << std::endl);
      }
      // if (input1->GetLargestPossibleRegion().GetSize()[i] != input2->GetLargestPossibleRegion().GetSize()[i]) {
//         itkExceptionMacro(<< "Input 1&2 must have the same size. " << std::endl
//                           << "\t input1 =  " << input1->GetLargestPossibleRegion().GetSize() << std::endl
//                           << "\t input2 =  " << input2->GetLargestPossibleRegion().GetSize() << std::endl);
//       }
    }

    // Perform default implementation
    Superclass::GenerateOutputInformation();

    // Get output pointer
    OutputImagePointer outputImage = this->GetOutput(0);

    // If InPlace, do not create output
    // DD(this->GetInPlace());
    if (this->GetInPlace() && this->CanRunInPlace()) {
      OutputImagePointer inputAsOutput
        = dynamic_cast<TOutputImage *>(const_cast<TInputImage1 *>(this->GetInput()));
      inputAsOutput->SetRequestedRegion(outputImage->GetLargestPossibleRegion());
      inputAsOutput->SetBufferedRegion(outputImage->GetLargestPossibleRegion());  
      // inputAsOutput->SetRegions(outputImage->GetLargestPossibleRegion());
      this->GraftOutput( inputAsOutput );
    }
    else {
      outputImage->SetRequestedRegion(outputImage->GetLargestPossibleRegion());
      outputImage->SetBufferedRegion(outputImage->GetLargestPossibleRegion());  
      outputImage->SetRegions(outputImage->GetLargestPossibleRegion());
      outputImage->Allocate();
      OutputImagePointer inputAsOutput
        = dynamic_cast<TOutputImage *>(const_cast<TInputImage1 *>(this->GetInput()));
      CopyValues<OutputImageType>(inputAsOutput, outputImage);
    }

    // Compute intersection bounding box (in physical coordinate) and regions (in pixel coordinate)
    typedef itk::BoundingBox<unsigned long, Dim> BBType;
    typename BBType::Pointer bbInput1 = BBType::New();    
    ComputeBBFromImageRegion<Input1ImageType>(input1, input1->GetLargestPossibleRegion(), bbInput1);    
    typename BBType::Pointer bbInput2 = BBType::New();    
    ComputeBBFromImageRegion<Input2ImageType>(input2, input2->GetLargestPossibleRegion(), bbInput2);
    typename BBType::Pointer bbOutput = BBType::New();    
    ComputeBBFromImageRegion<OutputImageType>(outputImage, outputImage->GetLargestPossibleRegion(), bbOutput);
    
    typename BBType::Pointer bb = BBType::New();    
    ComputeBBIntersection<Dim>(bb, bbInput1, bbInput2);
    ComputeBBIntersection<Dim>(bb, bb, bbOutput);
 
    ComputeRegionFromBB<Input1ImageType>(input1, bb, input1Region);
    ComputeRegionFromBB<Input2ImageType>(input2, bb, input2Region);
    ComputeRegionFromBB<OutputImageType>(outputImage, bb, outputRegion);

  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::
  GenerateInputRequestedRegion() {
    // Call default
    itk::InPlaceImageFilter<TInputImage1, TOutputImage>::GenerateInputRequestedRegion();
    // Get input pointers and set requested region to common region
    Input1ImagePointer input1 = dynamic_cast<TInputImage1*>(itk::ProcessObject::GetInput(0));
    Input2ImagePointer input2 = dynamic_cast<TInputImage2*>(itk::ProcessObject::GetInput(1));
    input1->SetRequestedRegion(input1Region);
    input2->SetRequestedRegion(input2Region);  
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage1, class TInputImage2, class TOutputImage>
  void 
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>::
  GenerateData() {
    // Get input pointers
    Input1ImageConstPointer input1 = dynamic_cast<const TInputImage1*>(itk::ProcessObject::GetInput(0));
    Input2ImageConstPointer input2 = dynamic_cast<const TInputImage2*>(itk::ProcessObject::GetInput(1));
    
    // Get output pointer
    OutputImagePointer output = this->GetOutput(0);
    
    // Get Region iterators
    itk::ImageRegionConstIterator<Input1ImageType> it1(input1, input1Region);
    itk::ImageRegionConstIterator<Input2ImageType> it2(input2, input2Region);
    itk::ImageRegionIterator<OutputImageType>      ot (output, outputRegion);
    it1.GoToBegin();
    it2.GoToBegin();
    ot.GoToBegin();

    switch (m_OperationType) {
    case AndNot: LoopAndNot(it1, it2, ot);  break;
    case And: LoopAnd(it1, it2, ot);  break;
    }
  }
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
#define LOOP_BEGIN(FUNCTION_NAME)                                       \
  template <class TInputImage1, class TInputImage2, class TOutputImage> \
  template <class Iter1, class Iter2>                                   \
  void                                                                  \
  BooleanOperatorLabelImageFilter<TInputImage1, TInputImage2, TOutputImage>:: \
  FUNCTION_NAME(Iter1 it1, Iter1 it2, Iter2 ot) {                       \
  while (!ot.IsAtEnd()) {
  
#define LOOP_END ++it1; ++it2; ++ot; }}
  //--------------------------------------------------------------------
  

  //--------------------------------------------------------------------
  LOOP_BEGIN(LoopAndNot)
  if ((it1.Get() != mBackgroundValue1) && (it2.Get() == mBackgroundValue2)) { ot.Set(mForegroundValue); }
  else { ot.Set(mBackgroundValue); }
  LOOP_END
  //--------------------------------------------------------------------

 
  //--------------------------------------------------------------------
  LOOP_BEGIN(LoopAnd)
  if ((it1.Get() != mBackgroundValue1) && (it2.Get() != mBackgroundValue2)) { ot.Set(mForegroundValue); }
  else { ot.Set(mBackgroundValue); }
  LOOP_END
  //--------------------------------------------------------------------

 
}//end clitk
 
#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX
