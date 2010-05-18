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
#include <QProgressDialog>
#include <QWidget>
#include <itkImage.h>
#include <itkWarpImageFilter.h>
#include <itkJoinSeriesImageFilter.h>
#include <itkImageFileReader.h>
#include <itkSubtractImageFilter.h>
#include <itkDisplacementFieldJacobianDeterminantFilter.h>
#include "vvToITK.h"
#include "vvFromITK.h"

#include "vvImageWarp.h"
#include "clitkCommon.h"

//====================================================================
vvImageWarp::vvImageWarp(vvImage::Pointer input,vvImage::Pointer vf,unsigned int ref_image,QWidget* parent):
  mRefImage(ref_image),
  parent_window(parent),
  mInputImage(input),
  mVF(vf)
{}


//====================================================================
template<unsigned int Dim>
void vvImageWarp::Update_WithDim()
{
#define TRY_TYPE(TYPE)							\
  if (clitk::IsSameType<TYPE>(mInputImage->GetScalarTypeAsString())) { this->Update_WithDimAndPixelType<Dim, TYPE>(); return; }
//  TRY_TYPE(signed char);
//  TRY_TYPE(uchar);
  TRY_TYPE(short);
//  TRY_TYPE(ushort);
//  TRY_TYPE(int); // no uint ...
//  TRY_TYPE(float);
  //TRY_TYPE(double);
#undef TRY_TYPE

  std::string list = clitk::CreateListOfTypes<char, clitk::uchar, short, ushort, int, float, double>();
  std::cerr << "Error, I don't know the type '" << mInputImage->GetScalarTypeAsString() << "' for the input image. "
            << std::endl << "Known types are " << list << std::endl;
  exit(0);
}

//====================================================================
template<unsigned int Dim, class PixelType>
void vvImageWarp::Update_WithDimAndPixelType()
{
  QProgressDialog progress(parent_window);
  progress.setCancelButton(0);
  progress.setLabelText("Computing warped and ventilation images...");
  progress.show();
  typedef itk::Image<PixelType,Dim> ImageType;
  typedef itk::Image<float,Dim> JacobianImageType;
  typedef itk::Image<itk::Vector<float,Dim>,Dim> VectorImageType;
  typedef std::vector<typename ImageType::ConstPointer> ImageSeriesType;
  ImageSeriesType input = vvImageToITKImageVector<Dim,PixelType>(mInputImage);
  ImageSeriesType output;

  typename itk::VTKImageToImageFilter<VectorImageType>::Pointer vf_connector=
    itk::VTKImageToImageFilter<VectorImageType>::New();

  //Warp, then join, then convert to vv
  typedef itk::WarpImageFilter<ImageType,ImageType,VectorImageType> WarpFilterType;
  typedef itk::DisplacementFieldJacobianDeterminantFilter<VectorImageType> JacobianFilterType;
  vvImage::Pointer result=vvImage::New();
  typedef itk::JoinSeriesImageFilter< ImageType,itk::Image<PixelType,Dim+1> > JoinFilterType;
  typedef itk::JoinSeriesImageFilter< JacobianImageType,itk::Image<float,Dim+1> > JacobianJoinFilterType;
  typename JoinFilterType::Pointer join=JoinFilterType::New();
  typename JoinFilterType::Pointer diff_join=JoinFilterType::New();
  typename JacobianJoinFilterType::Pointer jacobian_join=JacobianJoinFilterType::New();
  join->SetSpacing(1);
  join->SetOrigin(0); //Set the temporal origin
  diff_join->SetSpacing(1);
  diff_join->SetOrigin(0);
  jacobian_join->SetSpacing(1);
  jacobian_join->SetOrigin(0);
  typedef itk::SubtractImageFilter<ImageType,ImageType,ImageType> DiffFilter;
  std::vector<typename ImageType::Pointer> warped_images;

  for (unsigned int num = 0; num < input.size(); num++) {
    typename WarpFilterType::Pointer warp_filter=WarpFilterType::New();
    typename JacobianFilterType::Pointer jacobian_filter=JacobianFilterType::New();
    jacobian_filter->SetUseImageSpacingOn();
    vf_connector->SetInput(mVF->GetVTKImages()[num]);
    warp_filter->SetInput(input[num]);
    warp_filter->SetDeformationField(vf_connector->GetOutput());
    jacobian_filter->SetInput(vf_connector->GetOutput());
    warp_filter->SetOutputSpacing(input[num]->GetSpacing());
    warp_filter->SetOutputOrigin(input[num]->GetOrigin());
    warp_filter->SetEdgePaddingValue(-1000);
    warp_filter->Update();
    jacobian_filter->Update();
    warped_images.push_back(warp_filter->GetOutput());
    jacobian_join->PushBackInput(jacobian_filter->GetOutput());
    join->PushBackInput(warp_filter->GetOutput());
    progress.setValue(progress.value()+1);
  }
  for (typename std::vector<typename ImageType::Pointer>::const_iterator i = warped_images.begin(); i!=warped_images.end(); i++) {
    typename DiffFilter::Pointer diff_filter = DiffFilter::New();
    diff_filter->SetInput2(*i);
    diff_filter->SetInput1(*(warped_images.begin()+mRefImage));
    diff_filter->Update();
    diff_join->PushBackInput(diff_filter->GetOutput());
    progress.setValue(progress.value()+1);
  }
  join->Update();
  diff_join->Update();
  jacobian_join->Update();
  mWarpedImage = vvImageFromITK<Dim+1,PixelType>(join->GetOutput());
  mDiffImage = vvImageFromITK<Dim+1,PixelType>(diff_join->GetOutput());
  mJacobianImage = vvImageFromITK<Dim+1,float>(jacobian_join->GetOutput());
  //mJacobianImage = vvImageFromITK<Dim,float>(temporal_filter->GetOutput());
}
//====================================================================
//

bool vvImageWarp::ComputeWarpedImage()
{
  for (int i=0; i<mInputImage->GetNumberOfDimensions(); i++) {
    if (mInputImage->GetSpacing()[i] != mVF->GetSpacing()[i])
      return false;
  }
  switch (mInputImage->GetNumberOfDimensions()) {
//      case 2: this->Update_WithDim<2>(); break;;
//      case 3: this->Update_WithDim<3>(); break;;
  case 4:
    this->Update_WithDim<3>();
    break;;
  default:
    DD("Error: dimension not handled.");
  }
  return true;
}
