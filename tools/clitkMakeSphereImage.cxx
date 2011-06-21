#include "itkImage.h"
#include "itkSphereSpatialFunction.h"
#include <itkImageRegionIterator.h>
#include "itkImageFileWriter.h"

int main(int argc, char** argv) {
  
  const unsigned int dim = 3;
  typedef char PixelType;
  typedef itk::Image<PixelType, dim> ImageType;
  typedef typename ImageType::IndexType IndexType;
  typedef typename ImageType::PointType PointType;
  typedef typename ImageType::SizeType SizeType;
  typedef typename ImageType::RegionType RegionType;

  IndexType index;
  index.Fill(0);

  SizeType size;
  size[0] = atoi(argv[2]);
  size[1] = atoi(argv[3]);
  size[2] = atoi(argv[4]);
  
  PointType origin;
  origin[0] = atoi(argv[5]);
  origin[1] = atoi(argv[6]);
  origin[2] = atoi(argv[7]);
  
  RegionType region;
  region.SetIndex(index);
  region.SetSize(size);

  typename ImageType::Pointer image = ImageType::New();
  image->SetRegions(region);
  image->Allocate();
  
  typedef itk::SphereSpatialFunction<dim, PointType> ShpereFunctionType;
  typename ShpereFunctionType::Pointer sphere = ShpereFunctionType::New();
  
  double radius = atof(argv[8])/2;
  sphere->SetCenter(origin);
  sphere->SetRadius(radius);
  
  PixelType max = itk::NumericTraits<PixelType>::max();
  typedef itk::ImageRegionIterator<ImageType> ImageIteratorType;
  ImageIteratorType it(image, region);
  PointType point;
  for (it.GoToBegin(); !it.IsAtEnd(); ++it) {
    image->TransformIndexToPhysicalPoint(it.GetIndex(), point);
    if (sphere->Evaluate(point)) {
      PixelType value = static_cast<PixelType>(max*(point - origin).GetNorm()/radius); 
      it.Set(value);
    }
    else
      it.Set(0);
  }
  
  typedef itk::ImageFileWriter<ImageType> ImageWriterType;
  typename ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetInput(image);
  writer->SetFileName(argv[1]);
  writer->Update();
}