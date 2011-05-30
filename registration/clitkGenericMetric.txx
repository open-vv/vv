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

#ifndef __clitkGenericMetric_txx
#define __clitkGenericMetric_txx

#include "clitkGenericMetric.h"
#include <itkImageMaskSpatialObject.h>
#include <ctime>

namespace clitk
{

//=========================================================================================================================
//constructor
template <class args_info_type, class FixedImageType, class MovingImageType>
GenericMetric<args_info_type, FixedImageType, MovingImageType>::GenericMetric()
{
  m_Metric=NULL;
  m_Maximize=false;
  m_Verbose=false;
  m_FixedImageRegionGiven=false;
#ifdef ITK_USE_OPTIMISED_REGISTRATION_METHODS
  m_FixedImageSamplesIntensityThreshold=0;
  m_UseFixedImageSamplesIntensityThreshold=false;
#endif
  m_FixedImageMask=NULL;
}


//=========================================================================================================================
//Get the pointer
template <class args_info_type, class FixedImageType, class MovingImageType>
typename GenericMetric<args_info_type, FixedImageType, MovingImageType>::MetricPointer
GenericMetric<args_info_type,FixedImageType, MovingImageType>::GetMetricPointer()
{
  //============================================================================
  // Sanity check:
  // The image is required for the region and spacing
  if( ! this->m_FixedImage ) {
    itkExceptionMacro( "Fixed Image has not been set" );
  }

  // If the image come from a filter, then update that filter.
  if( this->m_FixedImage->GetSource() ) {
    this->m_FixedImage->Update();
  }

  // The metric region
  if( !  m_FixedImageRegionGiven ) {
    m_FixedImageRegion=m_FixedImage->GetLargestPossibleRegion();
  }


  //============================================================================
  //switch on  the  metric type chosen adn set metric specific members
  switch (m_ArgsInfo.metric_arg) {

  case 0: {
    typename itk::MeanSquaresImageToImageMetric< FixedImageType, MovingImageType >::Pointer m =
      itk::MeanSquaresImageToImageMetric< FixedImageType, MovingImageType >::New();

    //Set Parameters for this metric
    m_Maximize=false;
    if (m_Verbose) std::cout<<"Using the mean squares metric..."<<std::endl;
    m_Metric=m;
    break;
  }

  case 1: {
    typename  clitk::NormalizedCorrelationImageToImageMetric< FixedImageType, MovingImageType >::Pointer m
    =clitk::NormalizedCorrelationImageToImageMetric< FixedImageType, MovingImageType >::New();

    //Set Parameters for this metric
    m->SetSubtractMean(m_ArgsInfo.subtractMean_flag);
    m_Maximize=false;
    if (m_Verbose) {
      if ( !m_ArgsInfo.subtractMean_flag) std::cout<<"Using the normalized correlation metric without subtracting the mean..."<<std::endl;
      else  std::cout<<"Using the normalized correlation metric with subtraction of mean..."<<std::endl;
    }
    m_Metric=m;
    break;
  }

  case 2: {
    typename itk::CorrelationCoefficientHistogramImageToImageMetric< FixedImageType, MovingImageType >::Pointer m
    = itk::CorrelationCoefficientHistogramImageToImageMetric< FixedImageType, MovingImageType >::New();

    //Set parameters for this metric
    typename itk::CorrelationCoefficientHistogramImageToImageMetric<FixedImageType, MovingImageType>::HistogramSizeType size;
    for (unsigned int i=0; i < FixedImageDimension; i++)size[i]=m_ArgsInfo.bins_arg;
    m->SetHistogramSize(size);
    m_Maximize=false;
    if (m_Verbose) std::cout<<"Using the histogram correlation coefficient metric..."<<std::endl;
    m_Metric = m;
    break;
  }

  case 3: {
    typename itk::GradientDifferenceImageToImageMetric< FixedImageType, MovingImageType >::Pointer m
    = itk::GradientDifferenceImageToImageMetric< FixedImageType, MovingImageType >::New();

    //Set parameters for this metric
    m_Maximize=false;
    if (m_Verbose) std::cout<<"Using the gradient difference metric..."<<std::endl;
    m_Metric=m;
    break;
  }

  case 4: {
    typename itk::MutualInformationImageToImageMetric< FixedImageType, MovingImageType >::Pointer m
    = itk::MutualInformationImageToImageMetric< FixedImageType, MovingImageType >::New();

    //Set parameters for this metric
    m->SetFixedImageStandardDeviation( m_ArgsInfo.stdDev_arg );
    m->SetMovingImageStandardDeviation( m_ArgsInfo.stdDev_arg);
    m_Maximize=true;

    //Randomize samples if demanded
    if (m_ArgsInfo.random_flag ) m->ReinitializeSeed();
    else m->ReinitializeSeed(0);
    if (m_Verbose) std::cout<<"Using Viola-Wells MI..."<<std::endl;
    m_Metric=m;
    break;
  }

  case 5: {
    typename itk::MutualInformationHistogramImageToImageMetric< FixedImageType, MovingImageType >::Pointer m
    = itk::MutualInformationHistogramImageToImageMetric< FixedImageType, MovingImageType>::New();

    //Set parameters for this metric
    typename itk::MutualInformationHistogramImageToImageMetric<FixedImageType, MovingImageType>::HistogramSizeType size;
    for (unsigned int i=0; i < FixedImageDimension; i++)size[i]=m_ArgsInfo.bins_arg;
    m->SetHistogramSize(size);
    m_Maximize=true;
    if (m_Verbose) std::cout<<"Using the histogram MI with "<<m_ArgsInfo.bins_arg<<" bins..."<<std::endl;
    m_Metric=m;
    break;
  }

  case 6: {
    typename itk::MattesMutualInformationImageToImageMetric< FixedImageType, MovingImageType >::Pointer m
    = itk::MattesMutualInformationImageToImageMetric< FixedImageType, MovingImageType >::New();

    //Set parameters for this metric
    m_Maximize=false;
    m->SetNumberOfHistogramBins(m_ArgsInfo.bins_arg);

    //Randomize samples if demanded
    if (m_ArgsInfo.random_flag ) m->ReinitializeSeed();
    else m->ReinitializeSeed(0);

    // Two ways of calculating the derivatives
    m->SetUseExplicitPDFDerivatives(m_ArgsInfo.explicitPDFDerivatives_flag);


    if (m_Verbose) {
      std::cout<<"Using Mattes' MI with "<<m_ArgsInfo.bins_arg<<" bins..."<<std::endl;
      if (m_ArgsInfo.explicitPDFDerivatives_flag) std::cout<<"Calculating PDFs explicitly..."<<std::endl;
    }
    m_Metric=m;
    break;
  }

  case 7: {
    typename itk::NormalizedMutualInformationHistogramImageToImageMetric< FixedImageType, MovingImageType >::Pointer m
    =itk::NormalizedMutualInformationHistogramImageToImageMetric< FixedImageType, MovingImageType >::New();

    //Set parameters for this metric
    typename itk::NormalizedMutualInformationHistogramImageToImageMetric<FixedImageType, MovingImageType>::HistogramSizeType size;
    for (unsigned int i=0; i < FixedImageDimension; i++)size[i]=m_ArgsInfo.bins_arg;
    m->SetHistogramSize(size);
    m_Maximize=false;
    if (m_Verbose) std::cout<<"Using the normalized MI with "<<m_ArgsInfo.bins_arg<<" bins..."<<std::endl;
    m_Metric=m;
    break;
  }

  case 8: {
    typename clitk::CorrelationRatioImageToImageMetric< FixedImageType, MovingImageType >::Pointer m
    = clitk::CorrelationRatioImageToImageMetric< FixedImageType, MovingImageType >::New();

    //Set parameters for this metric
    m_Maximize=false;
    m->SetNumberOfBins(m_ArgsInfo.bins_arg);
    if (m_Verbose) std::cout<<"Using the correlation ratio..."<<std::endl;
    m_Metric =m;
    break;
  }

  case 9: {
    typename itk::MeanSquaresImageToImageMetricFor3DBLUTFFD< FixedImageType, MovingImageType >::Pointer m
    = itk::MeanSquaresImageToImageMetricFor3DBLUTFFD< FixedImageType, MovingImageType >::New();

    //Set Parameters for this metric
    m_Maximize=false;
    if (m_Verbose) std::cout<<"Using the mean squares metric for 3D BLUT FFD..."<<std::endl;
    m_Metric=m;
    break;
  }

  case 10: {
    typename  clitk::NormalizedCorrelationImageToImageMetricFor3DBLUTFFD< FixedImageType, MovingImageType >::Pointer m
    =clitk::NormalizedCorrelationImageToImageMetricFor3DBLUTFFD< FixedImageType, MovingImageType >::New();

    //Set Parameters for this metric
    m->SetSubtractMean(m_ArgsInfo.subtractMean_flag);
    m_Maximize=false;
    if (m_Verbose) {
      if ( !m_ArgsInfo.subtractMean_flag) std::cout<<"Using the normalized correlation metric for 3D BLUT FFD without subtracting the mean..."<<std::endl;
      else  std::cout<<"Using the normalized correlation metric 3D BLUT FFD with subtraction of mean..."<<std::endl;
    }
    m_Metric=m;
    break;
  }

  case 11: {
    typename itk::MattesMutualInformationImageToImageMetricFor3DBLUTFFD< FixedImageType, MovingImageType >::Pointer m
    = itk::MattesMutualInformationImageToImageMetricFor3DBLUTFFD< FixedImageType, MovingImageType >::New();

    //Set parameters for this metric
    m_Maximize=false;
    m->SetNumberOfHistogramBins(m_ArgsInfo.bins_arg);

    //Randomize samples if demanded
    if (m_ArgsInfo.random_flag ) m->ReinitializeSeed();
    else m->ReinitializeSeed(0);

    // Two ways of calculating the derivatives
    m->SetUseExplicitPDFDerivatives(m_ArgsInfo.explicitPDFDerivatives_flag);


    if (m_Verbose) {
      std::cout<<"Using Mattes' MI for 3D BLUT FFD with "<<m_ArgsInfo.bins_arg<<" bins..."<<std::endl;
      if (m_ArgsInfo.explicitPDFDerivatives_flag) std::cout<<"Calculating PDFs explicitly..."<<std::endl;
    }
    m_Metric=m;
    break;
  }

  }


  typedef itk::ImageMaskSpatialObject<itkGetStaticConstMacro(FixedImageDimension)> ImageMaskSpatialObjectType;
  typename ImageMaskSpatialObjectType::ConstPointer mask = NULL;
  if (m_FixedImageMask.IsNotNull())
    mask = dynamic_cast<const ImageMaskSpatialObjectType*>(m_FixedImageMask.GetPointer());

  typedef typename ImageMaskSpatialObjectType::RegionType ImageMaskRegionType;
  ImageMaskRegionType mask_region;
  if (mask.IsNotNull())
    mask_region = mask->GetAxisAlignedBoundingBoxRegion();

  // Common properties
  if( m_FixedImageMask.IsNotNull() )
    m_Metric->SetFixedImageMask(m_FixedImageMask);

  m_Metric->SetFixedImageRegion(m_FixedImageRegion);
  //m_Metric->SetFixedImageRegion(mask_region);


#ifdef ITK_USE_OPTIMIZED_REGISTRATION_METHODS

  //============================================================================
  // Set the lower intensity threshold
  if (m_ArgsInfo.intThreshold_given) {
    m_UseFixedImageSamplesIntensityThreshold=true;
    m_FixedImageSamplesIntensityThreshold=m_ArgsInfo.intThreshold_arg;
    m_Metric->SetFixedImageSamplesIntensityThreshold(m_FixedImageSamplesIntensityThreshold);
    if (m_Verbose) std::cout<<"Setting the fixed image intensity threshold to "<<m_FixedImageSamplesIntensityThreshold<<"..."<<std::endl;
  }


  //============================================================================
  // Set the number of samples

  // Sample all pixel
  if ( ( m_ArgsInfo.samples_arg==1.0) && (m_FixedImageMask.IsNull()) && (!m_UseFixedImageSamplesIntensityThreshold ) ) {
    m_Metric->SetUseAllPixels(true);
    if (m_Verbose) std::cout<<"Using all pixels (a fraction of "<<m_ArgsInfo.samples_arg<<")..."<<std::endl;
  }
  // JV the optimized metric will resample points to obtain the number of pixels:
  // Pass the correct number of spatial samples, with indexes
  else {
    std::vector<typename FixedImageType::IndexType> fiic;// fixedImageindexContainer
    unsigned int numberOfValidPixels=0;
    FixedImageIndexType index;
    FixedImagePointType inputPoint;

    // Calculate the number
    const unsigned int totalNumberOfPixels = m_FixedImageRegion.GetNumberOfPixels();
    const unsigned int totalNumberOfMaskPixels = mask_region.GetNumberOfPixels();
    const unsigned int numberOfDemandedPixels =  static_cast< unsigned int >( (double) totalNumberOfPixels *m_ArgsInfo.samples_arg );

    // --------------------------------------------------
    // Sample whole image sequentially and pass the indexes
    if (m_ArgsInfo.samples_arg==1.0) {
      if (m_Verbose) std::cout<<"Sequentially scanning the image for valid pixels..."<<std::endl;

      // Set up a region interator within the user specified fixed image region.
      typedef ImageRegionConstIteratorWithIndex<FixedImageType> RegionIterator;
      RegionIterator regionIter( m_FixedImage, m_FixedImageRegion );
      //RegionIterator regionIter( m_FixedImage, mask_region );

      // go over the whole region
      regionIter.GoToBegin();
      while(!regionIter.IsAtEnd() ) {

        // Get sampled index
        index = regionIter.GetIndex();

        // Mask?
        if( m_FixedImageMask.IsNotNull() ) {
          // Check if the Index is inside the mask, translate index to point
          m_FixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

          // If not inside the mask, ignore the point
          if( !m_FixedImageMask->IsInside( inputPoint ) ) {
            ++regionIter; // jump to next pixel
            continue;
          }
        }

        // Intensity?
        if( m_UseFixedImageSamplesIntensityThreshold &&
            ( regionIter.Get() < m_FixedImageSamplesIntensityThreshold) ) {
          ++regionIter; // jump to next pixel
          continue;
        }

        // Add point to the numbers
        fiic.push_back(index);
        ++numberOfValidPixels;
        ++regionIter;
      }
    }

    // --------------------------------------------------
    // Sample randomly
    else {
      if (m_Verbose) std::cout<<"Randomly scanning the image for valid pixels..."<<std::endl;

      // Set up a random interator within the user specified fixed image region.
      typedef ImageRandomConstIteratorWithIndex<FixedImageType> RandomIterator;
      
      
      RandomIterator randIter( m_FixedImage, m_FixedImageRegion );
      //RandomIterator randIter( m_FixedImage, mask_region );
      
      if (m_Verbose) std::cout << "Search region " << m_FixedImageRegion << std::endl;
      if (m_Verbose) std::cout << "Mask search region " << mask_region << std::endl;

      // Randomly sample the image
      short att = 1;
      short natts = 5;
      while (att <= natts) {
        if (m_Verbose) std::cout << "Attempt " << att << std::endl;
          
        int count_out = 0;
        int count_not_thres = 0;
        clock_t c = clock();
        randIter.ReinitializeSeed(c);
        randIter.SetNumberOfSamples( numberOfDemandedPixels * 1000 );
        randIter.GoToBegin();
        //int cnt = 0;
        while( (!randIter.IsAtEnd()) && (numberOfValidPixels<=numberOfDemandedPixels)  ) {
          // Get sampled index
          index = randIter.GetIndex();
          //if (m_Verbose) std::cout << "testing pixel " << index << std::endl;

          // Mask?
          if( m_FixedImageMask.IsNotNull() ) {

            // Check if the Index is inside the mask, translate index to point
            m_FixedImage->TransformIndexToPhysicalPoint( index, inputPoint );

            // If not inside the mask, ignore the point
            if( !m_FixedImageMask->IsInside( inputPoint ) ) {
              ++randIter; // jump to next pixel
              //if (m_Verbose) std::cout << "not inside " << inputPoint << std::endl;
              count_out++;
              continue;
            }

          }

          // Intensity?
          if( m_UseFixedImageSamplesIntensityThreshold &&
              randIter.Get() < m_FixedImageSamplesIntensityThreshold ) {
            ++randIter;
              //if (m_Verbose) std::cout << "not in threshold" << std::endl;
              count_not_thres++;
            continue;
          }

          // Add point to the numbers
          fiic.push_back(index);
          ++numberOfValidPixels;
          ++randIter;
        }
        
        if (m_Verbose) std::cout << "points outside = " << count_out << std::endl;
        if (m_Verbose) std::cout << "points not in threshold = " << count_not_thres << std::endl;
          
        if (fiic.size())
           break;
        
        att++;
      }
    }


    // Set the indexes of valid samples
    m_Metric->SetFixedImageIndexes(fiic);
    // m_Metric->SetNumberOfSpatialSamples( numberOfValidPixels);
    if (m_Verbose) std::cout<<"A fraction of "<<m_ArgsInfo.samples_arg<<" spatial samples was requested..."<<std::endl;
    double fraction=(double)numberOfValidPixels/ (double) totalNumberOfPixels;
    if (m_Verbose) std::cout<<"Found "<<numberOfValidPixels <<" valid pixels for a total of "<<totalNumberOfPixels<<" (a fraction of "<<fraction<<")..."<<std::endl;
    if (m_Verbose) std::cout<<"number of mask pixels "<<totalNumberOfMaskPixels<<std::endl;

  }

#else
  if (m_Verbose) std::cout<<"Not setting the fixed image intensity threshold or the fraction of samples to use (not compiled with USE_OPTIMIZED_REGISTRATION_METHODS)..."<<std::endl;


#endif
  //============================================================================
  //return the pointer
  return m_Metric;
}

}

#endif
