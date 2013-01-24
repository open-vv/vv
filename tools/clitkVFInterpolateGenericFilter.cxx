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
#ifndef CLITKVFRESAMPLEGENERICFILTER_CXX
#define CLITKVFRESAMPLEGENERICFILTER_CXX

#include "clitkVFInterpolateGenericFilter.h"
#include "itkInterpolateImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkNthElementImageAdaptor.h"

//--------------------------------------------------------------------
clitk::VFInterpolateGenericFilter::VFInterpolateGenericFilter():
  clitk::ImageToImageGenericFilter<Self>("VFInterpolate")
{
  //InitializeImageType<2>();
  InitializeImageType<3>();
  //  InitializeImageType<4>();
  mInterpolatorName = "nn";
  mBSplineOrder=3;
  mDistance=0;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void clitk::VFInterpolateGenericFilter::InitializeImageType()
{
  //typedef itk::Vector<float,Dim> v3f;
  //ADD_IMAGE_TYPE(Dim, v3f);
//   ADD_VEC_IMAGE_TYPE(Dim,2,double)
  ADD_VEC_IMAGE_TYPE(Dim,3,float)
  ADD_VEC_IMAGE_TYPE(Dim,3,double)
  
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
void clitk::VFInterpolateGenericFilter::UpdateWithInputImageType()
{

  if (m_NbOfComponents == 1) {
    std::cerr << "Error, only one components ? Use clitkImageInterpolate instead." << std::endl;
    exit(0);
  }
  typedef typename ImageType::PixelType PixelType;
//   if (m_NbOfComponents == 2) Update_WithDimAndPixelTypeAndComponent<ImageType::ImageDimension,PixelType,2>();
  if (m_NbOfComponents == 3) Update_WithDimAndPixelTypeAndComponent<ImageType::ImageDimension,PixelType,3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim, class PixelType, unsigned int DimCompo>
void clitk::VFInterpolateGenericFilter::Update_WithDimAndPixelTypeAndComponent()
{
  // Reading input
  //  typedef itk::Vector<PixelType, DimCompo> DisplacementType;
  typedef PixelType DisplacementType;
  typedef itk::Image< DisplacementType, Dim > ImageType;

  typename ImageType::Pointer input1 = clitk::readImage<ImageType>(m_InputFilenames[0], m_IOVerbose);
  typename ImageType::Pointer input2 = clitk::readImage<ImageType>(mInputFilename2, m_IOVerbose);
  
  // Main filter
  typename ImageType::Pointer outputImage = ComputeImage<ImageType>(input1, input2);

  // Write results
  SetNextOutput<ImageType>(outputImage);
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
clitk::VFInterpolateGenericFilter::ComputeImage(typename ImageType::Pointer inputImage1, typename ImageType::Pointer inputImage2)
{

  // Some typedefs
  typedef itk::Image<typename ImageType::PixelType::ValueType, ImageType::ImageDimension> ScalarImageType;
  typedef itk::Image<typename ImageType::PixelType::ValueType, ImageType::ImageDimension + 1> InterpolationImageType;
  typedef itk::NthElementImageAdaptor<ImageType, typename ImageType::PixelType::ValueType> ImageAdaptorType;
  typename ImageAdaptorType::Pointer adaptor1 = ImageAdaptorType::New();
  typename ImageAdaptorType::Pointer adaptor2 = ImageAdaptorType::New();
  adaptor1->SetImage(inputImage1);
  adaptor2->SetImage(inputImage2);
  
  // Create Image Filter
  typedef itk::InterpolateImageFilter<ImageAdaptorType, ScalarImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput1(adaptor1);
  filter->SetInput2(adaptor2);
  filter->SetDistance(mDistance);

  // Select interpolator
  if (mInterpolatorName == "nn") {
    typedef itk::NearestNeighborInterpolateImageFunction<InterpolationImageType> InterpolatorType;
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    filter->SetInterpolator(interpolator);
  } else {
    if (mInterpolatorName == "linear") {
      typedef itk::LinearInterpolateImageFunction<InterpolationImageType> InterpolatorType;
      typename InterpolatorType::Pointer interpolator =  InterpolatorType::New();
      filter->SetInterpolator(interpolator);
    } else {
      std::cerr << "Sorry, I do not know the interpolator (for vector field) '" << mInterpolatorName
                << "'. Known interpolators are :  nn, linear" << std::endl;
      exit(0);
    }
  }

  typename ImageType::Pointer output = ImageType::New();
  typename ImageAdaptorType::Pointer adaptorOutput = ImageAdaptorType::New();
  output->CopyInformation(inputImage1);
  output->SetRegions(inputImage1->GetLargestPossibleRegion());
  output->Allocate();

  typedef itk::ImageRegionIterator<ScalarImageType> IteratorType1;
  typedef itk::ImageRegionIterator<ImageAdaptorType> IteratorType2;
  
  for (unsigned int i = 0; i < ImageType::PixelType::Dimension; i++) {
    adaptor1->SelectNthElement(i);
    adaptor2->SelectNthElement(i);

    // Go !
    try {
      filter->Update();
    } catch( itk::ExceptionObject & err ) {
      std::cerr << "Error while filtering " << m_InputFilenames[0].c_str()
                << " " << err << std::endl;
      exit(0);
    }

    adaptorOutput->SelectNthElement(i);
    adaptorOutput->SetImage(output);
    
    IteratorType1 it1(filter->GetOutput(), filter->GetOutput()->GetLargestPossibleRegion());
    IteratorType2 it2(adaptorOutput, adaptorOutput->GetLargestPossibleRegion());
    
    it1.GoToBegin();
    it2.GoToBegin();
    while ( ! it1.IsAtEnd() ) {
      it2.Set(it1.Get());
      ++it1;
      ++it2;
    }
  }
  
  // Return result
  return output;

}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
void clitk::VFInterpolateGenericFilter::SetInterpolationName(const std::string & inter)
{
  mInterpolatorName = inter;
}
//--------------------------------------------------------------------

#endif

