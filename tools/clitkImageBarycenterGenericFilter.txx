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
#ifndef clitkImageBarycenterGenericFilter_txx
#define clitkImageBarycenterGenericFilter_txx

/* =================================================
 * @file   clitkImageBarycenterGenericFilter.txx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   29 june 2009
 *
 * @brief
 *
 ===================================================*/

// itk include
#include "itkConnectedComponentImageFilter.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include <clitkCommon.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
ImageBarycenterGenericFilter<args_info_type>::ImageBarycenterGenericFilter():
  ImageToImageGenericFilter<Self>("ImageBarycenter")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  //InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void ImageBarycenterGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void ImageBarycenterGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();

  if (mArgsInfo.input_given) {
    this->SetInputFilename(mArgsInfo.input_arg);
  }
  if (mArgsInfo.output_given) {
    this->SetOutputFilename(mArgsInfo.output_arg);
  }
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class args_info_type>
template<class InputImageType>
void
ImageBarycenterGenericFilter<args_info_type>::UpdateWithInputImageType()
{

  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);

  // Main filter
  typedef typename InputImageType::PixelType PixelType;
  typedef itk::Image<unsigned char, InputImageType::ImageDimension> OutputImageType;

  // Filter
    typedef unsigned short                                LabelType;
    typedef itk::ShapeLabelObject< LabelType, InputImageType::ImageDimension > ShapeLabelObjectType;
    typedef itk::LabelMap< ShapeLabelObjectType >         LabelMapType;

    typedef itk::ConnectedComponentImageFilter <InputImageType, OutputImageType > ConnectedComponentImageFilterType;
    typedef itk::LabelImageToShapeLabelMapFilter< OutputImageType, LabelMapType> I2LType;
    typename ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New ();
    connected->SetInput(input);
    connected->Update();
    typedef itk::LabelImageToShapeLabelMapFilter< OutputImageType, LabelMapType> I2LType;
    typename I2LType::Pointer i2l = I2LType::New();
    i2l->SetInput( connected->GetOutput() );
    i2l->SetComputePerimeter(true);
    i2l->Update();
    LabelMapType *labelMap = i2l->GetOutput();
    std::cout << "The input image has " << labelMap->GetNumberOfLabelObjects() << " labels." << std::endl;
    // Retrieve all attributes
    //Only 1 object so:
    unsigned int n = 0;
    //for (unsigned int n = 0; n < labelMap->GetNumberOfLabelObjects(); ++n)
    //{
      ShapeLabelObjectType *labelObject = labelMap->GetNthLabelObject(n);
      std::cout << "Label: "
                << typename itk::NumericTraits< typename LabelMapType::LabelType >::PrintType(labelObject->GetLabel()) << std::endl;
      //std::cout << "    BoundingBox: "
      //          << labelObject->GetBoundingBox() << std::endl;
      //std::cout << "    NumberOfPixels: "
      //          << labelObject->GetNumberOfPixels() << std::endl;
      //std::cout << "    PhysicalSize: "
      //          << labelObject->GetPhysicalSize() << std::endl;
      std::cout << "    Centroid: "
                << labelObject->GetCentroid() << std::endl;
      //std::cout << "    NumberOfPixelsOnBorder: "
      //          << labelObject->GetNumberOfPixelsOnBorder() << std::endl;
      //std::cout << "    PerimeterOnBorder: "
      //          << labelObject->GetPerimeterOnBorder() << std::endl;
      //std::cout << "    FeretDiameter: "
      //          << labelObject->GetFeretDiameter() << std::endl;
      //std::cout << "    PrincipalMoments: "
      //          << labelObject->GetPrincipalMoments() << std::endl;
      //std::cout << "    PrincipalAxes: "
      //          << labelObject->GetPrincipalAxes() << std::endl;
      //std::cout << "    Elongation: "
      //          << labelObject->GetElongation() << std::endl;
      //std::cout << "    Perimeter: "
      //          << labelObject->GetPerimeter() << std::endl;
      //std::cout << "    Roundness: "
      //          << labelObject->GetRoundness() << std::endl;
      //std::cout << "    EquivalentSphericalRadius: "
      //          << labelObject->GetEquivalentSphericalRadius() << std::endl;
      //std::cout << "    EquivalentSphericalPerimeter: "
      //          << labelObject->GetEquivalentSphericalPerimeter() << std::endl;
      //std::cout << "    EquivalentEllipsoidDiameter: "
      //          << labelObject->GetEquivalentEllipsoidDiameter() << std::endl;
      //std::cout << "    Flatness: "
      //          << labelObject->GetFlatness() << std::endl;
      //std::cout << "    PerimeterOnBorderRatio: "
      //          << labelObject->GetPerimeterOnBorderRatio() << std::endl;
      //}

    typename OutputImageType::Pointer outputImage = OutputImageType::New();
    typename OutputImageType::RegionType region_outputImage(input->GetLargestPossibleRegion());
    outputImage->SetRegions(region_outputImage);
    outputImage->SetOrigin(input->GetOrigin());
    outputImage->SetSpacing(input->GetSpacing());
    outputImage->SetDirection(input->GetDirection());
    outputImage->Allocate();
    outputImage->FillBuffer(0);

    typename InputImageType::IndexType barycenterIndex;
    input->TransformPhysicalPointToIndex(labelObject->GetCentroid(),barycenterIndex);

    outputImage->SetPixel(barycenterIndex,1);
    // Write/Save results
    this->template SetNextOutput<OutputImageType>(outputImage);
}
//--------------------------------------------------------------------


}//end clitk

#endif //#define clitkImageBarycenterGenericFilter_txx
