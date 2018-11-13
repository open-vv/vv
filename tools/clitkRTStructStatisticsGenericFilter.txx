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
#ifndef CLITKRTSTRUCTSTATISTICSGENERICFILTER_TXX
#define CLITKRTSTRUCTSTATISTICSGENERICFILTER_TXX

#include "clitkImageCommon.h"

#include "itkConnectedComponentImageFilter.h"
#include "itkLabelImageToShapeLabelMapFilter.h"

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
RTStructStatisticsGenericFilter<args_info_type>::RTStructStatisticsGenericFilter()
  :ImageToImageGenericFilter<Self>("RTStructStatisticsGenericFilter")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void RTStructStatisticsGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void RTStructStatisticsGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;

  // Set value
  this->SetIOVerbose(mArgsInfo.verbose_flag);

  if (mArgsInfo.input_given) this->AddInputFilename(mArgsInfo.input_arg);

  }
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<class ImageType>
void RTStructStatisticsGenericFilter<args_info_type>::UpdateWithInputImageType()
{
  // Read mask input
  typedef itk::Image<unsigned char, ImageType::ImageDimension> MaskInputImageType;
  typename MaskInputImageType::Pointer mask;
  mask = this->template GetInput<MaskInputImageType>(0);

  //Create the Shape Label Map from the mask
  typedef itk::Image< unsigned char, ImageType::ImageDimension > OutputImageType;
  typedef itk::ShapeLabelObject< unsigned char, ImageType::ImageDimension > ShapeLabelObjectType;
  typedef itk::LabelMap< ShapeLabelObjectType > LabelMapType;
  typedef itk::ConnectedComponentImageFilter <MaskInputImageType, OutputImageType > ConnectedComponentImageFilterType;
  typedef itk::LabelImageToShapeLabelMapFilter< OutputImageType, LabelMapType> I2LType;

  typename ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New ();
  connected->SetInput(mask);
  connected->FullyConnectedOn();
  connected->Update();

  //Create a map to contain all connectedComponent (even a little pixel)
  typename I2LType::Pointer i2l = I2LType::New();
  i2l->SetInput( connected->GetOutput() );
  i2l->SetComputePerimeter(true);
  i2l->Update();

  // Retrieve the biggest component
  LabelMapType *labelMap = i2l->GetOutput();
  int largestComponent(0);
  int nbPixel(0);
  for (unsigned int n = 0; n < labelMap->GetNumberOfLabelObjects(); ++n)
  {
    ShapeLabelObjectType *labelObject = labelMap->GetNthLabelObject(n);
    if (labelObject->GetNumberOfPixels() > nbPixel)
    {
        nbPixel = labelObject->GetNumberOfPixels();
        largestComponent = n;
    }
  }

  //Write statitistics on the largest component
  ShapeLabelObjectType *labelObject = labelMap->GetNthLabelObject(largestComponent);
  std::cout << " Centroid: " << std::endl;
  std::cout << labelObject->GetCentroid()[0] << std::endl;
  std::cout << labelObject->GetCentroid()[1] << std::endl;
  std::cout << labelObject->GetCentroid()[2] << std::endl;
  std::cout << " Roundness: " << std::endl;
  std::cout << labelObject->GetRoundness() << std::endl;

}
//--------------------------------------------------------------------



} // end namespace

#endif  //#define CLITKRTSTRUCTSTATISTICSGENERICFILTER_TXX
