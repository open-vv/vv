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
===========================================================================*/

#ifndef clitkReconstructWithConditionalGrayscaleDilateImageFilter_txx
#define clitkReconstructWithConditionalGrayscaleDilateImageFilter_txx

#include <itkGrayscaleDilateImageFilter.h>

namespace clitk
{

  //-------------------------------------------------------------------
  template<class ImageType>
  ReconstructWithConditionalGrayscaleDilateImageFilter<ImageType>::ReconstructWithConditionalGrayscaleDilateImageFilter()
  {
   m_Verbose=false;
   m_BackgroundValue=0;
   m_IterationNumber = 5;
  }
  //-------------------------------------------------------------------

  //-------------------------------------------------------------------
  template<class ImageType> 
  void 
  ReconstructWithConditionalGrayscaleDilateImageFilter<ImageType>::GenerateData()
  {
    // Get input image 
    typename ImageType::ConstPointer input = this->GetInput();

    // Main loop
    typename ImageType::Pointer output;
    for(int r=0; r<GetIterationNumber(); r++) {
      
      // Create kernel for GrayscaleDilateImageFilter
      typedef itk::BinaryBallStructuringElement<PixelType,ImageDimension > KernelType;
      KernelType k;
      k.SetRadius(1);
      k.CreateStructuringElement();
    
      // Check that BG is 0, because ConditionalGrayscaleDilateImageFilter consider BG is 0
      if (GetBackgroundValue() !=0) {
        FATAL("FATAL -> BG is not 0, check ReconstructWithConditionalGrayscaleDilateImageFilter");
        // TODO -> replace 0 with new label, replace BG with 0 ; reverse at the end
      }
      
      // ConditionalGrayscaleDilateImageFilter -> Dilate only BG value
      typedef itk::ConditionalGrayscaleDilateImageFilter<ImageType, ImageType, KernelType> FilterType;
      typename FilterType::Pointer m = FilterType::New();
      m->SetKernel(k);
      if (r==0) {
        m->SetInput(input); // First time
      }
      else m->SetInput(output);
      m->Update();
      output = m->GetOutput();
    }

    //---------------------------------
    this->SetNthOutput(0, output);
  }

}//end clitk
 
#endif //#define clitkReconstructWithConditionalGrayscaleDilateImageFilter_txx
