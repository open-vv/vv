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
#ifndef clitkMatrixTransformToVFGenericFilter_txx
#define clitkMatrixTransformToVFGenericFilter_txx

/* =================================================
 * @file   clitkMatrixTransformToVFGenericFilter.txx
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
  MatrixTransformToVFGenericFilter::UpdateWithDim()
  {
 //    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

//     if(PixelType == "short"){  
//       if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
//       UpdateWithDimAndPixelType<Dimension, signed short>(); 
//     }
//     //    else if(PixelType == "unsigned_short"){  
//     //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
//     //       UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
//     //     }
    
//     else if (PixelType == "unsigned_char"){ 
//       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
//       UpdateWithDimAndPixelType<Dimension, unsigned char>();
//     }
    
//     //     else if (PixelType == "char"){ 
//     //       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
//     //       UpdateWithDimAndPixelType<Dimension, signed char>();
//     //     }
//     else {
//       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
//       UpdateWithDimAndPixelType<Dimension, float>();
//     }
//   }


//   //-------------------------------------------------------------------
//   // Update with the number of dimensions and the pixeltype
//   //-------------------------------------------------------------------
//   template <unsigned int Dimension, class  PixelType> 
//   void 
//   MatrixTransformToVFGenericFilter::UpdateWithDimAndPixelType()
//   {

    // ImageTypes
    typedef itk::Vector<float,Dimension> Displacement;
    typedef itk::Image<Displacement, Dimension> OutputImageType;
    
    // Filter
#if ITK_VERSION_MAJOR >= 4
#  if ITK_VERSION_MINOR < 6
    typedef itk::TransformToDisplacementFieldSource<OutputImageType, double> ConvertorType;
#  else
    typedef itk::TransformToDisplacementFieldFilter<OutputImageType, double> ConvertorType;
#  endif
#endif

    typename   ConvertorType::Pointer filter= ConvertorType::New();

    // Output image info
    if (m_ArgsInfo.like_given)
      {
	typedef itk::ImageFileReader<OutputImageType> ReaderType;
	typename ReaderType::Pointer reader2=ReaderType::New();
	reader2->SetFileName(m_ArgsInfo.like_arg);
	reader2->Update();

	typename OutputImageType::Pointer image=reader2->GetOutput();
#if ITK_VERSION_MAJOR > 4 || (ITK_VERSION_MAJOR == 4 && ITK_VERSION_MINOR >= 6)
    filter->SetReferenceImage(image);
    filter->UseReferenceImageOn();
#else
    filter->SetOutputParametersFromImage(image);
#endif
      }
    else
      {
	unsigned int i=0;
	if(m_ArgsInfo.origin_given)
	  {
	    typename OutputImageType::PointType origin;
	    for(i=0;i<Dimension;i++)
	      origin[i]=m_ArgsInfo.origin_arg[i];
	    filter->SetOutputOrigin(origin);
	  }
	if (m_ArgsInfo.spacing_given)
	  {
	    typename OutputImageType::SpacingType spacing;
	    for(i=0;i<Dimension;i++)
	      spacing[i]=m_ArgsInfo.spacing_arg[i];
	    filter->SetOutputSpacing(spacing);
	  }
	if (m_ArgsInfo.size_given)
	  {
	    typename OutputImageType::SizeType size;
	    for(i=0;i<Dimension;i++)
	      size[i]=m_ArgsInfo.size_arg[i];
#if ITK_VERSION_MAJOR > 4 || (ITK_VERSION_MAJOR == 4 && ITK_VERSION_MINOR >= 6)
        filter->SetSize(size);
#else
        filter->SetOutputSize(size);
#endif
      }
      }
    
    // Transform
    typedef itk::AffineTransform<double, Dimension> TransformType;
    typename TransformType::Pointer transform =TransformType::New();
    itk::Matrix<double, Dimension+1, Dimension+1> homMatrix= ReadMatrix<Dimension>( m_ArgsInfo.matrix_arg);
    itk::Matrix<double, Dimension, Dimension> matrix =GetRotationalPartMatrix( homMatrix);
    itk::Vector<double, Dimension> offset= GetTranslationPartMatrix( homMatrix);
    transform->SetMatrix(matrix);
    transform->SetOffset(offset);
    filter->SetTransform(transform);
    filter->Update();
    typename OutputImageType::Pointer output=filter->GetOutput();

    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkMatrixTransformToVFGenericFilter_txx
