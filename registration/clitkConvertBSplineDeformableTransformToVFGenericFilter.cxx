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
#ifndef clitkConvertBSplineDeformableTransformToVFGenericFilter_cxx
#define clitkConvertBSplineDeformableTransformToVFGenericFilter_cxx

/* =================================================
 * @file   clitkConvertBSplineDeformableTransformToVFGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkConvertBSplineDeformableTransformToVFGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  ConvertBSplineDeformableTransformToVFGenericFilter::ConvertBSplineDeformableTransformToVFGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }

  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<>
  void 
  ConvertBSplineDeformableTransformToVFGenericFilter::UpdateWithDim<3>(std::string PixelType, int Components)
  {
    // Components
    if (Components !=3)
      {
	std::cerr<<"Number of components is "<<Components<<"! Only 3 components is supported."<<std::endl;
	return;
      }
    if (PixelType != "double")
      {
	std::cerr<<"PixelType is  "<<PixelType<<"! Only double coefficient images are supported."<<std::endl;
	std::cerr<<"Reading image as double..."<<std::endl;
      }
  
    // ImageTypes
    const unsigned int Dimension=3;
    typedef itk::Vector<double, Dimension> InputPixelType;
    typedef itk::Vector<float, Dimension> OutputPixelType;
    typedef itk::Image<InputPixelType, Dimension> InputImageType;
    typedef itk::Image<OutputPixelType, Dimension> OutputImageType;
  
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    InputImageType::Pointer input= reader->GetOutput();


    // -----------------------------------------------
    // Filter
    // -----------------------------------------------
    typedef itk::TransformToDeformationFieldSource<OutputImageType, double> ConvertorType;
    ConvertorType::Pointer filter= ConvertorType::New();

    //Output image info
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
	unsigned int i=0;
	if(m_ArgsInfo.origin_given)
	  {
	    OutputImageType::PointType origin;
	    for(i=0;i<Dimension;i++)
	      origin[i]=m_ArgsInfo.origin_arg[i];
	    filter->SetOutputOrigin(origin);
	  }
	if (m_ArgsInfo.spacing_given)
	  {
	    OutputImageType::SpacingType spacing;
	    for(i=0;i<Dimension;i++)
	      spacing[i]=m_ArgsInfo.spacing_arg[i];
	    filter->SetOutputSpacing(spacing);
	  }
	if (m_ArgsInfo.spacing_given)
	  {
	    OutputImageType::SizeType size;
	    for(i=0;i<Dimension;i++)
	      size[i]=m_ArgsInfo.size_arg[i];
	    filter->SetOutputSize(size);
	  }
      }

    if (m_Verbose)
      {
	std::cout<< "Setting output origin to "<<filter->GetOutputOrigin()<<"..."<<std::endl;
	std::cout<< "Setting output spacing to "<<filter->GetOutputSpacing()<<"..."<<std::endl;
	std::cout<< "Setting output size to "<<filter->GetOutputSize()<<"..."<<std::endl;
      }


    // -----------------------------------------------
    // Transform
    // -----------------------------------------------
    typedef clitk::BSplineDeformableTransform< double, Dimension, Dimension> TransformType; 
    TransformType::Pointer transform=TransformType::New();

    // Spline orders:  Default is cubic splines
    InputImageType::RegionType::SizeType splineOrders ;
    splineOrders.Fill(3);
    if (m_ArgsInfo.order_given)
      for(unsigned int i=0; i<Dimension;i++) 
	splineOrders[i]=m_ArgsInfo.order_arg[i];
    if (m_Verbose) std::cout<<"Setting the spline orders  to "<<splineOrders<<"..."<<std::endl;

    // Mask
    typedef itk::ImageMaskSpatialObject<  Dimension >   MaskType;
    MaskType::Pointer  spatialObjectMask=NULL;
    if (m_ArgsInfo.mask_given)
      {
	typedef itk::Image< unsigned char, Dimension >   ImageMaskType;
	typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
	MaskReaderType::Pointer  maskReader = MaskReaderType::New();
	maskReader->SetFileName(m_ArgsInfo.mask_arg);
	
	try 
	  { 
	    maskReader->Update(); 
	  } 
	catch( itk::ExceptionObject & err ) 
	  { 
	    std::cerr << "ExceptionObject caught while reading mask !" << std::endl; 
	    std::cerr << err << std::endl; 
	    return;
	  } 
	if (m_Verbose)std::cout <<"Mask was read..." <<std::endl;
	
	// Set the image to the spatialObject
	spatialObjectMask = MaskType::New();
	spatialObjectMask->SetImage( maskReader->GetOutput() );
      }


    // Samplingfactors
    InputImageType::SizeType samplingFactors; 
    for (unsigned int i=0; i< Dimension; i++)
      {   
	samplingFactors[i]= (int) ( input->GetSpacing()[i]/ filter->GetOutputSpacing()[i]);
	if (m_Verbose) std::cout<<"Setting sampling factor "<<i<<" to "<<samplingFactors[i]<<"..."<<std::endl;
      }


    // Set
    transform->SetSplineOrders(splineOrders);
    transform->SetMask(spatialObjectMask);
    transform->SetLUTSamplingFactors(samplingFactors);
    transform->SetCoefficientImage(input);
    filter->SetTransform(transform);


    // -----------------------------------------------
    // Update
    // -----------------------------------------------
    if (m_Verbose)std::cout<< "Converting the BSpline transform..."<<std::endl;
    try
      {
	filter->Update();
      }
    catch (itk::ExceptionObject)
      {
	std::cerr<<"Error: Exception thrown during execution convertion filter!"<<std::endl;
      }

    OutputImageType::Pointer output=filter->GetOutput();


    // -----------------------------------------------
    // Output
    // -----------------------------------------------
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }


  //-------------------------------------------------------------------
  // Update with the number of dimensions
  //-------------------------------------------------------------------
  template<>
  void 
  ConvertBSplineDeformableTransformToVFGenericFilter::UpdateWithDim<4>(std::string PixelType, int Components)
  {
    // Components
    if (Components !=3)
      {
	std::cerr<<"Number of components is "<<Components<<"! Only 3 components is supported."<<std::endl;
	return;
      }
    if (PixelType != "double")
      {
	std::cerr<<"PixelType is  "<<PixelType<<"! Only double coefficient images are supported."<<std::endl;
	std::cerr<<"Reading image as double..."<<std::endl;
      }
  
    // ImageTypes
    const unsigned int Dimension=4;
    const unsigned int SpaceDimension=3;
    typedef itk::Vector<double, SpaceDimension> InputPixelType;
    typedef itk::Vector<float, SpaceDimension> OutputPixelType;
    typedef itk::Image<InputPixelType, Dimension> InputImageType;
    typedef itk::Image<OutputPixelType, Dimension> OutputImageType;
  
    // Read the input
    typedef itk::ImageFileReader<InputImageType> InputReaderType;
    InputReaderType::Pointer reader = InputReaderType::New();
    reader->SetFileName( m_InputFileName);
    reader->Update();
    InputImageType::Pointer input= reader->GetOutput();


    // -----------------------------------------------
    // Filter
    // -----------------------------------------------
    typedef clitk::TransformToDeformationFieldSource<OutputImageType, double> ConvertorType;
    ConvertorType::Pointer filter= ConvertorType::New();

    //Output image info
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
	unsigned int i=0;
	if(m_ArgsInfo.origin_given)
	  {
	    OutputImageType::PointType origin;
	    for(i=0;i<Dimension;i++)
	      origin[i]=m_ArgsInfo.origin_arg[i];
	    filter->SetOutputOrigin(origin);
	  }
	if (m_ArgsInfo.spacing_given)
	  {
	    OutputImageType::SpacingType spacing;
	    for(i=0;i<Dimension;i++)
	      spacing[i]=m_ArgsInfo.spacing_arg[i];
	    filter->SetOutputSpacing(spacing);
	  }
	if (m_ArgsInfo.spacing_given)
	  {
	    OutputImageType::SizeType size;
	    for(i=0;i<Dimension;i++)
	      size[i]=m_ArgsInfo.size_arg[i];
	    filter->SetOutputSize(size);
	  }
      }
    //Output image info
    if (m_Verbose)
      {
	std::cout<< "Setting output origin to "<<filter->GetOutputOrigin()<<"..."<<std::endl;
	std::cout<< "Setting output spacing to "<<filter->GetOutputSpacing()<<"..."<<std::endl;
	std::cout<< "Setting output size to "<<filter->GetOutputSize()<<"..."<<std::endl;
      }


    // -----------------------------------------------
    // Transform
    // -----------------------------------------------
    typedef clitk::ShapedBLUTSpatioTemporalDeformableTransform< double, Dimension, Dimension > TransformType; 
    TransformType::Pointer transform=TransformType::New();
    transform->SetTransformShape(m_ArgsInfo.shape_arg);

    // Spline orders:  Default is cubic splines
    InputImageType::RegionType::SizeType splineOrders ;
    splineOrders.Fill(3);
    if (m_ArgsInfo.order_given)
      for(unsigned int i=0; i<Dimension;i++) 
	splineOrders[i]=m_ArgsInfo.order_arg[i];
    if (m_Verbose) std::cout<<"Setting the spline orders  to "<<splineOrders<<"..."<<std::endl;

    // Mask
    typedef itk::ImageMaskSpatialObject<  Dimension >   MaskType;
    MaskType::Pointer  spatialObjectMask=NULL;
    if (m_ArgsInfo.mask_given)
      {
	typedef itk::Image< unsigned char, Dimension >   ImageMaskType;
	typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
	MaskReaderType::Pointer  maskReader = MaskReaderType::New();
	maskReader->SetFileName(m_ArgsInfo.mask_arg);
	
	try 
	  { 
	    maskReader->Update(); 
	  } 
	catch( itk::ExceptionObject & err ) 
	  { 
	    std::cerr << "ExceptionObject caught while reading mask !" << std::endl; 
	    std::cerr << err << std::endl; 
	    return;
	  } 
	if (m_Verbose)std::cout <<"Mask was read..." <<std::endl;
	
	// Set the image to the spatialObject
	spatialObjectMask = MaskType::New();
	spatialObjectMask->SetImage( maskReader->GetOutput() );
      }


    // Samplingfactors
    InputImageType::SizeType samplingFactors; 
    for (unsigned int i=0; i< Dimension; i++)
      {   
	samplingFactors[i]= (int) ( input->GetSpacing()[i]/ filter->GetOutputSpacing()[i]);
	if (m_Verbose) std::cout<<"Setting sampling factor "<<i<<" to "<<samplingFactors[i]<<"..."<<std::endl;
      }
    if( !(m_ArgsInfo.shape_arg%2) )samplingFactors[Dimension-1]=5;

    // Set
    transform->SetSplineOrders(splineOrders);
    transform->SetMask(spatialObjectMask);
    transform->SetLUTSamplingFactors(samplingFactors);
    transform->SetCoefficientImage(input);
    filter->SetTransform(transform);


    // -----------------------------------------------
    // Update
    // -----------------------------------------------
    if (m_Verbose)std::cout<< "Converting the BSpline transform..."<<std::endl;
    try
      {
	filter->Update();
      }
    catch (itk::ExceptionObject)
      {
	std::cerr<<"Error: Exception thrown during execution convertion filter!"<<std::endl;
      }

    OutputImageType::Pointer output=filter->GetOutput();


    // -----------------------------------------------
    // Output
    // -----------------------------------------------
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_ArgsInfo.output_arg);
    writer->SetInput(output);
    writer->Update();

  }



  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void ConvertBSplineDeformableTransformToVFGenericFilter::Update()
  {
  
    // Read the Dimension and PixelType
    int Dimension, Components;
    std::string PixelType;
    ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType, Components);

    // Call UpdateWithDim
    //if(Dimension==2) UpdateWithDim<2>(PixelType, Components);
    if(Dimension==3) UpdateWithDim<3>(PixelType, Components);
    else if (Dimension==4) UpdateWithDim<4>(PixelType, Components); 
    else 
      {
	std::cout<<"Error, Only for 3 Dimensions!!!"<<std::endl ;
	return;
      }
  }

} //end clitk

#endif  //#define clitkConvertBSplineDeformableTransformToVFGenericFilter_cxx
