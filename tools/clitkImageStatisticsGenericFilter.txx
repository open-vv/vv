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
#ifndef clitkImageStatisticsGenericFilter_txx
#define clitkImageStatisticsGenericFilter_txx

#include "itkNthElementImageAdaptor.h"
#include "itkJoinSeriesImageFilter.h"

#include "clitkImageStatisticsGenericFilter.h"
#include "clitkCropLikeImageFilter.h"
#include "clitkResampleImageWithOptionsFilter.h"

namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<unsigned int Dimension, unsigned int Components>
  void
  ImageStatisticsGenericFilter::UpdateWithDim(std::string PixelType)
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
  ImageStatisticsGenericFilter::UpdateWithDimAndPixelType()
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
            resampler->SetOutputOrigin(labelImage->GetOrigin());
            resampler->SetGaussianFilteringEnabled(false);
            resampler->Update();
            labelImage = resampler->GetOutput();
            //writeImage<LabelImageType>(labelImage, "test1.mha");

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

        std::cout<<std::endl;
        if (m_Verbose) std::cout<<"-------------"<<std::endl;
        if (m_Verbose) std::cout<<"| Label: "<<label<<"  |"<<std::endl;
        if (m_Verbose) std::cout<<"-------------"<<std::endl;

        // Histograms
        if (m_ArgsInfo.histogram_given) {
          statisticsFilter->SetUseHistograms(true);
          statisticsFilter->SetHistogramParameters(m_ArgsInfo.bins_arg, m_ArgsInfo.lower_arg, m_ArgsInfo.upper_arg);
        }
        statisticsFilter->Update();

        // Output
        if (m_Verbose) std::cout<<"N° of pixels: ";
          std::cout<<statisticsFilter->GetCount(label)<<std::endl;

        if (m_Verbose) std::cout<<"Mean: ";
          std::cout<<statisticsFilter->GetMean(label)<<std::endl;

        if (m_Verbose) std::cout<<"SD: ";
          std::cout<<statisticsFilter->GetSigma(label)<<std::endl;

        if (m_Verbose) std::cout<<"Variance: ";
          std::cout<<statisticsFilter->GetVariance(label)<<std::endl;

        if (m_Verbose) std::cout<<"Min: ";
          std::cout<<statisticsFilter->GetMinimum(label)<<std::endl;

        if (m_Verbose) std::cout<<"Max: ";
          std::cout<<statisticsFilter->GetMaximum(label)<<std::endl;

        if (m_Verbose) std::cout<<"Sum: ";
          std::cout<<statisticsFilter->GetSum(label)<<std::endl;

        if (m_Verbose) std::cout<<"Bounding box: ";

        for(unsigned int i =0; i <statisticsFilter->GetBoundingBox(label).size(); i++)
          std::cout<<statisticsFilter->GetBoundingBox(label)[i]<<" ";
        std::cout<<std::endl;

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
      }
    }

    return;

  }


}//end clitk

#endif //#define clitkImageStatisticsGenericFilter_txx
