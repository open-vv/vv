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
#ifndef CLITKCONEBEAMPROJECTIMAGEGENERICFILTER_CXX
#define CLITKCONEBEAMPROJECTIMAGEGENERICFILTER_CXX

/**
 * @file   clitkConeBeamProjectImageGenericFilter.cxx
 * @author Jef Vandemeulebroucke <jef@creatis.insa-lyon.fr>
 * @date   Wed April 30  13:47:57 2008
 * 
 * @brief  
 * 
 * 
 */

#include "clitkConeBeamProjectImageGenericFilter.h"

namespace clitk
{

  //====================================================================
  // Constructor
  ConeBeamProjectImageGenericFilter::ConeBeamProjectImageGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }

  //====================================================================
  // Update
  void ConeBeamProjectImageGenericFilter::Update()
  {
    //Read the PixelType and dimension of the input image
    int Dimension;
    std::string PixelType;
    clitk::ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType);
    if (Dimension == 3)
      {
	if (m_Verbose) std::cout  << "Input was detected to be "<< Dimension << "D and " << PixelType << "..." << std::endl;
  
	if(PixelType == "short"){  
	  if (m_Verbose) std::cout  << "Launching Projection in signed short..." << std::endl;
	  UpdateWithPixelType<signed short>(); 
	}
	
// 	else if(PixelType == "unsigned_short"){  
// 	  if (m_Verbose) std::cout  << "Launching Projection in unsigned_short..." << std::endl;
// 	  UpdateWithPixelType<unsigned short>(); 
// 	}
	
	else if (PixelType == "unsigned_char"){ 
	  if (m_Verbose) std::cout  << "Launching Projection in unsigned_char..." << std::endl;
	  UpdateWithPixelType<unsigned char>();
	}
	
// 	else if (PixelType == "char"){ 
// 	  if (m_Verbose) std::cout  << "Launching Projection in signed_char..." << std::endl;
// 	  UpdateWithPixelType<signed char>();
// 	}
	else {
	  if (m_Verbose) std::cout  << "Launching Projection in float..." << std::endl;
	  UpdateWithPixelType<float>();
	}
      }
    
    else {itkExceptionMacro(<< "Input Image dimension is " << Dimension 
			    << " but I can only work on 3D images.");
    }
  }
  
  //================================================================================
  template <class  PixelType> void ConeBeamProjectImageGenericFilter::UpdateWithPixelType()
  {
    
    //===================================================================
    // Read the input image 
    const unsigned int InputImageDimension=3;
    typedef itk::Image<PixelType, InputImageDimension> InputImageType;
    typedef itk::ImageFileReader<InputImageType> ImageReaderType;
    typename  ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(m_InputFileName);
    reader->Update();
    typename InputImageType::Pointer input = reader->GetOutput();

    // Define the output type
    //JV always float?
    typedef float OutputPixelType; 
    const unsigned int OutputImageDimension=2;
    typedef itk::Image<OutputPixelType, OutputImageDimension> OutputImageType;

    // Create the ConeBeamProjectImageFilter
    typedef clitk::ConeBeamProjectImageFilter<InputImageType, OutputImageType> ConeBeamProjectImageFilterType;
    typename ConeBeamProjectImageFilterType::Pointer filter=ConeBeamProjectImageFilterType::New();

    // Pass all the necessary parameters
    filter->SetInput(input);
    filter->SetVerbose(m_Verbose);
    if (m_ArgsInfo.threads_given) filter->SetNumberOfThreads(m_ArgsInfo.threads_arg);
    
    // Projection parameters
    typename InputImageType::PointType iso;
    unsigned int i;
    if (m_ArgsInfo.iso_given)
      for(i=0;i<InputImageDimension;i++)
	{
	  iso[i]=m_ArgsInfo.iso_arg[i];
	  filter->SetIsoCenter(iso);
	}
    filter->SetSourceToScreen(m_ArgsInfo.screen_arg);
    filter->SetSourceToAxis(m_ArgsInfo.axis_arg); 
    filter->SetProjectionAngle(m_ArgsInfo.angle_arg);
    if (m_ArgsInfo.matrix_given)
      {
	itk::Matrix<double,4,4> rt =ReadMatrix3D(m_ArgsInfo.matrix_arg);
        filter->SetRigidTransformMatrix(rt);
      }
    filter->SetEdgePaddingValue(static_cast<OutputPixelType>(m_ArgsInfo.pad_arg));

    // Output image info
    if (m_ArgsInfo.like_given)
      {
	typedef itk::ImageFileReader<OutputImageType> ReaderType;
	ReaderType::Pointer reader2=ReaderType::New();
	reader2->SetFileName(m_ArgsInfo.like_arg);
	reader2->Update();

	OutputImageType::Pointer image=reader2->GetOutput();
	filter->SetOutputParametersFromImage(image);
      }
    else
      {
	if (m_ArgsInfo.origin_given)
	  {
	    OutputImageType::PointType origin;
	    for(i=0;i<OutputImageDimension;i++)
	      origin[i]=m_ArgsInfo.origin_arg[i];
	    filter->SetOutputOrigin(origin);
	  }
	if (m_ArgsInfo.spacing_given)
	  {
	    OutputImageType::SpacingType spacing;
	    for(i=0;i<OutputImageDimension;i++)
	      spacing[i]=m_ArgsInfo.spacing_arg[i];
	    filter->SetOutputSpacing(spacing);
	  }
	if (m_ArgsInfo.size_given)
	  {
	    OutputImageType::SizeType size;
	    for(i=0;i<OutputImageDimension;i++)
	      size[i]=m_ArgsInfo.size_arg[i];
	    filter->SetOutputSize(size);
	  }
      }

    //Go
    filter->Update();
    
    //Get the output
    OutputImageType::Pointer output=filter->GetOutput();
 
    //Write the output
    typedef itk::ImageFileWriter<OutputImageType> OutputWriterType;
    OutputWriterType::Pointer outputWriter = OutputWriterType::New();
    outputWriter->SetInput(output);
    outputWriter->SetFileName(m_ArgsInfo.output_arg); 
    if (m_Verbose)std::cout<<"Writing projected image..."<<std::endl;
    outputWriter->Update(); 
  }


  //====================================================================

}
#endif  //#define CLITKCONEBEAMPROJECTIMAGEGENERICFILTER_CXX
