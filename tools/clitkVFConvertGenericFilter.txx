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
#ifndef clitkVFConvertGenericFilter_txx
#define clitkVFConvertGenericFilter_txx

/* =================================================
 * @file   clitkVFConvertGenericFilter.txx
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
  VFConvertGenericFilter::UpdateWithDim(std::string PixelType, unsigned int Components)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D with "<< Components <<" components of "<< PixelType<<"..."<<std::endl;

    if (Components==2)
      {
	if (PixelType == "double"){ 
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D with "<< Components <<" components of double..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<double, 2> >();
	}
	else {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D with "<< Components <<" components of float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<float, 2> >();
	}
      }
    else if (Components==3)
      {
	if (PixelType == "double"){ 
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D with "<< Components <<" components of double..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<double, 3> >();
	}
	else {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D with "<< Components <<" components of float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<float, 3> >();
	}
      }
    else if (Components==4)
      {
	if (PixelType == "double"){ 
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D with "<< Components <<" components of double..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<double, 4> >();
	}
	else {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D with "<< Components <<" components of float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<float, 4> >();
	}
      }
    else std::cerr<<"Only 2,3 and 4 components!"<<std::endl;
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  VFConvertGenericFilter::UpdateWithDimAndPixelType()
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

    // Output
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(input);
    writer->Update();

  }


}//end clitk
 
#endif //#define clitkVFConvertGenericFilter_txx
