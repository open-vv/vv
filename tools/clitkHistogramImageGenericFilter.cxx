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
#ifndef clitkHistogramImageGenericFilter_cxx
#define clitkHistogramImageGenericFilter_cxx

/* =================================================
 * @file   clitkHistogramImageGenericFilter.cxx
 * @author Thomas Baudier <thomas.baudier@creatis.insa-lyon.fr>
 * @date   22 dec 2015
 *
 * @brief
 *
 ===================================================*/

#include "clitkHistogramImageGenericFilter.h"

// itk include
#include <itkImageToHistogramFilter.h>
#include <itkStatisticsImageFilter.h>

#include <clitkCommon.h>



namespace clitk
{

//--------------------------------------------------------------------
HistogramImageGenericFilter::HistogramImageGenericFilter():
  ImageToImageGenericFilter<Self>("HistogramImage")
{
  InitializeImageType<2>();
  InitializeImageType<3>();
  InitializeImageType<4>();
  mBinSize = 100;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<unsigned int Dim>
void HistogramImageGenericFilter::InitializeImageType()
{
  ADD_DEFAULT_IMAGE_TYPES(Dim);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkFloatArray* HistogramImageGenericFilter::GetArrayX()
{
  return(mArrayX);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
vtkFloatArray* HistogramImageGenericFilter::GetArrayY()
{
  return(mArrayY);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void HistogramImageGenericFilter::SetArgsInfo(const args_info_type & a)
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
  if (mArgsInfo.size_given) {
    SetSizeBin(mArgsInfo.size_arg);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void HistogramImageGenericFilter::SetSizeBin(const double size)
{
  mBinSize = size;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class InputImageType>
void
HistogramImageGenericFilter::UpdateWithInputImageType()
{

  // Reading input
  typename InputImageType::Pointer input = this->template GetInput<InputImageType>(0);
  typedef typename InputImageType::PixelType PixelType;
  typedef typename InputImageType::IndexType IndexType;
  typedef typename itk::Statistics::ImageToHistogramFilter<InputImageType> HistogramFilterType;
  typedef typename itk::StatisticsImageFilter<InputImageType> StatisticsImageFilterType;

  //compute range
  typename StatisticsImageFilterType::Pointer statisticsImageFilter = StatisticsImageFilterType::New ();
  statisticsImageFilter->SetInput(input);
  statisticsImageFilter->Update();
  double range = statisticsImageFilter->GetMaximum() - statisticsImageFilter->GetMinimum();
  //compute bin number
  typedef typename HistogramFilterType::HistogramSizeType SizeType;
  SizeType binNumber(1);
  if (!mArgsInfo.size_given)
    mBinSize = std::log(range);
  if (mBinSize == 0)
    mBinSize = 1;
  binNumber[0] = (int)(range/mBinSize);
  if (binNumber[0] == 0)
    binNumber[0] = 1;

  //compute histogram
  typename HistogramFilterType::Pointer histogramFilter = HistogramFilterType::New();
  histogramFilter->SetHistogramSize(binNumber);
  histogramFilter->SetAutoMinimumMaximum(true);
  typename HistogramFilterType::HistogramMeasurementVectorType lowerBound(1);
  typename HistogramFilterType::HistogramMeasurementVectorType upperBound(1);
  lowerBound[0] = statisticsImageFilter->GetMinimum();
  upperBound[0] = statisticsImageFilter->GetMaximum();
  histogramFilter->SetHistogramBinMinimum(lowerBound);
  histogramFilter->SetHistogramBinMaximum(upperBound);
  histogramFilter->SetInput(input);
  histogramFilter->Update();

  mArrayX = vtkSmartPointer<vtkFloatArray>::New();
  mArrayY = vtkSmartPointer<vtkFloatArray>::New();

  for(unsigned int i = 0; i < histogramFilter->GetOutput()->GetSize()[0]; ++i)
  {
    mArrayY->InsertNextTuple1(histogramFilter->GetOutput()->GetFrequency(i));
    mArrayX->InsertNextTuple1(statisticsImageFilter->GetMinimum() + (i+0.5)*mBinSize);
  }
}
//--------------------------------------------------------------------

//------------------------------------------------------------------------------
void HistogramImageGenericFilter::SaveAs()
{
  // Output
  std::string textFileName = GetOutputFilename();
  std::ofstream fileOpen(textFileName.c_str(), std::ofstream::trunc);

  if(!fileOpen) {
      cerr << "Error during saving" << endl;
      return;
  }

  int i(0);
  fileOpen << "Value represents the number of voxels around the corresponding intensity (by default the windows size around intensity is log(range))" << endl;
  fileOpen << "Intensity" << "\t" << "Value" << endl;

  while (i<mArrayX->GetNumberOfTuples()) {
      fileOpen << mArrayX->GetTuple(i)[0] << "\t" << mArrayY->GetTuple(i)[0] << endl;
      ++i;
  }

  fileOpen.close();
}
//------------------------------------------------------------------------------


}//end clitk

#endif  //#define clitkHistogramImageGenericFilter_cxx
