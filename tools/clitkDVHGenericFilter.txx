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
#ifndef clitkDVHGenericFilter_txx
#define clitkDVHGenericFilter_txx

/* =================================================
 * @file   clitkDVHGenericFilter.txx
 * @author Agata Krason <agata.krason@creatis.insa-lyon.fr>
 * @date   20 November 2013
 *
 * @brief Dose volume and image histogram
 *
 ===================================================*/

// itk include
#include "itkBinaryThresholdImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkMaskNegatedImageFilter.h"
#include "itkNthElementImageAdaptor.h"
#include "itkJoinSeriesImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

// clitk include
#include <clitkCommon.h>
#include "clitkImageCommon.h"
#include "clitkDVHGenericFilter.h"
#include "clitkCropLikeImageFilter.h"
#include "clitkResampleImageWithOptionsFilter.h"

//-------------------------------------------------------------------
//
namespace clitk
{

//-------------------------------------------------------------------
template<unsigned int Dimension, unsigned int Components>
void
DVHGenericFilter::UpdateWithDim(std::string PixelType)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<" with " << Components << " channel(s)..."<<std::endl;

  if(PixelType == "short"){  
    if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, signed short, Components>(); 
  }
  else if(PixelType == "unsigned_short"){  
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned short, Components>(); 
  }
  
  else if (PixelType == "unsigned_char"){ 
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, unsigned char, Components>();
  }
      
  else if(PixelType == "double"){  
    if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and double..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, double, Components>(); 
  }
  else {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, float, Components>();
  }
}


//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//-------------------------------------------------------------------
template <unsigned int Dimension, class  PixelType, unsigned int Components> 
void 
DVHGenericFilter::UpdateWithDimAndPixelType()
{

  // ImageTypes
  typedef unsigned char LabelPixelType;
  typedef itk::Image<itk::Vector<PixelType, Components>, Dimension> InputImageType;
  typedef itk::Image<LabelPixelType, Dimension> LabelImageType;
  
  // Read the input
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( m_InputFileName);
  reader->Update();
  typename InputImageType::Pointer input= reader->GetOutput();
  
  typedef itk::NthElementImageAdaptor<InputImageType, PixelType> InputImageAdaptorType;
  typedef itk::Image<PixelType, Dimension> OutputImageType;

  typename InputImageAdaptorType::Pointer input_adaptor = InputImageAdaptorType::New();
  input_adaptor->SetImage(input);
  
  // Filter
  typedef itk::LabelStatisticsImageFilter<InputImageAdaptorType, LabelImageType> StatisticsImageFilterType;
  typename StatisticsImageFilterType::Pointer statisticsFilter=StatisticsImageFilterType::New();
  statisticsFilter->SetInput(input_adaptor);

  // Label image
  typename LabelImageType::Pointer labelImage;
  if (m_ArgsInfo.mask_given) {
    int maskDimension, maskComponents;
    std::string maskPixelType;
    ReadImageDimensionAndPixelType(m_ArgsInfo.mask_arg, maskDimension, maskPixelType, maskComponents);

    if (maskDimension == Dimension - 1) {
      // Due to a limitation of filter itk::LabelStatisticsImageFilter, InputImageType and LabelImageType
      // must have the same image dimension. However, we want to support label images with Dl = Di - 1,
      // so we need to replicate the label image as many times as the size along dimension Di.
      if (m_Verbose) 
        std::cout << "Replicating label image to match input image's dimension... " << std::endl;
      
      typedef itk::Image<LabelPixelType, Dimension - 1> ReducedLabelImageType;
      typedef itk::ImageFileReader<ReducedLabelImageType> LabelImageReaderType;
      typedef itk::JoinSeriesImageFilter<ReducedLabelImageType, LabelImageType> JoinImageFilterType;

      
      typename LabelImageReaderType::Pointer labelImageReader=LabelImageReaderType::New();
      labelImageReader->SetFileName(m_ArgsInfo.mask_arg);
      labelImageReader->Update();

      typename JoinImageFilterType::Pointer joinFilter = JoinImageFilterType::New();
      typename InputImageType::SizeType size = input->GetLargestPossibleRegion().GetSize();
      for (unsigned int i = 0; i < size[Dimension - 1]; i++)
        joinFilter->PushBackInput(labelImageReader->GetOutput());
      
      joinFilter->Update();
      labelImage = joinFilter->GetOutput();
    }
    else {
      typedef itk::ImageFileReader<LabelImageType> LabelImageReaderType;
      typename LabelImageReaderType::Pointer labelImageReader=LabelImageReaderType::New();
      labelImageReader->SetFileName(m_ArgsInfo.mask_arg);
      labelImageReader->Update();
      labelImage= labelImageReader->GetOutput();

      // Check mask sampling/size
      if (!HaveSameSizeAndSpacing<LabelImageType, InputImageType>(labelImage, input)) {
        if (m_ArgsInfo.allow_resize_flag) {
          if (m_ArgsInfo.verbose_flag) {
            std::cout << "Resize mask image like input" << std::endl;
          }

          typedef clitk::ResampleImageWithOptionsFilter<LabelImageType> ResamplerType;
          typename ResamplerType::Pointer resampler = ResamplerType::New();
          resampler->SetInput(labelImage);
          resampler->SetOutputSpacing(input->GetSpacing());
          resampler->SetGaussianFilteringEnabled(false);
          resampler->Update();
          labelImage = resampler->GetOutput();
          labelImage->GetSpacing();
          typedef clitk::CropLikeImageFilter<LabelImageType> FilterType;
          typename FilterType::Pointer crop = FilterType::New();
          crop->SetInput(labelImage);
          crop->SetCropLikeImage(input);
          crop->Update();
          labelImage = crop->GetOutput();                        
          //writeImage<LabelImageType>(labelImage, "test2.mha");

        }
        else {
          std::cerr << "Mask image has a different size/spacing than input. Abort" << std::endl;
          exit(-1);
        }
      }

    }

  }
  else { 
    labelImage=LabelImageType::New();
    labelImage->SetRegions(input->GetLargestPossibleRegion());
    labelImage->SetOrigin(input->GetOrigin());
    labelImage->SetSpacing(input->GetSpacing());
    labelImage->Allocate();
    labelImage->FillBuffer(m_ArgsInfo.label_arg[0]);
  }
  statisticsFilter->SetLabelInput(labelImage);

  // Check/compute spacing
  const typename LabelImageType::SpacingType& spacing = input->GetSpacing();
  double spacing_cc = (spacing[0]*spacing[1]*spacing[2])/1000;
  // std::cout<<"Spacing x : "<<spacing[0]<<std::endl;
  // std::cout<<"Spacing y :  "<< spacing[1]<<std::endl;
  // std::cout<<"Spacing z :  "<< spacing[2]<<std::endl;
  // std::cout <<"spacing_cc : "<< spacing_cc << std::endl;

  // For each Label
  typename LabelImageType::PixelType label;
  unsigned int numberOfLabels;
  if (m_ArgsInfo.label_given)
    numberOfLabels=m_ArgsInfo.label_given;
  else
    numberOfLabels=1;

  unsigned int firstComponent = 0, lastComponent = 0;
  if (m_ArgsInfo.channel_arg == -1) {
    firstComponent = 0; 
    lastComponent = Components - 1;
  }
  else {
    firstComponent = m_ArgsInfo.channel_arg;
    lastComponent = m_ArgsInfo.channel_arg;
  }
  
  for (unsigned int c=firstComponent; c<=lastComponent; c++) {
    if (m_Verbose) std::cout << std::endl << "Processing channel " << c << std::endl;
    
    input_adaptor->SelectNthElement(c);
    input_adaptor->Update();
    
    for (unsigned int k=0; k< numberOfLabels; k++) {
      label=m_ArgsInfo.label_arg[k];
      // Histogram
      if (m_ArgsInfo.histogram_given)
      {
          std::cout<<"--------------"<<std::endl;
          std::cout<<"| Label:   |"<<label<<" |"<<std::endl;
          std::cout<<"--------------"<<std::endl;

        statisticsFilter->SetUseHistograms(true);
        statisticsFilter->SetHistogramParameters(m_ArgsInfo.bins_arg, m_ArgsInfo.lower_arg, m_ArgsInfo.upper_arg);
      }

      // DVHistogram
	  if(m_ArgsInfo.dvhistogram_given)
	  {
        statisticsFilter->SetUseHistograms(true);
        statisticsFilter->SetHistogramParameters(m_ArgsInfo.bins_arg, m_ArgsInfo.lower_arg, m_ArgsInfo.upper_arg);
      }

      statisticsFilter->Update();

      // Histogram
      if (m_ArgsInfo.histogram_given)
      {
          if (m_Verbose) std::cout<<"Median: ";
          std::cout<<statisticsFilter->GetMedian(label)<<std::endl;

        typename StatisticsImageFilterType::HistogramPointer histogram =statisticsFilter->GetHistogram(label);

        // Screen
        if (m_Verbose) std::cout<<"Histogram: "<<std::endl;
          std::cout<<"# MinBin\tMidBin\tMaxBin\tFrequency"<<std::endl;
        for( int i =0; i <m_ArgsInfo.bins_arg; i++)
          std::cout<<histogram->GetBinMin(0,i)<<"\t"<<histogram->GetMeasurement(i,0)<<"\t"<<histogram->GetBinMax(0,i)<<"\t"<<histogram->GetFrequency(i)<<std::endl;
        // Add to the file
        std::ofstream histogramFile(m_ArgsInfo.histogram_arg);
        histogramFile<<"#Histogram: "<<std::endl;
        histogramFile<<"#MinBin\tMidBin\tMaxBin\tFrequency"<<std::endl;
        for( int i =0; i <m_ArgsInfo.bins_arg; i++)
          histogramFile<<histogram->GetBinMin(0,i)<<"\t"<<histogram->GetMeasurement(i,0)<<"\t"<<histogram->GetBinMax(0,i)<<"\t"<<histogram->GetFrequency(i)<<std::endl;
      }

      // DVH
	  if(m_ArgsInfo.dvhistogram_given)
	  {
          typename StatisticsImageFilterType::HistogramPointer dvhistogram = statisticsFilter->GetHistogram(label);

          // Screen
          std::cout<<"Total volume : ";
          std::cout<<statisticsFilter->GetCount(label)<<" [No. of voxels]"<<std::endl;
          std::cout<<"Total volume : ";
          std::cout<<((statisticsFilter->GetCount(label))*spacing_cc)<<" [cc]"<<std::endl;
          std::cout<<"Dose mean: ";
          std::cout<<statisticsFilter->GetMean(label)<<" [Gy]"<<std::endl;
          std::cout<<"Dose min: ";
          std::cout<<statisticsFilter->GetMinimum(label)<<" [Gy]"<<std::endl;
          std::cout<<"Dose max: ";
          std::cout<<statisticsFilter->GetMaximum(label)<<" [Gy]"<<std::endl;
          std::cout<<" "<<std::endl;
          std::cout<<"#Dose[Gy] Volume_diff[No. of voxels]] Volume_cumul[No. of voxels] Volume_diff[%] Volume_cumul[%] Volume_diff[cc] Volume_cumul[cc]"<<std::endl;
          for( int i =0; i <m_ArgsInfo.bins_arg; i++)
          {
            double percentDiffVolume = ((dvhistogram->GetFrequency(i))*100)/(statisticsFilter->GetCount(label));
            double popCumulativeVolume = 0;
            for(int j=0; j<i; j++)
            {
                popCumulativeVolume+=(dvhistogram->GetFrequency(j));
            }
            double cumulativeVolume = popCumulativeVolume + (dvhistogram->GetFrequency(i));
            double percentCumulativeVolume =(cumulativeVolume*100)/(statisticsFilter->GetCount(label)) ;
            double ccCumulativeVolume = (popCumulativeVolume + (dvhistogram->GetFrequency(i)))*spacing_cc;
            std::cout<<dvhistogram->GetBinMax(0,i)<<"\t   "<<dvhistogram->GetFrequency(i)<<"\t  "<<cumulativeVolume<<"\t  "<<percentDiffVolume<<"\t "<<percentCumulativeVolume<<"\t  "<<((dvhistogram->GetFrequency(i))*spacing_cc)<<"\t  "<<ccCumulativeVolume<<std::endl;
          }

          // Add to the file
          std::ofstream dvhistogramFile(m_ArgsInfo.dvhistogram_arg);
          dvhistogramFile<<"Total volume : ";
          dvhistogramFile<<statisticsFilter->GetCount(label)<<" [No. of voxels]"<<std::endl;
          dvhistogramFile<<"Total volume : ";
          dvhistogramFile<<((statisticsFilter->GetCount(label))*spacing_cc)<<" [cc]"<<std::endl;
          dvhistogramFile<<"Dose mean: ";
          dvhistogramFile<<statisticsFilter->GetMean(label)<<" [Gy]"<<std::endl;
          dvhistogramFile<<"Dose min: ";
          dvhistogramFile<<statisticsFilter->GetMinimum(label)<<" [Gy]"<<std::endl;
          dvhistogramFile<<"Dose max: ";
          dvhistogramFile<<statisticsFilter->GetMaximum(label)<<" [Gy]"<<std::endl;
          dvhistogramFile<<"  "<<std::endl;
          dvhistogramFile<<"#Dose[Gy] Volume_diff[No. of voxels]] Volume_cumul[No. of voxels] Volume_diff[%] Volume_cumul[%] Volume_diff[cc] Volume_cumul[cc]"<<std::endl;
          for( int i =0; i <m_ArgsInfo.bins_arg; i++)
          {
             double percentDiffVolume = ((dvhistogram->GetFrequency(i))*100)/(statisticsFilter->GetCount(label));
             double popCumulativeVolume = 0;
             for(int j=0; j<i; j++)
             {
                 popCumulativeVolume+=(dvhistogram->GetFrequency(j));
             }
             double cumulativeVolume = popCumulativeVolume + (dvhistogram->GetFrequency(i));
             double percentCumulativeVolume =(cumulativeVolume*100)/(statisticsFilter->GetCount(label)) ;
             double ccCumulativeVolume = (popCumulativeVolume + (dvhistogram->GetFrequency(i)))*spacing_cc;
             dvhistogramFile<<dvhistogram->GetBinMax(0,i)<<"\t "<<dvhistogram->GetFrequency(i)<<"\t "<<cumulativeVolume<<"\t "<<percentDiffVolume<<"\t "<<percentCumulativeVolume<<"\t "<<((dvhistogram->GetFrequency(i))*spacing_cc)<<"\t "<<ccCumulativeVolume<<std::endl;
         }
	  }
    }
  }

  return;

}

}//end clitk

#endif //#define clitkDVHGenericFilter_txx
