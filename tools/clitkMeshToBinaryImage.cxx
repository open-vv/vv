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
#include "clitkMeshToBinaryImage_ggo.h"
#include "clitkMeshToBinaryImageFilter.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"

#include <itkImageIOBase.h>
#include <itkInvertIntensityImageFilter.h>

#include <vtkOBJReader.h>

#include <string>

template <unsigned dim>
void run(const args_info_clitkMeshToBinaryImage& argsInfo,
         const itk::ImageIOBase * header)
{
  typedef itk::Image<unsigned char, dim> ImageType;
  typename ImageType::Pointer like = ImageType::New();

  typename ImageType::SizeType size;
  for (unsigned i = 0; i < dim; ++i)
    size[i] = header->GetDimensions(i);
  typename ImageType::RegionType region;
  region.SetSize(size);
  like->SetRegions(region);

  typename ImageType::SpacingType spacing;
  for (unsigned i = 0; i < dim; ++i)
    spacing[i] = header->GetSpacing(i);
  like->SetSpacing(spacing);


  typename ImageType::PointType origin;
  for (unsigned i = 0; i < dim; ++i)
    origin[i] = header->GetOrigin(i);
  like->SetOrigin(origin);

  vtkSmartPointer<vtkOBJReader> reader = vtkOBJReader::New();
  reader->SetFileName(argsInfo.input_arg);
  reader->Update();

  typename clitk::MeshToBinaryImageFilter<ImageType>::Pointer filter =
    clitk::MeshToBinaryImageFilter<ImageType>::New();
  filter->SetExtrude(false);
  filter->SetMesh(reader->GetOutput());
  filter->SetLikeImage(like);
  filter->Update();

  typedef itk::InvertIntensityImageFilter<ImageType> InvertFilterType;
  typename InvertFilterType::Pointer ifilter = InvertFilterType::New();
  ifilter->SetInput(filter->GetOutput());
  ifilter->SetMaximum(1);

  clitk::writeImage(ifilter->GetOutput(), argsInfo.output_arg);
}

int main(int argc, char** argv)
{
  GGO(clitkMeshToBinaryImage, args_info);

  itk::ImageIOBase::Pointer header =
    clitk::readImageHeader(args_info.like_arg);
  switch (header->GetNumberOfDimensions())
  {
    case 2:
      run<2>(args_info, header);
      break;
    case 3:
      run<3>(args_info, header);
      break;
    case 4:
      run<4>(args_info, header);
      break;
  }

  return EXIT_SUCCESS;
}
