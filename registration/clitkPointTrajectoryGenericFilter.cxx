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
#ifndef clitkPointTrajectoryGenericFilter_cxx
#define clitkPointTrajectoryGenericFilter_cxx

/* =================================================
 * @file   clitkPointTrajectoryGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkPointTrajectoryGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  PointTrajectoryGenericFilter::PointTrajectoryGenericFilter()
  {
    m_Verbose=false;
    //    m_InputFileName="";
  }


  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void PointTrajectoryGenericFilter::Update()
  {
    // ImageTypes
    const unsigned int ImageDimension=4;
    const unsigned int SpaceDimension=3;
    typedef itk::Vector<double, SpaceDimension> CoefficientPixelType;
    typedef itk::Vector<float, SpaceDimension> VectorPixelType;
    typedef itk::Image<CoefficientPixelType, ImageDimension> CoefficientImageType;
    typedef itk::Image<VectorPixelType, ImageDimension> VectorFieldType;
  

    // -----------------------------------------------
    // Reference Point List 3D
    // -----------------------------------------------
    typedef itk::Point<double, SpaceDimension> SpacePointType;
    typedef clitk::List<SpacePointType> PointListType;
    PointListType referencePointList;
    if (m_Verbose) std::cout<<"Reference point list:"<<std::endl;
    referencePointList=PointListType(m_ArgsInfo.ref_arg, m_Verbose);


    // -----------------------------------------------
    // Transform: based on points, 4DVF, spatio-Temporal transform 
    // -----------------------------------------------
    typedef itk::Transform<double, 4, 4> TransformType;
    TransformType::Pointer transform;
    switch (m_ArgsInfo.transform_arg)
      {
	// ==========================
	// List of points
	// ==========================
      case 0: 
	{
	  //-----------------------------
	  // Input point lists
	  //-----------------------------   
	  typedef itk::Point<double, SpaceDimension> PointType;
	  typedef clitk::List<PointType> PointListType;
	  typedef clitk::Lists<PointType> PointListsType;
	  PointListsType inputPointLists, sortedPointLists;
	  
	  // Read the lists
	  for (unsigned int i=0; i<m_ArgsInfo.points_given; i++)
	    inputPointLists.push_back(PointListType(m_ArgsInfo.points_arg[i], m_Verbose) );
	
	  // Convert/sort the lists
	  sortedPointLists.resize(inputPointLists[0].size());
	  for (unsigned int i=0; i<inputPointLists[0].size(); i++)
	    {
	      sortedPointLists[i].push_back(referencePointList[i]);
	      for (unsigned int j=0; j<inputPointLists.size(); j++)
		{
		  sortedPointLists[i].push_back(inputPointLists[j][i]);
		}
	    }
					    
	  // Point list Transform
	  typedef clitk::PointListTransform<double, ImageDimension,ImageDimension> PointListTransformType;
	  PointListTransformType::Pointer pointListTransform=PointListTransformType::New();
	  pointListTransform->SetPointLists(sortedPointLists);
	  
	  // Vector Interpolator
	  typedef PointListTransformType::PointListImageType PointListImageType;
	  typedef clitk::GenericVectorInterpolator<args_info_clitkPointTrajectory,PointListImageType, double> GenericVectorInterpolatorType;
	  GenericVectorInterpolatorType::Pointer genericInterpolator=GenericVectorInterpolatorType::New();
	  genericInterpolator->SetArgsInfo(m_ArgsInfo);
	  typedef itk::VectorInterpolateImageFunction<PointListImageType, double> InterpolatorType; 
	  InterpolatorType::Pointer interpolator=genericInterpolator->GetInterpolatorPointer();
	  pointListTransform->SetInterpolator(interpolator);
	  transform=pointListTransform;

	  break;
	}

	// ==========================
	// 4D vector field
	// ==========================
      case 1: 
	{
	  // Deformation field transform
	  typedef clitk::DeformationFieldTransform<double, ImageDimension,ImageDimension, SpaceDimension> DeformationFieldTransformType;
	  DeformationFieldTransformType::Pointer deformationFieldTransform=DeformationFieldTransformType::New();
	  
	  // The deformation field
	  typedef DeformationFieldTransformType::DeformationFieldType DeformationFieldType;
	  typedef itk::ImageFileReader<DeformationFieldType> InputReaderType;
	  InputReaderType::Pointer reader = InputReaderType::New();
	  reader->SetFileName( m_ArgsInfo.input_arg);
	  reader->Update();
	  DeformationFieldType::Pointer input= reader->GetOutput();
	  deformationFieldTransform->SetDeformationField(input);
	  
	  // Vector Interpolator
	  typedef clitk::GenericVectorInterpolator<args_info_clitkPointTrajectory,DeformationFieldType, double> GenericVectorInterpolatorType;
	  GenericVectorInterpolatorType::Pointer genericInterpolator=GenericVectorInterpolatorType::New();
	  genericInterpolator->SetArgsInfo(m_ArgsInfo);
	  typedef itk::VectorInterpolateImageFunction<DeformationFieldType, double> InterpolatorType; 
	  InterpolatorType::Pointer interpolator=genericInterpolator->GetInterpolatorPointer();
	  deformationFieldTransform->SetInterpolator(interpolator);
	  transform=deformationFieldTransform;

	  break;
	}

	// ==========================	
	// Spatio-Temporal transform
	// ==========================
      case 2:
	{	
	  // S-T transform	
	  typedef clitk::ShapedBLUTSpatioTemporalDeformableTransform< double, ImageDimension, ImageDimension > TransformType; 
	  TransformType::Pointer spatioTemporalTransform = TransformType::New();


	  // Spline orders:  Default is cubic splines
	  CoefficientImageType::RegionType::SizeType splineOrders ;
	  splineOrders.Fill(3);
	  if (m_ArgsInfo.order_given)
	    for(unsigned int i=0; i<ImageDimension;i++) 
	      splineOrders[i]=m_ArgsInfo.order_arg[i];
	  if (m_Verbose) std::cout<<"Setting the spline orders  to "<<splineOrders<<"..."<<std::endl;
	  
	  // Coefficient image
	  typedef itk::ImageFileReader<CoefficientImageType> InputReaderType;
	  InputReaderType::Pointer reader = InputReaderType::New();
	  reader->SetFileName( m_ArgsInfo.input_arg);
	  reader->Update();
	  CoefficientImageType::Pointer input= reader->GetOutput();
	  // 	  itk::Vector<double,3> vector;
	  // 	  vector.Fill(0.);
	  // 	  vector[2]=100;
	  // 	  input->FillBuffer(vector);

	  // Mask
	  typedef itk::ImageMaskSpatialObject<  ImageDimension >   MaskType;
	  MaskType::Pointer  spatialObjectMask=NULL;
	  if (m_ArgsInfo.mask_given)
	    {
	      typedef itk::Image< unsigned char, ImageDimension >   ImageMaskType;
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
	  CoefficientImageType::SizeType samplingFactors; 
	  for (unsigned int i=0; i< ImageDimension-1; i++)
	    {   
	      samplingFactors[i]= (int) ( input->GetSpacing()[i]/ m_ArgsInfo.spacing_arg);
	      if (m_Verbose) std::cout<<"Setting sampling factor "<<i<<" to "<<samplingFactors[i]<<"..."<<std::endl;
	    }
	  samplingFactors[ImageDimension-1]= (int) ( input->GetSpacing()[ImageDimension-1]/ m_ArgsInfo.phaseIncrement_arg);
	  if (m_Verbose) std::cout<<"Setting sampling factor "<<ImageDimension-1<<" to "<<samplingFactors[ImageDimension-1]<<"..."<<std::endl;
	  if( (m_ArgsInfo.shape_arg==3) |
	      (m_ArgsInfo.shape_arg==4) |
	      (m_ArgsInfo.shape_arg==6) |
	      (m_ArgsInfo.shape_arg==8)
	      ) samplingFactors[ImageDimension-1]*=2.5;
	  
	  // Set
	  spatioTemporalTransform->SetTransformShape(m_ArgsInfo.shape_arg);
	  spatioTemporalTransform->SetSplineOrders(splineOrders);
	  spatioTemporalTransform->SetMask(spatialObjectMask);
	  spatioTemporalTransform->SetLUTSamplingFactors(samplingFactors);
	  spatioTemporalTransform->SetCoefficientImage(input);
	  transform=spatioTemporalTransform;
	  
	  break;
	}

// 	// ==========================	
// 	// Spatio-Temporal transform
// 	// ==========================
//       case 3:
// 	{	
// 	  // S-T transform	
// 	  typedef clitk::BSplineSpatioTemporalDeformableTransform< double, ImageDimension, ImageDimension > TransformType; 
// 	  TransformType::Pointer spatioTemporalTransform = TransformType::New();


// 	  // Spline orders:  Default is cubic splines
// 	  CoefficientImageType::RegionType::SizeType splineOrders ;
// 	  splineOrders.Fill(3);
// 	  if (m_ArgsInfo.order_given)
// 	    for(unsigned int i=0; i<ImageDimension;i++) 
// 	      splineOrders[i]=m_ArgsInfo.order_arg[i];
// 	  if (m_Verbose) std::cout<<"Setting the spline orders  to "<<splineOrders<<"..."<<std::endl;
	  
// 	  // Coefficient image
// 	  typedef itk::ImageFileReader<CoefficientImageType> InputReaderType;
// 	  InputReaderType::Pointer reader = InputReaderType::New();
// 	  reader->SetFileName( m_ArgsInfo.input_arg);
// 	  reader->Update();
// 	  CoefficientImageType::Pointer input= reader->GetOutput();
// 	  // 	  itk::Vector<double,3> vector;
// 	  // 	  vector.Fill(0.);
// 	  // 	  vector[2]=100;
// 	  // 	  input->FillBuffer(vector);
	  
// 	  // Mask
// 	  typedef itk::ImageMaskSpatialObject<  ImageDimension >   MaskType;
// 	  MaskType::Pointer  spatialObjectMask=NULL;
// 	  if (m_ArgsInfo.mask_given)
// 	    {
// 	      typedef itk::Image< unsigned char, ImageDimension >   ImageMaskType;
// 	      typedef itk::ImageFileReader< ImageMaskType >    MaskReaderType;
// 	      MaskReaderType::Pointer  maskReader = MaskReaderType::New();
// 	      maskReader->SetFileName(m_ArgsInfo.mask_arg);
	      
// 	      try 
// 		{ 
// 		  maskReader->Update(); 
// 		} 
// 	      catch( itk::ExceptionObject & err ) 
// 		{ 
// 		  std::cerr << "ExceptionObject caught while reading mask !" << std::endl; 
// 		  std::cerr << err << std::endl; 
// 		  return;
// 		} 
// 	      if (m_Verbose)std::cout <<"Mask was read..." <<std::endl;
	      
// 	      // Set the image to the spatialObject
// 	      spatialObjectMask = MaskType::New();
// 	      spatialObjectMask->SetImage( maskReader->GetOutput() );
// 	    }
	  
// 	  // Samplingfactors
// 	  CoefficientImageType::SizeType samplingFactors; 
// 	  for (unsigned int i=0; i< ImageDimension-1; i++)
// 	    {   
// 	      samplingFactors[i]= (int) ( input->GetSpacing()[i]/ m_ArgsInfo.spacing_arg);
// 	      if (m_Verbose) std::cout<<"Setting sampling factor "<<i<<" to "<<samplingFactors[i]<<"..."<<std::endl;
// 	    }
// 	  samplingFactors[ImageDimension-1]= (int) ( input->GetSpacing()[ImageDimension-1]/ m_ArgsInfo.phaseIncrement_arg);
// 	  if (m_Verbose) std::cout<<"Setting sampling factor "<<ImageDimension-1<<" to "<<samplingFactors[ImageDimension-1]<<"..."<<std::endl;
	  
// 	  // Set
// 	  //spatioTemporalTransform->SetTransformShape(m_ArgsInfo.shape_arg);
// 	  spatioTemporalTransform->SetSplineOrders(splineOrders);
// 	  spatioTemporalTransform->SetMask(spatialObjectMask);
// 	  spatioTemporalTransform->SetLUTSamplingFactors(samplingFactors);
// 	  spatioTemporalTransform->SetCoefficientImage(input);
// 	  transform=spatioTemporalTransform;

// 	  break;
// 	}
      }
	  
    
    // -----------------------------------------------
    // Construct Spatio-Temporal Point lists 4D
    // -----------------------------------------------
    typedef itk::Point<double, ImageDimension> SpaceTimePointType;
    typedef clitk::Lists<SpaceTimePointType> PointListsType;
    PointListsType pointLists(referencePointList.size());
    SpaceTimePointType spaceTimePoint;
    double phase;
    for (unsigned int i=0; i<referencePointList.size(); i++)
      {
	for (unsigned int d=0; d<SpaceDimension; d++)
	  {
	    spaceTimePoint[d]=referencePointList[i][d];
	  }
	phase=0;
	while (phase<10.)
	  {
	    spaceTimePoint[ImageDimension-1]=phase;
	    pointLists[i].push_back(spaceTimePoint);
	    phase+=m_ArgsInfo.phaseIncrement_arg;
	  }
      }


    // -----------------------------------------------
    // Transform Points
    // -----------------------------------------------
    typedef itk::Vector<double, ImageDimension> VectorType;
    typedef clitk::List<VectorType> VectorListType;
    typedef clitk::Lists<VectorType> VectorListsType;
    VectorListsType displacementLists(pointLists.size());
    VectorType displacement;
    for (unsigned int i=0; i<pointLists.size(); i++)
      {
	if (m_Verbose) std::cout<<"Transforming point "<<pointLists[i][0]<<"..."<<std::endl;
	for (unsigned int j=0; j<pointLists[i].size(); j++)
	  {
	    spaceTimePoint= transform->TransformPoint(pointLists[i][j]);
	    if (m_Verbose) std::cout<<"Transformed point "<<spaceTimePoint<<"..."<<std::endl;
	    displacement=spaceTimePoint-pointLists[i][j];
	    displacementLists[i].push_back(displacement);
	  }
      }


    // -----------------------------------------------
    // Write displacements
    // -----------------------------------------------
    std::vector<std::string> filenames;
    for (unsigned int i=0;i<displacementLists.size();i++)
      {
	std::ostringstream number_ostr;
	number_ostr << i;
	std::string number_str;
	if (i<10)  number_str=  "0"+number_ostr.str();
	else number_str =  number_ostr.str();
	filenames.push_back(m_ArgsInfo.trajectory_arg+number_str);
      }
    displacementLists.Write(filenames, m_Verbose );
  
  }


} //end clitk

#endif  //#define clitkPointTrajectoryGenericFilter_cxx
