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
#include <itkImageToVTKImageFilter.h>
#include <itkPixelTraits.h>
#include <vtkVersion.h>

//--------------------------------------------------------------------
template<class TItkImageType>
void vvImage::AddItkImage(TItkImageType *input)
{
  // Update input before conversion to enable exceptions thrown by the ITK pipeline.
  // Otherwise, vtkImageImport catches the exception for us.
  input->Update();
  
  // Convert from ITK object to VTK object
  mImageDimension = TItkImageType::ImageDimension;
  typedef itk::ImageToVTKImageFilter <TItkImageType> ConverterType;
  typename ConverterType::Pointer converter = ConverterType::New();
  mItkToVtkConverters.push_back(dynamic_cast< itk::ProcessObject *>(converter.GetPointer()));
  converter->SetInput(input);
  converter->Update();
  mVtkImages.push_back( converter->GetOutput() );

  // Account for direction in transform. The offset is already accounted for
  // in the VTK image coordinates, no need to put it in the transform.
  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  matrix->Identity();
  for(unsigned int i=0; i<input->GetImageDimension(); i++) {
    for(unsigned int j=0; j<input->GetImageDimension(); j++) {
#if VTK_MAJOR_VERSION <= 6
      (*matrix)[i][j] = input->GetDirection()[i][j];
      // Direction is used around the image origin in ITK
      (*matrix)[i][3] -= (*matrix)[i][j] * input->GetOrigin()[j];
#else
      (*matrix).SetElement(i, j, input->GetDirection()[i][j]);
      // Direction is used around the image origin in ITK
      (*matrix).SetElement(i, 3, (*matrix).GetElement(i,3) - (*matrix).GetElement(i,j) * input->GetOrigin()[j]);
#endif
    }
#if VTK_MAJOR_VERSION <= 6
    (*matrix)[i][3] += input->GetOrigin()[i];
#else
    (*matrix).SetElement(i, 3, (*matrix).GetElement(i,3) + input->GetOrigin()[i]);
#endif
  }

  // GetDirection provides the forward transform, vtkImageReslice wants the inverse
  matrix->Invert();

  mTransform.push_back(vtkSmartPointer<vtkTransform>::New());
  mTransform.back()->SetMatrix(matrix);
  //META DATA
  mDictionary.push_back(&(input->GetMetaDataDictionary()));
}
//--------------------------------------------------------------------
 
/** Dispatch the computation of scalar range between vector and scalar image */
template<class TPixelType, unsigned int VImageDimension>
void vvImage::ComputeScalarRangeBase(itk::Image<TPixelType,VImageDimension> *input)
{ 
 itkStaticConstMacro(Dimension1, unsigned int, itk::PixelTraits< TPixelType >::Dimension);
 ComputeScalarRange(DimensionDispatch< Dimension1 >(), input);
}

//--------------------------------------------------------------------
/** Compute the scalar range for a vector pixel type */
/** TO DO*/
template<class TPixelType, unsigned int VImageDimension>
void vvImage::ComputeScalarRange(DimensionDispatchBase, itk::Image<TPixelType,VImageDimension> *input)
{
}

//--------------------------------------------------------------------
/** Compute the scalar range for a scalar pixel type */
template<class TPixelType, unsigned int VImageDimension>
void vvImage::ComputeScalarRange(DimensionDispatch< 1 >, itk::Image<TPixelType,VImageDimension> *input)
{  
  typedef typename itk::Image<TPixelType,VImageDimension> TItkImageType;
  typedef itk::MinimumMaximumImageCalculator <TItkImageType> ImageCalculatorFilterType;
  
  typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
  TPixelType tempMin, tempMax;
  double tempRange[2];
  imageCalculatorFilter->SetImage(input);
  imageCalculatorFilter->Compute();
  tempMin= imageCalculatorFilter->GetMinimum();
  tempMax= imageCalculatorFilter->GetMaximum();

  tempRange[0] = (double) tempMin;
  tempRange[1] = (double) tempMax;

  if (tempRange[0] < mrange[0]) mrange[0]=tempRange[0];
  if (tempRange[1] > mrange[1]) mrange[1]=tempRange[1];
}
//--------------------------------------------------------------------

