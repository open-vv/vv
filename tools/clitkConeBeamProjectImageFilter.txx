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
#ifndef __clitkConeBeamProjectImageFilter_txx
#define __clitkConeBeamProjectImageFilter_txx
namespace clitk
{


  //=========================================================================================================================
  //Constructor
  template <class InputImageType, class OutputImageType>
  ConeBeamProjectImageFilter<InputImageType, OutputImageType>::
  ConeBeamProjectImageFilter()
  {
    // Initialize with the default values (Elekta Synergie)
    m_Verbose=false;
    m_IsInitialized=false;
    m_NumberOfThreadsIsGiven=false;

    // Geometry
    m_IsoCenter.Fill(0.0);
    m_SourceToScreen=1536.;
    m_SourceToAxis=1000.;
    m_ProjectionAngle=0.;
    m_RigidTransformMatrix.SetIdentity();
    m_EdgePaddingValue=itk::NumericTraits<OutputPixelType>::Zero;//density images
    
    // Pipe    
    m_Transform=TransformType::New();
    m_Resampler=ResampleImageFilterType::New();
    m_Interpolator= InterpolatorType::New();
    m_ExtractImageFilter=ExtractImageFilterType::New();
    m_FlipImageFilter=FlipImageFilterType::New();
    
    // Parameters for output
    this->m_OutputSpacing.Fill(0.8);
    this->m_OutputOrigin.Fill(0.0);
    this->m_OutputSize.Fill( 512 );
        
  }


  //-----------------------------------------------------------------------
  //     Set output info from image
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void 
  ConeBeamProjectImageFilter<InputImageType, OutputImageType>
  ::SetOutputParametersFromImage ( OutputImagePointer image )
  {
    this->SetOutputOrigin( image->GetOrigin() );
    this->SetOutputSpacing( image->GetSpacing() );
    this->SetOutputSize( image->GetLargestPossibleRegion().GetSize() );
  }


  //-----------------------------------------------------------------------
  //     Set output info from image
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void 
  ConeBeamProjectImageFilter<InputImageType, OutputImageType>
  ::SetOutputParametersFromImage ( OutputImageConstPointer image )
  {
    this->SetOutputOrigin( image->GetOrigin() );
    this->SetOutputSpacing( image->GetSpacing() );
    this->SetOutputSize( image->GetLargestPossibleRegion().GetSize() );
  }


  //=========================================================================================================================
  // Initialize
  template <class InputImageType, class OutputImageType>
  void 
  ConeBeamProjectImageFilter<InputImageType, OutputImageType>
  ::Initialize(void) throw (itk::ExceptionObject)
  {
    
    //====================================================================
    // Define the transform: composition of rigid transfo around origin  and a centered rotation
    // The center of rotation should be placed at the isocenter!

    if (m_Verbose) std::cout<<"The isocenter is at "<< m_IsoCenter <<"..." <<std::endl;
       
    // Get the rotation parameter array
    itk::Array<double> rotationParameters(3);
    const double dtr = ( atan(1.0) * 4.0 ) / 180.0; //constant for converting degrees into radians
    if (m_Verbose)std::cout<<"The projection angle is "<< m_ProjectionAngle <<"° (0° being lateral left)..."<< std::endl; 
    rotationParameters[0]= 0.;
    rotationParameters[1]= 0.;
    rotationParameters[2]= -dtr*(double) m_ProjectionAngle;

    // We first perform rigid transform (of source and panel), then a centered rotation around the transformed center
    itk::Matrix<double,3,3> rigidRotation = GetRotationalPartMatrix3D(m_RigidTransformMatrix);
    itk::Vector<double,3> rigidTranslation = GetTranslationPartMatrix3D(m_RigidTransformMatrix);
    typename InputImageType::PointType transformedCenter = rigidRotation * m_IsoCenter + rigidTranslation; 
    
    // Calculate the centered rotation matrix
    itk::Matrix<double,4,4> centeredRotationMatrix = GetCenteredRotationMatrix3D(rotationParameters,transformedCenter);
    
    // Compose this rotation with the rigid transform matrix
    itk::Matrix<double,4,4> finalTransform = m_RigidTransformMatrix * centeredRotationMatrix ;
    
    // Set the rotation
    itk::Matrix<double,3,3> finalRotation = GetRotationalPartMatrix3D(finalTransform);
    m_Transform->SetMatrix(finalRotation);
    if (m_Verbose)std::cout<<"The final rotation is "<<finalRotation<<"..."<<std::endl;

    // Set the translation
    itk::Vector<double,3> finalTranslation = GetTranslationPartMatrix3D(finalTransform);
    m_Transform->SetTranslation(finalTranslation);
    if (m_Verbose)std::cout<<"The final translation is "<<finalTranslation<<"..."<<std::endl;

  
    //====================================================================
    //Define a resample filter for the projection
    if (m_NumberOfThreadsIsGiven) m_Resampler->SetNumberOfThreads(m_NumberOfThreads);
    m_Resampler->SetDefaultPixelValue(m_EdgePaddingValue); 
    if (m_Verbose)std::cout<<"The edge padding value is "<<m_EdgePaddingValue<<"..."<<std::endl;

    //JV Original raycast does not take into account origin, but does implicit shift to center of volume
    //JV patched in RayCastInterpolateImageFunctionWithOrigin
    m_Interpolator->SetTransform(m_Transform);
    m_Interpolator->SetThreshold(0.);

    // Focalpoint: we presume that for an angle of 0° the kV is lateral left (for the patient on his back), or on the positive X-axis.
    typename  InterpolatorType::InputPointType focalpoint;
    focalpoint[0]= m_IsoCenter[0] + m_SourceToAxis;
    focalpoint[1]= m_IsoCenter[1]; 
    focalpoint[2]= m_IsoCenter[2]; 
    m_Interpolator->SetFocalPoint(focalpoint);
    if (m_Verbose)std::cout<<"The focalpoint is at "<< focalpoint <<"..."<< std::endl;
 
    // Connect the interpolator and the transform with the m_Resampler
    m_Resampler->SetInterpolator( m_Interpolator );
    m_Resampler->SetTransform( m_Transform );
 
    // Describe the output projection image
    typename  InputImageType::SizeType   sizeOuput;
    sizeOuput[0] = 1;    // number of pixels along X of the 2D DRR image 
    sizeOuput[1] = m_OutputSize[0];  // number of pixels along Y of the 2D DRR image 
    sizeOuput[2] = m_OutputSize[1];  // number of pixels along Z of the 2D DRR image 
    m_Resampler->SetSize( sizeOuput );
    if (m_Verbose)std::cout<<"The output size is "<< m_OutputSize <<"..."<< std::endl;

    typename  InputImageType::SpacingType spacingOutput;
    spacingOutput[0] = 1;    // pixel spacing along X of the 2D DRR image [mm]
    spacingOutput[1] = m_OutputSpacing[0];  // pixel spacing along Y of the 2D DRR image [mm]
    spacingOutput[2] = m_OutputSpacing[1];  // pixel spacing along Y of the 2D DRR image [mm]
    m_Resampler->SetOutputSpacing( spacingOutput );
    if (m_Verbose)std::cout<<"The output size is "<< m_OutputSpacing <<"..."<< std::endl;

    // The position of the DRR is specified, we presume that for an angle of 0° the flatpanel is located at the negative x-axis
    // JV -1 seems to correspond better with shearwarp of Simon Rit
    typename  InterpolatorType::InputPointType originOutput;
    originOutput[0] = m_IsoCenter[0]- (m_SourceToScreen - m_SourceToAxis);
    DD(m_PanelShift);
    originOutput[1] = m_IsoCenter[1]-static_cast<double>(sizeOuput[1]-1)*spacingOutput[1]/2.0 - m_PanelShift;
    originOutput[2] = m_IsoCenter[2]-static_cast<double>(sizeOuput[2]-1)*spacingOutput[2]/2.0; 
    m_Resampler->SetOutputOrigin( originOutput );
    if (m_Verbose)std::cout<<"The origin of the flat panel is at "<< originOutput <<",..."<< std::endl;

    // We define the region to be extracted. Projection was in the YZ plane, X should be set to zero
    typename InternalImageType::RegionType::SizeType sizeTemp=sizeOuput;
    sizeTemp[0]=0;
    typename InternalImageType::IndexType startTemp; //=temp->GetLargestPossibleRegion().GetIndex();
    startTemp.Fill(0);
    
    typename InternalImageType::RegionType desiredRegion;
    desiredRegion.SetSize( sizeTemp );
    desiredRegion.SetIndex( startTemp );
    m_ExtractImageFilter->SetExtractionRegion( desiredRegion );

    // Prepare the Flip Image filter
    typename  FlipImageFilterType::FlipAxesArrayType flipArray;
    flipArray[0] = 0;
    flipArray[1] = 1;
    m_FlipImageFilter->SetFlipAxes( flipArray );

    // Initialization complete
    m_IsInitialized=true;

  }

  //=========================================================================================================================
  // Update
  template <class InputImageType, class OutputImageType>
  void
  ConeBeamProjectImageFilter<InputImageType, OutputImageType>
  ::Update(void)
  {
    
    //==================================================================
    // If geometry changed reinitialize 
    if (! m_IsInitialized) this->Initialize();

    // Input
    m_Resampler->SetInput( m_Input ); 
    
    //==================================================================
    // Execute the filter
    if (m_Verbose)std::cout<<"Starting the projection..."<<std::endl;
    try {
      m_Resampler->Update();
    }
    catch( itk::ExceptionObject & err ) 
      { 
	std::cerr << "ExceptionObject caught! Projection failed!" << std::endl; 
	std::cerr << err << std::endl; 
      } 

    //==================================================================
    // Make a 2D image out of it
    typename InternalImageType::Pointer temp=m_Resampler->GetOutput();
    m_ExtractImageFilter->SetInput(temp);
    
    // We should flip the Y axis
    m_FlipImageFilter->SetInput(m_ExtractImageFilter->GetOutput());
    m_FlipImageFilter->Update();
      
    // Get output
    m_Output= m_FlipImageFilter->GetOutput();
    m_Output->Update();
    m_Output->SetOrigin(m_OutputOrigin);
  }

  //=========================================================================================================================
  // GetOutput
  template <class InputImageType, class OutputImageType>
  typename OutputImageType::Pointer 
  ConeBeamProjectImageFilter<InputImageType, OutputImageType>
  ::GetOutput(void)
  {
    //JV it is not safe to repeatedly call getoutput/ always call Update first
    return m_Output;
  }
  
}


#endif
