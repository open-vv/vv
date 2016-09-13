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
#ifndef clitkNVectorImageTo4DImageGenericFilter_txx
#define clitkNVectorImageTo4DImageGenericFilter_txx

/* =================================================
 * @file   clitkNVectorImageTo4DImageGenericFilter.txx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "itkVectorImageToImageAdaptor.h"
#include <sstream>

namespace clitk
{

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<unsigned int Dimension>
  void 
  NVectorImageTo4DImageGenericFilter::UpdateWithDim(std::string PixelType, unsigned int Components)
  {
    if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<< PixelType<<"..."<<std::endl;

    if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and 3D float..." << std::endl;
    UpdateWithDimAndPixelType<Dimension, float >();
  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions and the pixeltype
  //-------------------------------------------------------------------
  template <unsigned int Dimension, class  PixelType> 
  void 
  NVectorImageTo4DImageGenericFilter::UpdateWithDimAndPixelType()
  {
    // ImageTypes
    typedef itk::VectorImage<PixelType, Dimension> InputImageType;
    typedef itk::Image<PixelType, Dimension+1> OutputImageType;
    
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    typename InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input= reader->GetOutput();
    
    //Filter
    typedef itk::VectorImageToImageAdaptor<PixelType, Dimension> ImageAdaptorType;
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typename ImageAdaptorType::Pointer adaptor = ImageAdaptorType::New();
    typename OutputImageType::Pointer output = OutputImageType::New();
    typename WriterType::Pointer writer = WriterType::New();

    adaptor->SetExtractComponentIndex(0);
    adaptor->SetImage(input);
    std::string fileName=m_ArgsInfo.output_arg;
    
    //Create the output
    typename OutputImageType::IndexType index;
    index.Fill(0);
    typename OutputImageType::SizeType size;
    for (unsigned int pixelDim=0; pixelDim<input->GetNumberOfComponentsPerPixel(); ++pixelDim)
    {
      size[pixelDim]=input->GetLargestPossibleRegion().GetSize(pixelDim);
    }
    size[Dimension]=input->GetNumberOfComponentsPerPixel();
    typename OutputImageType::RegionType region;
    region.SetSize(size);
    region.SetIndex(index);    
    output->SetRegions(region);
    output->Allocate();
    writer->SetInput(output);
    
    //Copy each channel
    for (unsigned int pixelDim=0; pixelDim<input->GetNumberOfComponentsPerPixel(); ++pixelDim)
    {
      adaptor->SetExtractComponentIndex(pixelDim);
      
      itk::ImageRegionIterator<InputImageType> imageIterator(input,input->GetLargestPossibleRegion());

      while(!imageIterator.IsAtEnd())
      {
        typename OutputImageType::IndexType indexVector;
        indexVector.Fill(0);
        for (unsigned int indexDim=0; indexDim<Dimension; ++indexDim)
        {
          indexVector[indexDim]=imageIterator.GetIndex().GetElement(indexDim);
        }
        indexVector[Dimension]=pixelDim;
        
        output->SetPixel(indexVector, adaptor->GetPixel(imageIterator.GetIndex()));
        ++imageIterator;
      }
    }
    // Output
    writer->SetFileName(fileName);
    writer->Update();
  }
}//end clitk
 
#endif //#define clitkNVectorImageTo4DImageGenericFilter_txx
