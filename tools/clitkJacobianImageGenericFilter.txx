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
#ifndef clitkJacobianImageGenericFilter_txx
#define clitkJacobianImageGenericFilter_txx

/* =================================================
 * @file   clitkJacobianImageGenericFilter.txx
 * @author
 * @date
 *
 * @brief
 *
 ===================================================*/

#include "clitkImageCommon.h"
#include "itkDeformationFieldJacobianDeterminantFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVector.h"
#include "itkNormalizeImageFilter.h"

namespace clitk
{

//-----------------------------------------------------------
// Constructor
//-----------------------------------------------------------
template<class args_info_type>
JacobianImageGenericFilter<args_info_type>::JacobianImageGenericFilter()
{
  m_Verbose=false;
  m_InputFileName="";
}


//-----------------------------------------------------------
// Update
//-----------------------------------------------------------
template<class args_info_type>
void JacobianImageGenericFilter<args_info_type>::Update()
{
  // Read the Dimension and PixelType
  int Dimension;
  std::string PixelType;
  ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);


  // Call UpdateWithDim
  if(Dimension==2) UpdateWithDim<2>(PixelType);
  else if(Dimension==3) UpdateWithDim<3>(PixelType);
  // else if (Dimension==4)UpdateWithDim<4>(PixelType);
  else {
    std::cout<<"Error, Only for 2 or 3  Dimensions!!!"<<std::endl ;
    return;
  }
}

//-------------------------------------------------------------------
// Update with the number of dimensions
//-------------------------------------------------------------------
template<class args_info_type>
template<unsigned int Dimension>
void
JacobianImageGenericFilter<args_info_type>::UpdateWithDim(std::string PixelType)
{
  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

  //    if(PixelType == "short"){
  //       if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
  //       UpdateWithDimAndPixelType<Dimension, signed short>();
  //     }
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
  //  else {
  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
  UpdateWithDimAndPixelType<Dimension, itk::Vector<float, Dimension> >();
  // }
}


//-------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//-------------------------------------------------------------------
template<class args_info_type>
template <unsigned int Dimension, class  PixelType>
void
JacobianImageGenericFilter<args_info_type>::UpdateWithDimAndPixelType()
{
  std::string vfield_file = m_ArgsInfo.input_arg;
  
  const unsigned int dim = Dimension;
  typedef itk::Vector<double, dim> VectorType;
  typedef itk::Image<VectorType, dim> VectorFieldType;
  typedef itk::ImageFileReader<VectorFieldType> VectorFieldReaderType;
  
  typename VectorFieldReaderType::Pointer vfield_reader = VectorFieldReaderType::New();
  vfield_reader->SetFileName(vfield_file.c_str());
  
  typedef double OutputPixelType;
  typedef itk::Image<OutputPixelType, dim> ImageType;
  //typedef itk::DeformationFieldJacobianDeterminantFilter<VectorFieldType, PixelType, ImageType> JacobianFilterType;
  typedef itk::DisplacementFieldJacobianDeterminantFilter<VectorFieldType, OutputPixelType, ImageType> JacobianFilterType;

  typename VectorFieldType::Pointer vfield = vfield_reader->GetOutput();
  typename JacobianFilterType::Pointer jac = JacobianFilterType::New();
  jac->SetInput(vfield);
  jac->Update();
  
  typedef itk::NormalizeImageFilter<ImageType, ImageType> NormalizeFilterType;
  typename NormalizeFilterType::Pointer normalize = NormalizeFilterType::New();
  normalize->SetInput(jac->GetOutput());
  normalize->Update();

  typedef itk::ImageFileWriter<ImageType> ImageFileWriterType;
  typename ImageFileWriterType::Pointer image_writer = ImageFileWriterType::New();
  
  std::string image_file = m_ArgsInfo.output_arg;
  image_writer->SetFileName(image_file.c_str());
  image_writer->SetInput(jac->GetOutput());
  image_writer->Update();
  
}


}//end clitk

#endif //#define clitkJacobianImageGenericFilter_txx
