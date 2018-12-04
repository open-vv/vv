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
#ifndef CLITKSCINTIVOLSTATS_CXX
#define CLITKSCINTIVOLSTATS_CXX

// clitk include
#include "clitkScintivolStats_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"

#include "itkImageFileReader.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkExtractImageFilter.h"

#include <iostream>

//-------------------------------------------------------------------=
int main(int argc, char * argv[])
{

  // init command line
  GGO(clitkScintivolStats, args_info);
  CLITK_INIT;

  typedef itk::Image<double, 3> Input3DType;
  typedef itk::Image<double, 2> Input2DType;
  typedef itk::Image<char, 3> Input3DMaskType;
  typedef itk::Image<char, 2> Input2DMaskType;
  typedef itk::ImageFileReader<Input3DType> InputReader3DType;
  typedef itk::ImageFileReader<Input3DMaskType> InputReader3DMaskType;
  typedef itk::ImageFileReader<Input2DMaskType> InputReader2DMaskType;
  typedef itk::LabelStatisticsImageFilter<Input3DType, Input3DMaskType> LabelStatistics3DImageFilterType;
  typedef itk::LabelStatisticsImageFilter<Input2DType, Input2DMaskType> LabelStatistics2DImageFilterType;
  typedef itk::StatisticsImageFilter<Input2DType> Statistics2DImageFilterType;
  typedef itk::ExtractImageFilter<Input3DType, Input2DType> ExtractImageFilter;

  //Determine nbAcquisitionDynamic1 and nbAcquisitionDynamic2 (ie. the number of slice in dynamic1 and dynamic2)
  //Start to open the dynamic1 and dynamic2 and get the 3rd dimension size
  InputReader3DType::Pointer readerDynamic1 = InputReader3DType::New();
  readerDynamic1->SetFileName(args_info.dynamic1_arg);
  readerDynamic1->Update();
  Input3DType::Pointer dynamic1 = readerDynamic1->GetOutput();
  int nbAcquisitionDynamic1 = dynamic1->GetLargestPossibleRegion().GetSize()[2];

  InputReader3DType::Pointer readerDynamic2 = InputReader3DType::New();
  readerDynamic2->SetFileName(args_info.dynamic2_arg);
  readerDynamic2->Update();
  Input3DType::Pointer dynamic2 = readerDynamic2->GetOutput();
  int nbAcquisitionDynamic2 = dynamic2->GetLargestPossibleRegion().GetSize()[2];

  //Open the csv file
  //If it's scatter or attenuation correction, just append results to the csv file
  //If not open it normally and write the number of dynamic acquisition for the first and the second images (given by the ggo and from the dicom tag)
  std::ofstream csvFile;
  if (args_info.append_flag)
    csvFile.open (args_info.output_arg, std::ios::app);
  else {
    csvFile.open (args_info.output_arg);
    csvFile << "1;" << nbAcquisitionDynamic1 << ";" << nbAcquisitionDynamic2 << "\n";
  }

  //Read Tomo image, total and remnant masks
  InputReader3DType::Pointer readerTomo = InputReader3DType::New();
  readerTomo->SetFileName(args_info.tomo_arg);
  readerTomo->Update();
  Input3DType::Pointer tomo = readerTomo->GetOutput();

  InputReader3DMaskType::Pointer readerTotalLiver = InputReader3DMaskType::New();
  readerTotalLiver->SetFileName(args_info.totalLiverMask_arg);
  readerTotalLiver->Update();

  InputReader3DMaskType::Pointer readerRemnantLiver = InputReader3DMaskType::New();
  readerRemnantLiver->SetFileName(args_info.remnantLiverMask_arg);
  readerRemnantLiver->Update();

  //Find number of counts in tomo for total Liver and remnant Liver
  LabelStatistics3DImageFilterType::Pointer labelStatisticsImageFilterTotalLiver = LabelStatistics3DImageFilterType::New();
  labelStatisticsImageFilterTotalLiver->SetLabelInput(readerTotalLiver->GetOutput());
  labelStatisticsImageFilterTotalLiver->SetInput(tomo);
  labelStatisticsImageFilterTotalLiver->Update();

  LabelStatistics3DImageFilterType::Pointer labelStatisticsImageFilterRemnantLiver = LabelStatistics3DImageFilterType::New();
  labelStatisticsImageFilterRemnantLiver->SetLabelInput(readerRemnantLiver->GetOutput());
  labelStatisticsImageFilterRemnantLiver->SetInput(tomo);
  labelStatisticsImageFilterRemnantLiver->Update();

  //Write them in the csv file
  csvFile << args_info.acquisitionTimeTomo_arg << ";" << labelStatisticsImageFilterTotalLiver->GetSum(1) << ";" << labelStatisticsImageFilterRemnantLiver->GetSum(1) << "\n";

  //Read liver and heart masks for dynamic1
  InputReader2DMaskType::Pointer readerLiver = InputReader2DMaskType::New();
  readerLiver->SetFileName(args_info.liverMask_arg);
  readerLiver->Update();

  InputReader2DMaskType::Pointer readerHeart = InputReader2DMaskType::New();
  readerHeart->SetFileName(args_info.heartMask_arg);
  readerHeart->Update();

  for (unsigned int i=0; i<nbAcquisitionDynamic1; ++i) {
    //Compute time frame in s
    double timeFrame = args_info.frameDurationDynamic1_arg * i + args_info.frameDurationDynamic1_arg;

    //Extract the Region of interest of the dynamic1
    ExtractImageFilter::Pointer extractSlice = ExtractImageFilter::New();
    Input3DType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = i;
    Input3DType::SizeType size;
    size[0] = dynamic1->GetLargestPossibleRegion().GetSize()[0];
    size[1] = dynamic1->GetLargestPossibleRegion().GetSize()[1];
    size[2] = 0;
    Input3DType::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);
    extractSlice->SetExtractionRegion(desiredRegion);
    extractSlice->SetInput(dynamic1);
#if ITK_VERSION_MAJOR >= 4
    extractSlice->SetDirectionCollapseToIdentity();
#endif
    extractSlice->Update();

    //Find number of counts in dynamic1 slice for Liver and heart
    LabelStatistics2DImageFilterType::Pointer labelStatisticsImageFilterLiver = LabelStatistics2DImageFilterType::New();
    labelStatisticsImageFilterLiver->SetLabelInput(readerLiver->GetOutput());
    labelStatisticsImageFilterLiver->SetInput(extractSlice->GetOutput());
    labelStatisticsImageFilterLiver->SetCoordinateTolerance(0.001);
    labelStatisticsImageFilterLiver->SetDirectionTolerance(0.001);
    labelStatisticsImageFilterLiver->Update();

    LabelStatistics2DImageFilterType::Pointer labelStatisticsImageFilterHeart = LabelStatistics2DImageFilterType::New();
    labelStatisticsImageFilterHeart->SetLabelInput(readerHeart->GetOutput());
    labelStatisticsImageFilterHeart->SetInput(extractSlice->GetOutput());
    labelStatisticsImageFilterHeart->SetCoordinateTolerance(0.001);
    labelStatisticsImageFilterHeart->SetDirectionTolerance(0.001);
    labelStatisticsImageFilterHeart->Update();

    //Find number of counts in dynamic1 slice
    Statistics2DImageFilterType::Pointer statisticsImageFilterDynamic1 = Statistics2DImageFilterType::New();
    statisticsImageFilterDynamic1->SetInput(extractSlice->GetOutput());
    statisticsImageFilterDynamic1->Update();

    //Write them in the csv file
    csvFile << timeFrame << ";" << labelStatisticsImageFilterLiver->GetSum(1) << ";" << labelStatisticsImageFilterHeart->GetSum(1) << ";" << statisticsImageFilterDynamic1->GetSum() << "\n";
  }

  //Read parenchyma mask for dynamic2
  InputReader2DMaskType::Pointer readerParenchyma = InputReader2DMaskType::New();
  readerParenchyma->SetFileName(args_info.parenchymaMask_arg);
  readerParenchyma->Update();

  for (unsigned int i=0; i<nbAcquisitionDynamic2; ++i) {
    //Compute time frame in s
    double timeFrame = args_info.frameDurationDynamic2_arg * i + args_info.acquisitionTimeDynamic2_arg + args_info.frameDurationDynamic2_arg;

    //Extract the Region of interest of the dynamic1
    ExtractImageFilter::Pointer extractSlice = ExtractImageFilter::New();
    Input3DType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = i;
    Input3DType::SizeType size;
    size[0] = dynamic2->GetLargestPossibleRegion().GetSize()[0];
    size[1] = dynamic2->GetLargestPossibleRegion().GetSize()[1];
    size[2] = 0;
    Input3DType::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);
    extractSlice->SetExtractionRegion(desiredRegion);
    extractSlice->SetInput(dynamic2);
#if ITK_VERSION_MAJOR >= 4
    extractSlice->SetDirectionCollapseToIdentity();
#endif
    extractSlice->Update();

    //Find number of counts in dynamic2 slice for parenchyma
    LabelStatistics2DImageFilterType::Pointer labelStatisticsImageFilterParenchyma = LabelStatistics2DImageFilterType::New();
    labelStatisticsImageFilterParenchyma->SetLabelInput(readerParenchyma->GetOutput());
    labelStatisticsImageFilterParenchyma->SetInput(extractSlice->GetOutput());
    labelStatisticsImageFilterParenchyma->SetCoordinateTolerance(0.001);
    labelStatisticsImageFilterParenchyma->SetDirectionTolerance(0.001);
    labelStatisticsImageFilterParenchyma->Update();

    //Write them in the csv file
    csvFile << timeFrame << ";" << labelStatisticsImageFilterParenchyma->GetSum(1) << "\n";
  }

  csvFile.close();

  // this is the end my friend
  return 0;
}
//-------------------------------------------------------------------=

#endif /* end #define CLITKSCINTIVOLSTATS_CXX */
