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
#ifndef clitkAffineTransformGenericFilter_txx
#define clitkAffineTransformGenericFilter_txx

#include <sstream>
#include <istream>
#include <iterator>
#include <itkCenteredEuler3DTransform.h>
#include <itkRecursiveGaussianImageFilter.h>
#include "clitkElastix.h"
#include "clitkResampleImageWithOptionsFilter.h"

namespace clitk
{

  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  template<class args_info_type>
  AffineTransformGenericFilter<args_info_type>::AffineTransformGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }
  //-------------------------------------------------------------------
 

  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  template<class args_info_type>
  void AffineTransformGenericFilter<args_info_type>::Update()
  {
    // Read the Dimension and PixelType
    int Dimension, Components;
    std::string PixelType;
    ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType, Components);

    // Call UpdateWithDim
    if (Dimension==2)
        UpdateWithDim<2>(PixelType, Components);
    else if (Dimension==3)
        UpdateWithDim<3>(PixelType, Components);
    else if (Dimension==4)
        UpdateWithDim<4>(PixelType, Components);
    else {
        std::cout<<"Error, Only for 2, 3 or 4  Dimensions!!!"<<std::endl ;
        return;
      }
  }
  //-------------------------------------------------------------------
 

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<class args_info_type>
  template<unsigned int Dimension>
  void
  AffineTransformGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType, int Components)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<<Components<<" component(s) of "<<  PixelType<<"..."<<std::endl;

    if (Components==1) {
      if(PixelType == "short") {
        if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, signed short>();
      }
      else if(PixelType == "unsigned_short"){
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, unsigned short>();
      }

      else if (PixelType == "unsigned_char") {
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, unsigned char>();
      }

      //     else if (PixelType == "char"){
      //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
      //       UpdateWithDimAndPixelType<Dimension, signed char>();
      //     }
      else if(PixelType == "double"){
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and double..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, double>();
      }
      else {
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, float>();
      }
    }

    else if (Components==3) {
      if (m_Verbose) std::cout  << "Launching transform in "<< Dimension <<"D and 3D float (DVF)" << std::endl;
      UpdateWithDimAndVectorType<Dimension, itk::Vector<float, Dimension> >();
    }

    else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;

  }
  //-------------------------------------------------------------------


  //-------------------------------------------------------------------
  // Compute updated bounding box
  //-------------------------------------------------------------------
  template<class args_info_type>
  vnl_vector<double>
  AffineTransformGenericFilter<args_info_type>::ComputeSize(vnl_vector<double> inputSize, vnl_matrix<double> transformationMatrix, bool returnMin)
  {
    //Compute input corners
    int Dimension = inputSize.size();
    vnl_matrix<double> vnlOutputSize(std::pow(2, Dimension), Dimension);
    vnlOutputSize.fill(0);
    if (Dimension == 2) {
      for(unsigned int i=0; i< Dimension; i++)
        vnlOutputSize[3][i] = inputSize[i];
      vnlOutputSize[1][0] = inputSize[0];
      vnlOutputSize[2][1] = inputSize[1];
    } else if (Dimension == 3) {
      for(unsigned int i=0; i< Dimension; i++)
        vnlOutputSize[7][i] = inputSize[i];
      vnlOutputSize[1][0] = inputSize[0];
      vnlOutputSize[2][1] = inputSize[1];
      vnlOutputSize[3][2] = inputSize[2];
      vnlOutputSize[4][0] = inputSize[0];
      vnlOutputSize[4][1] = inputSize[1];
      vnlOutputSize[5][1] = inputSize[1];
      vnlOutputSize[5][2] = inputSize[2];
      vnlOutputSize[6][0] = inputSize[0];
      vnlOutputSize[6][2] = inputSize[2];
    } else { //Dimension ==4
      for(unsigned int i=0; i< Dimension; i++)
        vnlOutputSize[15][i] = inputSize[i];
      vnlOutputSize[1][0] = inputSize[0];
      vnlOutputSize[2][1] = inputSize[1];
      vnlOutputSize[3][2] = inputSize[2];
      vnlOutputSize[4][3] = inputSize[3];
      vnlOutputSize[5][0] = inputSize[0];
      vnlOutputSize[5][1] = inputSize[1];
      vnlOutputSize[6][0] = inputSize[0];
      vnlOutputSize[6][2] = inputSize[2];
      vnlOutputSize[7][0] = inputSize[0];
      vnlOutputSize[7][3] = inputSize[3];
      vnlOutputSize[8][1] = inputSize[1];
      vnlOutputSize[8][2] = inputSize[2];
      vnlOutputSize[9][1] = inputSize[1];
      vnlOutputSize[9][3] = inputSize[3];
      vnlOutputSize[10][2] = inputSize[2];
      vnlOutputSize[10][3] = inputSize[3];
      vnlOutputSize[11][0] = inputSize[0];
      vnlOutputSize[11][1] = inputSize[1];
      vnlOutputSize[11][2] = inputSize[2];
      vnlOutputSize[12][0] = inputSize[0];
      vnlOutputSize[12][1] = inputSize[1];
      vnlOutputSize[12][3] = inputSize[3];
      vnlOutputSize[13][0] = inputSize[0];
      vnlOutputSize[13][2] = inputSize[2];
      vnlOutputSize[13][3] = inputSize[3];
      vnlOutputSize[14][1] = inputSize[1];
      vnlOutputSize[14][2] = inputSize[2];
      vnlOutputSize[14][3] = inputSize[3];
    }

    //Compute the transformation of all corner
    for (unsigned int i=0; i< std::pow(2, Dimension); ++i)
      vnlOutputSize.set_row(i, transformationMatrix*vnlOutputSize.get_row(i));

    //Compute the bounding box taking the max and the min
    vnl_vector<double> minBB(vnlOutputSize.get_row(0)), maxBB(vnlOutputSize.get_row(0));
    for (unsigned int i=0; i< std::pow(2, Dimension); ++i) {
      for (unsigned int j=0; j< Dimension; ++j) {
        if (vnlOutputSize[i][j] < minBB[j])
          minBB[j] = vnlOutputSize[i][j];
        if (vnlOutputSize[i][j] > maxBB[j])
          maxBB[j] = vnlOutputSize[i][j];
      }
    }

    //Compute the size
    if (returnMin)
      return minBB;
    else {
      vnl_vector<double> size;
      size = maxBB - minBB;

      return size;
    }
  }
  //-------------------------------------------------------------------
 

  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template<class args_info_type>
  template <unsigned int Dimension, class  PixelType>
  void
  AffineTransformGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
  {

    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<PixelType, Dimension> OutputImageType;

    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();

    //Adaptative size, spacing origin (use previous clitkResampleImage)
    if (m_ArgsInfo.adaptive_given) {
      // Filter
      typedef clitk::ResampleImageWithOptionsFilter<InputImageType, OutputImageType> ResampleImageFilterType;
      typename ResampleImageFilterType::Pointer filter = ResampleImageFilterType::New();
      filter->SetInput(input);

      // Set Verbose
      filter->SetVerboseOptions(m_ArgsInfo.verbose_flag);

      // Set size / spacing
      static const unsigned int dim = OutputImageType::ImageDimension;
      typename OutputImageType::SpacingType spacing;
      typename OutputImageType::SizeType size;
      typename OutputImageType::PointType origin;
      typename OutputImageType::DirectionType direction;

      if (m_ArgsInfo.like_given) {
        itk::ImageIOBase::Pointer header = clitk::readImageHeader(m_ArgsInfo.like_arg);
        if (header) {
          for(unsigned int i=0; i<dim; i++){
            spacing[i] = header->GetSpacing(i);
            size[i] = header->GetDimensions(i);
            origin[i] = header->GetOrigin(i);
          }
          for(unsigned int i=0; i<dim; i++) {
            for(unsigned int j=0;j<dim;j++) {
                direction(i,j) = header->GetDirection(i)[j];
            }
          }
          filter->SetOutputSpacing(spacing);
          filter->SetOutputSize(size);
          filter->SetOutputOrigin(origin);
          filter->SetOutputDirection(direction);
        }
        else {
          std::cerr << "*** Warning : I could not read '" << m_ArgsInfo.like_arg << "' ***" << std::endl;
          exit(0);
        }
      }
      else {
        if (m_ArgsInfo.spacing_given == 1) {
          filter->SetOutputIsoSpacing(m_ArgsInfo.spacing_arg[0]);
        }
        else if ((m_ArgsInfo.spacing_given != 0) && (m_ArgsInfo.size_given != 0)) {
          std::cerr << "Error: use spacing or size, not both." << std::endl;
          exit(0);
        }
        else if (m_ArgsInfo.spacing_given) {
          if ((m_ArgsInfo.spacing_given != 0) && (m_ArgsInfo.spacing_given != dim)) {
            std::cerr << "Error: spacing should have one or " << dim << " values." << std::endl;
            exit(0);
          }
          for(unsigned int i=0; i<dim; i++)
            spacing[i] = m_ArgsInfo.spacing_arg[i];
          filter->SetOutputSpacing(spacing);
        }
        else if (m_ArgsInfo.size_given) {
          if ((m_ArgsInfo.size_given != 0) && (m_ArgsInfo.size_given != dim)) {
            std::cerr << "Error: size should have " << dim << " values." << std::endl;
            exit(0);
          }
          for(unsigned int i=0; i<dim; i++)
            size[i] = m_ArgsInfo.size_arg[i];
          filter->SetOutputSize(size);
        }
        for(unsigned int i=0; i<dim; i++){
          origin[i] = input->GetOrigin()[i];
        }
        for(unsigned int i=0; i<dim; i++) {
          for(unsigned int j=0;j<dim;j++) {
              direction(i,j) = input->GetDirection()[i][j];
          }
        }
        filter->SetOutputOrigin(origin);
        filter->SetOutputDirection(direction);
      }

      // Set temporal dimension
      //filter->SetLastDimensionIsTime(m_ArgsInfo.time_flag);

      // Set Gauss
      filter->SetGaussianFilteringEnabled(m_ArgsInfo.autogauss_flag);
      if (m_ArgsInfo.gauss_given != 0) {
        typename ResampleImageFilterType::GaussianSigmaType g;
        for(unsigned int i=0; i<dim; i++) {
          g[i] = m_ArgsInfo.gauss_arg[i];
        }
        filter->SetGaussianSigma(g);
      }

      // Set Interpolation
      int interp = m_ArgsInfo.interp_arg;
      if (interp == 0) {
        filter->SetInterpolationType(ResampleImageFilterType::NearestNeighbor);
      } else {
        if (interp == 1) {
          filter->SetInterpolationType(ResampleImageFilterType::Linear);
        } else {
          if (interp == 2) {
            filter->SetInterpolationType(ResampleImageFilterType::BSpline);
          } else {
            if (interp == 3) {
              filter->SetInterpolationType(ResampleImageFilterType::B_LUT);
            } else {
                std::cerr << "Error. I do not know interpolation '" << m_ArgsInfo.interp_arg
                          << "'. Choose among: nn, linear, bspline, blut, windowed sinc" << std::endl;
                exit(0);
            }
          }
        }
      }

      // Set default pixel value
      filter->SetDefaultPixelValue(m_ArgsInfo.pad_arg);

      // Set thread
      //if (m_ArgsInfo.thread_given) {
      //  filter->SetNumberOfThreads(m_ArgsInfo.thread_arg);
      //}

      // Go !
      filter->Update();
      typename OutputImageType::Pointer output = filter->GetOutput();
      //this->template SetNextOutput<OutputImageType>(outputImage);

      // Output
      typedef itk::ImageFileWriter<OutputImageType> WriterType;
      typename WriterType::Pointer writer = WriterType::New();
      writer->SetFileName(m_ArgsInfo.output_arg);
      writer->SetInput(output);
      writer->Update();

      return;
    }

    //Gaussian pre-filtering
    typename itk::Vector<double, Dimension> gaussianSigma;
    gaussianSigma.Fill(0);
    bool gaussianFilteringEnabled(false);
    bool autoGaussEnabled(false);
    if (m_ArgsInfo.autogauss_given) { // Gaussian filter auto
      autoGaussEnabled = m_ArgsInfo.autogauss_flag;
    }
    if (m_ArgsInfo.gauss_given) { // Gaussian filter set by user
      gaussianFilteringEnabled = true;
      if (m_ArgsInfo.gauss_given == 1)
      {
        for (unsigned int i=0; i<Dimension; i++)
        {
          gaussianSigma[i] = m_ArgsInfo.gauss_arg[0];
        }
      }
      else if (m_ArgsInfo.gauss_given == Dimension)
      {
        for (unsigned int i=0; i<Dimension; i++)
        {
          gaussianSigma[i] = m_ArgsInfo.gauss_arg[i];
        }
      }
      else
      {
        std::cerr << "Gaussian sigma dimension is incorrect" << std::endl;
        return;
      }
    }

    //Filter
    typedef  itk::ResampleImageFilter< InputImageType,OutputImageType >  ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // Matrix
    typename itk::Matrix<double, Dimension+1, Dimension+1> matrix;
    if (m_ArgsInfo.rotate_given || m_ArgsInfo.translate_given) {
        if (m_ArgsInfo.matrix_given) {
            std::cerr << "You must use either rotate/translate or matrix options" << std::endl;
            return;
        }
        itk::Array<double> transformParameters(2 * Dimension);
        transformParameters.Fill(0.0);
        if (m_ArgsInfo.rotate_given) {
            if (Dimension == 2)
                transformParameters[0] = m_ArgsInfo.rotate_arg[0];
            else
                for (unsigned int i = 0; i < 3; i++)
                    transformParameters[i] = m_ArgsInfo.rotate_arg[i];
        }
        if (m_ArgsInfo.translate_given) {
            int pos = 3;
            if (Dimension == 2)
              pos = 1;
            for (unsigned int i = 0; i < Dimension && i < 3; i++)
              transformParameters[pos++] = m_ArgsInfo.translate_arg[i];
        }
        matrix = GetForwardAffineMatrix<Dimension>(transformParameters);
    }
    else {
        if (m_ArgsInfo.matrix_given) {
            matrix= clitk::ReadMatrix<Dimension>(m_ArgsInfo.matrix_arg);
            if (m_Verbose)
                std::cout << "Reading the matrix..." << std::endl;
        }
        else {
          if (m_ArgsInfo.elastix_given) {
            std::string filename(m_ArgsInfo.elastix_arg);
            matrix = createMatrixFromElastixFile<Dimension>(filename, m_Verbose);
          }
          else
              matrix.SetIdentity();
        }
    }
    if (m_Verbose)
        std::cout << "Using the following matrix:" << std::endl
                  << matrix << std::endl;
    typename itk::Matrix<double, Dimension, Dimension> rotationMatrix = clitk::GetRotationalPartMatrix(matrix);
    typename itk::Vector<double, Dimension> translationPart = clitk::GetTranslationPartMatrix(matrix);

    // Transform
    typedef itk::AffineTransform<double, Dimension> AffineTransformType;
    typename AffineTransformType::Pointer affineTransform=AffineTransformType::New();
    affineTransform->SetMatrix(rotationMatrix);
    affineTransform->SetTranslation(translationPart);

    // Interp
    typedef clitk::GenericInterpolator<args_info_type, InputImageType, double> GenericInterpolatorType;
    typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
    genericInterpolator->SetArgsInfo(m_ArgsInfo);

    // Properties
    if (m_ArgsInfo.like_given) {
      typename InputReaderType::Pointer likeReader=InputReaderType::New();
      likeReader->SetFileName(m_ArgsInfo.like_arg);
      likeReader->Update();
      resampler->SetOutputParametersFromImage(likeReader->GetOutput());
      resampler->SetOutputDirection(likeReader->GetOutput()->GetDirection());
      if (autoGaussEnabled) { // Automated sigma when downsample
        for(unsigned int i=0; i<Dimension; i++) {
          if (likeReader->GetOutput()->GetSpacing()[i] > input->GetSpacing()[i]) { // downsample
            gaussianSigma[i] = 0.5*likeReader->GetOutput()->GetSpacing()[i];// / inputSpacing[i]);
          }
          else gaussianSigma[i] = 0; // will be ignore after
        }
      }
    } else if(m_ArgsInfo.transform_grid_flag) {
      typename itk::Matrix<double, Dimension+1, Dimension+1> invMatrix( matrix.GetInverse() );
      typename itk::Matrix<double, Dimension, Dimension> invRotMatrix( clitk::GetRotationalPartMatrix(invMatrix) );
      typename itk::Vector<double,Dimension> invTrans =  clitk::GetTranslationPartMatrix(invMatrix);
      
      // Display warning
      if (m_ArgsInfo.spacing_given)
        std::cout << "Warning --spacing ignored (because --transform_grid_flag)" << std::endl;
      if (m_ArgsInfo.origin_given)
        std::cout << "Warning --origin ignored (because --transform_grid_flag)" << std::endl;

      // Origin is influenced by translation but not by input direction
      typename InputImageType::PointType outputOrigin;
      outputOrigin = invRotMatrix *
        input->GetOrigin() +
        invTrans;

      // Size is influenced by affine transform matrix and input direction
      // Size is converted to double, transformed and converted back to size type.
      // Determine the bounding box tranforming all corners
      vnl_vector<double> vnlOutputSize(Dimension), vnlOutputmmSize(Dimension), vnlOutputOffset(Dimension);
      typename InputImageType::SpacingType outputSpacing;
      for(unsigned int i=0; i< Dimension; i++) {
        vnlOutputSize[i] = input->GetLargestPossibleRegion().GetSize()[i];
        vnlOutputmmSize[i] = input->GetLargestPossibleRegion().GetSize()[i]*input->GetSpacing()[i];
        vnlOutputOffset[i] = input->GetLargestPossibleRegion().GetSize()[i]*input->GetSpacing()[i];
      }
      vnlOutputSize = ComputeSize(vnlOutputSize, invRotMatrix.GetVnlMatrix() * input->GetDirection().GetVnlMatrix(), 0);
      vnlOutputmmSize = ComputeSize(vnlOutputmmSize, invRotMatrix.GetVnlMatrix() * input->GetDirection().GetVnlMatrix(), 0);
      vnlOutputOffset = ComputeSize(vnlOutputOffset, invRotMatrix.GetVnlMatrix() * input->GetDirection().GetVnlMatrix(), 1);
      for(unsigned int i=0; i< Dimension; i++) {
        outputSpacing[i] = vnlOutputmmSize[i]/lrint(vnlOutputSize[i]);
        outputOrigin[i] += vnlOutputOffset[i];
      }
      if (autoGaussEnabled) { // Automated sigma when downsample
        for(unsigned int i=0; i<Dimension; i++) {
          if (outputSpacing[i] > input->GetSpacing()[i]) { // downsample
            gaussianSigma[i] = 0.5*outputSpacing[i];// / inputSpacing[i]);
          }
          else gaussianSigma[i] = 0; // will be ignore after
        }
      }

      typename OutputImageType::SizeType outputSize;
      for(unsigned int i=0; i< Dimension; i++) {
        // If the size is negative, we have a flip and we must modify
        // the origin and the spacing accordingly.
        if(vnlOutputSize[i]<0.) {
          vnlOutputSize[i] *= -1.;
          outputOrigin[i] = outputOrigin[i] + outputSpacing[i] * (vnlOutputSize[i]-1);
          outputSpacing[i] *= -1.;
        }
        outputSize[i] = lrint(vnlOutputSize[i]);
      }
      resampler->SetSize( outputSize );
      resampler->SetOutputSpacing( outputSpacing );
      resampler->SetOutputOrigin( outputOrigin );
    } else {
      //Size
      typename OutputImageType::SizeType outputSize;
      if (m_ArgsInfo.size_given) {
        for(unsigned int i=0; i< Dimension; i++)
          outputSize[i]=m_ArgsInfo.size_arg[i];
      } else outputSize=input->GetLargestPossibleRegion().GetSize();

      //Spacing
      typename OutputImageType::SpacingType outputSpacing;
      if (m_ArgsInfo.spacing_given) {
        for(unsigned int i=0; i< Dimension; i++)
          outputSpacing[i]=m_ArgsInfo.spacing_arg[i];
      } else outputSpacing=input->GetSpacing();
      if (autoGaussEnabled) { // Automated sigma when downsample
        for(unsigned int i=0; i<Dimension; i++) {
          if (outputSpacing[i] > input->GetSpacing()[i]) { // downsample
            gaussianSigma[i] = 0.5*outputSpacing[i];// / inputSpacing[i]);
          }
          else gaussianSigma[i] = 0; // will be ignore after
        }
      }

      //Origin
      typename OutputImageType::PointType outputOrigin;
      if (m_ArgsInfo.origin_given) {
        for(unsigned int i=0; i< Dimension; i++)
          outputOrigin[i]=m_ArgsInfo.origin_arg[i];
      } else outputOrigin=input->GetOrigin();

      //Direction
      typename OutputImageType::DirectionType outputDirection;
      if (m_ArgsInfo.direction_given) {
        for(unsigned int j=0; j< Dimension; j++)
            for(unsigned int i=0; i< Dimension; i++)
                outputDirection[j][i]=m_ArgsInfo.direction_arg[i+Dimension*j];
      } else outputDirection=input->GetDirection();

      // Set
      resampler->SetSize( outputSize );
      resampler->SetOutputSpacing( outputSpacing );
      resampler->SetOutputOrigin(  outputOrigin );
      resampler->SetOutputDirection( outputDirection );

    }

    if (m_ArgsInfo.spacinglike_given) {
      typename InputReaderType::Pointer likeReader=InputReaderType::New();
      likeReader->SetFileName(m_ArgsInfo.spacinglike_arg);
      likeReader->Update(); 

      // set the support like the image 
      if (m_ArgsInfo.like_given) {
        typename OutputImageType::SizeType outputSize;
        outputSize[0] = ceil(resampler->GetSize()[0]*resampler->GetOutputSpacing()[0]
                             /likeReader->GetOutput()->GetSpacing()[0]);
        outputSize[1] = ceil(resampler->GetSize()[1]*resampler->GetOutputSpacing()[1]
                             /likeReader->GetOutput()->GetSpacing()[1]);
        outputSize[2] = ceil(resampler->GetSize()[2]*resampler->GetOutputSpacing()[2]
                             /likeReader->GetOutput()->GetSpacing()[2]);
        if (m_ArgsInfo.verbose_flag) {
          std::cout << "Compute the number of pixels such as the support is like " << m_ArgsInfo.like_arg << std::endl;
        }
        resampler->SetSize( outputSize );
      }

      resampler->SetOutputSpacing( likeReader->GetOutput()->GetSpacing() );      
    }

    if (m_ArgsInfo.verbose_flag) {
      std::cout << "Setting the output size to " << resampler->GetSize() << "..." << std::endl;
      std::cout << "Setting the output spacing to " << resampler->GetOutputSpacing() << "..." << std::endl;
      std::cout << "Setting the output origin to " << resampler->GetOutputOrigin() << "..." << std::endl;
      std::cout << "Setting the output direction to " << resampler->GetOutputDirection() << "..." << std::endl;
    }

    typedef itk::RecursiveGaussianImageFilter<InputImageType, InputImageType> GaussianFilterType;
    std::vector<typename GaussianFilterType::Pointer> gaussianFilters;
    if (gaussianFilteringEnabled || autoGaussEnabled) {
      for(unsigned int i=0; i<Dimension; i++) {
        if (gaussianSigma[i] != 0) {
          gaussianFilters.push_back(GaussianFilterType::New());
          gaussianFilters[i]->SetDirection(i);
          gaussianFilters[i]->SetOrder(GaussianFilterType::ZeroOrder);
          gaussianFilters[i]->SetNormalizeAcrossScale(false);
          gaussianFilters[i]->SetSigma(gaussianSigma[i]); // in millimeter !
          if (gaussianFilters.size() == 1) { // first
            gaussianFilters[0]->SetInput(input);
          } else {
            gaussianFilters[i]->SetInput(gaussianFilters[i-1]->GetOutput());
          }
        }
      }
      if (gaussianFilters.size() > 0) {
        resampler->SetInput(gaussianFilters[gaussianFilters.size()-1]->GetOutput());
      } else resampler->SetInput(input);
    } else resampler->SetInput(input);

    resampler->SetTransform( affineTransform );
    resampler->SetInterpolator( genericInterpolator->GetInterpolatorPointer());
    resampler->SetDefaultPixelValue( static_cast<PixelType>(m_ArgsInfo.pad_arg) );

    try {
      resampler->Update();
    } catch(itk::ExceptionObject) {
      std::cerr<<"Error resampling the image"<<std::endl;
    }

    typename OutputImageType::Pointer output = resampler->GetOutput();

    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }
  //-------------------------------------------------------------------
    

  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype (components)
  //-------------------------------------------------------------------
  template<class args_info_type>
  template<unsigned int Dimension, class PixelType>
  void AffineTransformGenericFilter<args_info_type>::UpdateWithDimAndVectorType()
  {
    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef itk::Image<PixelType, Dimension> OutputImageType;

    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();

    //Gaussian pre-filtering
    typename itk::Vector<double, Dimension> gaussianSigma;
    gaussianSigma.Fill(0);
    bool gaussianFilteringEnabled(false);
    bool autoGaussEnabled(false);
    if (m_ArgsInfo.autogauss_given) { // Gaussian filter auto
      autoGaussEnabled = m_ArgsInfo.autogauss_flag;
    }
    if (m_ArgsInfo.gauss_given) { // Gaussian filter set by user
      gaussianFilteringEnabled = true;
      if (m_ArgsInfo.gauss_given == 1)
      {
        for (unsigned int i=0; i<Dimension; i++)
        {
          gaussianSigma[i] = m_ArgsInfo.gauss_arg[0];
        }
      }
      else if (m_ArgsInfo.gauss_given == Dimension)
      {
        for (unsigned int i=0; i<Dimension; i++)
        {
          gaussianSigma[i] = m_ArgsInfo.gauss_arg[i];
        }
      }
      else
      {
        std::cerr << "Gaussian sigma dimension is incorrect" << std::endl;
        return;
      }
    }

    //Filter
#if ( ITK_VERSION_MAJOR < 5 )
    typedef  itk::VectorResampleImageFilter< InputImageType,OutputImageType, double >  ResampleFilterType;
#else
    typedef  itk::ResampleImageFilter< InputImageType,OutputImageType, double >  ResampleFilterType;
#endif
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // Matrix
    typename itk::Matrix<double, Dimension+1, Dimension+1> matrix;
    if (m_ArgsInfo.rotate_given || m_ArgsInfo.translate_given) {
        if (m_ArgsInfo.matrix_given) {
            std::cerr << "You must use either rotate/translate or matrix options" << std::endl;
            return;
        }
        itk::Array<double> transformParameters(2 * Dimension);
        transformParameters.Fill(0.0);
        if (m_ArgsInfo.rotate_given) {
            if (Dimension == 2)
              transformParameters[0] = m_ArgsInfo.rotate_arg[0];
            else
              for (unsigned int i = 0; i < 3; i++)
                transformParameters[i] = m_ArgsInfo.rotate_arg[i];
        }
        if (m_ArgsInfo.translate_given) {
            int pos = 3;
            if (Dimension == 2)
              pos = 1;
            for (unsigned int i = 0; i < Dimension && i < 3; i++)
              transformParameters[pos++] = m_ArgsInfo.translate_arg[i];
        }
        matrix = GetForwardAffineMatrix<Dimension>(transformParameters);
    }
    else {
        if (m_ArgsInfo.matrix_given) {
            matrix= clitk::ReadMatrix<Dimension>(m_ArgsInfo.matrix_arg);
            if (m_Verbose)
                std::cout << "Reading the matrix..." << std::endl;
        }
        else
            matrix.SetIdentity();
    }
    if (m_Verbose)
      std::cout << "Using the following matrix:" << std::endl
                << matrix << std::endl;
    typename itk::Matrix<double, Dimension, Dimension> rotationMatrix = clitk::GetRotationalPartMatrix(matrix);
    typename itk::Vector<double, Dimension> translationPart = clitk::GetTranslationPartMatrix(matrix);

    // Transform
    typedef itk::AffineTransform<double, Dimension> AffineTransformType;
    typename AffineTransformType::Pointer affineTransform=AffineTransformType::New();
    affineTransform->SetMatrix(rotationMatrix);
    affineTransform->SetTranslation(translationPart);

    // Interp
    typedef clitk::GenericVectorInterpolator<args_info_type, InputImageType, double> GenericInterpolatorType;
    typename GenericInterpolatorType::Pointer genericInterpolator=GenericInterpolatorType::New();
    genericInterpolator->SetArgsInfo(m_ArgsInfo);

    // Properties
    if (m_ArgsInfo.like_given) {
      typename InputReaderType::Pointer likeReader=InputReaderType::New();
      likeReader->SetFileName(m_ArgsInfo.like_arg);
      likeReader->Update();
      resampler->SetSize( likeReader->GetOutput()->GetLargestPossibleRegion().GetSize() );
      resampler->SetOutputSpacing( likeReader->GetOutput()->GetSpacing() );
      resampler->SetOutputOrigin(  likeReader->GetOutput()->GetOrigin() );
      resampler->SetOutputDirection( likeReader->GetOutput()->GetDirection() );
      if (autoGaussEnabled) { // Automated sigma when downsample
        for(unsigned int i=0; i<Dimension; i++) {
          if (likeReader->GetOutput()->GetSpacing()[i] > input->GetSpacing()[i]) { // downsample
            gaussianSigma[i] = 0.5*likeReader->GetOutput()->GetSpacing()[i];// / inputSpacing[i]);
          }
          else gaussianSigma[i] = 0; // will be ignore after
        }
      }
    } else {
      //Size
      typename OutputImageType::SizeType outputSize;
      if (m_ArgsInfo.size_given) {
        for(unsigned int i=0; i< Dimension; i++)
          outputSize[i]=m_ArgsInfo.size_arg[i];
      } else outputSize=input->GetLargestPossibleRegion().GetSize();
      std::cout<<"Setting the size to "<<outputSize<<"..."<<std::endl;

      //Spacing
      typename OutputImageType::SpacingType outputSpacing;
      if (m_ArgsInfo.spacing_given) {
        for(unsigned int i=0; i< Dimension; i++)
          outputSpacing[i]=m_ArgsInfo.spacing_arg[i];
      } else outputSpacing=input->GetSpacing();
      if (autoGaussEnabled) { // Automated sigma when downsample
        for(unsigned int i=0; i<Dimension; i++) {
          if (outputSpacing[i] > input->GetSpacing()[i]) { // downsample
            gaussianSigma[i] = 0.5*outputSpacing[i];// / inputSpacing[i]);
          }
          else gaussianSigma[i] = 0; // will be ignore after
        }
      }
      std::cout<<"Setting the spacing to "<<outputSpacing<<"..."<<std::endl;

      //Origin
      typename OutputImageType::PointType outputOrigin;
      if (m_ArgsInfo.origin_given) {
        for(unsigned int i=0; i< Dimension; i++)
          outputOrigin[i]=m_ArgsInfo.origin_arg[i];
      } else outputOrigin=input->GetOrigin();
      std::cout<<"Setting the origin to "<<outputOrigin<<"..."<<std::endl;

      //Direction
      typename OutputImageType::DirectionType outputDirection;
      if (m_ArgsInfo.direction_given) {
        for(unsigned int j=0; j< Dimension; j++)
            for(unsigned int i=0; i< Dimension; i++)
                outputDirection[j][i]=m_ArgsInfo.direction_arg[i+Dimension*j];
      } else outputDirection=input->GetDirection();
      std::cout<<"Setting the direction to "<<outputDirection<<"..."<<std::endl;

      // Set
      resampler->SetSize( outputSize );
      resampler->SetOutputSpacing( outputSpacing );
      resampler->SetOutputOrigin(  outputOrigin );
      resampler->SetOutputDirection( outputDirection );

    }

    typedef itk::RecursiveGaussianImageFilter<InputImageType, InputImageType> GaussianFilterType;
    std::vector<typename GaussianFilterType::Pointer> gaussianFilters;
    if (gaussianFilteringEnabled || autoGaussEnabled) {
      for(unsigned int i=0; i<Dimension; i++) {
        if (gaussianSigma[i] != 0) {
          gaussianFilters.push_back(GaussianFilterType::New());
          gaussianFilters[i]->SetDirection(i);
          gaussianFilters[i]->SetOrder(GaussianFilterType::ZeroOrder);
          gaussianFilters[i]->SetNormalizeAcrossScale(false);
          gaussianFilters[i]->SetSigma(gaussianSigma[i]); // in millimeter !
          if (gaussianFilters.size() == 1) { // first
            gaussianFilters[0]->SetInput(input);
          } else {
            gaussianFilters[i]->SetInput(gaussianFilters[i-1]->GetOutput());
          }
        }
      }
      if (gaussianFilters.size() > 0) {
        resampler->SetInput(gaussianFilters[gaussianFilters.size()-1]->GetOutput());
      } else resampler->SetInput(input);
    } else resampler->SetInput(input);

    resampler->SetInput( input );
    resampler->SetTransform( affineTransform );
    resampler->SetInterpolator( genericInterpolator->GetInterpolatorPointer());
    resampler->SetDefaultPixelValue( static_cast<PixelType>(m_ArgsInfo.pad_arg) );

    try {
      resampler->Update();
    } catch(itk::ExceptionObject) {
      std::cerr<<"Error resampling the image"<<std::endl;
    }

    typename OutputImageType::Pointer output = resampler->GetOutput();

    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }
  //-------------------------------------------------------------------

} //end clitk

#endif //#define clitkAffineTransformGenericFilter_txx
