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
#ifndef CLITKSUVPEAKGENERICFILTER_TXX
#define CLITKSUVPEAKGENERICFILTER_TXX

#include "clitkImageCommon.h"
#include <itkConvolutionImageFilter.h>

namespace clitk
{

//--------------------------------------------------------------------
template<class args_info_type>
SUVPeakGenericFilter<args_info_type>::SUVPeakGenericFilter()
  :ImageToImageGenericFilter<Self>("SUVPeakGenericFilter")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dim>
void SUVPeakGenericFilter<args_info_type>::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
void SUVPeakGenericFilter<args_info_type>::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;

  // Set value
  this->SetIOVerbose(mArgsInfo.verbose_flag);
  
  if (mArgsInfo.input_given) this->AddInputFilename(mArgsInfo.input_arg);
  
  if (mArgsInfo.mask_given)  this->AddInputFilename(mArgsInfo.mask_arg);
  }
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<class ImageType>
void SUVPeakGenericFilter<args_info_type>::UpdateWithInputImageType()
{
  // Read input
  typename ImageType::Pointer input = this->template GetInput<ImageType>(0);

  //Read mask
  typedef itk::Image<unsigned char, ImageType::ImageDimension> MaskImageType;
  typename MaskImageType::Pointer mask;
  if(mArgsInfo.mask_given) {
      mask = this->template GetInput<MaskImageType>(1);
  }
  else {
      mask = MaskImageType::New();
      mask->SetRegions(input->GetLargestPossibleRegion());
      mask->SetOrigin(input->GetOrigin());
      mask->SetSpacing(input->GetSpacing());
      mask->Allocate();
      mask->FillBuffer(1);
  }

  double volume = 1000; //1 cc into mc
  const double PI = 3.141592653589793238463;
  double radius = std::pow(3*volume/(4*PI),1./3);
  
  typename ImageType::Pointer kernel = ComputeMeanFilterKernel<ImageType>(input->GetSpacing(), radius);

  // Perform the convolution
  typedef itk::ConvolutionImageFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(input);
  filter->SetKernelImage(kernel);
  filter->Update();
  typename ImageType::Pointer output = filter->GetOutput();
  
  
  typedef itk::ImageRegionConstIteratorWithIndex<ImageType> IteratorType;
  typedef itk::ImageRegionConstIteratorWithIndex<MaskImageType> MIteratorType;
  IteratorType iters(output, output->GetLargestPossibleRegion());
  MIteratorType iterm(mask, mask->GetLargestPossibleRegion());
  iters.GoToBegin();
  iterm.GoToBegin();
  double max = 0.0;
  typename ImageType::IndexType index;
  while (!iters.IsAtEnd()) {
    if (iterm.Get() == 1) { // inside the mask
      if (iters.Get() > max) {
        max = iters.Get();
        index = iters.GetIndex();
      }
    }
    ++iters;
    ++iterm;
  }
  typename ImageType::PointType p;
  output->TransformIndexToPhysicalPoint(index, p);
  std::cout<<"SUV Peak found in "<< p << " with the value " << max << std::endl;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class args_info_type>
template<class ImageType>
typename ImageType::Pointer 
SUVPeakGenericFilter<args_info_type>::ComputeMeanFilterKernel(const typename ImageType::SpacingType & spacing, double radius)
{
  // Some kind of cache to speed up a bit
  static std::map<double, typename ImageType::Pointer> cache;
  if (cache.find(radius) != cache.end()) {
    return cache.find(radius)->second;
  }

  // Compute a kernel that corresponds to a sphere with 1 inside, 0
  // outside and in between proportional to the intersection between
  // the pixel and the sphere. Computed by Monte-Carlo because I don't
  // know an equation that compute the intersection volume between a
  // box and a sphere ...
  //auto kernel = ImageType::New();
  typename ImageType::Pointer kernel = ImageType::New();

  // Size of the kernel in pixel (minimum 3 pixels)
  typename ImageType::SizeType size;
  size[0] = std::max((int)ceil(radius*2/spacing[0]), 3);
  size[1] = std::max((int)ceil(radius*2/spacing[1]), 3);
  size[2] = std::max((int)ceil(radius*2/spacing[2]), 3);

  // Compute the region, such as the origin is at the center
  typename ImageType::IndexType start;
  start.Fill(0);
  typename ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);
  kernel->SetRegions(region);
  kernel->SetSpacing(spacing);
  typename ImageType::PointType origin;
  origin[0] = -(double)size[0]/2.0*spacing[0]+spacing[0]/2.0;
  origin[1] = -(double)size[1]/2.0*spacing[1]+spacing[1]/2.0;
  origin[2] = -(double)size[2]/2.0*spacing[2]+spacing[2]/2.0;
  kernel->SetOrigin(origin);
  kernel->Allocate();

  // Fill the kernel
  itk::ImageRegionIteratorWithIndex<ImageType> iter(kernel, region);
  typename ImageType::PointType center;
  center.Fill(0.0);
  typename ImageType::PointType hh; // half a voxel
  hh[0] = spacing[0]/2.0;
  hh[1] = spacing[1]/2.0;
  hh[2] = spacing[2]/2.0;
  double h = hh.EuclideanDistanceTo(center); // distance of half a pixel to its center.
  std::srand(time(NULL));
  double sum = 0.0;
  while (!iter.IsAtEnd()) {
    typename ImageType::IndexType index = iter.GetIndex();
    typename ImageType::PointType p;
    kernel->TransformIndexToPhysicalPoint(index, p);
    double d = p.EuclideanDistanceTo(center) + h;
    if (d<radius) { // inside the sphere
      iter.Set(1.0);
      sum += 1.0;
    }
    else { // the box intersect the sphere. We randomly pick point in
           // the box and compute the probability to be in/out the
           // sphere
      int n = 500; // number of samples
      double w = 0.0;
      //for(auto i=0; i<n; i++) {
      for(int i=0; i<n; i++) {
        // random position inside the current pixel
        typename ImageType::PointType pos;
        pos[0] = p[0]+(((double)std::rand()/(double)RAND_MAX)-0.5)*spacing[0];
        pos[1] = p[1]+(((double)std::rand()/(double)RAND_MAX)-0.5)*spacing[1];
        pos[2] = p[2]+(((double)std::rand()/(double)RAND_MAX)-0.5)*spacing[2];
        // distance to center
        double distance = pos.EuclideanDistanceTo(center);
        // lower/greater than radius
        if (distance < radius) w += 1.0;
      }
      w = w/(double)n;
      iter.Set(w);
      sum += w;
    }
    ++iter;
  }

  // Normalize
  iter.GoToBegin();
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Get()/sum);
    ++iter;
  }

  // Put in cache
  cache[radius] = kernel;

  return kernel;
}
//--------------------------------------------------------------------

} // end namespace

#endif  //#define CLITKSUVPEAKGENERICFILTER_TXX
