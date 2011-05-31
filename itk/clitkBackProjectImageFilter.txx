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
#ifndef __clitkBackProjectImageFilter_txx
#define __clitkBackProjectImageFilter_txx
#include "clitkBackProjectImageFilter.h"
#include "itkContinuousIndex.h"
#include "vnl/vnl_math.h"
#include "itkLinearInterpolateImageFunction.h"

namespace clitk
{

  //-----------------------------------------------------------------------
  //     Constructor
  //-----------------------------------------------------------------------

  template<class InputImageType, class OutputImageType>
  BackProjectImageFilter< InputImageType, OutputImageType >
  ::BackProjectImageFilter()
  {

    //Parameters for backprojection
    this->m_IsoCenter.Fill(0.0);
    this->m_SourceToScreen = 1536.0;
    this->m_SourceToAxis = 1000.0;
    this->m_EdgePaddingValue = itk::NumericTraits<OutputPixelType>::Zero;//density images
    this->m_RigidTransformMatrix.SetIdentity();
    this->m_PanelShift[0] = 0.;
    this->m_PanelShift[1] = 0.;

    //Parameters for output
    this->m_OutputSpacing.Fill(1);
    this->m_OutputOrigin.Fill(0.0);
    this->m_OutputSize.Fill( 512 );
      
    this->m_IsInitialized=false;
  }


  //-----------------------------------------------------------------------
  //     PrintSelf
  //-----------------------------------------------------------------------
  template<class InputImageType, class OutputImageType>
  void
  BackProjectImageFilter< InputImageType, OutputImageType >
  ::PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    this->Superclass::PrintSelf(os,indent);

    os << indent << "IsoCenter: " << m_IsoCenter << std::endl;
    os << indent << "SourceToScreen: " << m_SourceToScreen << std::endl;
    os << indent << "SourceToAxis: " << m_SourceToAxis << std::endl;
    os << indent << "Edge Padding Value: " << m_EdgePaddingValue << std::endl;
    os << indent << "Rigid Transform matrix: " << m_EdgePaddingValue << std::endl; 
  }


  //-----------------------------------------------------------------------
  //     Set output info from image
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void 
  BackProjectImageFilter<InputImageType, OutputImageType>
  ::SetOutputParametersFromImage ( const OutputImagePointer image )
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
  BackProjectImageFilter<InputImageType, OutputImageType>
  ::SetOutputParametersFromImage (const OutputImageConstPointer image )
  {
    this->SetOutputOrigin( image->GetOrigin() );
    this->SetOutputSpacing( image->GetSpacing() );
    this->SetOutputSize( image->GetLargestPossibleRegion().GetSize() );
  }


  //-----------------------------------------------------------------------
  //     Generate output information
  //-----------------------------------------------------------------------
  template<class InputImageType, class OutputImageType>
  void
  BackProjectImageFilter< InputImageType, OutputImageType >
  ::GenerateOutputInformation( void )
  {
    // Don not call the superclass' implementation of this method (!=Dimensions)
    // Superclass::GenerateOutputInformation();
        
    // get pointer to the output
    OutputImagePointer outputPtr = this->GetOutput();
    InputImageConstPointer  inputPtr  = this->GetInput();
    
    if ( !outputPtr || ! inputPtr)
      {
	return;
      }
    
    // Set the output image largest possible region.  Use a RegionCopier
    // so that the input and output images can be different dimensions.
    OutputImageRegionType outputLargestPossibleRegion;
    this->CallCopyInputRegionToOutputRegion(outputLargestPossibleRegion,
					    inputPtr->GetLargestPossibleRegion());


    //OutputImageRegionType region;
    outputLargestPossibleRegion.SetSize( m_OutputSize );
    outputPtr->SetLargestPossibleRegion( outputLargestPossibleRegion );
    outputPtr->SetSpacing( m_OutputSpacing );
    outputPtr->SetOrigin( m_OutputOrigin );
    
  }

  
  //-----------------------------------------------------------------------
  //     Generate input Requested region
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void
  BackProjectImageFilter<InputImageType,OutputImageType>
  ::GenerateInputRequestedRegion()
  {
    //Call superclass
    Superclass::GenerateInputRequestedRegion();
    
    if (!this->GetOutput())
      {
	return;
      }
    OutputImageRegionType outputRegion = this->GetOutput()->GetRequestedRegion();
    InputImagePointer inputPtr =  const_cast<InputImageType *>(this->GetInput());
    
    if ( !inputPtr )
      {
	// Because DataObject::PropagateRequestedRegion() allows only
	// InvalidRequestedRegionError, it's impossible to write simply:
	// itkExceptionMacro(<< "Missing input " << idx);
	itk::InvalidRequestedRegionError e(__FILE__, __LINE__);
	e.SetLocation(ITK_LOCATION);
	e.SetDescription("Missing input.");
	e.SetDataObject(this->GetOutput());
	throw e;
      }
    
    InputImageRegionType inputRegion;
    inputRegion=inputPtr->GetLargestPossibleRegion();
    inputPtr->SetRequestedRegion(inputRegion);
    
  }
  


  //-----------------------------------------------------------------------
  //     Before threaded data
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void 
  BackProjectImageFilter<InputImageType, OutputImageType>
  ::BeforeThreadedGenerateData( void )
  {
    if (!m_IsInitialized) this->Initialize();
  }


 //-----------------------------------------------------------------------
  //     Initialize
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void 
  BackProjectImageFilter<InputImageType, OutputImageType>
  ::Initialize( void ) throw (itk::ExceptionObject)
  {
    //Change the origin of the 2D input
    typename  InputImageType::ConstPointer inputPtr=this->GetInput();
    m_ModifiedInput=InputImageType::New();
    m_ModifiedInput->CopyInformation(inputPtr);
    InputSizeType size=inputPtr->GetLargestPossibleRegion().GetSize();
    InputSpacingType spacing=inputPtr->GetSpacing();
    
    //Change the origin 
    InputPointType newOrigin;
    newOrigin[0] =-static_cast<double>(size[0]-1)*spacing[0]/2.0;//-static_cast<double>(size[0]-1)*spacing[0]/2.0;
    newOrigin[1] =-static_cast<double>(size[1]-1)*spacing[1]/2.0;//-static_cast<double>(size[1]-1)*spacing[1]/2.0;
    m_ModifiedInput->SetOrigin(newOrigin);
    
    // Calculate the projection matrix
    this->CalculateProjectionMatrix();

    // Calculate the coordinate increments
    this->CalculateCoordinateIncrements();

    m_IsInitialized=true;
  }


  //-----------------------------------------------------------------------
  //     Calculate Projection Matrix
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void 
  BackProjectImageFilter<InputImageType, OutputImageType>
  ::CalculateProjectionMatrix( void )
  {
    InputSpacingType inputSpacing=this->GetInput()->GetSpacing();
    
    // Projection on YZ plane+pixelTrans
    itk::Matrix<double,3,4> temp;
    temp.Fill(itk::NumericTraits<double>::Zero);
    //     temp(0,0)=-0.5*inputSpacing[0];
    //     temp(1,0)=-0.5*inputSpacing[1];
    temp(2,0)=1;
    temp(0,1)=m_SourceToScreen;//Invert Y/X? for correspondence to real projections
    temp(1,2)=m_SourceToScreen;
    //     temp(0,3)=m_SourceToAxis*0.5*inputSpacing[0];
    //     temp(1,3)=m_SourceToAxis*0.5*inputSpacing[1];
    temp(2,3)=-m_SourceToAxis;//-m_IsoCenter[0]-m_SourceToAxis;

    // Get the rotation parameter array
    itk::Array<double> rotationParameters(3);
    const double dtr = ( atan(1.0) * 4.0 ) / 180.0; //constant for converting degrees into radians
    rotationParameters[0]= 0.;
    rotationParameters[1]= 0.;
    rotationParameters[2]= -dtr*(double) m_ProjectionAngle;

    // We first perform rigid transform (of source and panel), then a centered rotation around the transformed center
    itk::Matrix<double,3,3> rigidRotation = GetRotationalPartMatrix3D(m_RigidTransformMatrix);
    itk::Vector<double,3> rigidTranslation = GetTranslationPartMatrix3D(m_RigidTransformMatrix);
    OutputPointType transformedIsoCenter = rigidRotation * m_IsoCenter + rigidTranslation; 
    
    // Calculate the centered rotation matrix
    itk::Matrix<double,4,4> centeredRotationMatrix=GetCenteredRotationMatrix3D(rotationParameters,transformedIsoCenter);
    
    // Define the voxel pixel transforms (shift half a pixel/voxel) 
    itk::Matrix<double,4,4> voxelTrans; voxelTrans.SetIdentity();
    for (unsigned int i=0;i<3;i++)voxelTrans(i,3)=0.5*m_OutputSpacing[i];
    
    // Compose the rotation with the rigid transform matrix and the voxel transform
    itk::Matrix<double,4,4> finalTransform = centeredRotationMatrix * m_RigidTransformMatrix;
    itk::Matrix<double,4,4> invFinalTransform ( finalTransform.GetInverse());
    invFinalTransform=invFinalTransform*voxelTrans;
      
    // Apply rigid transform to the projection matrix
    for (unsigned int i=0; i<3;i++)
      for (unsigned int j=0; j<4;j++)
	{
	  m_ProjectionMatrix(i,j)=itk::NumericTraits<double>::Zero;
	  for (unsigned int k=0; k<4;k++)
	    m_ProjectionMatrix(i,j)+=temp(i,k)*invFinalTransform(k,j);
	}
  }


  //-----------------------------------------------------------------------
  //     Calculate the coordinate increments
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void 
  BackProjectImageFilter<InputImageType, OutputImageType>
  ::CalculateCoordinateIncrements( void )
  {
    //Compute Line increment
    m_LineInc[0]=m_ProjectionMatrix[0][0]*m_OutputSpacing[0];
    m_LineInc[1]=m_ProjectionMatrix[1][0]*m_OutputSpacing[0];
    m_LineInc[2]=m_ProjectionMatrix[2][0]*m_OutputSpacing[0];
    
    //Compute column increment (and restart at the beginning of the line)
    m_ColInc[0]=m_ProjectionMatrix[0][1]*m_OutputSpacing[1]-m_OutputSize[0]*m_LineInc[0];
    m_ColInc[1]=m_ProjectionMatrix[1][1]*m_OutputSpacing[1]-m_OutputSize[0]*m_LineInc[1];
    m_ColInc[2]=m_ProjectionMatrix[2][1]*m_OutputSpacing[1]-m_OutputSize[0]*m_LineInc[2];

    //Compute plane increment  (and restart at the beginning of the columns)
    m_PlaneInc[0]=m_ProjectionMatrix[0][2]*m_OutputSpacing[2]-m_ProjectionMatrix[0][1]*m_OutputSpacing[1]*m_OutputSize[1];
    m_PlaneInc[1]=m_ProjectionMatrix[1][2]*m_OutputSpacing[2]-m_ProjectionMatrix[1][1]*m_OutputSpacing[1]*m_OutputSize[1];
    m_PlaneInc[2]=m_ProjectionMatrix[2][2]*m_OutputSpacing[2]-m_ProjectionMatrix[2][1]*m_OutputSpacing[1]*m_OutputSize[1];
  }


  //-----------------------------------------------------------------------
  //     Threaded generate data
  //-----------------------------------------------------------------------
  template <class InputImageType, class OutputImageType>
  void 
  BackProjectImageFilter<InputImageType, OutputImageType>
  ::ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread,  int threadId )
  {
    //Projection pointer
    InputImageConstPointer inputPtr=this->GetInput();
    
    //Volume pointer
    OutputImagePointer outputPTr= this->GetOutput();
    
    //Iterator for the thread region
    typedef itk::ImageRegionIterator<OutputImageType> OutputIteratorType;
    OutputIteratorType iterator(outputPTr, outputRegionForThread);
    OutputSizeType outputSizeForThread=outputRegionForThread.GetSize();
    
    //Some temp variables
    OutputPixelType value;
    HomogeneOutputPointType homOutputPoint;
    HomogeneInputPointType homInputPoint;
    OutputPointType oPoint;
    InputPointType iPoint;
    iPoint.Fill(itk::NumericTraits<double>::Zero);
    OutputIndexType oIndex;
    ContinuousInputIndexType iIndex;
    InputSizeType inputSize=inputPtr->GetLargestPossibleRegion().GetSize();

    //Get the first output coordinate
    oIndex=iterator.GetIndex();//costly but only once a thread
    outputPTr->TransformIndexToPhysicalPoint(oIndex,oPoint);
    
    for (unsigned int i=0;i<OutputImageDimension;i++) homOutputPoint[i]=oPoint[i];
    homOutputPoint[3]=1;
       
    //Compute the first input coordinate (invert Y/X)
    homInputPoint= (m_ProjectionMatrix * homOutputPoint);
    iPoint[0]=-homInputPoint[0]/homInputPoint[2] + m_PanelShift[0];
    iPoint[1]=homInputPoint[1]/homInputPoint[2] + m_PanelShift[1];

    typedef itk::LinearInterpolateImageFunction< InputImageType, double > InterpolatorType;
    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetInputImage(this->GetInput());

    //Run over all output voxels
    for (unsigned int i=0; i<outputSizeForThread[2]; i++)
      {
	for (unsigned int j=0; j<outputSizeForThread[1]; j++)
	  {
	    for (unsigned int k=0; k<outputSizeForThread[0]; k++)
	      {
		iPoint[0]=-homInputPoint[0]/homInputPoint[2] + m_PanelShift[0];
		iPoint[1]=homInputPoint[1]/homInputPoint[2] + m_PanelShift[1];

		//Check wether inside, convert to index (use modified with correct origin)
		if (m_ModifiedInput->TransformPhysicalPointToContinuousIndex(iPoint, iIndex) && interpolator->IsInsideBuffer(iIndex))
                    value = interpolator->EvaluateAtContinuousIndex(iIndex);
		//Outside: padding value
		else
                  value=m_EdgePaddingValue;
		//Set it
		iterator.Set(value);

		//Advance one step in the line
		homInputPoint+=m_LineInc;
		++iterator;
	      }

	    //Advance one line
	    homInputPoint+=m_ColInc;
	  }

	//Advance one plane
	homInputPoint+=m_PlaneInc;
      }
  }


} // namespace clitk


#endif
