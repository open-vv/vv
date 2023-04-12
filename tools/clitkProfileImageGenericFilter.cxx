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
#ifndef clitkProfileImageGenericFilter_cxx
#define clitkProfileImageGenericFilter_cxx

/* =================================================
 * @file   clitkProfileImageGenericFilter.cxx
 * @author Thomas Baudier <thomas.baudier@creatis.insa-lyon.fr>
 * @date   22 dec 2015
 *
 * @brief
 *
 ===================================================*/

#include "clitkProfileImageGenericFilter.h"

// itk include
#include <itkLineIterator.h>
#include <itkPoint.h>

#include <clitkCommon.h>



namespace clitk
{

//--------------------------------------------------------------------
ProfileImageGenericFilter::ProfileImageGenericFilter():
  ImageToImageGenericFilter<Self>("ProfileImage")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void ProfileImageGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkFloatArray* ProfileImageGenericFilter::GetArrayX()
{
  return(mArrayX);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkFloatArray* ProfileImageGenericFilter::GetArrayY()
{
  return(mArrayY);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkFloatArray* ProfileImageGenericFilter::GetCoord()
{
  return(mCoord);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void ProfileImageGenericFilter::SetArgsInfo(const args_info_type & a)
{
  mArgsInfo=a;
  if (mArgsInfo.verbose_given)
    SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_given && mArgsInfo.imagetypes_flag)
    this->PrintAvailableImageTypes();

  if (mArgsInfo.input_given) {
    SetInputFilename(mArgsInfo.input_arg);
  }
  if (mArgsInfo.output_given) {
    SetOutputFilename(mArgsInfo.output_arg);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class InputImageType>
void
ProfileImageGenericFilter::UpdateWithInputImageType()
{

  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
  typedef typename InputImageType::PixelType PixelType;
  typedef typename InputImageType::IndexType IndexType;

  mArrayX = vtkSmartPointer<vtkFloatArray>::New();
  mArrayY = vtkSmartPointer<vtkFloatArray>::New();
  mCoord = vtkSmartPointer<vtkFloatArray>::New();
  mCoord->SetNumberOfComponents(InputImageType::ImageDimension);
  mCoordmm = vtkSmartPointer<vtkFloatArray>::New();
  mCoordmm->SetNumberOfComponents(InputImageType::ImageDimension);
  mDimension = InputImageType::ImageDimension;
  
  /*typename InputImageType::Pointer outputImage;
  outputImage = InputImageType::New();
 
  outputImage->SetRegions(input->GetLargestPossibleRegion());
  outputImage->Allocate();
  outputImage->FillBuffer(0); */
  
  //Iterator
  IndexType pointBegin, pointEnd;
  
  for (int i = 0; i < mArgsInfo.point1_given; ++i) {
    pointBegin[i] = mArgsInfo.point1_arg[i];
    pointEnd[i] = mArgsInfo.point2_arg[i];
  }
  
  itk::LineConstIterator<InputImageType> itProfile(input, pointBegin, pointEnd);
  itProfile.GoToBegin();
  int lineNumber(1);
  double *tuple;
  double distance;
  tuple = new double[InputImageType::ImageDimension];
  itk::Point<double, InputImageType::ImageDimension> transformedFirstPoint;
  itk::Point<double, InputImageType::ImageDimension> transformedCurrentPoint;
  
  input->TransformIndexToPhysicalPoint(itProfile.GetIndex(), transformedFirstPoint);
  
  while (!itProfile.IsAtEnd())
  {    
    // Fill in the table the intensity value
    mArrayY->InsertNextTuple1(itProfile.Get());
        
    for (int i=0; i<InputImageType::ImageDimension; ++i) {
        tuple[i] = itProfile.GetIndex()[i];
    }

    input->TransformIndexToPhysicalPoint(itProfile.GetIndex(), transformedCurrentPoint);
    distance = transformedFirstPoint.EuclideanDistanceTo(transformedCurrentPoint);

    // Fill in the table the distance value
    mArrayX->InsertNextTuple1(distance);
    
    // Fill in the table the voxel coordinate value
    mCoord->InsertNextTuple(tuple); //index
    for (int i=0; i<InputImageType::ImageDimension; ++i) {
        tuple[i] = transformedCurrentPoint[i];
    }
    mCoordmm->InsertNextTuple(tuple); //mm
    ++lineNumber;
    ++itProfile;
  }

  if (mArgsInfo.output_given) {
    std::string str(mArgsInfo.output_arg);
    this->WriteOutput(str);
  }
  
  /*
  itk::LineIterator<InputImageType> otProfile(outputImage, pointBegin, pointEnd);
  otProfile.GoToBegin();  
  while (!otProfile.IsAtEnd())
  {    
    otProfile.Set(1.0);
    ++otProfile;
  }
  
  this->template SetNextOutput<InputImageType>(outputImage): */
  
  delete [] tuple;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void ProfileImageGenericFilter::WriteOutput(std::string outputFilename)
{
  std::ofstream fileOpen(outputFilename.c_str(), std::ofstream::trunc);

  if(!fileOpen) {
      cerr << "Error during saving" << endl;
      return;
  }

  double *tuple;
  tuple = new double[mDimension];
  int i(0);
  fileOpen << "The Bresenham algorithm is used to travel along the line. Values represent the center of each crossed voxel (in voxel and mm)" << endl;
  fileOpen << "Id" << "\t" << "Value" << "\t" ;
  fileOpen << "x(vox)" << "\t" << "y(vox)" << "\t";
  if (mDimension >=3)
      fileOpen << "z(vox)" << "\t";
  if (mDimension >=4)
      fileOpen << "t" << "\t";
  fileOpen << "x(mm)" << "\t" << "y(mm)" << "\t";
  if (mDimension >=3)
      fileOpen << "z(mm)" << "\t";
  if (mDimension >=4)
      fileOpen << "t" << "\t";
  fileOpen << endl;

  while (i<mArrayX->GetNumberOfTuples()) {
      fileOpen << i << "\t" << mArrayY->GetTuple(i)[0] << "\t" ;

      mCoord->GetTuple(i, tuple);
      for (int j=0; j<mDimension ; ++j) {
          fileOpen << tuple[j] << "\t" ;
      }
      mCoordmm->GetTuple(i, tuple);
      for (int j=0; j<mDimension ; ++j) {
          fileOpen << tuple[j] << "\t" ;
      }
      if (mDimension == 4) {
          fileOpen << tuple[3] << "\t" ;
      }
      fileOpen << endl;
      ++i;
  }

  delete [] tuple;

  fileOpen.close();
}
//--------------------------------------------------------------------


}//end clitk

#endif  //#define clitkProfileImageGenericFilter_cxx
