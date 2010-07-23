/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
#ifndef clitkVectorImageToImageGenericFilter_txx
#define clitkVectorImageToImageGenericFilter_txx

/* =================================================
 * @file   clitkVectorImageToImageGenericFilter.txx
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
  VectorImageToImageGenericFilter::UpdateWithDim(std::string PixelType, unsigned int Components)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

    if (Components==3)
      {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and 3D float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, itk::Vector<float, 3> >();
      }
    else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  VectorImageToImageGenericFilter::UpdateWithDimAndPixelType()
  {
    // ImageTypes
    typedef itk::Image<PixelType, Dimension> InputImageType;
    typedef typename PixelType::ComponentType ComponentType;
    typedef itk::Image<ComponentType, Dimension> OutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();

    // Filter
    typedef clitk::VectorImageToImageFilter<InputImageType, OutputImageType> FilterType;
    typename FilterType::Pointer filter=FilterType::New();
    filter->SetInput(input);
    filter->SetComponentIndex(m_ArgsInfo.componentIndex_arg);
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
 
#endif //#define clitkVectorImageToImageGenericFilter_txx
