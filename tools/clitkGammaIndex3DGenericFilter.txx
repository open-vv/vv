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
#ifndef CLITKGAMMAINDEX3DGENERICFILTER_TXX
#define CLITKGAMMAINDEX3DGENERICFILTER_TXX

#include "clitkImageCommon.h"

#include "itkApproximateSignedDistanceMapImageFilter.h"
#include "itkAddImageFilter.h"
#include <itkSquareImageFilter.h>
#include <itkDivideImageFilter.h>
#include <itkSqrtImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
GammaIndex3DGenericFilter<args_info_type>::GammaIndex3DGenericFilter()
  :ImageToImageGenericFilter<Self>("GammaIndex3DGenericFilter"), mDose(3), mDistance(3)
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  //InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void GammaIndex3DGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void GammaIndex3DGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;

  // Set value
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  mDistance = mArgsInfo.distance_arg;
  mDose = mArgsInfo.dose_arg;

  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

  if (mArgsInfo.input1_given) this->AddInputFilename(mArgsInfo.input1_arg);
  if (mArgsInfo.input2_given) this->AddInputFilename(mArgsInfo.input2_arg);

  if (mArgsInfo.output_given) this->SetOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<class ImageType>
void GammaIndex3DGenericFilter<args_info_type>::UpdateWithInputImageType()
{
  // Read input1
  typename ImageType::Pointer input1 = this->template GetInput<ImageType>(0);

  // Set input image iterator
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  IteratorType it(input1, input1->GetLargestPossibleRegion());

  // typedef input2
  typename ImageType::Pointer input2 = this->template GetInput<ImageType>(1);

  //Create output
  typename ImageType::Pointer output = ImageType::New();
  output->SetRegions(input1->GetLargestPossibleRegion());
  output->SetOrigin(input1->GetOrigin());
  output->SetSpacing(input1->GetSpacing());
  output->Allocate();
  
  int sizeTotal = input1->GetLargestPossibleRegion().GetSize(0)*input1->GetLargestPossibleRegion().GetSize(1)*input1->GetLargestPossibleRegion().GetSize(2);
  int sizeTemp(0);
  double minSpacing(std::numeric_limits<double>::max());
  for (unsigned int temp=0; temp < input1->GetSpacing().Size(); ++temp)
  {
    if  (input1->GetSpacing()[temp] < minSpacing)
      minSpacing = input1->GetSpacing()[temp];
  }
  

  while (!it.IsAtEnd())
  {
    double doseRef = it.Get();
    
    //Create a region around it.Get()
    typename ImageType::RegionType smallRegion;
    typename ImageType::RegionType tempRegion;
    smallRegion=input2->GetLargestPossibleRegion();

    typename ImageType::SizeType sizeRegion;
    sizeRegion.Fill(2*mDistance/minSpacing+1); //5 voxels de cote
    
    typename ImageType::OffsetType offsetRegion;
    offsetRegion.Fill((sizeRegion[0]-1)/2);

    typename ImageType::IndexType startRegion;
    startRegion=it.GetIndex();
    startRegion=startRegion-offsetRegion;
    
    tempRegion.SetIndex(startRegion);
    tempRegion.SetSize(sizeRegion);
    smallRegion.Crop(tempRegion);

    //compute the dose difference
    typedef itk::AddImageFilter <ImageType, ImageType, ImageType> AddImageFilterType;
    typename AddImageFilterType::Pointer addImageFilter = AddImageFilterType::New();
    addImageFilter->SetInput(input2);
    addImageFilter->SetConstant2(-doseRef);
    addImageFilter->GetOutput()->SetRequestedRegion(smallRegion);
    addImageFilter->Update();

    //compute the distance
    typename ImageType::Pointer distanceMap = ImageType::New();
    distanceMap->SetRegions(smallRegion);
    distanceMap->SetOrigin(input1->GetOrigin());
    distanceMap->SetSpacing(input1->GetSpacing());
    distanceMap->Allocate();
    distanceMap->FillBuffer(0.0);
    //distanceMap->SetPixel(it.GetIndex(),1.0);

    typedef  itk::ApproximateSignedDistanceMapImageFilter< ImageType, ImageType  > ApproximateSignedDistanceMapImageFilterType;
    typename ApproximateSignedDistanceMapImageFilterType::Pointer approximateSignedDistanceMapImageFilter = ApproximateSignedDistanceMapImageFilterType::New();
    approximateSignedDistanceMapImageFilter->SetInput(distanceMap);
    approximateSignedDistanceMapImageFilter->SetInsideValue(1.0);
    approximateSignedDistanceMapImageFilter->SetOutsideValue(0.0);
    approximateSignedDistanceMapImageFilter->GetOutput()->SetRequestedRegion(smallRegion);
    approximateSignedDistanceMapImageFilter->Update();

    //Square and Divide by dose & distance criteria
    typedef itk::SquareImageFilter <ImageType, ImageType> SquareImageFilterType;
    typename SquareImageFilterType::Pointer squareImageFilter = SquareImageFilterType::New();
    squareImageFilter->SetInput(addImageFilter->GetOutput());
    squareImageFilter->GetOutput()->SetRequestedRegion(smallRegion);
    squareImageFilter->Update();

    typedef itk::DivideImageFilter <ImageType, ImageType, ImageType > DivideImageFilterType;
    typename DivideImageFilterType::Pointer divideImageFilter = DivideImageFilterType::New ();
    divideImageFilter->SetInput1(squareImageFilter->GetOutput());
    divideImageFilter->SetConstant2(mDose*mDose);
    divideImageFilter->GetOutput()->SetRequestedRegion(smallRegion);
    divideImageFilter->Update();

    typename SquareImageFilterType::Pointer squareImageFilter2 = SquareImageFilterType::New();
    squareImageFilter2->SetInput(approximateSignedDistanceMapImageFilter->GetOutput());
    squareImageFilter2->GetOutput()->SetRequestedRegion(smallRegion);
    squareImageFilter2->Update();

    typename DivideImageFilterType::Pointer divideImageFilter2 = DivideImageFilterType::New ();
    divideImageFilter2->SetInput1(squareImageFilter2->GetOutput());
    divideImageFilter2->SetConstant2(mDistance*mDistance);
    divideImageFilter2->GetOutput()->SetRequestedRegion(smallRegion);
    divideImageFilter2->Update();

    //Sum the 2 images and take the square root
    typename AddImageFilterType::Pointer addImageFilter2 = AddImageFilterType::New();
    addImageFilter2->SetInput1(divideImageFilter2->GetOutput());
    addImageFilter2->SetInput2(divideImageFilter2->GetOutput());
    addImageFilter2->GetOutput()->SetRequestedRegion(smallRegion);
    addImageFilter2->Update();

    typedef itk::SqrtImageFilter<ImageType, ImageType> SqrtFilterType;
    typename SqrtFilterType::Pointer sqrtFilter = SqrtFilterType::New();
    sqrtFilter->SetInput(addImageFilter2->GetOutput());
    sqrtFilter->GetOutput()->SetRequestedRegion(smallRegion);
    sqrtFilter->Update();

    //find min
    typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;
    typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(sqrtFilter->GetOutput());
    //imageCalculatorFilter->GetOutput()->SetRequestedRegion(smallRegion);
    imageCalculatorFilter->ComputeMinimum();

    //Set the value
    output->SetPixel(it.GetIndex(),imageCalculatorFilter->GetMinimum());
    ++it;
    
    ++sizeTemp;
    std::cout << (double)sizeTemp / sizeTotal * 100.0 << std::endl;
  }

  this->template SetNextOutput<ImageType>(output);

}
//--------------------------------------------------------------------

} // end namespace

#endif  //#define CLITKGAMMAINDEX3DGENERICFILTER_TXX
