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
    if(Dimension==2) UpdateWithDim<2>(PixelType, Components);
    else 
    if(Dimension==3) UpdateWithDim<3>(PixelType, Components);
    else if (Dimension==4)UpdateWithDim<4>(PixelType, Components);
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
      //    else if(PixelType == "unsigned_short"){
      //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
      //       UpdateWithDimAndPixelType<Dimension, unsigned short>();
      //     }

      else if (PixelType == "unsigned_char") {
        if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
        UpdateWithDimAndPixelType<Dimension, unsigned char>();
      }

      //     else if (PixelType == "char"){
      //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
      //       UpdateWithDimAndPixelType<Dimension, signed char>();
      //     }
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

    //Filter
    typedef  itk::ResampleImageFilter< InputImageType,OutputImageType >  ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // Matrix
    typename itk::Matrix<double, Dimension+1, Dimension+1> matrix;
    if (m_ArgsInfo.rotate_given || m_ArgsInfo.translate_given)
      {
        if (m_ArgsInfo.matrix_given)
          {
            std::cerr << "You must use either rotate/translate or matrix options" << std::cout;
            return;
          }
        itk::Array<double> transformParameters(2 * Dimension);
        transformParameters.Fill(0.0);
        if (m_ArgsInfo.rotate_given)
          {
            if (Dimension == 2)
              transformParameters[0] = m_ArgsInfo.rotate_arg[0];
            else
              for (unsigned int i = 0; i < 3; i++)
                transformParameters[i] = m_ArgsInfo.rotate_arg[i];
          }
        if (m_ArgsInfo.translate_given)
          {
            int pos = 3;
            if (Dimension == 2)
              pos = 1;
            for (unsigned int i = 0; i < Dimension && i < 3; i++)
              transformParameters[pos++] = m_ArgsInfo.translate_arg[i];
          }
        if (Dimension == 4)
          {
            matrix.SetIdentity();
            itk::Matrix<double, 4, 4> tmp = GetForwardAffineMatrix3D(transformParameters);
            for (unsigned int i = 0; i < 3; ++i)
              for (unsigned int j = 0; j < 3; ++j)
                matrix[i][j] = tmp[i][j];
            for (unsigned int i = 0; i < 3; ++i)
              matrix[i][4] = tmp[i][3];
          }
        else
          matrix = GetForwardAffineMatrix<Dimension>(transformParameters);
      }
    else
      {
        if (m_ArgsInfo.matrix_given)
          {
            matrix= clitk::ReadMatrix<Dimension>(m_ArgsInfo.matrix_arg);
            if (m_Verbose) std::cout << "Reading the matrix..." << std::endl;
          }
        else {
          if (m_ArgsInfo.elastix_given) {
            matrix = createMatrixFromElastixFile<Dimension,PixelType>(m_ArgsInfo.elastix_arg);
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
    } else if(m_ArgsInfo.transform_grid_flag) {
      typename itk::Matrix<double, Dimension+1, Dimension+1> invMatrix( matrix.GetInverse() );
      typename itk::Matrix<double, Dimension, Dimension> invRotMatrix( clitk::GetRotationalPartMatrix(invMatrix) );
      typename itk::Vector<double,Dimension> invTrans =  clitk::GetTranslationPartMatrix(invMatrix);
      
      // Display warning
      if (m_ArgsInfo.spacing_given)
        std::cout << "Warning --spacing ignored (because --transform_grid_flag)" << std::endl;
      if (m_ArgsInfo.origin_given)
        std::cout << "Warning --origin ignored (because --transform_grid_flag)" << std::endl;

      // Spacing is influenced by affine transform matrix and input direction
      typename InputImageType::SpacingType outputSpacing;
      outputSpacing = invRotMatrix *
        input->GetDirection() *
        input->GetSpacing();

      // Origin is influenced by translation but not by input direction
      typename InputImageType::PointType outputOrigin;
      outputOrigin = invRotMatrix *
        input->GetOrigin() +
        invTrans;

      // Size is influenced by affine transform matrix and input direction
      // Size is converted to double, transformed and converted back to size type.
      vnl_vector<double> vnlOutputSize(Dimension);
      for(unsigned int i=0; i< Dimension; i++) {
        vnlOutputSize[i] = input->GetLargestPossibleRegion().GetSize()[i];
      }
      vnlOutputSize = invRotMatrix *
        input->GetDirection().GetVnlMatrix() *
        vnlOutputSize;
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

      //Origin
      typename OutputImageType::PointType outputOrigin;
      if (m_ArgsInfo.origin_given) {
        for(unsigned int i=0; i< Dimension; i++)
          outputOrigin[i]=m_ArgsInfo.origin_arg[i];
      } else outputOrigin=input->GetOrigin();

      // Set
      resampler->SetSize( outputSize );
      resampler->SetOutputSpacing( outputSpacing );
      resampler->SetOutputOrigin(  outputOrigin );

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
    }

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

    //Filter
    typedef  itk::VectorResampleImageFilter< InputImageType,OutputImageType, double >  ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    // Matrix
    typename itk::Matrix<double, Dimension+1, Dimension+1> matrix;
    if (m_ArgsInfo.rotate_given || m_ArgsInfo.translate_given)
      {
        if (m_ArgsInfo.matrix_given)
          {
            std::cerr << "You must use either rotate/translate or matrix options" << std::cout;
            return;
          }
        itk::Array<double> transformParameters(2 * Dimension);
        transformParameters.Fill(0.0);
        if (m_ArgsInfo.rotate_given)
          {
            if (Dimension == 2)
              transformParameters[0] = m_ArgsInfo.rotate_arg[0];
            else
              for (unsigned int i = 0; i < 3; i++)
                transformParameters[i] = m_ArgsInfo.rotate_arg[i];
          }
        if (m_ArgsInfo.translate_given)
          {
            int pos = 3;
            if (Dimension == 2)
              pos = 1;
            for (unsigned int i = 0; i < Dimension && i < 3; i++)
              transformParameters[pos++] = m_ArgsInfo.translate_arg[i];
          }
        if (Dimension == 4)
          {
            matrix.SetIdentity();
            itk::Matrix<double, 4, 4> tmp = GetForwardAffineMatrix3D(transformParameters);
            for (unsigned int i = 0; i < 3; ++i)
              for (unsigned int j = 0; j < 3; ++j)
                matrix[i][j] = tmp[i][j];
            for (unsigned int i = 0; i < 3; ++i)
              matrix[i][4] = tmp[i][3];
          }
        else
          matrix = GetForwardAffineMatrix<Dimension>(transformParameters);
      }
    else
      {
        if (m_ArgsInfo.matrix_given)
          {
            matrix= clitk::ReadMatrix<Dimension>(m_ArgsInfo.matrix_arg);
            if (m_Verbose) std::cout << "Reading the matrix..." << std::endl;
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
      std::cout<<"Setting the spacing to "<<outputSpacing<<"..."<<std::endl;

      //Origin
      typename OutputImageType::PointType outputOrigin;
      if (m_ArgsInfo.origin_given) {
        for(unsigned int i=0; i< Dimension; i++)
          outputOrigin[i]=m_ArgsInfo.origin_arg[i];
      } else outputOrigin=input->GetOrigin();
      std::cout<<"Setting the origin to "<<outputOrigin<<"..."<<std::endl;

      // Set
      resampler->SetSize( outputSize );
      resampler->SetOutputSpacing( outputSpacing );
      resampler->SetOutputOrigin(  outputOrigin );

    }

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
  
  
  //-------------------------------------------------------------------
  template<class args_info_type>
  template<unsigned int Dimension, class PixelType>
  typename itk::Matrix<double, Dimension+1, Dimension+1>
   AffineTransformGenericFilter<args_info_type>::createMatrixFromElastixFile(std::string filename)
  {
    if (Dimension != 3) {
      FATAL("Only 3D yet" << std::endl);
    }
    typename itk::Matrix<double, Dimension+1, Dimension+1> matrix;

    // Open file
    std::ifstream is;
    clitk::openFileForReading(is, filename);

    // Check Transform
    std::string s; 
    bool b = GetElastixValueFromTag(is, "Transform ", s);
    if (!b) {
      FATAL("Error must read 'Transform' in " << filename << std::endl);
    }
    if (s != "EulerTransform") {
      FATAL("Sorry only 'EulerTransform'" << std::endl);
    }

    // FIXME check
    //    (InitialTransformParametersFileName "NoInitialTransform")

    // Get CenterOfRotationPoint
    GetElastixValueFromTag(is, "CenterOfRotationPoint ", s); // space is needed
    if (!b) {
      FATAL("Error must read 'CenterOfRotationPoint' in " << filename << std::endl);
    }
    std::vector<std::string> cor; 
    GetValuesFromValue(s, cor);

    // Get Transformparameters
    GetElastixValueFromTag(is, "TransformParameters ", s); // space is needed
    if (!b) {
      FATAL("Error must read 'TransformParameters' in " << filename << std::endl);
    }
    std::vector<std::string> results; 
    GetValuesFromValue(s, results);
    
    // construct a stream from the string
    itk::CenteredEuler3DTransform<double>::Pointer mat = itk::CenteredEuler3DTransform<double>::New();
    itk::CenteredEuler3DTransform<double>::ParametersType p;
    p.SetSize(9);
    for(uint i=0; i<3; i++)
      p[i] = atof(results[i].c_str()); // Rotation
    for(uint i=0; i<3; i++)
      p[i+3] = atof(cor[i].c_str()); // Centre of rotation
    for(uint i=0; i<3; i++)
      p[i+6] = atof(results[i+3].c_str()); // Translation
    mat->SetParameters(p);
    
    if (m_Verbose) {
      std::cout << "Rotation      (deg) : " << rad2deg(p[0]) << " " << rad2deg(p[1]) << " " << rad2deg(p[2]) << std::endl;
      std::cout << "Translation   (phy) : " << p[3] << " " << p[4] << " " << p[5] << std::endl;
      std::cout << "Center of rot (phy) : " << p[6] << " " << p[7] << " " << p[8] << std::endl;
    }

    for(uint i=0; i<3; i++)
      for(uint j=0; j<3; j++)
        matrix[i][j] = mat->GetMatrix()[i][j];
    // Offset is -Rc + t + c
    matrix[0][3] = mat->GetOffset()[0];
    matrix[1][3] = mat->GetOffset()[1];
    matrix[2][3] = mat->GetOffset()[2];
    matrix[3][3] = 1;
    
    return matrix;
  }

  //-------------------------------------------------------------------
  template<class args_info_type>
  bool
  AffineTransformGenericFilter<args_info_type>::GetElastixValueFromTag(std::ifstream & is, 
                                                                       std::string tag, 
                                                                       std::string & value)
  {
    std::string line;
    is.seekg (0, is.beg);
    while(std::getline(is, line))   {
      unsigned pos = line.find(tag);
      if (pos<line.size()) {
        value=line.substr(pos+tag.size(),line.size()-2);// remove last ')'
        value.erase (std::remove (value.begin(), value.end(), '"'), value.end());
        value.erase (std::remove (value.begin(), value.end(), ')'), value.end());
        return true;
      }
   }
    return false;
  }
  //-------------------------------------------------------------------


  //-------------------------------------------------------------------
  template<class args_info_type>
  void
  AffineTransformGenericFilter<args_info_type>::GetValuesFromValue(const std::string & s, 
                                                                   std::vector<std::string> & values)
  {
    std::stringstream strstr(s);
    std::istream_iterator<std::string> it(strstr);
    std::istream_iterator<std::string> end;
    std::vector<std::string> results(it, end);
    values.clear();
    values.resize(results.size());
    for(uint i=0; i<results.size(); i++) values[i] = results[i];
  }
  //-------------------------------------------------------------------


} //end clitk

#endif //#define clitkAffineTransformGenericFilter_txx
