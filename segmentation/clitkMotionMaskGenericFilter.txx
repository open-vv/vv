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
#ifndef clitkMotionMaskGenericFilter_txx
#define clitkMotionMaskGenericFilter_txx

/* =================================================
 * @file   clitkMotionMaskGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/


namespace clitk
{

//-------------------------------------------------------------------
// Update with the number of dimensions
//-------------------------------------------------------------------
template<unsigned int Dimension>
void
MotionMaskGenericFilter::UpdateWithDim(std::string PixelType)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

  if(PixelType == "short") {
    if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, signed short>();
  }
  //    else if(PixelType == "unsigned_short"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, unsigned short>();
  //     }

  //     else if (PixelType == "unsigned_char"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, unsigned char>();
  //     }

  //     else if (PixelType == "char"){
  //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, signed char>();
  //     }
  else {
    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, float>();
  }
}

//-------------------------------------------------------------------
// Air
//-------------------------------------------------------------------
template <unsigned int Dimension, class  PixelType>
typename itk::Image<MotionMaskGenericFilter::InternalPixelType, Dimension>::Pointer
MotionMaskGenericFilter::GetAirImage(typename itk::Image<PixelType, Dimension>::Pointer input,
                                     typename itk::Image<MotionMaskGenericFilter::InternalPixelType, Dimension>::Pointer lungs)
{

  // ImageTypes
  typedef int InternalPixelType;
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image< InternalPixelType, Dimension> InternalImageType; //labeling doesn't work with unsigned char?

  //----------------------------------------------------------------------------------------------------
  // Typedef for Processing
  //----------------------------------------------------------------------------------------------------
  typedef itk::ImageFileReader<InternalImageType> FeatureReaderType;
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> InputBinarizeFilterType;
  typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> InversionFilterType;
  typedef itk::ThresholdImageFilter<InternalImageType> ThresholdFilterType;
  typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
  typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelFilterType;
  typedef itk::MirrorPadImageFilter<InternalImageType, InternalImageType> MirrorPadImageFilterType;


  typename InternalImageType::Pointer air;
  if (m_ArgsInfo.featureAir_given) {
    typename FeatureReaderType::Pointer featureReader =FeatureReaderType::New();
    featureReader->SetFileName(m_ArgsInfo.featureAir_arg);
    if (m_Verbose) std::cout<<"Reading the air feature image..."<<std::endl;
    featureReader->Update();
    air=featureReader->GetOutput();

    //---------------------------------
    // Pad
    //---------------------------------
    if(m_ArgsInfo.pad_flag) {
      typedef itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;
      IteratorType it(air, air->GetLargestPossibleRegion());
      typename InternalImageType::IndexType index;
      while(!it.IsAtEnd()) {
        index=it.GetIndex();

        //Pad borders of all dimensions but 2 (cranio-caudal)
        for (unsigned int i=0; i<Dimension; i++){
          if(i==2)
            continue;
          if(index[i]==air->GetLargestPossibleRegion().GetIndex()[i]
              || index[i]==(unsigned int)air->GetLargestPossibleRegion().GetIndex()[i]+ (unsigned int) air->GetLargestPossibleRegion().GetSize()[i]-1)
            it.Set(1);
        }
        ++it;
      }
    }
  } else {
    if (m_Verbose) std::cout<<"Extracting the air feature image..."<<std::endl;
    //---------------------------------
    // Binarize the image
    //---------------------------------
    typename InputBinarizeFilterType::Pointer binarizeFilter=InputBinarizeFilterType::New();
    binarizeFilter->SetInput(input);
    binarizeFilter->SetLowerThreshold(static_cast<PixelType>(m_ArgsInfo.lowerThresholdAir_arg));
    binarizeFilter->SetUpperThreshold(static_cast<PixelType>(m_ArgsInfo.upperThresholdAir_arg));
    if (m_Verbose) std::cout<<"Binarizing the image using thresholds "<<m_ArgsInfo.lowerThresholdAir_arg
                              <<", "<<m_ArgsInfo.upperThresholdAir_arg<<"..."<<std::endl;
    binarizeFilter->Update();
    air = binarizeFilter->GetOutput();

    //---------------------------------
    // Pad
    //---------------------------------
    if(m_ArgsInfo.pad_flag) {
      typedef itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;
      IteratorType it(air, air->GetLargestPossibleRegion());
      typename InternalImageType::IndexType index;
      while(!it.IsAtEnd()) {
        index=it.GetIndex();

        //Pad borders of all dimensions but 2 (cranio-caudal)
        for (unsigned int i=0; i<Dimension; i++){
          if(i==2)
            continue;
          if(index[i]==air->GetLargestPossibleRegion().GetIndex()[i]
              || index[i]==(unsigned int)air->GetLargestPossibleRegion().GetIndex()[i]+ (unsigned int) air->GetLargestPossibleRegion().GetSize()[i]-1)
            it.Set(binarizeFilter->GetInsideValue());
        }
        ++it;
      }
    }

    //---------------------------------
    // Remove lungs (in place)
    //---------------------------------
    typedef itk::ImageRegionIterator<InternalImageType> IteratorType;
    IteratorType itAir(air, binarizeFilter->GetOutput()->GetLargestPossibleRegion());
    IteratorType itLungs(lungs, binarizeFilter->GetOutput()->GetLargestPossibleRegion()); //lungs padded, used air region
    itAir.GoToBegin();
    itLungs.GoToBegin();
    while(!itAir.IsAtEnd()) {
      if(!itLungs.Get())     // The lungs are set to 0 at that stage
        itAir.Set(0);
      ++itAir;
      ++itLungs;
    }

    //---------------------------------
    // Label the connected components
    //---------------------------------
    typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
    connectFilter->SetInput(binarizeFilter->GetOutput());
    connectFilter->SetBackgroundValue(0);
    connectFilter->SetFullyConnected(false);
    if (m_Verbose) std::cout<<"Labeling the connected components..."<<std::endl;

    //---------------------------------
    // Sort the labels according to size
    //---------------------------------
    typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
    relabelFilter->SetInput(connectFilter->GetOutput());
    if (m_Verbose) std::cout<<"Sorting the labels..."<<std::endl;

    //---------------------------------
    // Keep the label
    //---------------------------------
    typename ThresholdFilterType::Pointer thresholdFilter=ThresholdFilterType::New();
    thresholdFilter->SetInput(relabelFilter->GetOutput());
    thresholdFilter->SetUpper(1);
    if (m_Verbose) std::cout<<"Keeping the first label..."<<std::endl;
    thresholdFilter->Update();
    air=thresholdFilter->GetOutput();
  }

  //---------------------------------
  // Invert
  //---------------------------------
  typename InversionFilterType::Pointer inversionFilter=InversionFilterType::New();
  inversionFilter->SetInput(air);
  inversionFilter->SetLowerThreshold(0);
  inversionFilter->SetUpperThreshold(0);
  inversionFilter->SetInsideValue(1);
  inversionFilter->Update();

  //---------------------------------
  // Mirror
  //---------------------------------
  typename MirrorPadImageFilterType::Pointer mirrorPadImageFilter=MirrorPadImageFilterType::New();
  mirrorPadImageFilter->SetInput(inversionFilter->GetOutput());
  typename InternalImageType::SizeType padSize;
  padSize.Fill(0);
  padSize[2]=input->GetLargestPossibleRegion().GetSize()[2];
  mirrorPadImageFilter->SetPadLowerBound(padSize);
  if (m_Verbose) std::cout<<"Mirroring the entire image along the CC axis..."<<std::endl;
  mirrorPadImageFilter->Update();
  air=mirrorPadImageFilter->GetOutput();
  //writeImage<InternalImageType>(air,"/home/srit/tmp/air.mhd");

  return air;
}


//-------------------------------------------------------------------
// Bones
//-------------------------------------------------------------------
template <unsigned int Dimension, class  PixelType>
typename itk::Image<MotionMaskGenericFilter::InternalPixelType, Dimension>::Pointer
MotionMaskGenericFilter::GetBonesImage(typename itk::Image<PixelType, Dimension>::Pointer input )
{

  // ImageTypes
  typedef int InternalPixelType;
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image< InternalPixelType, Dimension> InternalImageType; //labeling doesn't work with unsigned char?

  //----------------------------------------------------------------------------------------------------
  // Typedef for Processing
  //----------------------------------------------------------------------------------------------------
  typedef itk::ImageFileReader<InternalImageType> FeatureReaderType;
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> InputBinarizeFilterType;
  typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> InversionFilterType;
  typedef itk::ThresholdImageFilter<InternalImageType> ThresholdFilterType;
  typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
  typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelFilterType;
  typedef itk::MirrorPadImageFilter<InternalImageType, InternalImageType> MirrorPadImageFilterType;


  typename InternalImageType::Pointer bones;
  if (m_ArgsInfo.featureBones_given) {
    typename FeatureReaderType::Pointer featureReader =FeatureReaderType::New();
    featureReader->SetFileName(m_ArgsInfo.featureBones_arg);
    if (m_Verbose) std::cout<<"Reading the bones feature image..."<<std::endl;
    featureReader->Update();
    bones=featureReader->GetOutput();
  } else {
    if (m_Verbose) std::cout<<"Extracting the bones feature image..."<<std::endl;
    //---------------------------------
    // Binarize the image
    //---------------------------------
    typename InputBinarizeFilterType::Pointer binarizeFilter=InputBinarizeFilterType::New();
    binarizeFilter->SetInput(input);
    binarizeFilter->SetLowerThreshold(static_cast<PixelType>(m_ArgsInfo.lowerThresholdBones_arg));
    binarizeFilter->SetUpperThreshold(static_cast<PixelType>(m_ArgsInfo.upperThresholdBones_arg));
    if (m_Verbose) std::cout<<"Binarizing the image using thresholds "<<m_ArgsInfo.lowerThresholdBones_arg
                              <<", "<<m_ArgsInfo.upperThresholdBones_arg<<"..."<<std::endl;

    //---------------------------------
    // Label the connected components
    //---------------------------------
    typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
    connectFilter->SetInput(binarizeFilter->GetOutput());
    connectFilter->SetBackgroundValue(0);
    connectFilter->SetFullyConnected(false);
    if (m_Verbose) std::cout<<"Labeling the connected components..."<<std::endl;

    //---------------------------------
    // Sort the labels according to size
    //---------------------------------
    typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
    relabelFilter->SetInput(connectFilter->GetOutput());
    if (m_Verbose) std::cout<<"Sorting the labels..."<<std::endl;

    //---------------------------------
    // Keep the label
    //---------------------------------
    typename ThresholdFilterType::Pointer thresholdFilter=ThresholdFilterType::New();
    thresholdFilter->SetInput(relabelFilter->GetOutput());
    thresholdFilter->SetUpper(1);
    if (m_Verbose) std::cout<<"Keeping the first label..."<<std::endl;
    thresholdFilter->Update();
    bones=thresholdFilter->GetOutput();

  }

  //---------------------------------
  // Invert
  //---------------------------------
  typename InversionFilterType::Pointer inversionFilter=InversionFilterType::New();
  inversionFilter->SetInput(bones);
  inversionFilter->SetLowerThreshold(0);
  inversionFilter->SetUpperThreshold(0);
  inversionFilter->SetInsideValue(1);
  inversionFilter->Update();

  //---------------------------------
  // Mirror
  //---------------------------------
  typename MirrorPadImageFilterType::Pointer mirrorPadImageFilter=MirrorPadImageFilterType::New();
  mirrorPadImageFilter->SetInput(inversionFilter->GetOutput());
  typename InternalImageType::SizeType padSize;
  padSize.Fill(0);
  padSize[2]=input->GetLargestPossibleRegion().GetSize()[2];
  mirrorPadImageFilter->SetPadLowerBound(padSize);
  if (m_Verbose) std::cout<<"Mirroring the entire image along the CC axis..."<<std::endl;
  mirrorPadImageFilter->Update();
  bones=mirrorPadImageFilter->GetOutput();
  // writeImage<InternalImageType>(bones,"/home/jef/tmp/bones.mhd");

  return bones;
}




//-------------------------------------------------------------------
// Lungs
//-------------------------------------------------------------------
template <unsigned int Dimension, class  PixelType>
typename itk::Image<MotionMaskGenericFilter::InternalPixelType, Dimension>::Pointer
MotionMaskGenericFilter::GetLungsImage(typename itk::Image<PixelType, Dimension>::Pointer input )
{
  // ImageTypes
  typedef int InternalPixelType;
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image< InternalPixelType, Dimension> InternalImageType; //labeling doesn't work with unsigned char?

  //----------------------------------------------------------------------------------------------------
  // Typedef for Processing
  //----------------------------------------------------------------------------------------------------
  typedef itk::ImageFileReader<InternalImageType> FeatureReaderType;
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> InputBinarizeFilterType;
  typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> InversionFilterType;
  typedef itk::ThresholdImageFilter<InternalImageType> ThresholdFilterType;
  typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
  typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelFilterType;
  typedef itk::MirrorPadImageFilter<InternalImageType, InternalImageType> MirrorPadImageFilterType;

  typename InternalImageType::Pointer lungs;
  if (m_ArgsInfo.featureLungs_given) {
    typename FeatureReaderType::Pointer featureReader =FeatureReaderType::New();
    featureReader->SetFileName(m_ArgsInfo.featureLungs_arg);
    if (m_Verbose) std::cout<<"Reading the lungs feature image..."<<std::endl;
    featureReader->Update();
    lungs=featureReader->GetOutput();
  } else {
    if (m_Verbose) std::cout<<"Extracting the lungs feature image..."<<std::endl;
    //---------------------------------
    // Binarize the image
    //---------------------------------
    typename InputBinarizeFilterType::Pointer binarizeFilter=InputBinarizeFilterType::New();
    binarizeFilter->SetInput(input);
    binarizeFilter->SetLowerThreshold(static_cast<PixelType>(m_ArgsInfo.lowerThresholdLungs_arg));
    binarizeFilter->SetUpperThreshold(static_cast<PixelType>(m_ArgsInfo.upperThresholdLungs_arg));
    if (m_Verbose) std::cout<<"Binarizing the image using a threshold "<<m_ArgsInfo.lowerThresholdLungs_arg
                              <<", "<<m_ArgsInfo.upperThresholdLungs_arg<<"..."<<std::endl;

    //---------------------------------
    // Label the connected components
    //---------------------------------
    typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
    connectFilter->SetInput(binarizeFilter->GetOutput());
    connectFilter->SetBackgroundValue(0);
    connectFilter->SetFullyConnected(true);
    if (m_Verbose) std::cout<<"Labeling the connected components..."<<std::endl;
    connectFilter->Update();
    if (m_Verbose) std::cout<<"found "<< connectFilter->GetObjectCount() << std::endl;

    //---------------------------------
    // Sort the labels according to size
    //---------------------------------
    typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
    relabelFilter->SetInput(connectFilter->GetOutput());
    if (m_Verbose) std::cout<<"Sorting the labels..."<<std::endl;
    // writeImage<InternalImageType> (relabelFilter->GetOutput(), "/home/vdelmon/tmp/labels.mhd");

    //---------------------------------
    // Keep the label
    //---------------------------------
    typename ThresholdFilterType::Pointer thresholdFilter=ThresholdFilterType::New();
    thresholdFilter->SetInput(relabelFilter->GetOutput());
    thresholdFilter->SetLower(1);
    thresholdFilter->SetUpper(2);
    if (m_Verbose) std::cout<<"Keeping the first two labels..."<<std::endl;
    thresholdFilter->Update();
    lungs=thresholdFilter->GetOutput();

  }


  //---------------------------------
  // Invert
  //---------------------------------
  typename InversionFilterType::Pointer inversionFilter=InversionFilterType::New();
  inversionFilter->SetInput(lungs);
  inversionFilter->SetLowerThreshold(0);
  inversionFilter->SetUpperThreshold(0);
  inversionFilter->SetInsideValue(1);
  inversionFilter->Update();

  //---------------------------------
  // Mirror
  //---------------------------------
  typename MirrorPadImageFilterType::Pointer mirrorPadImageFilter=MirrorPadImageFilterType::New();
  mirrorPadImageFilter->SetInput(inversionFilter->GetOutput());
  typename InternalImageType::SizeType padSize;
  padSize.Fill(0);
  padSize[2]=input->GetLargestPossibleRegion().GetSize()[2];
  mirrorPadImageFilter->SetPadLowerBound(padSize);
  if (m_Verbose) std::cout<<"Mirroring the entire image along the CC axis..."<<std::endl;
  mirrorPadImageFilter->Update();
  lungs=mirrorPadImageFilter->GetOutput();
  // writeImage<InternalImageType>(lungs,"/home/jef/tmp/lungs.mhd");

  return lungs;
}

//-------------------------------------------------------------------
// Resample
//-------------------------------------------------------------------
template <unsigned int Dimension, class  PixelType>
typename itk::Image<MotionMaskGenericFilter::InternalPixelType, Dimension>::Pointer
MotionMaskGenericFilter::Resample( typename itk::Image<InternalPixelType,Dimension>::Pointer input )
{

  typedef int InternalPixelType;
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image< InternalPixelType, Dimension> InternalImageType; //labeling doesn't work with unsigned char?

  //---------------------------------
  // Resample to new spacing
  //---------------------------------
  typename InternalImageType::SizeType size_low;
  typename InternalImageType::SpacingType spacing_low;
  for (unsigned int i=0; i<Dimension; i++)
    if (m_ArgsInfo.spacing_given==Dimension)
      for (unsigned int i=0; i<Dimension; i++) {
        spacing_low[i]=m_ArgsInfo.spacing_arg[i];
        size_low[i]=ceil(input->GetLargestPossibleRegion().GetSize()[i]*input->GetSpacing()[i]/spacing_low[i]);
      }
    else
      for (unsigned int i=0; i<Dimension; i++) {
        spacing_low[i]=m_ArgsInfo.spacing_arg[0];
        size_low[i]=ceil(input->GetLargestPossibleRegion().GetSize()[i]*input->GetSpacing()[i]/spacing_low[i]);
      }

  typename InternalImageType::PointType origin;
  input->TransformIndexToPhysicalPoint(input->GetLargestPossibleRegion().GetIndex(), origin);
  typedef itk::ResampleImageFilter<InternalImageType, InternalImageType> ResampleImageFilterType;
  typename ResampleImageFilterType::Pointer resampler =ResampleImageFilterType::New();
  typedef itk::NearestNeighborInterpolateImageFunction<InternalImageType, double> InterpolatorType;
  typename InterpolatorType::Pointer interpolator=InterpolatorType::New();
  resampler->SetInterpolator(interpolator);
  resampler->SetOutputOrigin(origin);
  resampler->SetOutputSpacing(spacing_low);
  resampler->SetSize(size_low);
  resampler->SetInput(input);
  resampler->Update();
  typename InternalImageType::Pointer output =resampler->GetOutput();
  return output;
}


//-------------------------------------------------------------------
// Initialize ellips
//-------------------------------------------------------------------
template <unsigned int Dimension, class  PixelType>
typename itk::Image<MotionMaskGenericFilter::InternalPixelType, Dimension>::Pointer
MotionMaskGenericFilter::InitializeEllips( typename itk::Vector<double,Dimension> center, typename itk::Image<InternalPixelType,Dimension>::Pointer bones_low )
{

  // ImageTypes
  typedef int InternalPixelType;
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image< InternalPixelType, Dimension> InternalImageType; //labeling doesn't work with unsigned char?
  typedef itk::Image< unsigned char , Dimension> OutputImageType;
  typedef itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;
  typedef clitk::SetBackgroundImageFilter<InternalImageType,InternalImageType, InternalImageType> SetBackgroundFilterType;
  typedef itk::LabelStatisticsImageFilter<InternalImageType,InternalImageType> LabelStatisticsImageFilterType;
  typedef itk::CastImageFilter<InternalImageType,OutputImageType> CastImageFilterType;


  typename InternalImageType::Pointer ellips;
  if (m_ArgsInfo.ellips_given || m_ArgsInfo.grownEllips_given || m_ArgsInfo.filledRibs_given) {
    if(m_ArgsInfo.ellips_given) {
      typedef itk::ImageFileReader<InternalImageType> FeatureReaderType;
      typename FeatureReaderType::Pointer featureReader = FeatureReaderType::New();
      featureReader->SetFileName(m_ArgsInfo.ellips_arg);
      featureReader->Update();
      ellips=featureReader->GetOutput();
    }
  } else {
    if(m_Verbose) {
      std::cout<<std::endl;
      std::cout<<"=========================================="<<std::endl;
      std::cout<<"||       Initializing ellipsoide        ||"<<std::endl;
      std::cout<<"=========================================="<<std::endl;
    }

    //---------------------------------
    // Offset from center
    //---------------------------------
    typename itk::Vector<double,Dimension> offset;
    if(m_ArgsInfo.offset_given== Dimension) {
      for(unsigned int i=0; i<Dimension; i++)
        offset[i]=m_ArgsInfo.offset_arg[i];
    } else {
      offset.Fill(0.);
      offset[1]=-50;
    }
    itk::Vector<double,Dimension> centerEllips=center+offset;
    if (m_Verbose) {
      std::cout<<"Placing the center of the initial ellipsoide at (mm) "<<std::endl;
      std::cout<<centerEllips[0];
      for (unsigned int i=1; i<Dimension; i++)
        std::cout<<" "<<centerEllips[i];
      std::cout<<std::endl;
    }

    //---------------------------------
    // The ellips
    //---------------------------------
    ellips=InternalImageType::New();
    ellips->SetRegions (bones_low->GetLargestPossibleRegion());
    ellips->SetOrigin(bones_low->GetOrigin());
    ellips->SetSpacing(bones_low->GetSpacing());
    ellips->Allocate();
    ellips->FillBuffer(0);

    // Axes
    typename itk::Vector<double, Dimension> axes;
    if (m_ArgsInfo.axes_given==Dimension)
      for(unsigned int i=0; i<Dimension; i++)
        axes[i]=m_ArgsInfo.axes_arg[i];
    else {
      axes[0]=100;
      axes[1]=30;
      axes[2]=150;
    }

    // Draw an ellips
    IteratorType itEllips(ellips, ellips->GetLargestPossibleRegion());
    itEllips.GoToBegin();
    typename InternalImageType::PointType point;
    typename InternalImageType::IndexType index;
    double distance;

    if (m_Verbose) std::cout<<"Drawing the initial ellipsoide..."<<std::endl;
    while (!itEllips.IsAtEnd()) {
      index=itEllips.GetIndex();
      ellips->TransformIndexToPhysicalPoint(index, point);
      distance=0.0;
      for(unsigned int i=0; i<Dimension; i++)
        distance+=powf( ( (centerEllips[i]-point[i])/axes[i] ), 2);

      if (distance<1)
        itEllips.Set(1);
      ++itEllips;
    }
  }


  //---------------------------------
  // Write the ellips
  //---------------------------------
  if (m_ArgsInfo.writeEllips_given) {
    typename CastImageFilterType::Pointer caster=CastImageFilterType::New();
    caster->SetInput(ellips);
    writeImage<OutputImageType>(caster->GetOutput(), m_ArgsInfo.writeEllips_arg, m_Verbose);
  }

  return ellips;

}


//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//-------------------------------------------------------------------
template <unsigned int Dimension, class  PixelType>
void
MotionMaskGenericFilter::UpdateWithDimAndPixelType()
{

  // ImageTypes
  typedef int InternalPixelType;
  typedef itk::Image<PixelType, Dimension> InputImageType;
  typedef itk::Image< InternalPixelType, Dimension> InternalImageType; //labeling doesn't work with unsigned char?
  typedef itk::Image< float, Dimension> LevelSetImageType; //labeling doesn't work with unsigned char?
  typedef itk::Image<unsigned char, Dimension> OutputImageType;


  //----------------------------------------------------------------------------------------------------
  // Typedef for Processing
  //----------------------------------------------------------------------------------------------------
  typedef itk::ImageFileReader<InternalImageType> FeatureReaderType;
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> InputBinarizeFilterType;
  typedef itk::BinaryThresholdImageFilter< LevelSetImageType,InternalImageType>    LevelSetBinarizeFilterType;
  typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> InversionFilterType;
  typedef itk::ThresholdImageFilter<InternalImageType> ThresholdFilterType;
  typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
  typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelFilterType;
  typedef itk::MirrorPadImageFilter<InternalImageType, InternalImageType> MirrorPadImageFilterType;
  typedef itk::NearestNeighborInterpolateImageFunction<InternalImageType, double> InterpolatorType;
  typedef itk::ResampleImageFilter<InternalImageType, InternalImageType> ResampleImageFilterType;
  typedef itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;
  typedef itk::GeodesicActiveContourLevelSetImageFilter<LevelSetImageType, InternalImageType> LevelSetImageFilterType;
  typedef itk::SignedDanielssonDistanceMapImageFilter<InternalImageType, LevelSetImageType> DistanceMapImageFilterType;
  typedef clitk::SetBackgroundImageFilter<InternalImageType,InternalImageType, InternalImageType> SetBackgroundFilterType;
  typedef itk::LabelStatisticsImageFilter<InternalImageType,InternalImageType> LabelStatisticsImageFilterType;
  typedef itk::CastImageFilter<InternalImageType,OutputImageType> CastImageFilterType;


  // Read the input
  typedef itk::ImageFileReader<InputImageType> InputReaderType;
  typename InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( m_InputFileName);
  reader->Update();
  typename InputImageType::Pointer input= reader->GetOutput();

  //     // globals for avi
  //     unsigned int number=0;


  if(m_Verbose) {
    std::cout<<std::endl;
    std::cout<<"=========================================="<<std::endl;
    std::cout<<"||         Making feature images        ||"<<std::endl;
    std::cout<<"=========================================="<<std::endl;
  }

  //--------------------------------------------------------------------------------
  // Lungs
  //-------------------------------------------------------------------------------
  typename InternalImageType::Pointer lungs = this->GetLungsImage<Dimension, PixelType>(input);

  //-------------------------------------------------------------------------------
  // Air
  //-------------------------------------------------------------------------------
  typename InternalImageType::Pointer air = this->GetAirImage<Dimension, PixelType>(input, lungs);

  //-------------------------------------------------------------------------------
  // Bones
  //-------------------------------------------------------------------------------
  typename InternalImageType::Pointer bones = this->GetBonesImage<Dimension, PixelType>(input);

  //----------------------------------------------------------------------------------------------------
  // Write feature images
  //----------------------------------------------------------------------------------------------------
  if(m_ArgsInfo.writeFeature_given) {
    typename SetBackgroundFilterType::Pointer setBackgroundFilter = SetBackgroundFilterType::New();
    setBackgroundFilter->SetInput(air);
    setBackgroundFilter->SetInput2(bones);
    setBackgroundFilter->SetMaskValue(0);
    setBackgroundFilter->SetOutsideValue(2);
    setBackgroundFilter->Update();
    typename InternalImageType::Pointer bones_air =setBackgroundFilter->GetOutput();

    typename SetBackgroundFilterType::Pointer setBackgroundFilter2 = SetBackgroundFilterType::New();
    setBackgroundFilter2->SetInput(bones_air);
    setBackgroundFilter2->SetInput2(lungs);
    setBackgroundFilter2->SetMaskValue(0);
    setBackgroundFilter2->SetOutsideValue(3);
    setBackgroundFilter2->Update();
    typename InternalImageType::Pointer lungs_bones_air =setBackgroundFilter2->GetOutput();

    typename CastImageFilterType::Pointer caster=CastImageFilterType::New();
    caster->SetInput(lungs_bones_air);
    writeImage<OutputImageType>(caster->GetOutput(), m_ArgsInfo.writeFeature_arg, m_Verbose);
  }

  //----------------------------------------------------------------------------------------------------
  // Low dimensional versions
  //----------------------------------------------------------------------------------------------------
  typename InternalImageType::Pointer bones_low =Resample<Dimension , PixelType>(bones);
  typename InternalImageType::Pointer lungs_low =Resample<Dimension , PixelType>(lungs);
  typename InternalImageType::Pointer air_low =Resample<Dimension , PixelType>(air);

  //---------------------------------
  // Pad
  //---------------------------------
  if(m_ArgsInfo.pad_flag) {
    typedef itk::ImageRegionIteratorWithIndex<InternalImageType> IteratorType;
    IteratorType it(air_low, air_low->GetLargestPossibleRegion());
    typename InternalImageType::IndexType index;
    while(!it.IsAtEnd()) {
      index=it.GetIndex();
      for (unsigned int i=0; i<Dimension; i++)
        if(index[i]==air_low->GetLargestPossibleRegion().GetIndex()[i]
            || index[i]==(unsigned int)air_low->GetLargestPossibleRegion().GetIndex()[i]+ (unsigned int) air_low->GetLargestPossibleRegion().GetSize()[i]-1)
          it.Set(0);
      ++it;
    }
  }

  //---------------------------------
  // Center
  //---------------------------------
  typename itk::Vector<double,Dimension> center;
  typedef itk::ImageMomentsCalculator<InternalImageType> MomentsCalculatorType;
  typename    MomentsCalculatorType::Pointer momentsCalculator=MomentsCalculatorType::New();
  momentsCalculator->SetImage(air);
  momentsCalculator->Compute();
  center=momentsCalculator->GetCenterOfGravity();
  if (m_Verbose) {
    std::cout<<"The center of gravity of the patient body is located at (mm) "<<std::endl;
    std::cout<<center[0];
    for (unsigned int i=1; i<Dimension; i++)
      std::cout<<" "<<center[i];
    std::cout<<std::endl;
  }

  //----------------------------------------------------------------------------------------------------
  // Ellipsoide initialization
  //----------------------------------------------------------------------------------------------------
  typename InternalImageType::Pointer m_Ellips= InitializeEllips<Dimension, PixelType>(center,bones_low);

  //----------------------------------------------------------------------------------------------------
  // Grow ellips
  //----------------------------------------------------------------------------------------------------
  typename InternalImageType::Pointer grownEllips;
  if (m_ArgsInfo.grownEllips_given || m_ArgsInfo.filledRibs_given) {
    if (m_ArgsInfo.grownEllips_given) {

      typename FeatureReaderType::Pointer featureReader = FeatureReaderType::New();
      featureReader->SetFileName(m_ArgsInfo.grownEllips_arg);
      featureReader->Update();
      grownEllips=featureReader->GetOutput();
    }
  } else {

    if(m_Verbose) {
      std::cout<<std::endl;
      std::cout<<"=========================================="<<std::endl;
      std::cout<<"||          Growing ellipsoide           ||"<<std::endl;
      std::cout<<"=========================================="<<std::endl;
    }

    //---------------------------------
    // Detect abdomen
    //---------------------------------
    typename InternalImageType::PointType dPoint;
    if (m_ArgsInfo.detectionPoint_given) {
      for (unsigned int i=0; i<Dimension; i++)
        dPoint[i]=m_ArgsInfo.detectionPoint_arg[i];
    } else {
      typename InternalImageType::RegionType searchRegion=air->GetLargestPossibleRegion();
      typename InternalImageType::SizeType searchRegionSize = searchRegion.GetSize();
      typename InternalImageType::IndexType searchRegionIndex = searchRegion.GetIndex();
      searchRegionIndex[2]+=searchRegionSize[2]/2;
      searchRegionSize[2]=1;
      searchRegion.SetSize(searchRegionSize);
      searchRegion.SetIndex(searchRegionIndex);
      IteratorType itAbdomen(air, searchRegion);
      itAbdomen.GoToBegin();

      typename InternalImageType::PointType aPoint;
      typename InternalImageType::IndexType aIndex;

      if (m_Verbose) std::cout<<"Detecting the abdomen..."<<std::endl;
      while (!itAbdomen.IsAtEnd()) {
        if(itAbdomen.Get()) break;
        ++itAbdomen;
      }
      aIndex=itAbdomen.GetIndex();
      air->TransformIndexToPhysicalPoint(aIndex,aPoint);
      if (m_Verbose) std::cout<<"Detected the abdomen at "<<aPoint<<".."<<std::endl;


      //---------------------------------
      // Detect abdomen in additional images?
      //---------------------------------
      if (m_ArgsInfo.detectionPairs_given) {
        for (unsigned int i=0; i<m_ArgsInfo.detectionPairs_given; i++) {
          typename InternalImageType::Pointer airAdd;
          //---------------------------------
          // Read the input
          //--------------------------------
          typedef itk::ImageFileReader<InputImageType> InputReaderType;
          typename InputReaderType::Pointer reader = InputReaderType::New();
          reader->SetFileName( m_ArgsInfo.detectionPairs_arg[i]);
          reader->Update();
          typename InputImageType::Pointer additional= reader->GetOutput();
          if (m_Verbose) std::cout<<"Extracting the air from additional image "<< i<<"..."<<std::endl;

          //---------------------------------
          // Binarize the image
          //---------------------------------
          typename InputBinarizeFilterType::Pointer binarizeFilter=InputBinarizeFilterType::New();
          binarizeFilter->SetInput(additional);
          binarizeFilter->SetLowerThreshold(static_cast<PixelType>(m_ArgsInfo.lowerThresholdAir_arg));
          binarizeFilter->SetUpperThreshold(static_cast<PixelType>(m_ArgsInfo.upperThresholdAir_arg));
          if (m_Verbose) std::cout<<"Binarizing the image using thresholds "<<m_ArgsInfo.lowerThresholdAir_arg
                                    <<", "<<m_ArgsInfo.upperThresholdAir_arg<<"..."<<std::endl;

          //---------------------------------
          // Label the connected components
          //---------------------------------
          typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
          connectFilter->SetInput(binarizeFilter->GetOutput());
          connectFilter->SetBackgroundValue(0);
          connectFilter->SetFullyConnected(false);
          if (m_Verbose) std::cout<<"Labeling the connected components..."<<std::endl;

          //---------------------------------
          // Sort the labels according to size
          //---------------------------------
          typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
          relabelFilter->SetInput(connectFilter->GetOutput());
          if (m_Verbose) std::cout<<"Sorting the labels..."<<std::endl;

          //---------------------------------
          // Keep the label
          //---------------------------------
          typename ThresholdFilterType::Pointer thresholdFilter=ThresholdFilterType::New();
          thresholdFilter->SetInput(relabelFilter->GetOutput());
          thresholdFilter->SetUpper(1);
          if (m_Verbose) std::cout<<"Keeping the first label..."<<std::endl;
          thresholdFilter->Update();
          airAdd=thresholdFilter->GetOutput();


          //---------------------------------
          // Invert
          //---------------------------------
          typename InversionFilterType::Pointer inversionFilter=InversionFilterType::New();
          inversionFilter->SetInput(airAdd);
          inversionFilter->SetLowerThreshold(0);
          inversionFilter->SetUpperThreshold(0);
          inversionFilter->SetInsideValue(1);
          inversionFilter->Update();

          //---------------------------------
          // Mirror
          //---------------------------------
          typename MirrorPadImageFilterType::Pointer mirrorPadImageFilter=MirrorPadImageFilterType::New();
          mirrorPadImageFilter->SetInput(inversionFilter->GetOutput());
          typename InternalImageType::SizeType padSize;
          padSize.Fill(0);
          padSize[2]=input->GetLargestPossibleRegion().GetSize()[2];
          mirrorPadImageFilter->SetPadLowerBound(padSize);
          if (m_Verbose) std::cout<<"Mirroring the entire image along the CC axis..."<<std::endl;
          mirrorPadImageFilter->Update();
          airAdd=mirrorPadImageFilter->GetOutput();

          //---------------------------------
          // Detect abdomen
          //---------------------------------
          typename InternalImageType::RegionType searchRegion=airAdd->GetLargestPossibleRegion();
          typename InternalImageType::SizeType searchRegionSize = searchRegion.GetSize();
          typename InternalImageType::IndexType searchRegionIndex = searchRegion.GetIndex();
          searchRegionIndex[2]+=searchRegionSize[2]/2;
          searchRegionSize[2]=1;
          searchRegion.SetSize(searchRegionSize);
          searchRegion.SetIndex(searchRegionIndex);
          IteratorType itAbdomen(airAdd, searchRegion);
          itAbdomen.GoToBegin();

          typename InternalImageType::PointType additionalPoint;
          typename InternalImageType::IndexType aIndex;

          if (m_Verbose) std::cout<<"Detecting the abdomen..."<<std::endl;
          while (!itAbdomen.IsAtEnd()) {
            if(itAbdomen.Get()) break;
            ++itAbdomen;
          }
          aIndex=itAbdomen.GetIndex();
          airAdd->TransformIndexToPhysicalPoint(aIndex,additionalPoint);
          if (m_Verbose) std::cout<<"Detected the abdomen in the additional image at "<<additionalPoint<<".."<<std::endl;

          if(additionalPoint[1]< aPoint[1]) {
            aPoint=additionalPoint;
            if (m_Verbose) std::cout<<"Modifying the detected abdomen to "<<aPoint<<".."<<std::endl;

          }
        }
      }


      // Determine the detection point
      dPoint.Fill(0.0);
      dPoint+=center;
      dPoint[1]=aPoint[1];
      if(m_ArgsInfo.offsetDetect_given==Dimension)
        for(unsigned int i=0; i <Dimension; i++)
          dPoint[i]+=m_ArgsInfo.offsetDetect_arg[i];
      else
        dPoint[1]+=-10;

    }
    if (m_Verbose) std::cout<<"Setting the detection point to "<<dPoint<<".."<<std::endl;


    //---------------------------------
    // Pad the rib image and ellips image
    //---------------------------------
    typename InternalImageType::Pointer padded_ellips;
    typename InternalImageType::Pointer padded_bones_low;

    // If detection point not inside the image: pad
    typename InternalImageType::IndexType dIndex;
    if (!bones_low->TransformPhysicalPointToIndex(dPoint, dIndex)) {
      typename InternalImageType::SizeType padSize;
      padSize.Fill(0);
      padSize[1]=abs(dIndex[1])+1;
      if (m_Verbose) std::cout<<"Padding the images with padding size "<<padSize<<" to include the detection point..."<<std::endl;

      typename MirrorPadImageFilterType::Pointer padBonesFilter=MirrorPadImageFilterType::New();
      padBonesFilter->SetInput(bones_low);
      padBonesFilter->SetPadLowerBound(padSize);
      padBonesFilter->Update();
      padded_bones_low=padBonesFilter->GetOutput();

      typename MirrorPadImageFilterType::Pointer padEllipsFilter=MirrorPadImageFilterType::New();
      padEllipsFilter->SetInput(m_Ellips);
      padEllipsFilter->SetPadLowerBound(padSize);
      padEllipsFilter->Update();
      padded_ellips=padEllipsFilter->GetOutput();
    }

    else {
      padded_bones_low=bones_low;
      padded_ellips=m_Ellips;
    }


    //---------------------------------
    // Calculate distance map
    //---------------------------------
    typename DistanceMapImageFilterType::Pointer distanceMapImageFilter = DistanceMapImageFilterType::New();
    distanceMapImageFilter->SetInput(padded_ellips);
    distanceMapImageFilter->SetInsideIsPositive(false);
    if (m_Verbose) std::cout<<"Calculating the distance map..."<<std::endl;
    distanceMapImageFilter->Update();
    if (m_ArgsInfo.writeDistMap_given) {
      writeImage<LevelSetImageType>(distanceMapImageFilter->GetOutput(), m_ArgsInfo.writeDistMap_arg, m_Verbose);
      
    }

    //---------------------------------
    // Grow while monitoring detection point
    //---------------------------------
    typename LevelSetImageFilterType::Pointer levelSetFilter=LevelSetImageFilterType::New();
    levelSetFilter->SetInput(  distanceMapImageFilter->GetOutput() );
    levelSetFilter->SetFeatureImage( padded_bones_low );
    levelSetFilter->SetPropagationScaling( 1 );
    levelSetFilter->SetCurvatureScaling( m_ArgsInfo.curve1_arg );
    levelSetFilter->SetAdvectionScaling( 0 );
    levelSetFilter->SetMaximumRMSError( m_ArgsInfo.maxRMS1_arg );
    levelSetFilter->SetNumberOfIterations( m_ArgsInfo.iter1_arg );
    levelSetFilter->SetUseImageSpacing(true);

    // 	//---------------------------------
    // 	// Script for making movie
    // 	//---------------------------------
    //     	std::string script="source /home/jef/thesis/presentations/20091014_JDD/videos/motionmask/make_motion_mask_movie_4mm.sh ";


    if (m_Verbose) std::cout<<"Starting level set segmentation..."<<std::endl;
    unsigned int totalNumberOfIterations=0;
    while (true) {
      levelSetFilter->Update();
      totalNumberOfIterations+=levelSetFilter->GetElapsedIterations();

      if ( levelSetFilter->GetOutput()->GetPixel(dIndex) < 0 ) {
        if (m_Verbose) std::cout<<"Detection point reached!"<<std::endl;
        break;
      } else {
        if (m_Verbose) std::cout<<"Detection point not reached after "<<totalNumberOfIterations<<" iterations..."<<std::endl;
        levelSetFilter->SetInput(levelSetFilter->GetOutput());
        if(m_ArgsInfo.monitor_given) writeImage<LevelSetImageType>(levelSetFilter->GetOutput(), m_ArgsInfo.monitor_arg, m_Verbose);

        // 		// script
        // 		std::ostringstream number_str;
        // 		number_str << number;
        // 		std::string param =  number_str.str();
        // 		system((script+param).c_str());
        // 		number+=1;

      }
      if ( (totalNumberOfIterations> 10000) || (levelSetFilter->GetRMSChange()< m_ArgsInfo.maxRMS1_arg) ) break;
    }

    // Output values
    if (m_Verbose) std::cout<<"Level set segmentation stopped after "<<totalNumberOfIterations<<" iterations..."<<std::endl;
    std::cout << "Max. RMS error: " << levelSetFilter->GetMaximumRMSError() << std::endl;
    std::cout << "RMS change: " << levelSetFilter->GetRMSChange() << std::endl;

    // Threshold
    typename LevelSetBinarizeFilterType::Pointer thresholder = LevelSetBinarizeFilterType::New();
    thresholder->SetUpperThreshold( 0.0 );
    thresholder->SetOutsideValue( 0 );
    thresholder->SetInsideValue( 1 );
    thresholder->SetInput( levelSetFilter->GetOutput() );
    if (m_Verbose) std::cout<<"Thresholding the output level set..."<<std::endl;
    thresholder->Update();
    grownEllips=thresholder->GetOutput();
  }

  //---------------------------------
  // Write the grown ellips
  //---------------------------------
  if (m_ArgsInfo.writeGrownEllips_given) {
    typename CastImageFilterType::Pointer caster=CastImageFilterType::New();
    caster->SetInput(grownEllips);
    writeImage<OutputImageType>(caster->GetOutput(), m_ArgsInfo.writeGrownEllips_arg, m_Verbose);
  }


  //----------------------------------------------------------------------------------------------------
  // Grow inside ribs
  //----------------------------------------------------------------------------------------------------
  typename InternalImageType::Pointer filledRibs;
  if (m_ArgsInfo.filledRibs_given) {
    typename FeatureReaderType::Pointer featureReader = FeatureReaderType::New();
    featureReader->SetFileName(m_ArgsInfo.filledRibs_arg);
    if (m_Verbose) std::cout<<"Reading filled ribs mask..."<<std::endl;
    featureReader->Update();
    filledRibs=featureReader->GetOutput();
  } else {
    if(m_Verbose) {
      std::cout<<std::endl;
      std::cout<<"=========================================="<<std::endl;
      std::cout<<"||        Filling the ribs image         ||"<<std::endl;
      std::cout<<"=========================================="<<std::endl;
    }
    //---------------------------------
    // Make feature image air+bones
    //---------------------------------
    typename SetBackgroundFilterType::Pointer setBackgroundFilter = SetBackgroundFilterType::New();
    setBackgroundFilter->SetInput(air_low);
    setBackgroundFilter->SetInput2(bones_low);
    setBackgroundFilter->SetMaskValue(0);
    setBackgroundFilter->SetOutsideValue(0);
    setBackgroundFilter->Update();
    typename InternalImageType::Pointer bones_air_low =setBackgroundFilter->GetOutput();

    //---------------------------------
    // Resampling previous solution
    //---------------------------------
    if (m_Verbose) std::cout<<"Resampling previous solution..."<<std::endl;
    typename ResampleImageFilterType::Pointer resampler =ResampleImageFilterType::New();
    typename InternalImageType::PointType origin;
    bones_low->TransformIndexToPhysicalPoint(bones_low->GetLargestPossibleRegion().GetIndex(), origin);
    resampler->SetOutputOrigin(origin);
    resampler->SetOutputSpacing(bones_low->GetSpacing());
    typename InternalImageType::SizeType size_low= bones_low->GetLargestPossibleRegion().GetSize();
    resampler->SetSize(size_low);
    typename InterpolatorType::Pointer interpolator=InterpolatorType::New();
    resampler->SetInterpolator(interpolator);
    resampler->SetInput(grownEllips);
    resampler->Update();
    typename InternalImageType::Pointer grownEllips =resampler->GetOutput();


    //---------------------------------
    // Calculate Distance Map
    //---------------------------------
    typename DistanceMapImageFilterType::Pointer distanceMapImageFilter = DistanceMapImageFilterType::New();
    distanceMapImageFilter->SetInput(grownEllips);
    distanceMapImageFilter->SetInsideIsPositive(false);
    if (m_Verbose) std::cout<<"Calculating distance map..."<<std::endl;
    distanceMapImageFilter->Update();

    //---------------------------------
    // Grow while monitoring lung volume coverage
    //---------------------------------
    typename LevelSetImageFilterType::Pointer levelSetFilter=LevelSetImageFilterType::New();
    levelSetFilter->SetInput(  distanceMapImageFilter->GetOutput() );
    levelSetFilter->SetFeatureImage( bones_air_low );
    levelSetFilter->SetPropagationScaling( 1 );
    levelSetFilter->SetCurvatureScaling( m_ArgsInfo.curve2_arg );
    levelSetFilter->SetAdvectionScaling( 0 );
    levelSetFilter->SetMaximumRMSError( m_ArgsInfo.maxRMS2_arg );
    levelSetFilter->SetNumberOfIterations( m_ArgsInfo.iter2_arg );
    levelSetFilter->SetUseImageSpacing(true);

    //---------------------------------
    // Invert lung mask
    //---------------------------------
    typename InversionFilterType::Pointer invertor= InversionFilterType::New();
    invertor->SetInput(lungs_low);
    invertor->SetLowerThreshold(0);
    invertor->SetUpperThreshold(0);
    invertor->SetInsideValue(1);
    invertor->Update();
    typename InternalImageType::Pointer lungs_low_inv=invertor->GetOutput();

    //---------------------------------
    // Calculate lung volume
    //---------------------------------
    typename LabelStatisticsImageFilterType::Pointer statisticsFilter= LabelStatisticsImageFilterType::New();
    statisticsFilter->SetInput(lungs_low_inv);
    statisticsFilter->SetLabelInput(lungs_low);
    statisticsFilter->Update();
    unsigned int volume=statisticsFilter->GetSum(0);

    // Prepare threshold
    typename LevelSetBinarizeFilterType::Pointer thresholder = LevelSetBinarizeFilterType::New();
    thresholder->SetUpperThreshold(     0.0 );
    thresholder->SetOutsideValue(  0  );
    thresholder->SetInsideValue(  1 );


    // Start monitoring
    unsigned int totalNumberOfIterations=0;
    unsigned int coverage=0;


    // 	//---------------------------------
    // 	// Script for making movie
    // 	//---------------------------------
    //   	std::string script="source /home/jef/thesis/presentations/20091014_JDD/videos/motionmask/make_motion_mask_movie_4mm.sh ";

    while (true) {
      levelSetFilter->Update();
      totalNumberOfIterations+=levelSetFilter->GetElapsedIterations();

      thresholder->SetInput( levelSetFilter->GetOutput() );
      thresholder->Update();
      statisticsFilter->SetInput(thresholder->GetOutput());
      statisticsFilter->Update();
      coverage=statisticsFilter->GetSum(0);

      // Compare the volumes
      if ( coverage >= m_ArgsInfo.fillingLevel_arg * volume/100 ) {
        if (m_Verbose) std::cout<<"Lungs filled for "<< m_ArgsInfo.fillingLevel_arg<<"% !"<<std::endl;
        break;
      } else {
        if (m_Verbose) std::cout<<"After "<<totalNumberOfIterations<<" iterations, lungs are covered for "
                                  <<(double)coverage/volume*100.0<<"%..."<<std::endl;
        levelSetFilter->SetInput(levelSetFilter->GetOutput());
        if(m_ArgsInfo.monitor_given) writeImage<LevelSetImageType>(levelSetFilter->GetOutput(), m_ArgsInfo.monitor_arg, m_Verbose);

        // 		// script
        // 		std::ostringstream number_str;
        // 		number_str << number;
        // 		std::string param =  number_str.str();
        // 		system((script+param).c_str());
        // 		number+=1;

      }
      if ( (totalNumberOfIterations> 30000) || (levelSetFilter->GetRMSChange()< m_ArgsInfo.maxRMS2_arg) ) break;
    }

    // Output values
    if (m_Verbose) std::cout<<"Level set segmentation stopped after "<<totalNumberOfIterations<<" iterations..."<<std::endl;
    std::cout << "Max. RMS error: " << levelSetFilter->GetMaximumRMSError() << std::endl;
    std::cout << "RMS change: " << levelSetFilter->GetRMSChange() << std::endl;

    // Threshold
    thresholder->SetInput( levelSetFilter->GetOutput() );
    thresholder->Update();
    filledRibs=thresholder->GetOutput();
    // writeImage<InternalImageType>(filledRibs,"/home/jef/tmp/filled_ribs_before.mhd", m_Verbose);

  }

  //---------------------------------
  // Write the filled ribs
  //---------------------------------
  if (m_ArgsInfo.writeFilledRibs_given) {
    typename CastImageFilterType::Pointer caster=CastImageFilterType::New();
    caster->SetInput(filledRibs);
    writeImage<OutputImageType>(caster->GetOutput(), m_ArgsInfo.writeFilledRibs_arg, m_Verbose);
  }


  //----------------------------------------------------------------------------------------------------
  // Collapse to the lungs
  //----------------------------------------------------------------------------------------------------
  if(m_Verbose) {
    std::cout<<std::endl;
    std::cout<<"=========================================="<<std::endl;
    std::cout<<"||     Collapsing to the lung image     ||"<<std::endl;
    std::cout<<"=========================================="<<std::endl;
  }

  //---------------------------------
  // Make feature image air+bones
  //---------------------------------
  if (m_Verbose) std::cout<<"Making feature images..."<<std::endl;
  typename SetBackgroundFilterType::Pointer setBackgroundFilter = SetBackgroundFilterType::New();
  setBackgroundFilter->SetInput(air);
  setBackgroundFilter->SetInput2(bones);
  setBackgroundFilter->SetMaskValue(0);
  setBackgroundFilter->SetOutsideValue(0);
  setBackgroundFilter->Update();
  typename InternalImageType::Pointer bones_air =setBackgroundFilter->GetOutput();

  typename SetBackgroundFilterType::Pointer setBackgroundFilter2 = SetBackgroundFilterType::New();
  setBackgroundFilter2->SetInput(bones_air);
  setBackgroundFilter2->SetInput2(lungs);
  setBackgroundFilter2->SetMaskValue(0);
  setBackgroundFilter2->SetOutsideValue(0);
  setBackgroundFilter2->Update();
  typename InternalImageType::Pointer lungs_bones_air =setBackgroundFilter2->GetOutput();

  //---------------------------------
  // Prepare previous solution
  //---------------------------------
  if (m_Verbose) std::cout<<"Resampling previous solution..."<<std::endl;
  typename ResampleImageFilterType::Pointer resampler =ResampleImageFilterType::New();
  typedef itk::NearestNeighborInterpolateImageFunction<InternalImageType, double> InterpolatorType;
  typename InterpolatorType::Pointer interpolator=InterpolatorType::New();
  resampler->SetOutputStartIndex(bones->GetLargestPossibleRegion().GetIndex());
  resampler->SetInput(filledRibs);
  resampler->SetInterpolator(interpolator);
  resampler->SetOutputParametersFromImage(bones);
  resampler->Update();
  filledRibs =resampler->GetOutput();

  if (m_Verbose) std::cout<<"Setting lungs to 1..."<<std::endl;
  typename SetBackgroundFilterType::Pointer setBackgroundFilter3 = SetBackgroundFilterType::New();
  setBackgroundFilter3->SetInput(filledRibs);
  setBackgroundFilter3->SetInput2(lungs);
  setBackgroundFilter3->SetMaskValue(0);
  setBackgroundFilter3->SetOutsideValue(1);
  setBackgroundFilter3->Update();
  filledRibs=setBackgroundFilter3->GetOutput();

  if (m_Verbose) std::cout<<"Removing overlap with bones..."<<std::endl;
  typename SetBackgroundFilterType::Pointer setBackgroundFilter4 = SetBackgroundFilterType::New();
  setBackgroundFilter4->SetInput(filledRibs);
  setBackgroundFilter4->SetInput2(bones);
  setBackgroundFilter4->SetMaskValue(0);
  setBackgroundFilter4->SetOutsideValue(0);
  setBackgroundFilter4->Update();
  filledRibs =setBackgroundFilter4->GetOutput();
  //   writeImage<InternalImageType>(filledRibs,"/home/jef/tmp/filledRibs_pp.mhd");
  //---------------------------------
  // Calculate Distance Map
  //---------------------------------
  //  typedef  itk::ApproximateSignedDistanceMapImageFilter <InternalImageType, LevelSetImageType> DistanceMapImageFilterType2;
  typename DistanceMapImageFilterType::Pointer distanceMapImageFilter = DistanceMapImageFilterType::New();
  distanceMapImageFilter->SetInput(filledRibs);
  distanceMapImageFilter->SetInsideIsPositive(false);
  // distanceMapImageFilter->SetInsideValue(0);
//     distanceMapImageFilter->SetOutsideValue(1);
  if (m_Verbose) std::cout<<"Calculating distance map..."<<std::endl;
  distanceMapImageFilter->Update();

  //---------------------------------
  // Collapse
  //---------------------------------
  typename LevelSetImageFilterType::Pointer levelSetFilter=LevelSetImageFilterType::New();
  levelSetFilter->SetInput(  distanceMapImageFilter->GetOutput() );
  levelSetFilter->SetFeatureImage( lungs_bones_air );
  levelSetFilter->SetPropagationScaling(m_ArgsInfo.prop3_arg);
  levelSetFilter->SetCurvatureScaling( m_ArgsInfo.curve3_arg );
  levelSetFilter->SetAdvectionScaling( 0 );
  levelSetFilter->SetMaximumRMSError( m_ArgsInfo.maxRMS3_arg );
  levelSetFilter->SetNumberOfIterations( m_ArgsInfo.iter3_arg );
  levelSetFilter->SetUseImageSpacing(true);

  //     //script
  //     std::string script="source /home/jef/thesis/presentations/20091014_JDD/videos/motionmask/make_motion_mask_movie_4mm.sh ";

  // Start monitoring
  if (m_Verbose) std::cout<<"Starting the levelset..."<<std::endl;
  unsigned int totalNumberOfIterations=0;
  while (true) {
    levelSetFilter->Update();

    // monitor state
    totalNumberOfIterations+=levelSetFilter->GetElapsedIterations();
    levelSetFilter->SetInput(levelSetFilter->GetOutput());
    if(m_ArgsInfo.monitor_given) writeImage<LevelSetImageType>(levelSetFilter->GetOutput(), m_ArgsInfo.monitor_arg);
    std::cout << "After "<<totalNumberOfIterations<<" iteration the RMS change is " << levelSetFilter->GetRMSChange() <<"..."<< std::endl;

    // 	// script
    // 	std::ostringstream number_str;
    // 	number_str << number;
    // 	std::string param =  number_str.str();
    // 	system((script+param).c_str());
    // 	number+=1;

    // stopping condition
    if ( (totalNumberOfIterations>= (unsigned int) m_ArgsInfo.maxIter3_arg) || (levelSetFilter->GetRMSChange()< m_ArgsInfo.maxRMS3_arg) ) break;
    levelSetFilter->SetNumberOfIterations( std::min ((unsigned int) m_ArgsInfo.iter3_arg, (unsigned int) m_ArgsInfo.maxIter3_arg-totalNumberOfIterations ) );
  }

  // Output values
  if (m_Verbose) {
    std::cout<<"Level set segmentation stopped after "<<totalNumberOfIterations<<" iterations..."<<std::endl;
    std::cout << "Max. RMS error: " << levelSetFilter->GetMaximumRMSError() << std::endl;
    std::cout << "RMS change: " << levelSetFilter->GetRMSChange() << std::endl;
  }

  // Threshold
  typedef itk::BinaryThresholdImageFilter< LevelSetImageType,InternalImageType>    LevelSetBinarizeFilterType;
  typename LevelSetBinarizeFilterType::Pointer thresholder = LevelSetBinarizeFilterType::New();
  thresholder->SetUpperThreshold( 0.0 );
  thresholder->SetOutsideValue( 0 );
  thresholder->SetInsideValue( 1 );
  thresholder->SetInput( levelSetFilter->GetOutput() );
  thresholder->Update();
  typename InternalImageType::Pointer output = thresholder->GetOutput();


  //----------------------------------------------------------------------------------------------------
  // Prepare the output
  //----------------------------------------------------------------------------------------------------

  //---------------------------------
  // Set Air to zero
  //---------------------------------
  if (m_Verbose) std::cout<<"Removing overlap mask with air..."<<std::endl;
  typename SetBackgroundFilterType::Pointer setBackgroundFilter5 = SetBackgroundFilterType::New();
  setBackgroundFilter5->SetInput(output);
  setBackgroundFilter5->SetInput2(air);
  setBackgroundFilter5->SetMaskValue(0);
  setBackgroundFilter5->SetOutsideValue(0);
  setBackgroundFilter5->Update();
  output=setBackgroundFilter5->GetOutput();

  //---------------------------------
  // Open & close  to cleanup
  //---------------------------------
  if ( m_ArgsInfo.openClose_flag) {

    //---------------------------------
    // Structuring element
    //---------------------------------
    typedef itk::BinaryBallStructuringElement<InternalPixelType,Dimension > KernelType;
    KernelType structuringElement;
    structuringElement.SetRadius(1);
    structuringElement.CreateStructuringElement();

    //---------------------------------
    // Open
    //---------------------------------
    typedef itk::BinaryMorphologicalOpeningImageFilter<InternalImageType, InternalImageType , KernelType> OpenFilterType;
    typename OpenFilterType::Pointer openFilter = OpenFilterType::New();
    openFilter->SetInput(output);
    openFilter->SetBackgroundValue(0);
    openFilter->SetForegroundValue(1);
    openFilter->SetKernel(structuringElement);
    if(m_Verbose) std::cout<<"Opening the output image..."<<std::endl;

    //---------------------------------
    // Close
    //---------------------------------
    typedef itk::BinaryMorphologicalClosingImageFilter<InternalImageType, InternalImageType , KernelType> CloseFilterType;
    typename CloseFilterType::Pointer closeFilter = CloseFilterType::New();
    closeFilter->SetInput(openFilter->GetOutput());
    closeFilter->SetSafeBorder(true);
    closeFilter->SetForegroundValue(1);
    closeFilter->SetKernel(structuringElement);
    if(m_Verbose) std::cout<<"Closing the output image..."<<std::endl;
    closeFilter->Update();
    output=closeFilter->GetOutput();

  }
  //writeImage<InternalImageType>(output,"/home/jef/tmp/mm_double.mhd");

  // Extract the upper part
  typedef  itk::CropImageFilter<InternalImageType, InternalImageType> CropImageFilterType;
  typename CropImageFilterType::Pointer cropFilter=CropImageFilterType::New();
  cropFilter->SetInput(output);
  typename InputImageType::SizeType lSize, uSize;
  uSize.Fill(0);
  lSize.Fill(0);
  lSize[2]=input->GetLargestPossibleRegion().GetSize()[2];
  cropFilter->SetLowerBoundaryCropSize(lSize);
  cropFilter->SetUpperBoundaryCropSize(uSize);
  cropFilter->Update();

  // Output
  typename CastImageFilterType::Pointer caster=CastImageFilterType::New();
  caster->SetInput(cropFilter->GetOutput());
  writeImage<OutputImageType>(caster->GetOutput(), m_ArgsInfo.output_arg, m_Verbose);

}

}//end clitk

#endif //#define clitkMotionMaskGenericFilter_txx
