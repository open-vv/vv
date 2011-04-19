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

//--------------------------------------------------------------------
template<class ImageType>
clitk::MorphoMathFilter<ImageType>::MorphoMathFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<ImageType, ImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0);
  SetForegroundValue(1);
  SetOperationType(0);
  SizeType p;
  p.Fill(1);
  SetRadius(p);
  SetBoundaryToForegroundFlag(false);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
clitk::MorphoMathFilter<ImageType>::~MorphoMathFilter()
{
  // Nothing
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void clitk::MorphoMathFilter<ImageType>::
SetRadiusInMM(PointType & p)
{
  m_RadiusInMM = p;
  m_RadiusInMMIsSet = true;
  m_RadiusIsSet = false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void clitk::MorphoMathFilter<ImageType>::
SetRadius(SizeType & p)
{
  m_Radius = p;
  m_RadiusIsSet = true;
  m_RadiusInMMIsSet = false;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void clitk::MorphoMathFilter<ImageType>::
SetOperationType(int type)
{
  switch (type) {
  case 0: m_OperationType = Erode; return;
  case 1: m_OperationType = Dilate; return;
  case 2: m_OperationType = Open; return;
  case 3: m_OperationType = Close; return;
  case 4: m_OperationType = CondErode; return;
  case 5: m_OperationType = CondDilate; return;
  default: clitkExceptionMacro("Operation type must be between 0-5 (0=Erode, 1=Dilate, 2=Close (erode(dilate(x))), 3=Open (dilate(erode(x))), 4=CondErode, 5=CondDilate)");
  }    
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::MorphoMathFilter<ImageType>::
GenerateInputRequestedRegion() 
{
  // Call default
  itk::ImageToImageFilter<ImageType, ImageType>::GenerateInputRequestedRegion();
  // Get input pointers and set requested region to common region
  ImagePointer input1 = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));
  input1->SetRequestedRegion(input1->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::MorphoMathFilter<ImageType>::
GenerateOutputInformation() 
{
  //---------------------------------
  // Define the images
  //---------------------------------
  ImagePointer m_input = dynamic_cast<ImageType*>(itk::ProcessObject::GetInput(0));

  //---------------------------------
  // Cast into internalimagetype
  //---------------------------------
  typedef itk::CastImageFilter<ImageType, InternalImageType> InputCastImageFilterType;
  typename InputCastImageFilterType::Pointer caster = InputCastImageFilterType::New();
  caster->SetInput(m_input);
  caster->Update();
  input =caster->GetOutput();
  
  //---------------------------------
  // Compute the radius in pixel
  //---------------------------------
  if (m_RadiusInMMIsSet) {
    for(uint i=0; i<ImageType::ImageDimension; i++) {
      m_Radius[i] = (uint)lrint(m_RadiusInMM[i]/input->GetSpacing()[i]);
    }
  }

  //---------------------------------
  // Extend the image if needed
  //---------------------------------
  if (GetExtendSupportFlag()) {
    typedef itk::ConstantPadImageFilter<InternalImageType, InternalImageType> PadFilterType;
    typename PadFilterType::Pointer padFilter = PadFilterType::New();
    padFilter->SetInput(input);
    typename ImageType::SizeType lower;
    typename ImageType::SizeType upper;
    for(uint i=0; i<3; i++) {
      lower[i] = upper[i] = 2*(m_Radius[i]+1);
    }
    padFilter->SetPadLowerBound(lower);
    padFilter->SetPadUpperBound(upper);
    padFilter->Update();
    if (GetVerboseFlag()) std::cout << "Extend the image to 2x " << lower << std::endl;
    input = padFilter->GetOutput();
  }
  
  // Set output size
  ImagePointer outputImage = this->GetOutput(0);
  outputImage->SetRegions(input->GetLargestPossibleRegion());
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::MorphoMathFilter<ImageType>::
GenerateData() 
{
  //---------------------------------
  // Build kernel
  //---------------------------------
  typedef itk::BinaryBallStructuringElement<PixelType,ImageDimension> KernelType;
  KernelType structuringElement;
  if (GetVerboseFlag()) {
    std::cout << "Radius in pixel : " << m_Radius << std::endl;
  }
  structuringElement.SetRadius(m_Radius);
  structuringElement.CreateStructuringElement();

  //---------------------------------
  // Switch according to operation type
  //---------------------------------
  typedef itk::ImageToImageFilter<InternalImageType, InternalImageType> ImageFilterType;
  typename ImageFilterType::Pointer filter; 
  switch(m_OperationType)
    {
    case Erode: {
      typedef itk::BinaryErodeImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
      typename FilterType::Pointer m = FilterType::New();
      m->SetBackgroundValue(this->GetBackgroundValue());
      m->SetForegroundValue(this->GetForegroundValue());
      m->SetBoundaryToForeground(GetBoundaryToForegroundFlag());
      m->SetKernel(structuringElement);
      
      filter=m;
      if (GetVerboseFlag()) std::cout<<"Using the erode filter..."<<std::endl;
      break;
    }

    case Dilate:
      {
        typedef itk::BinaryDilateImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
        typename FilterType::Pointer m = FilterType::New();
        m->SetBackgroundValue(this->GetBackgroundValue());
        m->SetForegroundValue(this->GetForegroundValue());
        m->SetBoundaryToForeground(GetBoundaryToForegroundFlag());
        m->SetKernel(structuringElement);

        filter=m;
        if (GetVerboseFlag()) std::cout<<"Using the dilate filter..."<<std::endl;
        break;
      }

    case Close:
      {
        typedef itk::BinaryMorphologicalClosingImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
        typename FilterType::Pointer m = FilterType::New();
        m->SetForegroundValue(this->GetForegroundValue());
        m->SetSafeBorder(GetBoundaryToForegroundFlag());
        m->SetKernel(structuringElement);

        filter=m;
        if (GetVerboseFlag()) std::cout<<"Using the closing filter..."<<std::endl;
        break;
      }

    case Open:
      {
        typedef itk::BinaryMorphologicalOpeningImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
        typename FilterType::Pointer m = FilterType::New();
        m->SetBackgroundValue(this->GetBackgroundValue());
        m->SetForegroundValue(this->GetForegroundValue());
        m->SetKernel(structuringElement);

        filter=m;
        if (GetVerboseFlag()) std::cout<<"Using the opening filter..."<<std::endl;
        break;
      }

    case CondErode:
      {
        typedef clitk::ConditionalBinaryErodeImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
        typename FilterType::Pointer m = FilterType::New();
        m->SetBackgroundValue(this->GetBackgroundValue());
        m->SetForegroundValue(this->GetForegroundValue());
        m->SetBoundaryToForeground(GetBoundaryToForegroundFlag());
        m->SetKernel(structuringElement);
          
        filter=m;
        if (GetVerboseFlag()) std::cout<<"Using the conditional erode filter..."<<std::endl;
        break;
      }

    case CondDilate:
      {
        typedef clitk::ConditionalBinaryDilateImageFilter<InternalImageType, InternalImageType , KernelType> FilterType;
        typename FilterType::Pointer m = FilterType::New();
        m->SetBackgroundValue(this->GetBackgroundValue());
        m->SetForegroundValue(this->GetForegroundValue());
        m->SetBoundaryToForeground(GetBoundaryToForegroundFlag());
        m->SetKernel(structuringElement);
          
        filter=m;
        if (GetVerboseFlag()) std::cout<<"Using the conditional dilate filter..."<<std::endl;
        break;
      }

    }
  

  //---------------------------------
  // Execute the filter
  //---------------------------------
  filter->SetInput(input);
  filter->Update();

  //---------------------------------
  // Write the output
  //---------------------------------
  typedef itk::CastImageFilter< InternalImageType, ImageType > OutputCastImageFilterType;
  typename OutputCastImageFilterType::Pointer oCaster = OutputCastImageFilterType::New();
  oCaster->SetInput(filter->GetOutput());
  oCaster->Update();
  this->SetNthOutput(0, oCaster->GetOutput());
}
//--------------------------------------------------------------------


