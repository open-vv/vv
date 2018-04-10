
#include "clitkConvertBLUTCoeffsToVFFilter.h"
#include "clitkBSplineDeformableTransform.h"
//#include "clitkTransformToDeformationFieldSource.h"
//#include "clitkShapedBLUTSpatioTemporalDeformableTransform.h"
#include "itkImageMaskSpatialObject.h"

//#include "clitkConvertBSplineDeformableTransformToVFGenericFilter.h"
#include "clitkVectorImageToImageFilter.h"
#include "itkBSplineDeformableTransform.h"

namespace clitk 
{
  template <class TDVFType>
  ConvertBLUTCoeffsToVFFilter<TDVFType>::ConvertBLUTCoeffsToVFFilter()
  {
    m_Verbose = false;
    m_TransformType = 0;
    m_OutputOrigin.Fill(0);
    m_OutputSpacing.Fill(1);
    m_OutputSize.Fill(1);
    m_BLUTSplineOrders.Fill(3);

    m_Filter = ConvertorType::New();
    m_BLUTTransform = BLUTTransformType::New();
    m_ITKTransform = ITKTransformType::New();
  }

  template <class TDVFType>
  void ConvertBLUTCoeffsToVFFilter<TDVFType>::GenerateOutputInformation()
  {
    if (m_Verbose)
      std::cout << "ConvertBLUTCoeffsToVFFilter<TDVFType>::GenerateOutputInformation()" << std::endl;

    Superclass::GenerateOutputInformation();
    OutputImagePointer output = this->GetOutput();
    if (!m_LikeFileName.empty())
    {
        typename LikeImageType::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(m_LikeFileName.c_str(), itk::ImageIOFactory::ReadMode);
        imageIO->SetFileName(m_LikeFileName.c_str());
        imageIO->ReadImageInformation();

        typename ConvertorType::SizeType output_size;
        typename ConvertorType::SpacingType output_spacing;
        typename ConvertorType::OriginType output_origin;
        typename ConvertorType::DirectionType output_direction;
        for (unsigned int i = 0; i < OutputImageType::ImageDimension; i++) {
          output_size[i] = imageIO->GetDimensions(i);
          output_spacing[i] = imageIO->GetSpacing(i);
          output_origin[i] = imageIO->GetOrigin(i);
          //for (unsigned int j = 0; j < Dimension; j++)
          //  output_direction[i][j] = imageIO->GetDirection(i)[j];
        }
        
        if (m_Verbose)
          std::cout << output_origin << output_size << output_spacing << std::endl;

        output->SetOrigin(output_origin);
        output->SetSpacing(output_spacing);
        //output->SetDirection(output_direction);
        OutputImageRegionType output_region;
        output_region.SetSize(output_size);
        output->SetRegions(output_region);
    }
    else
    {
        if (m_Verbose)
          std::cout << m_OutputOrigin << m_OutputSize << m_OutputSpacing << std::endl;

        output->SetOrigin(m_OutputOrigin);
        output->SetSpacing(m_OutputSpacing);
        OutputImageRegionType output_region;
        output_region.SetSize(m_OutputSize);
        output->SetRegions(output_region);
    }

  }

  template <class TDVFType>
  void ConvertBLUTCoeffsToVFFilter<TDVFType>::GenerateData()
  {
    // Read the input
    typedef itk::ImageFileReader<BLUTCoefficientImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName(m_InputFileName.c_str());
    reader->Update();
    typename BLUTCoefficientImageType::Pointer input = reader->GetOutput();
    OutputImagePointer output = this->GetOutput();

    if (m_TransformType != 0 ) { // using BLUT
      // Spline orders:  Default is cubic splines
        if (m_Verbose) {
          std::cout << "Using clitk::BLUT." << std::endl;
          std::cout << "Setting spline orders and sampling factors." << std::endl;
        }

      m_BLUTTransform->SetSplineOrders(m_BLUTSplineOrders);

      typename BLUTCoefficientImageType::SizeType samplingFactors;
      for (unsigned int i=0; i< OutputImageType::ImageDimension; i++)
      {
          samplingFactors[i]= (int) ( input->GetSpacing()[i]/ output->GetSpacing()[i]);
          if (m_Verbose) std::cout<<"Setting sampling factor "<<i<<" to "<<samplingFactors[i]<<"..."<<std::endl;
      }
      m_BLUTTransform->SetLUTSamplingFactors(samplingFactors);
      m_BLUTTransform->SetCoefficientImage(input);

      // Mask
      typedef itk::ImageMaskSpatialObject<BLUTCoefficientImageType::ImageDimension >   MaskType;
      typename MaskType::Pointer  spatialObjectMask=ITK_NULLPTR;
      if (!m_MaskFileName.empty())
      {
          typedef itk::Image< unsigned char, BLUTCoefficientImageType::ImageDimension >   ImageMaskType;
          typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
          typename MaskReaderType::Pointer  maskReader = MaskReaderType::New();
          maskReader->SetFileName(m_MaskFileName.c_str());

          try
          {
              maskReader->Update();
          }
          catch ( itk::ExceptionObject & err )
          {
              std::cerr << "ExceptionObject caught while reading mask !" << std::endl;
              std::cerr << err << std::endl;
              return;
          }
          if (m_Verbose)std::cout <<"Mask was read..." <<std::endl;

          // Set the image to the spatialObject
          spatialObjectMask = MaskType::New();
          spatialObjectMask->SetImage( maskReader->GetOutput() );
          m_BLUTTransform->SetMask(spatialObjectMask);
      }

      m_GenericTransform = m_BLUTTransform;
    }
    else { // using ITK transform
      if (m_Verbose) {
        std::cout << "Using itk::BSpline" << std::endl;
        std::cout << "Extracting components from input coefficient image and creating one coefficient image per-component" << std::endl;
      }
        
      typedef clitk::VectorImageToImageFilter<BLUTCoefficientImageType, typename ITKTransformType::ImageType> FilterType;
      typename FilterType::Pointer component_filter[BLUTCoefficientImageType::ImageDimension];
      typename ITKTransformType::CoefficientImageArray coefficient_images;

      for (unsigned int i=0; i < BLUTCoefficientImageType::ImageDimension; i++) {
          component_filter[i] = FilterType::New();
          component_filter[i]->SetInput(input);
          component_filter[i]->SetComponentIndex(i);
          component_filter[i]->Update();
          coefficient_images[i] = component_filter[i]->GetOutput();
      }

      // RP: 16/01/2013
      // ATTENTION: Apparently, there's a bug in the SetCoefficientImages function of ITK 4.x
      // I needed to use SetParametersByValue instead.
      //
      typename ITKTransformType::ParametersType params(input->GetPixelContainer()->Size() * BLUTCoefficientImageType::ImageDimension);
      for (unsigned int i=0; i < BLUTCoefficientImageType::ImageDimension; i++) {
        for (unsigned int j=0; j < coefficient_images[i]->GetPixelContainer()->Size(); j++)
          params[input->GetPixelContainer()->Size() * i + j] = coefficient_images[i]->GetPixelContainer()->GetBufferPointer()[j]; 
      }
      typename ITKTransformTypeZero::Pointer zero;
      typename ITKTransformTypeOne::Pointer one;
      typename ITKTransformTypeTwo::Pointer two;
      typename ITKTransformTypeFour::Pointer four;

      switch(m_BLUTSplineOrders[0])
      {
        case 0:
          zero = ITKTransformTypeZero::New();
          zero->SetGridOrigin(input->GetOrigin());
          zero->SetGridDirection(input->GetDirection());
          zero->SetGridRegion(input->GetLargestPossibleRegion());
          zero->SetGridSpacing(input->GetSpacing());
          m_GenericTransform = zero;
          break;
        case 1:
          one = ITKTransformTypeOne::New();
          one->SetGridOrigin(input->GetOrigin());
          one->SetGridDirection(input->GetDirection());
          one->SetGridRegion(input->GetLargestPossibleRegion());
          one->SetGridSpacing(input->GetSpacing());
          m_GenericTransform = one;
          break;
        case 2:
          two = ITKTransformTypeTwo::New();
          two->SetGridOrigin(input->GetOrigin());
          two->SetGridDirection(input->GetDirection());
          two->SetGridRegion(input->GetLargestPossibleRegion());
          two->SetGridSpacing(input->GetSpacing());
          m_GenericTransform = two;
          break;
        case 3:
          m_ITKTransform = ITKTransformType::New();
          m_ITKTransform->SetGridOrigin(input->GetOrigin());
          m_ITKTransform->SetGridDirection(input->GetDirection());
          m_ITKTransform->SetGridRegion(input->GetLargestPossibleRegion());
          m_ITKTransform->SetGridSpacing(input->GetSpacing());
          m_GenericTransform = m_ITKTransform;
          break;
        case 4:
          four = ITKTransformTypeFour::New();
          four->SetGridOrigin(input->GetOrigin());
          four->SetGridDirection(input->GetDirection());
          four->SetGridRegion(input->GetLargestPossibleRegion());
          four->SetGridSpacing(input->GetSpacing());
          m_GenericTransform = four;
          break;
      }
      m_GenericTransform->SetParametersByValue(params);
    }

#if ITK_VERSION_MAJOR > 4 || (ITK_VERSION_MAJOR == 4 && ITK_VERSION_MINOR >= 6)
    m_Filter->SetSize(output->GetLargestPossibleRegion().GetSize());
#else
    m_Filter->SetOutputSize(output->GetLargestPossibleRegion().GetSize());
#endif
    m_Filter->SetOutputOrigin(output->GetOrigin());
    m_Filter->SetOutputSpacing(output->GetSpacing());
    m_Filter->SetTransform(m_GenericTransform);

    m_Filter->Update();

    this->SetNthOutput(0, m_Filter->GetOutput());
  }


}
