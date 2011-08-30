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
#ifndef __clitkShapedBLUTSpatioTemporalDeformableTransform_txx
#define __clitkShapedBLUTSpatioTemporalDeformableTransform_txx
#include "clitkShapedBLUTSpatioTemporalDeformableTransform.h"

//itk
#include "itkContinuousIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkIdentityTransform.h"

namespace clitk
{

  // Constructor with default arguments
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
#if ITK_VERSION_MAJOR >= 4
  ::ShapedBLUTSpatioTemporalDeformableTransform():Superclass(0)
#else
  ::ShapedBLUTSpatioTemporalDeformableTransform():Superclass(OutputDimension,0)
#endif
  {
    unsigned int i;
    
    //JV default spline order
    for ( i=0;i<InputDimension; i++)
      m_SplineOrders[i]=3;

    //JV default LUTSamplingfactor
    for ( i=0;i<InputDimension; i++)
      m_LUTSamplingFactors[i]=20;

    for (i=0;i<InputDimension; i++)
      m_SupportSize[i] = m_SplineOrders[i]+1;

    //Instantiate a Vector Bspline Interpolator
    m_VectorInterpolator =VectorInterpolatorType::New();
    m_VectorInterpolator->SetLUTSamplingFactors(m_LUTSamplingFactors);
    m_VectorInterpolator->SetSplineOrders(m_SplineOrders);

    // Set Bulk transform to NULL (no bulk performed)
    m_BulkTransform = NULL;

    // Mask
    m_Mask=NULL;

    // Shape
    m_TransformShape=0;

    // Default grid size is zero
    m_NullSize.Fill(0);
    m_NullIndex.Fill(0);
  
    //JV region containing the parameters
    m_GridRegion.SetSize( m_NullSize);
    m_GridRegion.SetIndex( m_NullIndex );

    //JV use second region over the images
    m_PaddedGridRegion.SetSize(m_NullSize);
    m_PaddedGridRegion.SetIndex(m_NullIndex);
    
    //JV Maintain two origins
    m_GridOrigin.Fill( 0.0 );  // default origin is all zeros
    m_PaddedGridOrigin.Fill( 0.0 );  // default origin is all zeros
    
    m_GridSpacing.Fill( 1.0 ); // default spacing is all ones
    m_GridDirection.SetIdentity(); // default spacing is all ones

    m_InternalParametersBuffer = ParametersType(0);
    // Make sure the parameters pointer is not NULL after construction.
    m_InputParametersPointer = &m_InternalParametersBuffer;

    // Initialize coeffient images
    m_WrappedImage = CoefficientImageType::New();
    m_WrappedImage->SetRegions( m_GridRegion );
    m_WrappedImage->SetOrigin( m_GridOrigin.GetDataPointer() );
    m_WrappedImage->SetSpacing( m_GridSpacing.GetDataPointer() );
    m_WrappedImage->SetDirection( m_GridDirection );

    // JV Initialize the padded version
    m_PaddedCoefficientImage = CoefficientImageType::New();
    m_PaddedCoefficientImage->SetRegions( m_PaddedGridRegion );
    m_PaddedCoefficientImage->SetOrigin( m_GridOrigin.GetDataPointer() );
    m_PaddedCoefficientImage->SetSpacing( m_GridSpacing.GetDataPointer() );
    m_PaddedCoefficientImage->SetDirection( m_GridDirection );

    m_CoefficientImage = NULL;
  
    // Variables for computing interpolation
    for (i=0; i <InputDimension;i++)
      {
	m_Offset[i] = m_SplineOrders[i] / 2;
	if ( m_SplineOrders[i] % 2 ) 
	  {
	    m_SplineOrderOdd[i] = true;
	  }
	else
	  {
	    m_SplineOrderOdd[i] = false;
	  }
      }

    //JV padded should be used when checking
    m_ValidRegion = m_PaddedGridRegion;
	
    // Initialize jacobian images 
    for (i=0; i <OutputDimension;i++)
      {
	m_JacobianImage[i] = JacobianImageType::New();
	m_JacobianImage[i]->SetRegions( m_GridRegion );
	m_JacobianImage[i]->SetOrigin( m_GridOrigin.GetDataPointer() );
	m_JacobianImage[i]->SetSpacing( m_GridSpacing.GetDataPointer() );
	m_JacobianImage[i]->SetDirection( m_GridDirection );
      }
    
    /** Fixed Parameters store the following information:
     *     Grid Size
     *     Grid Origin
     *     Grid Spacing
     *     Grid Direction */
    //JV we add the splineOrders, LUTsamplingfactor, m_Mask  and m_BulkTransform
    /*
      Spline orders
      Sampling factors
      m_Mask
      m_BulkTransform
      The size of these is equal to the  NInputDimensions
    *********************************************************/
    this->m_FixedParameters.SetSize ( NInputDimensions * (NInputDimensions + 5)+3 );
    this->m_FixedParameters.Fill ( 0.0 );
    for ( i=0; i<NInputDimensions; i++)
      {
	this->m_FixedParameters[2*NInputDimensions+i] = m_GridSpacing[i];
      }
    for (unsigned int di=0; di<NInputDimensions; di++)
      {
	for (unsigned int dj=0; dj<NInputDimensions; dj++)
	  {
	    this->m_FixedParameters[3*NInputDimensions+(di*NInputDimensions+dj)] = m_GridDirection[di][dj];
	  }
      }

    //JV add splineOrders
    for ( i=0; i<NInputDimensions; i++)
      {
	this->m_FixedParameters[ ( (3+NInputDimensions)*NInputDimensions)+i] = (this->GetSplineOrders())[i];
      }
    
    //JV add LUTsamplingFactors
    for ( i=0; i<NInputDimensions; i++)
      {
	this->m_FixedParameters[( (4+NInputDimensions)*NInputDimensions)+i ] = this->GetLUTSamplingFactors()[i];
      }

    // JV add the mask pointer
    this->m_FixedParameters[( (5+NInputDimensions)*NInputDimensions)]=(double)((size_t)m_Mask);

    // JV add the bulkTransform pointer
    this->m_FixedParameters[( (5+NInputDimensions)*NInputDimensions) +1]=(double)((size_t)m_BulkTransform);

    // JV add the Transform shape
    this->m_FixedParameters[( (5+NInputDimensions)*NInputDimensions) +2]=(double)(m_TransformShape);


    // Calculate the PointToIndex matrices
    DirectionType scale;
    for( unsigned int i=0; i<OutputDimension; i++)
      {
	scale[i][i] = m_GridSpacing[i];
      }

    m_IndexToPoint = m_GridDirection * scale;
    m_PointToIndex = m_IndexToPoint.GetInverse();
 
    m_LastJacobianIndex = m_ValidRegion.GetIndex();


    //Weights to be used for the BC
    m_Weights.resize(4);
    std::vector<double> weights(5);
    for (i=0; i<4; i++) 
      weights[0]=0;
    // NN
    weights[0]=1;
    weights[1]=1./2;
    weights[2]=0;
    weights[3]=0;
    weights[4]=0;
    m_Weights[0]=weights;

    // Linear
    weights[0]=1;
    weights[1]=1./2;
    weights[2]=0;
    weights[3]=0;
    weights[4]=0;
    m_Weights[1]=weights;

    // quadratic
    weights[0]=3./4.;
    weights[1]=1./2.;
    weights[2]=1./8.;
    weights[3]=0;
    weights[4]=0;
    m_Weights[2]=weights;

    // cubic
    weights[0]=2./3.;
    weights[1]=23./48. ;
    weights[2]=1./6.;
    weights[3]=1./48.;
    weights[4]=0;
    m_Weights[3]=weights;

    // Update the WeightRatios
    m_WeightRatio.resize(4);
    for (unsigned int i=0; i<4; i++)
      {
	m_WeightRatio[i].resize(4);
	for (unsigned int j=0; j<4; j++)
	  m_WeightRatio[i][j]=m_Weights[m_SplineOrders[InputDimension-1]][i]/ m_Weights[m_SplineOrders[InputDimension-1]][j];
      }

    //JV initialize some variables for jacobian calculation
    m_SupportRegion.SetSize(m_SupportSize);
    m_SupportIndex.Fill(0);
    m_SupportRegion.SetIndex(m_SupportIndex);
    for (  i = 0; i < SpaceDimension ; i++ ) 
      m_ZeroVector[i]=itk::NumericTraits<JacobianValueType>::Zero;
    
    m_InitialOffset=0;
    m_FirstRegion.SetSize(m_NullSize);
    m_FirstRegion.SetIndex(m_NullIndex);
    m_SecondRegion.SetSize(m_NullSize);
    m_SecondRegion.SetIndex(m_NullIndex);
    m_ThirdRegion.SetSize(m_NullSize);
    m_ThirdRegion.SetIndex(m_NullIndex);
        
    m_BCValues.resize(0);
    m_BCRegions.resize(0);
    m_BCSize=0;
    m_BC2Values.resize(0);
    m_BC2Regions.resize(0);
    m_BC2Size=0;
    m_BC3Values.resize(0);
    m_BC3Regions.resize(0);
    m_BC3Size=0;


    for (  i = 0; i < SpaceDimension ; i++ )
      {
  	m_FirstIterator[i]= IteratorType( m_JacobianImage[i], m_FirstRegion);
    	m_SecondIterator[i]= IteratorType( m_JacobianImage[i], m_SecondRegion);
    	m_ThirdIterator[i]= IteratorType( m_JacobianImage[i], m_ThirdRegion);
      	m_BCIterators[i].resize(0);
	m_BC2Iterators[i].resize(0);
	m_BC3Iterators[i].resize(0);
      }

    this->Modified();

  }
    

  // Destructor
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::~ShapedBLUTSpatioTemporalDeformableTransform()
  {

  }


  // JV set Spline Order
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetSplineOrder(const unsigned int & splineOrder)
  {
    SizeType splineOrders;
    for (unsigned int i=0;i<InputDimension;i++)splineOrders[i]=splineOrder;

    this->SetSplineOrders(splineOrders);
  }
   

  // JV set Spline Orders
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetSplineOrders(const SizeType & splineOrders)
  {
    if(m_SplineOrders!=splineOrders)
      {
	m_SplineOrders=splineOrders;
	
	//update the interpolation function
	m_VectorInterpolator->SetSplineOrders(m_SplineOrders);
	
	//update the varaibles for computing interpolation
	for (unsigned int i=0; i <InputDimension;i++)
	  {
	    m_SupportSize[i] = m_SplineOrders[i]+1;
	    m_Offset[i] = m_SplineOrders[i] / 2;
	    
	    if ( m_SplineOrders[i] % 2 ) 
	      {
		m_SplineOrderOdd[i] = true;
	      }
	    else
	      {
		m_SplineOrderOdd[i] = false;
	      }
	  }

	//SupportSize is updated!, update regions
	//JV initialize some variables for jacobian calculation
	m_SupportRegion.SetSize(m_SupportSize);
	m_SupportIndex.Fill(0);
	m_SupportRegion.SetIndex(m_SupportIndex);
	
	// Update the WeightRatios
	for (unsigned int i=0; i<4; i++)
	  for (unsigned int j=0; j<4; j++)
	    m_WeightRatio[i][j]=m_Weights[m_SplineOrders[InputDimension-1]][i]/ m_Weights[m_SplineOrders[InputDimension-1]][j];

	this->Modified();
      }
  }
   

  // JV set sampling factor
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetLUTSamplingFactor( const int & samplingFactor)
  {
    SizeType samplingFactors;
    for (unsigned int i=0; i<NInputDimensions; i++)
      samplingFactors[i]=samplingFactor;
    
    //update the interpolation function
    this->SetLUTSamplingFactors(samplingFactors);
  }


  // JV set sampling factors
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetLUTSamplingFactors( const SizeType & samplingFactors)
  {
    if(m_LUTSamplingFactors!=samplingFactors)
      {
	for (unsigned int i=0; i<NInputDimensions; i++)
	  m_LUTSamplingFactors[i]=samplingFactors[i];
	
	//update the interpolation function
	m_VectorInterpolator->SetLUTSamplingFactors(m_LUTSamplingFactors);
	
	this->Modified();
      }
  }


  // Get the number of parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  unsigned int
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetNumberOfParameters(void) const
  {

    // The number of parameters equal SpaceDimension * number of
    // of pixels in the grid region.
    return ( static_cast<unsigned int>( SpaceDimension ) *
	     static_cast<unsigned int>( m_GridRegion.GetNumberOfPixels() ) );

  }


  // Get the padded number of parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  unsigned int
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetPaddedNumberOfParameters(void) const
  {

    // The number of parameters equal SpaceDimension * number of
    // of pixels in the grid region.
    return ( static_cast<unsigned int>( SpaceDimension ) *
	     static_cast<unsigned int>( m_PaddedGridRegion.GetNumberOfPixels() ) );

  }



  // Get the number of parameters per dimension
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  unsigned int
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetNumberOfParametersPerDimension(void) const
  {
    // The number of parameters per dimension equal number of
    // of pixels in the grid region.
    return ( static_cast<unsigned int>( m_GridRegion.GetNumberOfPixels() ) );

  }


  // Set the grid region
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridRegion( const RegionType & region )
  {
    if ( m_GridRegion != region )
      {
	m_GridRegion = region;
	m_PaddedGridRegion=region;
	
	// JV set the padded region
	typename CoefficientImageType::RegionType::SizeType paddedSize= region.GetSize();
	
	//JV   size dependes on shape
	switch (m_TransformShape)
	  {
	  case 0:
	  case 1:
	    paddedSize[InputDimension-1]+=4;
	    break;
	  case 2:
	  case 3:
	    paddedSize[InputDimension-1]+=4;
	    break;
	  case 4:
	  case 5:
	    paddedSize[InputDimension-1]+=3;
	    break;
	  case 6:
	  case 7:
	    paddedSize[InputDimension-1]+=2;
	    break;
	  case 8:
	  case 9:
	    paddedSize[InputDimension-1]+=3;
	    break;
	  default:
	    paddedSize[InputDimension-1]=+1;
	  }
	m_PaddedGridRegion.SetSize(paddedSize);
	
	// Set regions for each coefficient and jacobian image
	m_WrappedImage->SetRegions( m_GridRegion );
	m_PaddedCoefficientImage->SetRegions( m_PaddedGridRegion );
	m_PaddedCoefficientImage->Allocate();
	for (unsigned int j=0; j <OutputDimension;j++)
	  {
	    m_JacobianImage[j]->SetRegions( m_GridRegion );
	  }
	
	// JV used the padded version for the valid region
	// Set the valid region
	// If the grid spans the interval [start, last].
	// The valid interval for evaluation is [start+offset, last-offset]
	// when spline order is even.
	// The valid interval for evaluation is [start+offset, last-offset)
	// when spline order is odd.
	// Where offset = vcl_floor(spline / 2 ).
	// Note that the last pixel is not included in the valid region
	// with odd spline orders.
	typename RegionType::SizeType size = m_PaddedGridRegion.GetSize();
	typename RegionType::IndexType index = m_PaddedGridRegion.GetIndex();
	for ( unsigned int j = 0; j < NInputDimensions; j++ )
	  {
	    index[j] += 
	      static_cast< typename RegionType::IndexValueType >( m_Offset[j] );
	    size[j] -= 
	      static_cast< typename RegionType::SizeValueType> ( 2 * m_Offset[j] );
	    m_ValidRegionLast[j] = index[j] +
	      static_cast< typename RegionType::IndexValueType >( size[j] ) - 1;
	  }
	m_ValidRegion.SetSize( size );
	m_ValidRegion.SetIndex( index );
	
	// If we are using the default parameters, update their size and set to identity.
	// Input parameters point to internal buffer => using default parameters.
	if (m_InputParametersPointer == &m_InternalParametersBuffer)
	  {
	    // Check if we need to resize the default parameter buffer.
	    if ( m_InternalParametersBuffer.GetSize() != this->GetNumberOfParameters() )
	      {
		m_InternalParametersBuffer.SetSize( this->GetNumberOfParameters() );
		// Fill with zeros for identity.
		m_InternalParametersBuffer.Fill( 0 );
	      }
	  }

	this->Modified();
      }
  }


  // Set the grid spacing
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridSpacing( const SpacingType & spacing )
  {
    if ( m_GridSpacing != spacing )
      {
	m_GridSpacing = spacing;

	// Set spacing for each coefficient and jacobian image
	m_WrappedImage->SetSpacing( m_GridSpacing.GetDataPointer() );
	m_PaddedCoefficientImage->SetSpacing( m_GridSpacing.GetDataPointer() );
	for (unsigned int j=0; j <OutputDimension; j++) m_JacobianImage[j]->SetSpacing( m_GridSpacing.GetDataPointer() );
	
	// Set scale
	DirectionType scale;
	for( unsigned int i=0; i<OutputDimension; i++)
	  {
	    scale[i][i] = m_GridSpacing[i];
	  }

	m_IndexToPoint = m_GridDirection * scale;
	m_PointToIndex = m_IndexToPoint.GetInverse();

	this->Modified();
      }

  }

  // Set the grid direction
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridDirection( const DirectionType & direction )
  {
    if ( m_GridDirection != direction )
      {
	m_GridDirection = direction;

	// Set direction for each coefficient and jacobian image
	m_WrappedImage->SetDirection( m_GridDirection );
	m_PaddedCoefficientImage->SetDirection( m_GridDirection );
	for (unsigned int j=0; j <OutputDimension; j++) m_JacobianImage[j]->SetDirection( m_GridDirection );
	
	// Set scale
	DirectionType scale;
	for( unsigned int i=0; i<OutputDimension; i++)
	  {
	    scale[i][i] = m_GridSpacing[i];
	  }

	m_IndexToPoint = m_GridDirection * scale;
	m_PointToIndex = m_IndexToPoint.GetInverse();

	this->Modified();
      }

  }


  // Set the grid origin
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridOrigin( const OriginType& origin )
  {
    if( m_GridOrigin!=origin)
      {
	m_GridOrigin = origin;
	
	// JV The origin depends on the shape 
	switch (m_TransformShape)
	  {
	  case 0:
	  case 1:
	    m_PaddedGridOrigin=origin;
	    m_PaddedGridOrigin[InputDimension-1]=origin[InputDimension-1]-2* m_GridSpacing[InputDimension-1];
	    break;
	  case 2:
	  case 3:
	    m_PaddedGridOrigin=origin;
	    m_PaddedGridOrigin[InputDimension-1]=origin[InputDimension-1]-1* m_GridSpacing[InputDimension-1];
	    break;
	  case 4:
	  case 5:
	    m_PaddedGridOrigin=origin;
	    m_PaddedGridOrigin[InputDimension-1]=origin[InputDimension-1]-1* m_GridSpacing[InputDimension-1];
	    break;
	  case 6:
	  case 7:
	    m_PaddedGridOrigin=origin;
	    break;
	  case 8:
	  case 9:
	    m_PaddedGridOrigin=origin;
	    m_PaddedGridOrigin[InputDimension-1]=origin[InputDimension-1]-1* m_GridSpacing[InputDimension-1];
	    break;
	  default: 
	    m_PaddedGridOrigin=origin;
	  }
    
	// Set origin for each coefficient and jacobianimage
	m_WrappedImage->SetOrigin( m_GridOrigin.GetDataPointer() );
	m_PaddedCoefficientImage->SetOrigin( m_PaddedGridOrigin.GetDataPointer() );
	for (unsigned int j=0; j <OutputDimension; j++) m_JacobianImage[j]->SetOrigin( m_GridOrigin.GetDataPointer() );
	
	this->Modified();
      }
  }


  // Set the parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetIdentity()
  {
    if( m_InputParametersPointer )
      {
	ParametersType * parameters =
	  const_cast<ParametersType *>( m_InputParametersPointer );
	parameters->Fill( 0.0 );
	this->Modified();
      }
    else 
      {
	itkExceptionMacro( << "Input parameters for the spline haven't been set ! "
			   << "Set them using the SetParameters or SetCoefficientImage method first." );
      }
  }


  // Set the parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetParameters( const ParametersType & parameters )
  {

    // Check if the number of parameters match the
    // Expected number of parameters
    if ( parameters.Size() != this->GetNumberOfParameters() )
      {
	itkExceptionMacro(<<"Mismatched between parameters size "
			  << parameters.size() 
			  << " and region size "
			  << m_GridRegion.GetNumberOfPixels() );
      }

    // Clean up buffered parameters
    m_InternalParametersBuffer = ParametersType( 0 );

    // Keep a reference to the input parameters
    m_InputParametersPointer = &parameters;

    // Wrap flat array as images of coefficients
    this->WrapAsImages();

    //JV Set padded input to vector interpolator
    m_VectorInterpolator->SetInputImage(this->GetPaddedCoefficientImage());

    // Modified is always called since we just have a pointer to the
    // parameters and cannot know if the parameters have changed.
    this->Modified();
  }


  // Set the Fixed Parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetFixedParameters( const ParametersType & parameters )
  {
 
    // JV number should be exact, no defaults for spacing
    if ( parameters.Size() != NInputDimensions * (5 + NInputDimensions)+3 )
      {
	itkExceptionMacro(<< "Mismatched between parameters size "
			  << parameters.size() 
			  << " and number of fixed parameters "
			  << NInputDimensions * (5 + NInputDimensions)+3 );
      }
    /********************************************************* 
    Fixed Parameters store the following information:
        Grid Size
        Grid Origin
        Grid Spacing
        Grid Direction */
    // JV we add the splineOrders, LUTsamplingfactor, mask pointer and bulktransform pointer
    /*
      Spline orders
      Sampling factors
      m_Mask
      m_BulkTransform
      m_TransformShape

      The size of these is equal to the  NInputDimensions
    *********************************************************/
  
    /** Set the Grid Parameters */
    SizeType   gridSize;
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	gridSize[i] = static_cast<int> (parameters[i]);
      }
    RegionType bsplineRegion;
    bsplineRegion.SetSize( gridSize );
  
    /** Set the Origin Parameters */
    OriginType origin;
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	origin[i] = parameters[NInputDimensions+i];
      }
  
    /** Set the Spacing Parameters */
    SpacingType spacing;
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	spacing[i] = parameters[2*NInputDimensions+i];
      }

    /** Set the Direction Parameters */
    DirectionType direction;
    for (unsigned int di=0; di<NInputDimensions; di++)
      {
	for (unsigned int dj=0; dj<NInputDimensions; dj++)
	  {
	    direction[di][dj] = parameters[3*NInputDimensions+(di*NInputDimensions+dj)];
	  }
      }

    //JV add the spline orders
    SizeType splineOrders;
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	splineOrders[i]=parameters[(3+NInputDimensions)*NInputDimensions+i];
      }

    //JV add the LUT sampling factor
    SizeType  samplingFactors;
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	samplingFactors[i]=parameters[(4+NInputDimensions)*NInputDimensions+i];
      }

    //JV add the MaskPointer
    m_Mask=((MaskPointer)( (size_t)parameters[(5+NInputDimensions)*NInputDimensions]));

    //JV add the MaskPointer
    m_BulkTransform=((BulkTransformPointer)( (size_t)parameters[(5+NInputDimensions)*NInputDimensions+1]));

    //JV add the TransformShape
    m_TransformShape=((unsigned int)parameters[(5+NInputDimensions)*NInputDimensions+2]);

    // Set the members 
    this->SetSplineOrders( splineOrders );
    this->SetGridSpacing( spacing );
    this->SetGridDirection( direction );
    this->SetGridOrigin( origin );
    this->SetGridRegion( bsplineRegion );
    this->SetLUTSamplingFactors( samplingFactors );
      
  }


  // Wrap flat parameters as images
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::WrapAsImages()
  {
    //JV Wrap parameter array in vectorial image, changed parameter order: A1x A1y A1z, A2x ....
    PixelType * dataPointer =reinterpret_cast<PixelType *>( const_cast<double *>(m_InputParametersPointer->data_block() )) ;
    unsigned int numberOfPixels = m_GridRegion.GetNumberOfPixels();
    
    m_WrappedImage->GetPixelContainer()->SetImportPointer( dataPointer,numberOfPixels);//InputDimension
    m_CoefficientImage = m_WrappedImage;

    //=====================================
    //JV Create padded structure adding BC
    //=====================================
    PadCoefficientImage();
        
    //=====================================
    //JV Wrap jacobian into OutputDimension X Vectorial images
    //=====================================
#if ITK_VERSION_MAJOR >= 4
    this->m_SharedDataBSplineJacobian.set_size( OutputDimension, this->GetNumberOfParameters() );
#else
    this->m_Jacobian.set_size( OutputDimension, this->GetNumberOfParameters() );
#endif

    // Use memset to set the memory
    // JV four rows of three comps of parameters
#if ITK_VERSION_MAJOR >= 4
    JacobianPixelType * jacobianDataPointer = reinterpret_cast<JacobianPixelType *>(this->m_SharedDataBSplineJacobian.data_block());
#else
    JacobianPixelType * jacobianDataPointer = reinterpret_cast<JacobianPixelType *>(this->m_Jacobian.data_block());
#endif
    memset(jacobianDataPointer, 0,  OutputDimension*numberOfPixels*sizeof(JacobianPixelType));

    for (unsigned int j=0; j<OutputDimension; j++)
      {
	m_JacobianImage[j]->GetPixelContainer()->
	  SetImportPointer( jacobianDataPointer, numberOfPixels );
	jacobianDataPointer += numberOfPixels;
      }

    // Reset the J parameters 
    m_LastJacobianIndex = m_ValidRegion.GetIndex();
    m_FirstRegion.SetSize(m_NullSize);
    m_SecondRegion.SetSize(m_NullSize);
    for ( unsigned int j = 0; j < SpaceDimension ; j++ )
      {
	m_FirstIterator[j]= IteratorType( m_JacobianImage[j], m_FirstRegion);
	m_SecondIterator[j]= IteratorType( m_JacobianImage[j], m_SecondRegion);
      }

    m_BCValues.resize(0);
    m_BCRegions.resize(0);
    m_BCSize=0;
    m_BC2Values.resize(0);
    m_BC2Regions.resize(0);
    m_BC2Size=0;
    m_BC3Values.resize(0);
    m_BC3Regions.resize(0);
    m_BC3Size=0;

  }


  // Set the parameters by value
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetParametersByValue( const ParametersType & parameters )
  {

    // Check if the number of parameters match the
    // Expected number of parameters
    if ( parameters.Size() != this->GetNumberOfParameters() )
      {
	itkExceptionMacro(<<"Mismatched between parameters size "
			  << parameters.size() 
			  << " and region size "
			  << m_GridRegion.GetNumberOfPixels() );
      }

    // Copy it
    m_InternalParametersBuffer = parameters;
    m_InputParametersPointer = &m_InternalParametersBuffer;

    // Wrap flat array as images of coefficients
    this->WrapAsImages();

    //JV Set padded input to vector interpolator
    m_VectorInterpolator->SetInputImage(this->GetPaddedCoefficientImage());

    // Modified is always called since we just have a pointer to the
    // Parameters and cannot know if the parameters have changed.
    this->Modified();

  }

  // Get the parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  const 
  typename ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::ParametersType &
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetParameters( void ) const
  {
    /** NOTE: For efficiency, this class does not keep a copy of the parameters - 
     * it just keeps pointer to input parameters. 
     */
    if (NULL == m_InputParametersPointer)
      {
	itkExceptionMacro( <<"Cannot GetParameters() because m_InputParametersPointer is NULL. Perhaps SetCoefficientImage() has been called causing the NULL pointer." );
      }

    return (*m_InputParametersPointer);
  }


  // Get the parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  const 
  typename ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::ParametersType &
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetFixedParameters( void ) const
  {
    RegionType resRegion = this->GetGridRegion(  );
  
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	this->m_FixedParameters[i] = (resRegion.GetSize())[i];
      }
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	this->m_FixedParameters[NInputDimensions+i] = (this->GetGridOrigin())[i];
      } 
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	this->m_FixedParameters[2*NInputDimensions+i] =  (this->GetGridSpacing())[i];
      }
    for (unsigned int di=0; di<NInputDimensions; di++)
      {
	for (unsigned int dj=0; dj<NInputDimensions; dj++)
	  {
	    this->m_FixedParameters[3*NInputDimensions+(di*NInputDimensions+dj)] = (this->GetGridDirection())[di][dj];
	  }
      }

    //JV add splineOrders
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	this->m_FixedParameters[(3+NInputDimensions)*NInputDimensions+i] = (this->GetSplineOrders())[i];
      }
    
    //JV add LUTsamplingFactor
    for (unsigned int i=0; i<NInputDimensions; i++)
      {
	this->m_FixedParameters[(4+NInputDimensions)*NInputDimensions+i] = (this->GetLUTSamplingFactors())[i];
      }
    
    //JV add the mask
    this->m_FixedParameters[(5+NInputDimensions)*NInputDimensions]=(double)((size_t) m_Mask);

    //JV add the bulktransform pointer
    this->m_FixedParameters[(5+NInputDimensions)*NInputDimensions+1]=(double)((size_t) m_BulkTransform);

    //JV add the transform shape
    this->m_FixedParameters[(5+NInputDimensions)*NInputDimensions+2]=(double)( m_TransformShape);
    
    return (this->m_FixedParameters);
  }

  
  // Set the B-Spline coefficients using input images
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void 
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetCoefficientImage( CoefficientImagePointer image )
  {
    this->SetGridSpacing( image->GetSpacing() );
    this->SetGridOrigin( image->GetOrigin() );
    this->SetGridDirection( image->GetDirection() );
    this->SetGridRegion( image->GetBufferedRegion() );
    m_CoefficientImage = image;
    
    //JV
    //   m_WrappedImage=m_CoefficientImage;

    // Update the interpolator
    this->PadCoefficientImage();
    m_VectorInterpolator->SetInputImage(this->GetPaddedCoefficientImage());

    // Clean up buffered parameters
    m_InternalParametersBuffer = ParametersType( 0 );
    m_InputParametersPointer  = NULL;
      
  }  


  //   // Set the B-Spline coefficients using input images
  //   template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  //   void 
  //   ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  //   ::SetPaddedCoefficientImage( CoefficientImagePointer image )
  //   {  
  //     //JV modify the region
  //     typename CoefficientImageType::RegionType region=image->GetBufferedRegion();
  //     typename CoefficientImageType::RegionType::SizeType size=region.GetSize();
  //     size[InputDimension-1]-=2;
  //     region.SetSize(size);
    
  //     //Set properties
  //     this->SetGridRegion( region );
  //     this->SetGridSpacing( image->GetSpacing() );
  //     this->SetGridDirection( image->GetDirection() );
  //     this->SetGridOrigin( image->GetOrigin() );
  //     m_PaddedCoefficientImage = image;
  //     this->ExtractCoefficientImage();
  //     m_VectorInterpolator->SetInputImage(this->GetPaddedCoefficientImage());
    
  //     // Clean up buffered parameters
  //     m_InternalParametersBuffer = ParametersType( 0 );
  //     m_InputParametersPointer  = NULL;
      
  //   } 

  // Set the B-Spline coefficients using input images
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  typename   ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::CoefficientImageType::Pointer
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::ExtractTemporalRow(const typename CoefficientImageType::Pointer& coefficientImage, unsigned int temporalIndex)
  {
    // Region
    typename   CoefficientImageType::RegionType sourceRegion=coefficientImage->GetLargestPossibleRegion();
    sourceRegion.SetSize(InputDimension-1, 1);
    sourceRegion.SetIndex(InputDimension-1, temporalIndex);

    // Extract
    typedef clitk::ExtractImageFilter<CoefficientImageType, CoefficientImageType> ExtractImageFilterType;
    typename ExtractImageFilterType::Pointer extract=ExtractImageFilterType::New();
    extract->SetInput(coefficientImage);
    extract->SetExtractionRegion(sourceRegion);
    extract->Update();
    typename CoefficientImageType::Pointer row= extract->GetOutput();

    // Set index to zero
    sourceRegion.SetIndex(InputDimension-1, 0);
    row->SetRegions(sourceRegion);
    return row;

  }

  // Set the B-Spline coefficients using input images
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void 
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::PadCoefficientImage(void)
  {
  
    // Define paste, extract and combine filters 
    typedef itk::PasteImageFilter<CoefficientImageType, CoefficientImageType, CoefficientImageType> PasteImageFilterType;
    typedef clitk::ExtractImageFilter<CoefficientImageType, CoefficientImageType> ExtractImageFilterType;
    typedef clitk::LinearCombinationImageFilter<CoefficientImageType, CoefficientImageType> LinearCombinationFilterType;

    // Regions
    typename   CoefficientImageType::RegionType sourceRegion=m_PaddedCoefficientImage->GetLargestPossibleRegion();
    typename   CoefficientImageType::RegionType destinationRegion=m_PaddedCoefficientImage->GetLargestPossibleRegion();
    typename   CoefficientImageType::RegionType::SizeType sourceSize=sourceRegion.GetSize();
    typename   CoefficientImageType::RegionType::SizeType destinationSize=destinationRegion.GetSize();    
    typename   CoefficientImageType::IndexType sourceIndex=sourceRegion.GetIndex();
    typename   CoefficientImageType::IndexType destinationIndex=destinationRegion.GetIndex();
 
    // JV Padding depends on the shape 
    switch (m_TransformShape)
      {
	/*  The shapes are
	    0: egg     4 CP  3 DOF
	    1: egg     5 CP  4 DOF 
	    2: rabbit  4 CP  3 DOF 
	    3: rabbit  5 CP  4 DOF
	    4: sputnik 4 CP  4 DOF
	    5: sputnik 5 CP  5 DOF
	    6: diamond 6 CP  5 DOF
	    7: diamond 7 CP  6 DOF
	*/
	
      case 0:
	{
	  // ----------------------------------------------------------------------
	  // The egg with 4 internal CP (starting from inhale)
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and
	  // Coeff      row  BC1 BC2  0  BC3  1   2  BC4
	  // PaddedCoeff  R:  0   1   2   3   4   5   6 
	  // BC1= R4
	  // BC2= R5
	  // BC3= -weights[2]/weights[0] ( R2+R4 ) 
	  // BC4= R2
	  // ---------------------------------------------------------------------
	
	  //---------------------------------
	  // 1. First two temporal rows are identical: paste 1-2 to 0-1
	  typename PasteImageFilterType::Pointer paster0=PasteImageFilterType::New();
	  paster0->SetDestinationImage(m_PaddedCoefficientImage);
	  paster0->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(NInputDimensions-1,1);
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  destinationIndex[InputDimension-1]=0;
	  paster0->SetDestinationIndex(destinationIndex);
	  paster0->SetSourceRegion(sourceRegion);
	  
	  //---------------------------------
	  // 1. Next temporal row = paste 0 to 2 
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(paster0->GetOutput());
	  paster1->SetSourceImage(row0);
	  destinationIndex[InputDimension-1]=2;
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(row0->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 1, 2 (of coeff image)
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row0);
	  combine1->SetInput(1,row1);
	  combine1->SetA(-m_WeightRatio[2][0]);
	  combine1->SetB(-m_WeightRatio[2][0]);
	  combine1->Update();
	  typename CoefficientImageType::Pointer bc3Row=combine1->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc3Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc3Row->GetLargestPossibleRegion());
	
	  //---------------------------------
	  // 3. Next two temporal rows identical: paste 1,2 to 4,5
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(InputDimension-1,1);
	  sourceRegion.SetSize(InputDimension-1,2);
	  destinationIndex[InputDimension-1]=4;
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(sourceRegion);
	
	  // ---------------------------------
	  // 4. Row at index 6=BC4= R2
	  // Paste BC3 row at index 5
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(row0);
	  destinationIndex[InputDimension-1]=6;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(row0->GetLargestPossibleRegion());
	
	  // Update the chain!
	  paster4->Update();
	  m_PaddedCoefficientImage= paster4->GetOutput();

	  break;
	}
	
      case 1:
	{
	  // ----------------------------------------------------------------------
	  // The egg with 5 internal CP (starting from inhale)
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and
	  // Coeff      row  BC1 BC2  0  BC3  1   2   3  BC4
	  // PaddedCoeff  R:  0   1   2   3   4   5   6   7
	  // BC1= R5
	  // BC2= R6
	  // BC3= -weights[3]/weights[1] ( R2+R5 ) - R4 
	  // BC4= R2
	  // ---------------------------------------------------------------------
	  //---------------------------------
	  // 1. First two temporal rows are identical: paste 2-3 to 0-1
	  typename PasteImageFilterType::Pointer paster0=PasteImageFilterType::New();
	  paster0->SetDestinationImage(m_PaddedCoefficientImage);
	  paster0->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(NInputDimensions-1,2);
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  destinationIndex[InputDimension-1]=0;
	  paster0->SetDestinationIndex(destinationIndex);
	  paster0->SetSourceRegion(sourceRegion);
	  
	  //---------------------------------
	  // 1. Next temporal row = paste 0 to 2 
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(paster0->GetOutput());
	  paster1->SetSourceImage(row0);
	  destinationIndex[InputDimension-1]=2;
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(row0->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 1, 2 (of coeff image)
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename CoefficientImageType::Pointer row2=this->ExtractTemporalRow(m_CoefficientImage, 2);
	  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row0);
	  combine1->SetInput(1,row2);
	  combine1->SetA(1);
	  combine1->SetB(1);
	  // combine1->Update();
	  typename LinearCombinationFilterType::Pointer combine2=LinearCombinationFilterType::New();
	  combine2->SetInput(0,row1);
	  combine2->SetInput(1,combine1->GetOutput());
	  combine2->SetA(-1.);
	  combine2->SetB(-m_WeightRatio[3][1]);
	  combine2->Update();
	  typename CoefficientImageType::Pointer bc3Row=combine2->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc3Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc3Row->GetLargestPossibleRegion());
	
	  //---------------------------------
	  // 3. Next three temporal rows identical: paste 1,2,3 to 4,5,6
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(InputDimension-1,1);
	  sourceRegion.SetSize(InputDimension-1,3);
	  destinationIndex[InputDimension-1]=4;
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(sourceRegion);
	
	  // ---------------------------------
	  // 4. Row at index 7=BC4= R2
	  // Paste BC3 row at index 5
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(row0);
	  destinationIndex[InputDimension-1]=7;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(row0->GetLargestPossibleRegion());
	
	  // Update the chain!
	  paster4->Update();
	  m_PaddedCoefficientImage= paster4->GetOutput();

	  break;
	}


// 	  // ----------------------------------------------------------------------
// 	  // The egg with 5 internal CP: 
// 	  // Periodic, C2 smooth everywhere and constrained to zero at the reference
// 	  // Coeff       row R5 BC1  0   1   2   3  BC2  R2  
// 	  // PaddedCoeff R:  0   1   2   3   4   5   6   7    
// 	  // BC1= -weights[2]/weights[0] ( R2+R5)
// 	  // BC2= BC1  
// 	  // ---------------------------------------------------------------------

// 	  // Extract rows with index 0 and 3
// 	  typename    CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
// 	  typename    CoefficientImageType::Pointer row3=this->ExtractTemporalRow(m_CoefficientImage, 3);
	  
// 	  // Paste the first row
// 	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
// 	  destinationIndex.Fill(0);
// 	  paster1->SetDestinationIndex(destinationIndex);
// 	  paster1->SetSourceRegion(row3->GetLargestPossibleRegion());
// 	  paster1->SetSourceImage(row3);
// 	  paster1->SetDestinationImage(m_PaddedCoefficientImage);

// 	  // Linearly Combine rows for BC1 and BC2
// 	  typedef clitk::LinearCombinationImageFilter<CoefficientImageType, CoefficientImageType> LinearCombinationFilterType;
// 	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
// 	  combine1->SetFirstInput(row0);
// 	  combine1->SetSecondInput(row3);
// 	  combine1->SetA(-m_WeightRatio[2][0]);
// 	  combine1->SetB(-m_WeightRatio[2][0]);
// 	  combine1->Update();
// 	  typename CoefficientImageType::Pointer bcRow=combine1->GetOutput();

// 	  // Paste the second row
// 	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
// 	  destinationIndex[InputDimension-1]=1;
// 	  paster2->SetDestinationIndex(destinationIndex);
// 	  paster2->SetSourceRegion(bcRow->GetLargestPossibleRegion());
// 	  paster2->SetSourceImage(bcRow);
// 	  paster2->SetDestinationImage(paster1->GetOutput());

// 	  // Paste the coefficientImage
// 	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
// 	  destinationIndex[InputDimension-1]=2;
// 	  paster3->SetDestinationIndex(destinationIndex);
// 	  paster3->SetSourceRegion(m_CoefficientImage->GetLargestPossibleRegion());
// 	  paster3->SetSourceImage(m_CoefficientImage);
// 	  paster3->SetDestinationImage(paster2->GetOutput());

// 	  // Paste the last two rows
// 	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
// 	  destinationIndex[InputDimension-1]=6;
// 	  paster4->SetDestinationIndex(destinationIndex);
// 	  paster4->SetSourceRegion(bcRow->GetLargestPossibleRegion());
// 	  paster4->SetSourceImage(bcRow);
// 	  paster4->SetDestinationImage(paster3->GetOutput());

// 	  typename PasteImageFilterType::Pointer paster5=PasteImageFilterType::New();
// 	  destinationIndex[InputDimension-1]=7;
// 	  paster5->SetDestinationIndex(destinationIndex);
// 	  paster5->SetSourceRegion(row0->GetLargestPossibleRegion());
// 	  paster5->SetSourceImage(row0);
// 	  paster5->SetDestinationImage(paster4->GetOutput());
	  
// 	  // Update the chain!
// 	  paster5->Update();
// 	  m_PaddedCoefficientImage= paster5->GetOutput();
	  
// 	  break;
// 	}

      case 2:
	{
	  // ----------------------------------------------------------------------
	  // The rabbit with 4 internal CP
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and the extremes fixed with anit-symm bc
	  // Coeff      row  BC1  0   1  BC2  2  BC3 BC4 
	  // PaddedCoeff  R:  0   1   2   3   4   5   6 
	  // BC1= 2*R1-R2
	  // BC2= -weights[2]/weights[0] ( R2+R4 )
	  // BC3=  R1
	  // BC4= 2*R1-R4
	  // ---------------------------------------------------------------------

	  // ---------------------------------
	  // 0. First Row =BC1
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename LinearCombinationFilterType::Pointer combine0=LinearCombinationFilterType::New();
	  combine0->SetInput(0,row0);
	  combine0->SetInput(1,row1);
	  combine0->SetA(2.);
	  combine0->SetB(-1.);
	  combine0->Update();
	  typename CoefficientImageType::Pointer bc1Row=combine0->GetOutput();

	  typename PasteImageFilterType::Pointer paster0=PasteImageFilterType::New();
	  paster0->SetDestinationImage(m_PaddedCoefficientImage);
	  paster0->SetSourceImage(bc1Row);
	  destinationIndex[InputDimension-1]=0;
	  paster0->SetDestinationIndex(destinationIndex);
	  paster0->SetSourceRegion(bc1Row->GetLargestPossibleRegion());

	  //---------------------------------
	  // 1. Next two temporal rows are identical: paste 0-1 to 1-2
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(paster0->GetOutput());
	  paster1->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(NInputDimensions-1,0);
	  destinationIndex[InputDimension-1]=1;
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 1, 2 (of coeff image)
	  typename CoefficientImageType::Pointer row2=this->ExtractTemporalRow(m_CoefficientImage, 2);
  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row1);
	  combine1->SetInput(1,row2);
	  combine1->SetA(-m_WeightRatio[2][0]);
	  combine1->SetB(-m_WeightRatio[2][0]);
	  combine1->Update();
	  typename CoefficientImageType::Pointer bc2Row=combine1->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc2Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc2Row->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 3. Next temporal row is identical: paste 2 to 4
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(row2);
	  destinationIndex[InputDimension-1]=4;
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(row2->GetLargestPossibleRegion());

	  // ---------------------------------
	  // 4. Row at index 5=BC (paddedcoeff image) R1
	  // Paste BC3 row at index 5
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(row0);
	  destinationIndex[InputDimension-1]=5;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(row0->GetLargestPossibleRegion());
    
	  // ---------------------------------
	  // 5. Paste BC4 row at index 6
	  typename LinearCombinationFilterType::Pointer combine3=LinearCombinationFilterType::New();
	  combine3->SetInput(0,row0);
	  combine3->SetInput(1,row2);
	  combine3->SetA(2.);
	  combine3->SetB(-1.);
	  combine3->Update();
	  typename CoefficientImageType::Pointer bc4Row=combine3->GetOutput();
	  typename PasteImageFilterType::Pointer paster5=PasteImageFilterType::New();
	  paster5->SetDestinationImage(paster4->GetOutput());
	  paster5->SetSourceImage(bc4Row);
	  destinationIndex[InputDimension-1]=6;
	  paster5->SetDestinationIndex(destinationIndex);
	  paster5->SetSourceRegion(bc4Row->GetLargestPossibleRegion());
	  
	  // Update the chain!
	  paster5->Update();
	  m_PaddedCoefficientImage= paster5->GetOutput();

	  break;
	}
     
      case 3: 
	{
	  // ----------------------------------------------------------------------
	  // The rabbit with 5 internal CP
	  // Periodic, constrained to zero at the reference at position 3.5 
	  // and the extremes fixed with anti-symmetrical BC
	  // Coeff      row  BC1  0   1  BC2  2   3  BC3 BC4
	  // PaddedCoeff  R:  0   1   2   3   4   5   6   7    
	  // BC1= 2*R1-R2
	  // BC2= -weights[3]/weights[1] ( R2+R5 ) - R4
	  // BC3= R1
	  // BC4= 2*R1-R5
	  // ---------------------------------------------------------------------

	  // ---------------------------------
	  // 0. First Row =BC1
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename LinearCombinationFilterType::Pointer combine0=LinearCombinationFilterType::New();
	  combine0->SetInput(0,row0);
	  combine0->SetInput(1,row1);
	  combine0->SetA(2.);
	  combine0->SetB(-1.);
	  combine0->Update();
	  typename CoefficientImageType::Pointer bc1Row=combine0->GetOutput();

	  typename PasteImageFilterType::Pointer paster0=PasteImageFilterType::New();
	  paster0->SetDestinationImage(m_PaddedCoefficientImage);
	  paster0->SetSourceImage(bc1Row);
	  destinationIndex[InputDimension-1]=0;
	  paster0->SetDestinationIndex(destinationIndex);
	  paster0->SetSourceRegion(bc1Row->GetLargestPossibleRegion());

	  //---------------------------------
	  // 1. Next two temporal rows are identical: paste 0-1 to 1-2
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(paster0->GetOutput());
	  paster1->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(InputDimension-1,0);
	  destinationIndex[InputDimension-1]=1;
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 1, 3 (of coeff image)
	  //typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename CoefficientImageType::Pointer row3=this->ExtractTemporalRow(m_CoefficientImage, 3);
  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row1);
	  combine1->SetInput(1,row3);
	  combine1->SetA(1.);
	  combine1->SetB(1.);
	  combine1->Update();
	  
	  // Extract row at index 2 (coeff image)
	  typename CoefficientImageType::Pointer row2=this->ExtractTemporalRow(m_CoefficientImage, 2);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine2=LinearCombinationFilterType::New();
	  combine2->SetInput(0,combine1->GetOutput());
	  combine2->SetInput(1,row2);
	  combine2->SetA(-m_WeightRatio[3][1]);
	  combine2->SetB(-1.);
	  combine2->Update();
	  typename CoefficientImageType::Pointer bc2Row=combine2->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc2Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc2Row->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 3. Next two temporal rows are identical: paste 2,3 to 4,5
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(InputDimension-1,2);
	  destinationIndex[InputDimension-1]=4;
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(sourceRegion);

	  // ---------------------------------
	  // 4. Row at index 6=BC (paddedcoeff image)R1
	  // Paste BC3 row at index 6
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(row0);
	  destinationIndex[InputDimension-1]=6;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(row0->GetLargestPossibleRegion());

	  // ---------------------------------
	  // 5. Paste BC4 row at index 7
	  typename LinearCombinationFilterType::Pointer combine3=LinearCombinationFilterType::New();
	  combine3->SetInput(0,row0);
	  combine3->SetInput(1,row3);
	  combine3->SetA(2.);
	  combine3->SetB(-1.);
	  combine3->Update();
	  typename CoefficientImageType::Pointer bc4Row=combine3->GetOutput();
	  typename PasteImageFilterType::Pointer paster5=PasteImageFilterType::New();
	  paster5->SetDestinationImage(paster4->GetOutput());
	  paster5->SetSourceImage(bc4Row);
	  destinationIndex[InputDimension-1]=7;
	  paster5->SetDestinationIndex(destinationIndex);
	  paster5->SetSourceRegion(bc4Row->GetLargestPossibleRegion());
	  
	  // Update the chain!
	  paster5->Update();
	  m_PaddedCoefficientImage= paster5->GetOutput();

	  break;
	}
	
      case 4: 
	{
	  // ----------------------------------------------------------------------
	  // The sputnik with 4 internal CP
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and one indepent extremes copied
	  // Coeff     row BC1  0   1  BC2  2  BC2  3
	  // PaddedCoeff R: 0   1   2   3   4   5   6      
	  // BC1= R6
	  // BC2= -weights[2]/weights[0] ( R2+R4 )
	  // BC3=  weights[2]/weights[0] ( R2-R4) + R1
	  // ---------------------------------------------------------------------

	  //---------------------------------
	  // 1. First Row is equal to last row: paste 3 row to 0
	  typename CoefficientImageType::Pointer row3=this->ExtractTemporalRow(m_CoefficientImage, 3);
	  typename PasteImageFilterType::Pointer paster0=PasteImageFilterType::New();
	  paster0->SetDestinationImage(m_PaddedCoefficientImage);
	  paster0->SetSourceImage(row3);
	  destinationIndex[InputDimension-1]=0;
	  paster0->SetDestinationIndex(destinationIndex);
	  paster0->SetSourceRegion(row3->GetLargestPossibleRegion());

	  //---------------------------------
	  // 1. Next two temporal rows are identical: paste 0-1 to 1-2
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(paster0->GetOutput());
	  paster1->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(NInputDimensions-1,0);
	  destinationIndex[InputDimension-1]=1;
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 1, 2 (of coeff image)
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename CoefficientImageType::Pointer row2=this->ExtractTemporalRow(m_CoefficientImage, 2);
  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row1);
	  combine1->SetInput(1,row2);
	  combine1->SetA(-m_WeightRatio[2][0]);
	  combine1->SetB(-m_WeightRatio[2][0]);
	  combine1->Update();
	  typename CoefficientImageType::Pointer bc1Row=combine1->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc1Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc1Row->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 3. Next temporal row is identical: paste 2 to 4
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(row2);
	  destinationIndex[InputDimension-1]=4;
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(row2->GetLargestPossibleRegion());

	  //---------------------------------
	  // 4. Final row at index 5=BC3 (paddedcoeff image)R1 R2 R4 corresponds to index in coeff image 0 1 2
 	  // Extract row at index 1, 2 (of coeff image)already done
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
    
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine3=LinearCombinationFilterType::New();
	  combine3->SetInput(0,row1);
	  combine3->SetInput(1,row2);
	  combine3->SetA(1.);
	  combine3->SetB(-1.);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine4=LinearCombinationFilterType::New();
	  combine4->SetInput(0,combine3->GetOutput());
	  combine4->SetInput(1,row0);
	  combine4->SetA(m_WeightRatio[2][0]);
	  combine4->SetB(1.);
	  combine4->Update();
	  typename CoefficientImageType::Pointer bc2Row=combine4->GetOutput();

	  // Paste BC row at index 5
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(bc2Row);
	  destinationIndex[InputDimension-1]=5;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(bc2Row->GetLargestPossibleRegion());
    
	  // Paste row 3  at index 6
	  typename PasteImageFilterType::Pointer paster5=PasteImageFilterType::New();
	  paster5->SetDestinationImage(paster4->GetOutput());
	  paster5->SetSourceImage(row3);
	  destinationIndex[InputDimension-1]=6;
	  paster5->SetDestinationIndex(destinationIndex);
	  paster5->SetSourceRegion(row3->GetLargestPossibleRegion());
	  
	  // Update the chain!
	  paster5->Update();
	  m_PaddedCoefficientImage= paster5->GetOutput();

	  break;
	}

      case 5: 
	{

	  // ----------------------------------------------------------------------
	  // The sputnik with 5 internal CP
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and one indepent extreme
	  // Coeff     row BC1  0   1  BC2  2   3  BC3  4
	  // PaddedCoeff R: 0   1   2   3   4   5   6   7   
	  // BC1= R2 + R5 - R7
	  // BC2= -weights[3]/weights[1] ( R2+R5 ) - R4
	  // BC3= R1 + 0.5 R2 - 0.5 R7
	  // ----------------------------------------------------------------------
	  //---------------------------------
	  // 1. First Row =BC 
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename CoefficientImageType::Pointer row3=this->ExtractTemporalRow(m_CoefficientImage, 3);
	  typename CoefficientImageType::Pointer row4=this->ExtractTemporalRow(m_CoefficientImage, 4);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine0=LinearCombinationFilterType::New();
	  combine0->SetInput(0,row1);
	  combine0->SetInput(1,row3);
	  combine0->SetA(1.);
	  combine0->SetB(1.);
	  //combine0->Update();
	  typename LinearCombinationFilterType::Pointer combine0bis=LinearCombinationFilterType::New();
	  combine0bis->SetInput(0,combine0->GetOutput());
	  combine0bis->SetInput(1,row4);
	  combine0bis->SetA(1.);
	  combine0bis->SetB(-1.);
	  combine0bis->Update();
	  typename CoefficientImageType::Pointer bc1Row=combine0bis->GetOutput();

	  typename PasteImageFilterType::Pointer paster0=PasteImageFilterType::New();
	  paster0->SetDestinationImage(m_PaddedCoefficientImage);
	  paster0->SetSourceImage(bc1Row);
	  destinationIndex[InputDimension-1]=0;
	  paster0->SetDestinationIndex(destinationIndex);
	  paster0->SetSourceRegion(bc1Row->GetLargestPossibleRegion());

	  //---------------------------------
	  // 1. Next two temporal rows are identical: paste 0-1 to 1-2
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(paster0->GetOutput());
	  paster1->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(NInputDimensions-1,0);
	  destinationIndex[InputDimension-1]=1;
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 1, 2 (of coeff image)
	  //typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename CoefficientImageType::Pointer row2=this->ExtractTemporalRow(m_CoefficientImage, 2);
  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row1);
	  combine1->SetInput(1,row3);
	  combine1->SetA(1.);
	  combine1->SetB(1.);
	  combine1->Update();

	  typename LinearCombinationFilterType::Pointer combine2=LinearCombinationFilterType::New();
	  combine2->SetInput(0,combine1->GetOutput());
	  combine2->SetInput(1,row2);
	  combine2->SetA(-m_WeightRatio[3][1]);
	  combine2->SetB(-1.);
	  combine2->Update();
	  typename CoefficientImageType::Pointer bc2Row=combine2->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc2Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc2Row->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 3. Next two temporal rows are identical: paste 2,3 to 4,5
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(InputDimension-1,2);
	  destinationIndex[InputDimension-1]=4;
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 4. Final row at index 6=BC3
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
    
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine3=LinearCombinationFilterType::New();
	  combine3->SetInput(0,row1);
	  combine3->SetInput(1,row4);
	  combine3->SetA(1.);
	  combine3->SetB(-1.);
	  typename LinearCombinationFilterType::Pointer combine4=LinearCombinationFilterType::New();
	  combine4->SetInput(0,row0);
	  combine4->SetInput(1,combine3->GetOutput());
	  combine4->SetA(1.);
	  combine4->SetB(.5);
	  combine4->Update();
	  typename CoefficientImageType::Pointer bc3Row=combine4->GetOutput();

	  // Paste BC row at index 6
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(bc3Row);
	  destinationIndex[InputDimension-1]=6;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(bc3Row->GetLargestPossibleRegion());
    
	  // Paste row 4  at index 7
	  typename PasteImageFilterType::Pointer paster5=PasteImageFilterType::New();
	  paster5->SetDestinationImage(paster4->GetOutput());
	  paster5->SetSourceImage(row4);
	  destinationIndex[InputDimension-1]=7;
	  paster5->SetDestinationIndex(destinationIndex);
	  paster5->SetSourceRegion(row4->GetLargestPossibleRegion());
	  
	  // Update the chain!
	  paster5->Update();
	  m_PaddedCoefficientImage= paster5->GetOutput();

	  break;
	}

      case 6: 
	{
	  // ----------------------------------------------------------------------
	  // The diamond with 4 internal CP:
	  // Periodic, constrained to zero at the reference at position 3
	  // Coeff      row 0   1   2  BC1  3  BC2  4
	  // PaddedCoeff  R:0   1   2   3   4   5   6    
	  // BC1= -weights[2]/weights[0] ( R2+R4 )
	  // BC2= weights[2]/weights[0]  ( R0+R2-R4-R6 ) + R1
	  // ---------------------------------------------------------------------

	  //---------------------------------
	  // 1. First Three temporal rows are identical: paste 0-2 to 0-2
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(m_PaddedCoefficientImage);
	  paster1->SetSourceImage(m_CoefficientImage);
	  sourceIndex.Fill(0);
	  destinationIndex.Fill(0);
	  sourceSize[NInputDimensions-1]=3;
	  sourceRegion.SetSize(sourceSize);
	  sourceRegion.SetIndex(sourceIndex);
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 0, 4 (of coeff image)
	  typename CoefficientImageType::Pointer row2=this->ExtractTemporalRow(m_CoefficientImage, 2);
	  typename CoefficientImageType::Pointer row3=this->ExtractTemporalRow(m_CoefficientImage, 3);
  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row2);
	  combine1->SetInput(1,row3);
	  combine1->SetA(-m_WeightRatio[2][0]);
	  combine1->SetB(-m_WeightRatio[2][0]);
	  combine1->Update();
	  typename CoefficientImageType::Pointer bc1Row=combine1->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc1Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc1Row->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 3. Next  row identical: paste 3 to 4
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(row3);
	  destinationIndex[InputDimension-1]=4;
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(row3->GetLargestPossibleRegion());

	  //---------------------------------
	  // 4. Final row at index 6=BC (paddedcoeff image)R0 R2 R5 R7 R1 corresponds to index in coeff image 0 2 4 5 1
 	  // Extract row at index 0, 2 (of coeff image)already done
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
	  typename CoefficientImageType::Pointer row4=this->ExtractTemporalRow(m_CoefficientImage, 4);
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
    
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine3=LinearCombinationFilterType::New();
	  combine3->SetInput(0,row0);
	  combine3->SetInput(1,row2);
	  combine3->SetA(1.);
	  combine3->SetB(1.);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine4=LinearCombinationFilterType::New();
	  combine4->SetInput(0,row3);
	  combine4->SetInput(1,row4);
	  combine4->SetA(1.);
	  combine4->SetB(1.);
	  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine5=LinearCombinationFilterType::New();
	  combine5->SetInput(0,combine3->GetOutput());
	  combine5->SetInput(1,combine4->GetOutput());
	  combine5->SetA(1.);
	  combine5->SetB(-1.);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine6=LinearCombinationFilterType::New();
	  combine6->SetInput(0,combine5->GetOutput());
	  combine6->SetInput(1,row1);
	  combine6->SetA(m_WeightRatio[2][0]);
	  combine6->SetB(1.);
	  combine6->Update();
	  typename CoefficientImageType::Pointer bc2Row=combine6->GetOutput();

	  // Paste BC row at index 5
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(bc2Row);
	  destinationIndex[InputDimension-1]=5;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(bc2Row->GetLargestPossibleRegion());
    
	  // Paste last row at index 6
	  typename PasteImageFilterType::Pointer paster5=PasteImageFilterType::New();
	  paster5->SetDestinationImage(paster4->GetOutput());
	  paster5->SetSourceImage(row4);
	  destinationIndex[InputDimension-1]=6;
	  paster5->SetDestinationIndex(destinationIndex);
	  paster5->SetSourceRegion(row4->GetLargestPossibleRegion());
	  
	  // Update the chain!
	  paster5->Update();
	  m_PaddedCoefficientImage= paster5->GetOutput();

	  break;
	}
      case 7: 
	{
	  // ----------------------------------------------------------------------
	  // The diamond with 5 internal CP: 
	  // periodic, constrained to zero at the reference at position 3.5
	  // Coeff      row 0   1   2  BC1  3   4  BC2  5
	  // PaddedCoeff  R:0   1   2   3   4   5   6   7    
	  // BC1= -weights[3]/weights[1] ( R2+R5 ) - R4
	  // BC2= weights[2]/weights[0] ( R0+R2-R5-R7 ) + R1
	  // ---------------------------------------------------------------------

	  //---------------------------------
	  // 1. First Three temporal rows are identical: paste 0-2 to 0-2
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(m_PaddedCoefficientImage);
	  paster1->SetSourceImage(m_CoefficientImage);
	  sourceIndex.Fill(0);
	  destinationIndex.Fill(0);
	  sourceSize[NInputDimensions-1]=3;
	  sourceRegion.SetSize(sourceSize);
	  sourceRegion.SetIndex(sourceIndex);
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 0, 4 (of coeff image)
	  typename CoefficientImageType::Pointer row2=this->ExtractTemporalRow(m_CoefficientImage, 2);
	  typename CoefficientImageType::Pointer row4=this->ExtractTemporalRow(m_CoefficientImage, 4);
  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row2);
	  combine1->SetInput(1,row4);
	  combine1->SetA(1.);
	  combine1->SetB(1.);
	  combine1->Update();
	  
	  // Extract row at index 3 (coeff image)
	  typename CoefficientImageType::Pointer row3=this->ExtractTemporalRow(m_CoefficientImage, 3);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine2=LinearCombinationFilterType::New();
	  combine2->SetInput(0,combine1->GetOutput());
	  combine2->SetInput(1,row3);
	  combine2->SetA(-m_WeightRatio[3][1] );
	  combine2->SetB(-1.);
	  combine2->Update();
	  typename CoefficientImageType::Pointer bc1Row=combine2->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc1Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc1Row->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 3. Next two temporal rows are identical: paste 3,4 to 4,5
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(m_CoefficientImage);
	  sourceIndex[InputDimension-1]=3;
	  destinationIndex[InputDimension-1]=4;
	  sourceSize[NInputDimensions-1]=2;
	  sourceRegion.SetSize(sourceSize);
	  sourceRegion.SetIndex(sourceIndex);
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 4. Final row at index 6=BC (paddedcoeff image)R0 R2 R5 R7 R1 corresponds to index in coeff image 0 2 4 5 1
 	  // Extract row at index 0, 2 (of coeff image)already done
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
	  typename CoefficientImageType::Pointer row5=this->ExtractTemporalRow(m_CoefficientImage, 5);
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
    
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine3=LinearCombinationFilterType::New();
	  combine3->SetInput(0,row0);
	  combine3->SetInput(1,row2);
	  combine3->SetA(1.);
	  combine3->SetB(1.);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine4=LinearCombinationFilterType::New();
	  combine4->SetInput(0,row4);
	  combine4->SetInput(1,row5);
	  combine4->SetA(1.);
	  combine4->SetB(1.);
	  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine5=LinearCombinationFilterType::New();
	  combine5->SetInput(0,combine3->GetOutput());
	  combine5->SetInput(1,combine4->GetOutput());
	  combine5->SetA(1.);
	  combine5->SetB(-1.);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine6=LinearCombinationFilterType::New();
	  combine6->SetInput(0,combine5->GetOutput());
	  combine6->SetInput(1,row1);
	  combine6->SetA(m_WeightRatio[2][0]);
	  combine6->SetB(1.);
	  combine6->Update();
	  typename CoefficientImageType::Pointer bc2Row=combine6->GetOutput();

	  // Paste BC row at index 6
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(bc2Row);
	  destinationIndex[InputDimension-1]=6;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(bc2Row->GetLargestPossibleRegion());
    
	  // Paste last row at index 7
	  typename PasteImageFilterType::Pointer paster5=PasteImageFilterType::New();
	  paster5->SetDestinationImage(paster4->GetOutput());
	  paster5->SetSourceImage(row5);
	  destinationIndex[InputDimension-1]=7;
	  paster5->SetDestinationIndex(destinationIndex);
	  paster5->SetSourceRegion(row5->GetLargestPossibleRegion());
	  
	  // Update the chain!
	  paster5->Update();
	  m_PaddedCoefficientImage= paster5->GetOutput();

	  break;
	}

      case 9:
	{
	  // ----------------------------------------------------------------------
	  // The sputnik with 5 internal CP T''(0)=T''(10)
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and one indepent extreme
	  // Coeff     row BC1  0   1  BC2  2   3  BC3  4
	  // PaddedCoeff R: 0   1   2   3   4   5   6   7   
	  // BC1= -R2+R5+R7
	  // BC2= -weights[3]/weights[1] ( R2+R5 ) - R4
	  // BC3= R1
	  // ---------------------------------------------------------------------
	 
	  //---------------------------------
	  // 1. First Row =BC 
	  typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename CoefficientImageType::Pointer row3=this->ExtractTemporalRow(m_CoefficientImage, 3);
	  typename CoefficientImageType::Pointer row4=this->ExtractTemporalRow(m_CoefficientImage, 4);

	  // Combine
	  typename LinearCombinationFilterType::Pointer combine0=LinearCombinationFilterType::New();
	  combine0->SetInput(0,row3);
	  combine0->SetInput(1,row4);
	  combine0->SetA(1.);
	  combine0->SetB(1.);
	  typename LinearCombinationFilterType::Pointer combine0bis=LinearCombinationFilterType::New();
	  combine0bis->SetInput(0,combine0->GetOutput());
	  combine0bis->SetInput(1,row1);
	  combine0bis->SetA(1.);
	  combine0bis->SetB(-1.);
	  combine0bis->Update();
	  typename CoefficientImageType::Pointer bc1Row=combine0bis->GetOutput();

	  typename PasteImageFilterType::Pointer paster0=PasteImageFilterType::New();
	  paster0->SetDestinationImage(m_PaddedCoefficientImage);
	  paster0->SetSourceImage(bc1Row);
	  destinationIndex[InputDimension-1]=0;
	  paster0->SetDestinationIndex(destinationIndex);
	  paster0->SetSourceRegion(bc1Row->GetLargestPossibleRegion());

	  //---------------------------------
	  // 1. Next two temporal rows are identical: paste 0-1 to 1-2
	  typename PasteImageFilterType::Pointer paster1=PasteImageFilterType::New();
	  paster1->SetDestinationImage(paster0->GetOutput());
	  paster1->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(NInputDimensions-1,0);
	  destinationIndex[InputDimension-1]=1;
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  paster1->SetDestinationIndex(destinationIndex);
	  paster1->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 2. Middle row at index 3=BC
	  // Extract row at index 1, 2 (of coeff image)
	  // typename CoefficientImageType::Pointer row1=this->ExtractTemporalRow(m_CoefficientImage, 1);
	  typename CoefficientImageType::Pointer row2=this->ExtractTemporalRow(m_CoefficientImage, 2);
  
	  // Combine
	  typename LinearCombinationFilterType::Pointer combine1=LinearCombinationFilterType::New();
	  combine1->SetInput(0,row1);
	  combine1->SetInput(1,row3);
	  combine1->SetA(1.);
	  combine1->SetB(1.);
	  combine1->Update();

	  typename LinearCombinationFilterType::Pointer combine2=LinearCombinationFilterType::New();
	  combine2->SetInput(0,combine1->GetOutput());
	  combine2->SetInput(1,row2);
	  combine2->SetA(-m_WeightRatio[3][1]);
	  combine2->SetB(-1.);
	  combine2->Update();
	  typename CoefficientImageType::Pointer bc2Row=combine2->GetOutput();

	  // Paste middleRow at index 3 (padded coeff)
	  typename PasteImageFilterType::Pointer paster2=PasteImageFilterType::New();
	  paster2->SetDestinationImage(paster1->GetOutput());
	  paster2->SetSourceImage(bc2Row);
	  destinationIndex[InputDimension-1]=3;
	  paster2->SetDestinationIndex(destinationIndex);
	  paster2->SetSourceRegion(bc2Row->GetLargestPossibleRegion());
	  
	  //---------------------------------
	  // 3. Next two temporal rows are identical: paste 2,3 to 4,5
	  typename PasteImageFilterType::Pointer paster3=PasteImageFilterType::New();
	  paster3->SetDestinationImage(paster2->GetOutput());
	  paster3->SetSourceImage(m_CoefficientImage);
	  sourceRegion.SetIndex(InputDimension-1,2);
	  destinationIndex[InputDimension-1]=4;
	  sourceRegion.SetSize(NInputDimensions-1,2);
	  paster3->SetDestinationIndex(destinationIndex);
	  paster3->SetSourceRegion(sourceRegion);

	  //---------------------------------
	  // 4. Final row at index 6=BC3
	  typename CoefficientImageType::Pointer row0=this->ExtractTemporalRow(m_CoefficientImage, 0);
    
	  // 	  // Combine
	  // 	  typename LinearCombinationFilterType::Pointer combine3=LinearCombinationFilterType::New();
	  // 	  combine3->SetInput(0,row0);
	  // 	  combine3->SetInput(1,row1);
	  // 	  combine3->SetA(1.);
	  // 	  combine3->SetB(0.5);
	  // 	  combine3->Update();
	  // 	  typename CoefficientImageType::Pointer bc3Row=combine3->GetOutput();

	  // Paste BC row at index 6
	  typename PasteImageFilterType::Pointer paster4=PasteImageFilterType::New();
	  paster4->SetDestinationImage(paster3->GetOutput());
	  paster4->SetSourceImage(row0);
	  destinationIndex[InputDimension-1]=6;
	  paster4->SetDestinationIndex(destinationIndex);
	  paster4->SetSourceRegion(row0->GetLargestPossibleRegion());
    
	  // Paste row 4  at index 7
	  typename PasteImageFilterType::Pointer paster5=PasteImageFilterType::New();
	  paster5->SetDestinationImage(paster4->GetOutput());
	  paster5->SetSourceImage(row4);
	  destinationIndex[InputDimension-1]=7;
	  paster5->SetDestinationIndex(destinationIndex);
	  paster5->SetSourceRegion(row4->GetLargestPossibleRegion());
	  
	  // Update the chain!
	  paster5->Update();
	  m_PaddedCoefficientImage= paster5->GetOutput();

	  break;
	}

      default:
	DD ("Shape not available");
      }
    
  }
  
  
  // // Extract coefficients from padded version
  // template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  // void 
  // ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  // ::ExtractCoefficientImage( )
  // {
  //   ////DD("extract coeff image");
  //   typename ExtractImageFilterType::Pointer extractImageFilter = ExtractImageFilterType::New();
  //   typename CoefficientImageType::RegionType extractionRegion=m_PaddedCoefficientImage->GetLargestPossibleRegion();
  //   typename CoefficientImageType::RegionType::SizeType extractionSize=extractionRegion.GetSize();
  //   typename CoefficientImageType::RegionType::IndexType extractionIndex = extractionRegion.GetIndex();
  //   extractionSize[InputDimension-1]-=4;    
  //   extractionIndex[InputDimension-1]=2;
  //   extractionRegion.SetSize(extractionSize);
  //   extractionRegion.SetIndex(extractionIndex);
  //   extractImageFilter->SetInput(m_PaddedCoefficientImage);
  //   extractImageFilter->SetExtractionRegion(extractionRegion);
  //   extractImageFilter->Update();
  //   m_CoefficientImage=extractImageFilter->GetOutput();
  // } 


  // Print self
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::PrintSelf(std::ostream &os, itk::Indent indent) const
  {

    this->Superclass::PrintSelf(os, indent);

    os << indent << "GridRegion: " << m_GridRegion << std::endl;
    os << indent << "GridOrigin: " << m_GridOrigin << std::endl;
    os << indent << "GridSpacing: " << m_GridSpacing << std::endl;
    os << indent << "GridDirection: " << m_GridDirection << std::endl;
    os << indent << "IndexToPoint: " << m_IndexToPoint << std::endl;
    os << indent << "PointToIndex: " << m_PointToIndex << std::endl;

    os << indent << "CoefficientImage: [ ";
    os << m_CoefficientImage.GetPointer() << " ]" << std::endl;

    os << indent << "WrappedImage: [ ";
    os << m_WrappedImage.GetPointer() << " ]" << std::endl;
 
    os << indent << "InputParametersPointer: " 
       << m_InputParametersPointer << std::endl;
    os << indent << "ValidRegion: " << m_ValidRegion << std::endl;
    os << indent << "LastJacobianIndex: " << m_LastJacobianIndex << std::endl;
    os << indent << "BulkTransform: ";
    os << m_BulkTransform << std::endl;

    if ( m_BulkTransform )
      {
	os << indent << "BulkTransformType: " 
	   << m_BulkTransform->GetNameOfClass() << std::endl;
      }
    os << indent << "VectorBSplineInterpolator: ";
    os << m_VectorInterpolator.GetPointer() << std::endl;
    os << indent << "Mask: ";
    os << m_Mask<< std::endl;
  }


  // Verify 
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  bool 
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::InsideValidRegion( const ContinuousIndexType& index ) const
  {
    bool inside = true;

    if ( !m_ValidRegion.IsInside( index ) )
      {
	inside = false;
      }

    // JV verify all dimensions 
    if ( inside)
      {
	typedef typename ContinuousIndexType::ValueType ValueType;
	for( unsigned int j = 0; j < NInputDimensions; j++ )
	  {
	    if (m_SplineOrderOdd[j])
	      {
		if ( index[j] >= static_cast<ValueType>( m_ValidRegionLast[j] ) )
		  { 
		    inside = false;
		    break;
		  }
	      }
	  }
      }
    return inside;
  }


  // Transform a point
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  typename ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::OutputPointType
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::TransformPoint(const InputPointType &inputPoint) const 
  {
  
    InputPointType transformedPoint;
    OutputPointType outputPoint;

    // BulkTransform
    if ( m_BulkTransform )
      {
	transformedPoint = m_BulkTransform->TransformPoint( inputPoint );
      }
    else
      {
	transformedPoint = inputPoint;
      }
    
    // Deformable transform
    if ( m_PaddedCoefficientImage )
      {
	// Check if inside mask
	if(m_Mask &&  !(m_Mask->IsInside(inputPoint) ) )
	  {
	    // Outside: no (deformable) displacement
	    return transformedPoint;
	  }		
	
	// Check if inside valid region
	bool inside = true;
	ContinuousIndexType index;
	this->TransformPointToContinuousIndex( inputPoint, index );
	inside = this->InsideValidRegion( index );
	if ( !inside )
	  {
	    // Outside: no (deformable) displacement
	    DD("outside valid region");
	    return transformedPoint;
	  }
		
	// Call the vector interpolator
	itk::Vector<TCoordRep,SpaceDimension> displacement=m_VectorInterpolator->EvaluateAtContinuousIndex(index);
	
	// JV add for the spatial dimensions
	outputPoint=transformedPoint;
	for (unsigned int i=0; i<NInputDimensions-1; i++)
	  outputPoint[i] += displacement[i];

      }

    else
      {
	itkWarningMacro( << "B-spline coefficients have not been set" );
	outputPoint = transformedPoint;
      }
    
    return outputPoint;
  }



  //JV Deformably transform a point
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  typename ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::OutputPointType
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::DeformablyTransformPoint(const InputPointType &inputPoint) const 
  {
    OutputPointType outputPoint;
    if ( m_PaddedCoefficientImage )
      {

	// Check if inside mask
	if(m_Mask && !(m_Mask->IsInside(inputPoint) ) )
	  {
	    // Outside: no (deformable) displacement
	    return inputPoint;
	  }	
	
	// Check if inside
	bool inside = true;
	ContinuousIndexType index;
	this->TransformPointToContinuousIndex( inputPoint, index );
	inside = this->InsideValidRegion( index );

	if ( !inside )
	  {
	    //outside: no (deformable) displacement
	    outputPoint = inputPoint;
	    return outputPoint;
	  }

	// Call the vector interpolator
	itk::Vector<TCoordRep,SpaceDimension> displacement=m_VectorInterpolator->EvaluateAtContinuousIndex(index);
	
	// JV add for the spatial dimensions
	outputPoint=inputPoint;
	for (unsigned int i=0; i<NInputDimensions-1; i++)
	  outputPoint[i] += displacement[i];
      }

    // No coefficients available
    else
      {
	itkWarningMacro( << "B-spline coefficients have not been set" );
	outputPoint = inputPoint;
      }
   
    return outputPoint;
  }
  

  // JV weights are identical as for transformpoint, could be done simultaneously in metric!!!!
  // Compute the Jacobian in one position 
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
#if ITK_VERSION_MAJOR >= 4
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::ComputeJacobianWithRespectToParameters( const InputPointType & point, JacobianType & jacobian) const
#else
  const 
  typename ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::JacobianType & 
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetJacobian( const InputPointType & point ) const
#endif
  {
  
    //========================================================
    // Zero all components of jacobian
    //========================================================
    // JV not thread safe (m_LastJacobianIndex), instantiate N transforms
    // NOTE: for efficiency, we only need to zero out the coefficients
    // that got fill last time this method was called.
    unsigned int j=0,b=0;
      
    //Set the previously-set to zero 
    for ( j = 0; j < SpaceDimension; j++ )
      {
	m_FirstIterator[j].GoToBegin();
	while ( !m_FirstIterator[j].IsAtEnd() )
	  {
	    m_FirstIterator[j].Set( m_ZeroVector );
	    ++(m_FirstIterator[j]);
	  }
      }
    
    //Set the previously-set to zero 
    for ( j = 0; j < SpaceDimension; j++ )
      {
	m_SecondIterator[j].GoToBegin();
	while ( ! (m_SecondIterator[j]).IsAtEnd() )
	  {
	    m_SecondIterator[j].Set( m_ZeroVector );
	    ++(m_SecondIterator[j]);
	  }
      }

    //Set the previously-set to zero 
    if (m_ThirdSize)
      for ( j = 0; j < SpaceDimension; j++ )
	{
	  m_ThirdIterator[j].GoToBegin();
	  while ( ! (m_ThirdIterator[j]).IsAtEnd() )
	    {
	      m_ThirdIterator[j].Set( m_ZeroVector );
	      ++(m_ThirdIterator[j]);
	    }
	}

    //Set the previously-set to zero 
    if (m_BCSize)
      for (b=0; b<m_BCSize;b++)
	if ( !( m_FirstRegion.IsInside(m_BCRegions[b]) | m_SecondRegion.IsInside(m_BCRegions[b]) )  )
	  for ( j = 0; j < SpaceDimension; j++ ) 
	    {
	      m_BCIterators[j][b].GoToBegin();
	      while ( ! (m_BCIterators[j][b]).IsAtEnd() )
		{
		  m_BCIterators[j][b].Set( m_ZeroVector );
		  ++(m_BCIterators[j][b]);
		}
	    }
	
    //Set the previously-set to zero 
    if (m_BC2Size)
      for (b=0; b<m_BC2Size;b++)
	if ( !( m_FirstRegion.IsInside(m_BC2Regions[b]) | m_SecondRegion.IsInside(m_BC2Regions[b]) )  )
	  for ( j = 0; j < SpaceDimension; j++ ) 
	    {
	      m_BC2Iterators[j][b].GoToBegin();
	      while ( ! (m_BC2Iterators[j][b]).IsAtEnd() )
		{
		  m_BC2Iterators[j][b].Set( m_ZeroVector );
		  ++(m_BC2Iterators[j][b]);
		}
	    }

    //Set the previously-set to zero 
    if (m_BC3Size)
      for (b=0; b<m_BC3Size;b++)
	if ( !( m_FirstRegion.IsInside(m_BC3Regions[b]) | m_SecondRegion.IsInside(m_BC3Regions[b]) )  )
	  for ( j = 0; j < SpaceDimension; j++ ) 
	    {
	      m_BC3Iterators[j][b].GoToBegin();
	      while ( ! (m_BC3Iterators[j][b]).IsAtEnd() )
		{
		  m_BC3Iterators[j][b].Set( m_ZeroVector );
		  ++(m_BC3Iterators[j][b]);
		}
	    }


    //========================================================
    // For each dimension, copy the weight to the support region
    //========================================================

    // Check if inside mask
    if(m_Mask &&  !(m_Mask->IsInside(point) ) )
      {
	// Outside: no (deformable) displacement
#if ITK_VERSION_MAJOR >= 4
        jacobian = m_SharedDataBSplineJacobian;
        return;
#else
	return this->m_Jacobian;
#endif
      }	

    // Get index   
    this->TransformPointToContinuousIndex( point, m_Index );

    // NOTE: if the support region does not lie totally within the grid
    // we assume zero displacement and return the input point
    if ( !this->InsideValidRegion( m_Index ) )
      {
#if ITK_VERSION_MAJOR >= 4
        jacobian = m_SharedDataBSplineJacobian;
        return;
#else
	return this->m_Jacobian;
#endif
      }

    // Compute interpolation weights
    const WeightsDataType *weights=NULL;
    m_VectorInterpolator->EvaluateWeightsAtContinuousIndex( m_Index, &weights, m_LastJacobianIndex);

    // Get support
    m_SupportRegion.SetIndex( m_LastJacobianIndex );
    WrapRegion(m_SupportRegion, m_FirstRegion, m_SecondRegion, m_ThirdRegion, m_BCRegions, m_BCValues, m_BC2Regions, m_BC2Values, m_BC3Regions, m_BC3Values, m_InitialOffset);
    m_ThirdSize=m_ThirdRegion.GetSize()[InputDimension-1]; 
    m_BCSize=m_BCRegions.size();
    m_BC2Size=m_BC2Regions.size();
    m_BC3Size=m_BC3Regions.size();

    // Reset the iterators
    for ( j = 0; j < SpaceDimension ; j++ ) 
      {
	m_FirstIterator[j] = IteratorType( m_JacobianImage[j], m_FirstRegion);
	m_SecondIterator[j] = IteratorType( m_JacobianImage[j], m_SecondRegion);
	if(m_ThirdSize)	m_ThirdIterator[j] = IteratorType( m_JacobianImage[j], m_ThirdRegion);

	m_BCIterators[j].resize(m_BCSize);
	for (b=0; b<m_BCSize;b++)
	  m_BCIterators[j][b]= IteratorType( m_JacobianImage[j], m_BCRegions[b]);
	m_BC2Iterators[j].resize(m_BC2Size);
	for (b=0; b<m_BC2Size;b++)
	  m_BC2Iterators[j][b]= IteratorType( m_JacobianImage[j], m_BC2Regions[b]);
	m_BC3Iterators[j].resize(m_BC3Size);
	for (b=0; b<m_BC3Size;b++)
	  m_BC3Iterators[j][b]= IteratorType( m_JacobianImage[j], m_BC3Regions[b]);
      }

    // Skip if on a fixed condition
    if(m_InitialOffset)
      {
	if (m_BCSize)  weights+=m_InitialOffset*m_BCRegions[0].GetNumberOfPixels();
	else std::cerr<<"InitialOffset without BCSize: Error!!!!!!!"<<std::endl;
      }

    //copy weight to jacobian image
    for ( j = 0; j < SpaceDimension; j++ )
      {
	// For each dimension, copy the weight to the support region
	while ( ! (m_FirstIterator[j]).IsAtEnd() )
	  {
	    m_ZeroVector[j]=*weights;
	    (m_FirstIterator[j]).Set( m_ZeroVector);
	    ++(m_FirstIterator[j]);
	    weights++;
	  }
	
	m_ZeroVector[j]=itk::NumericTraits<JacobianValueType>::Zero;
	if (j != SpaceDimension-1)  weights-=m_FirstRegion.GetNumberOfPixels();
      }
            
    // Skip BC1 and go to the second region
    if (m_BCSize) weights+=m_BCRegions[0].GetNumberOfPixels();
      
    // For each dimension, copy the weight to the support region
    //copy weight to jacobian image
    for ( j = 0; j < SpaceDimension; j++ )
      {
	while ( ! (m_SecondIterator[j]).IsAtEnd() )
	  {
	    m_ZeroVector[j]=*weights;
	    (m_SecondIterator[j]).Set( m_ZeroVector);
	    ++(m_SecondIterator[j]);
	    weights++;
	  }
	weights-=m_SecondRegion.GetNumberOfPixels();
	m_ZeroVector[j]=itk::NumericTraits<JacobianValueType>::Zero;
      }

    // Now do BC1:
    if (m_BCSize)
      {	
	//  Put pointer in correct position
	weights-=m_BCRegions[0].GetNumberOfPixels();
	
	for ( j = 0; j < SpaceDimension; j++ )
	  {
	    for ( b=0; b < m_BCSize; b++ )
	      {
		while ( ! (m_BCIterators[j][b]).IsAtEnd() )
		  {
		    //copy weight to jacobian image
		    m_ZeroVector[j]=(*weights) * m_BCValues[b];
		    (m_BCIterators[j][b]).Value()+= m_ZeroVector;
		    ++(m_BCIterators[j][b]);
		    weights++;
		  }
		weights-=m_BCRegions[b].GetNumberOfPixels();
	      }
	    m_ZeroVector[j]=itk::NumericTraits<JacobianValueType>::Zero;
	  }
	weights+=m_BCRegions[m_BCSize-1].GetNumberOfPixels();
      }
        
    // Add the BC2 to the weights
    if (m_BC2Size)
      {
	// Move further in the weights pointer
	weights+=m_SecondRegion.GetNumberOfPixels();

	for ( j = 0; j < SpaceDimension; j++ )
	  {
	    for ( b=0; b < m_BC2Size; b++ )
	      {
		while ( ! (m_BC2Iterators[j][b]).IsAtEnd() )
		  {
		    //copy weight to jacobian image
		    m_ZeroVector[j]=(*weights) * m_BC2Values[b];
		    (m_BC2Iterators[j][b]).Value()+= m_ZeroVector;
		    ++(m_BC2Iterators[j][b]);
		    weights++;
		  }
		weights-=m_BC2Regions[b].GetNumberOfPixels();
	      }
	    m_ZeroVector[j]=itk::NumericTraits<JacobianValueType>::Zero;
	  }
	// Move further in the weights pointer
	weights+=m_BC2Regions[m_BC2Size-1].GetNumberOfPixels();
      }
  
    // Third Region
    if(m_ThirdSize)
      {
	// For each dimension, copy the weight to the support region
	//copy weight to jacobian image
	for ( j = 0; j < SpaceDimension; j++ )
	  {
	    while ( ! (m_ThirdIterator[j]).IsAtEnd() )
	      {
		m_ZeroVector[j]=*weights;
		(m_ThirdIterator[j]).Value()+= m_ZeroVector;
		++(m_ThirdIterator[j]);
		weights++;
	      }

	    // Move further in the weights pointer?
	    if (j != SpaceDimension-1) weights-=m_ThirdRegion.GetNumberOfPixels();
	    m_ZeroVector[j]=itk::NumericTraits<JacobianValueType>::Zero;
	  }
      }

    // Add the BC3 to the weights
    if (m_BC3Size)
      {
	for ( j = 0; j < SpaceDimension; j++ )
	  {
	    for ( b=0; b < m_BC3Size; b++ )
	      {
		while ( ! (m_BC3Iterators[j][b]).IsAtEnd() )
		  {
		    //copy weight to jacobian image
		    m_ZeroVector[j]=(*weights) * m_BC3Values[b];
		    (m_BC3Iterators[j][b]).Value()+= m_ZeroVector;
		    ++(m_BC3Iterators[j][b]);
		    weights++;
		  }
		weights-=m_BC3Regions[b].GetNumberOfPixels();
	      }
	    m_ZeroVector[j]=itk::NumericTraits<JacobianValueType>::Zero;
	  }
      }

    // Return the result
#if ITK_VERSION_MAJOR >= 4
    jacobian = m_SharedDataBSplineJacobian;
#else
    return this->m_Jacobian;
#endif
  }

 
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  inline void 
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::WrapRegion( const RegionType & m_SupportRegion, 
		RegionType & m_FirstRegion, 
		RegionType & m_SecondRegion,
		RegionType & m_ThirdRegion,  
		std::vector<RegionType>& m_BCRegions,std::vector<double>& m_BCValues, 
		std::vector<RegionType>& m_BC2Regions,std::vector<double>& m_BC2Values,
		std::vector<RegionType>& m_BC3Regions,std::vector<double>& m_BC3Values,
		unsigned int& m_InitialOffset ) const
  {

    // Synchronize regions
    m_InitialOffset=0;
    m_FirstRegion=m_SupportRegion;
    m_BCRegion=m_SupportRegion;
    m_BCRegion.SetSize(InputDimension-1,1);
    m_SecondRegion=m_SupportRegion;
    m_ThirdRegion=m_SupportRegion;
    m_ThirdRegion.SetSize(InputDimension-1,0);
    m_BC3Regions.resize(0);


    // BC depends on shape
    switch(m_TransformShape)
      {
	/*  The shapes are
	    0: egg     4 CP  3 DOF
	    1: egg     5 CP  4 DOF 
	    2: rabbit  4 CP  3 DOF 
	    3: rabbit  5 CP  4 DOF
	    4: sputnik 4 CP  4 DOF
	    5: sputnik 5 CP  5 DOF
	    6: diamond 6 CP  5 DOF
	    7: diamond 7 CP  6 DOF
	*/
  
      case 0:
	{	  
	  // ----------------------------------------------------------------------
	  // The egg with 4 internal CP (starting from inhale)
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and
	  // Coeff      row  BC1 BC2  0  BC3  1   2  BC4
	  // PaddedCoeff  R:  0   1   2   3   4   5   6 
	  // BC1= R4
	  // BC2= R5
	  // BC3= -weights[2]/weights[0] ( R2+R4 ) 
	  // BC4= R2
	  // ---------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		
		// BC
		m_BCRegions.resize(0);
		     
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,0);
 
		// BC
		m_BC2Regions.resize(2);
		m_BC2Values.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -m_WeightRatio[2][0];
		    		      
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,2);
		      
		// BC
		m_BCRegions.resize(0);
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BC2Regions.resize(2);
		m_BC2Values.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -m_WeightRatio[2][0];

		// Third Part
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		      
		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -m_WeightRatio[2][0];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,1);
		      
		// BC2
		m_BC2Regions.resize(0);

		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -m_WeightRatio[2][0];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,1);
		      
		// BC2
		m_BC2Regions.resize(1);
		m_BC2Values.resize(1);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=1;

		break;
	      }
		    
	    default:
	      {
		DD("supportindex > 3 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
		
	  break;
	}

      case 1:
	{
	  // ----------------------------------------------------------------------
	  // The egg with 5 internal CP (starting from inhale)
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and
	  // Coeff      row  BC1 BC2  0  BC3  1   2   3  BC4
	  // PaddedCoeff  R:  0   1   2   3   4   5   6   7
	  // BC1= R5
	  // BC2= R6
	  // BC3= -weights[3]/weights[1] ( R2+R5 ) - R4 
	  // BC4= R2
	  // ---------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,2);
		
		// BC
		m_BCRegions.resize(0);
		     
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,0);
 
		// BC
		m_BC2Regions.resize(3);
		m_BC2Values.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=-m_WeightRatio[3][1];
		    		      
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,3);
		      
		// BC
		m_BCRegions.resize(0);
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BC2Regions.resize(3);
		m_BC2Values.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=-m_WeightRatio[3][1];

		// Third Part
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		      
		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,1);
		      
		// BC2
		m_BC2Regions.resize(0);

		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,3);
		m_SecondRegion.SetIndex(InputDimension-1,1);
		      
		// BC2
		m_BC2Regions.resize(0);
	
		break;
	      }
	    case 4:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,3);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		      
		// BC
		m_BCRegions.resize(0);
	
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,0);
		      
		// BC2
		m_BC2Regions.resize(0);
	
		break;
	      }
		    
	    default:
	      {
		DD("supportindex > 3 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
		
	  break;
	}
// 	  // ----------------------------------------------------------------------
// 	  // The egg with 5 internal CP: 
// 	  // Periodic, C2 smooth everywhere and constrained to zero at the reference
// 	  // Coeff       row R5 BC1  0   1   2   3  BC2  R2  
// 	  // PaddedCoeff R:  0   1   2   3   4   5   6   7    
// 	  // BC1= -weights[2]/weights[0] ( R2+R5)
// 	  // BC2= BC1  
// 	  // ---------------------------------------------------------------------
// 	  switch(m_SupportRegion.GetIndex(InputDimension-1))
// 	    {
// 	    case 0:
// 	      {
// 		// Lower end 
// 		m_FirstRegion.SetSize(InputDimension-1,1);
// 		m_FirstRegion.SetIndex(InputDimension-1,3);
			
// 		// BC
// 		m_BCRegions.resize(2);
// 		m_BCValues.resize(2);
// 		m_BCRegion.SetIndex(InputDimension-1,0);
// 		m_BCRegions[0]=m_BCRegion;
// 		m_BCValues[0]=-m_WeightRatio[2][0];
// 		m_BCRegion.SetIndex(InputDimension-1,3);
// 		m_BCRegions[1]=m_BCRegion;
// 		m_BCValues[1]=-m_WeightRatio[2][0];
	      
// 		// Second part
// 		m_SecondRegion.SetSize(InputDimension-1,2);
// 		m_SecondRegion.SetIndex(InputDimension-1,0);

// 		// BC2
// 		m_BC2Regions.resize(0);

// 		break;
// 	      }
// 	    case 1:
// 	      {
// 		// Lower end 
// 		m_FirstRegion.SetSize(InputDimension-1,0);
		      
// 		// BC
// 		m_BCRegions.resize(2);
// 		m_BCValues.resize(2);
// 		m_BCRegion.SetIndex(InputDimension-1,0);
// 		m_BCRegions[0]=m_BCRegion;
// 		m_BCValues[0]=-m_WeightRatio[2][0];
// 		m_BCRegion.SetIndex(InputDimension-1,3);
// 		m_BCRegions[1]=m_BCRegion;
// 		m_BCValues[1]=-m_WeightRatio[2][0];
	      
// 		// Second part
// 		m_SecondRegion.SetSize(InputDimension-1,3);
// 		m_SecondRegion.SetIndex(InputDimension-1,0);

// 		// BC2
// 		m_BC2Regions.resize(0);

// 		break;
// 	      }
// 	    case 2:
// 	      {
// 		// Lower end 
// 		m_FirstRegion.SetSize(InputDimension-1,4);
// 		m_FirstRegion.SetIndex(InputDimension-1,0);
	      
// 		// BC
// 		m_BCRegions.resize(0);
	    	      
// 		// Second part
// 		m_SecondRegion.SetSize(InputDimension-1,0);
	
// 		// BC2
// 		m_BC2Regions.resize(0);
	      
// 		break;
// 	      }
// 	    case 3:
// 	      {
// 		// Lower end 
// 		m_FirstRegion.SetSize(InputDimension-1,3);
// 		m_FirstRegion.SetIndex(InputDimension-1,1);
		
// 		// BC
// 		m_BCRegions.resize(2);
// 		m_BCValues.resize(2);
// 		m_BCRegion.SetIndex(InputDimension-1,0);
// 		m_BCRegions[0]=m_BCRegion;
// 		m_BCValues[0]=-m_WeightRatio[2][0];
// 		m_BCRegion.SetIndex(InputDimension-1,3);
// 		m_BCRegions[1]=m_BCRegion;
// 		m_BCValues[1]=-m_WeightRatio[2][0];
		
// 		// Second part
// 		m_SecondRegion.SetSize(InputDimension-1,0);

// 		// BC2
// 		m_BC2Regions.resize(0);
		 
// 		break;
// 	      }

// 	    case 4:
// 	      {
// 		// Lower end 
// 		m_FirstRegion.SetSize(InputDimension-1,2);
// 		m_FirstRegion.SetIndex(InputDimension-1,2);
		
// 		// BC
// 		m_BCRegions.resize(2);
// 		m_BCValues.resize(2);
// 		m_BCRegion.SetIndex(InputDimension-1,0);
// 		m_BCRegions[0]=m_BCRegion;
// 		m_BCValues[0]=-m_WeightRatio[2][0];
// 		m_BCRegion.SetIndex(InputDimension-1,3);
// 		m_BCRegions[1]=m_BCRegion;
// 		m_BCValues[1]=-m_WeightRatio[2][0];

// 		// Second part
// 		m_SecondRegion.SetSize(InputDimension-1,1);
// 		m_SecondRegion.SetIndex(InputDimension-1,0);

// 		// BC2
// 		m_BC2Regions.resize(0);

// 		break;
// 	      }

// 	    default:
// 	      {
// 		DD("supportindex > 4 ???");
// 		DD(m_SupportRegion.GetIndex(InputDimension-1));
// 		DD(m_TransformShape);
// 	      }
// 	    }// end swith index
// 	  break;
	  
// 	} // end case 1 shape

      case 2:
	{
	  // ----------------------------------------------------------------------
	  // The rabbit with 4 internal CP
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and the extremes fixed with anit-symm bc
	  // Coeff      row  BC1  0   1  BC2  2  BC3 BC4 
	  // PaddedCoeff  R:  0   1   2   3   4   5   6 
	  // BC1= 2*R1-R0
	  // BC2= -weights[2]/weights[0] ( R2+R4 )
	  // BC3=  R1
	  // BC4= 2*R1-R4
	  // ---------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);

		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=2;
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		     
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,0);
 
		// BC
		m_BC2Regions.resize(2);
		m_BC2Values.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -m_WeightRatio[2][0];
		    		      
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -m_WeightRatio[2][0];
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(0);
		      
		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		      
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -m_WeightRatio[2][0];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(1);
		m_BC2Values.resize(1);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=1;
			   		      
		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -m_WeightRatio[2][0];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(1);
		m_BC2Values.resize(1);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=1;

		// BC3
		m_BC3Regions.resize(2);
		m_BC3Values.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC3Regions[0]=m_BCRegion;
		m_BC3Values[0]=2;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC3Regions[1]=m_BCRegion;
		m_BC3Values[1]= -1;

		break;
	      }
		    
	    default:
	      {
		DD("supportindex > 3 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
		
	  break;
	} // end case 2 shape
      case 3: 
	{
	  // ----------------------------------------------------------------------
	  // The rabbit with 5 internal CP
	  // Periodic, constrained to zero at the reference at position 3.5 
	  // and the extremes fixed with anti-symmetrical BC
	  // Coeff      row  BC1  0   1  BC2  2   3  BC3 BC4
	  // PaddedCoeff  R:  0   1   2   3   4   5   6   7    
	  // BC1= 2*R1-R2
	  // BC2= -weights[3]/weights[1] ( R2+R5 ) - R4
	  // BC3= R1
	  // BC4= 2*R1-R5
	  // ---------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
			    
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=2.;
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1.;

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,0);
	
		// BC2
		m_BC2Regions.resize(3);
		m_BC2Values.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=-m_WeightRatio[3][1];	  
		      
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(0);
		      
		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];	  
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(0);
		      
		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];	  	  
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(1);
		m_BC2Values.resize(1);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=1.;
				      
		break;
	      }
		    
	    case 4:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BCRegions.resize(1);
		m_BCValues.resize(1);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=1.;
			      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,0);
				      
		// BC2
		m_BC2Regions.resize(2);
		m_BC2Values.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=2;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]=-1;
		      
		break;
		      
	      }
	    default:
	      {
		DD("supportindex > 4 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
	  
	  break;
	} // end case 3 shape

      case 4: 
	{
	  // ----------------------------------------------------------------------
	  // The sputnik with 4 internal CP
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and one indepent extremes copied
	  // Coeff     row BC1  0   1  BC2  2  BC2  3
	  // PaddedCoeff R: 0   1   2   3   4   5   6      
	  // BC1= R6
	  // BC2= -weights[2]/weights[0] ( R2+R4 )
	  // BC3=  weights[2]/weights[0] ( R2-R4) + R1
	  // ---------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);

		// BC
		m_BCRegions.resize(1);
		m_BCValues.resize(1);
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=1;

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,0);		
		
		// BC2
		m_BC2Regions.resize(2);
		m_BC2Values.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -m_WeightRatio[2][0];
		      
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -m_WeightRatio[2][0];
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(0);
		      
		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		      
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -m_WeightRatio[2][0];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(3);
		m_BC2Values.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=1;
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]=m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=-m_WeightRatio[2][0];	  
		   		      
		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -m_WeightRatio[2][0];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(3);
		m_BC2Values.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=1;
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]=m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=-m_WeightRatio[2][0];

		// Third part
		m_ThirdRegion.SetSize(InputDimension-1,1);
		m_ThirdRegion.SetIndex(InputDimension-1,3);

		break;
	      }
		    
	    default:
	      {
		DD("supportindex > 3 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
		
	  break;
	} // end case 4 shape
       
      case 5: 
	{
	  // ----------------------------------------------------------------------
	  // The sputnik with 5 internal CP
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and one indepent extreme
	  // Coeff     row BC1  0   1  BC2  2   3  BC3  4
	  // PaddedCoeff R: 0   1   2   3   4   5   6   7   
	  // BC1= R2 + R5 - R7
	  // BC2= -weights[3]/weights[1] ( R2+R5 ) - R4
	  // BC3= R1 + 0.5 R2 - 0.5 R7
	  // ----------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);

		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=1.;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]=1.;
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-1.;

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,0);		
		
		// BC2
		m_BC2Regions.resize(3);
		m_BC2Values.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=-m_WeightRatio[3][1];
		      
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(0);
		      
		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(0);
			   		      
		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(3);
		m_BC2Values.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=1;
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]=0.5;
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=-0.5;
	
		break;
	      }
	    case 4:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,2);		
      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=1;
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]=0.5;
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-0.5;
	
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,4);
		      
		// BC2
		m_BC2Regions.resize(0);

		break;
	      }
	    default:
	      {
		DD("supportindex > 4 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
		
	  break;
	} // end case 5 shape



     
      case 6: 
	{
	  // ----------------------------------------------------------------------
	  // The diamond with 4 internal CP:
	  // Periodic, constrained to zero at the reference at position 3
	  // Coeff      row 0   1   2  BC1  3  BC2  4
	  // PaddedCoeff  R:0   1   2   3   4   5   6    
	  // BC1= -weights[2]/weights[0] ( R2+R4 )
	  // BC2= weights[2]/weights[0]  ( R0+R2-R4-R6 ) + R1
	  // ---------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,3);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]=-m_WeightRatio[2][0];
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,0);

		// BC2
		m_BC2Regions.resize(0);
		
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]=-m_WeightRatio[2][0];  
	      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,3);

		// BC2
		m_BC2Regions.resize(0);

		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,2);
		
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]=-m_WeightRatio[2][0];	  	  
	      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,3);

		// BC2
		m_BC2Regions.resize(5);
		m_BC2Values.resize(5);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]=1;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=m_WeightRatio[2][0];	  
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BC2Regions[3]=m_BCRegion;
		m_BC2Values[3]=-m_WeightRatio[2][0];	
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BC2Regions[4]=m_BCRegion;
		m_BC2Values[4]=-m_WeightRatio[2][0];
		
		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		
		// BC
		m_BCRegions.resize(2);
		m_BCValues.resize(2);
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]=-m_WeightRatio[2][0];	  	  
	      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,3);

		// BC2
		m_BC2Regions.resize(5);
		m_BC2Values.resize(5);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]=1;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=m_WeightRatio[2][0];	  
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BC2Regions[3]=m_BCRegion;
		m_BC2Values[3]=-m_WeightRatio[2][0];	
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BC2Regions[4]=m_BCRegion;
		m_BC2Values[4]=-m_WeightRatio[2][0];
	
		// Third part
		m_ThirdRegion.SetSize(InputDimension-1,1);
		m_ThirdRegion.SetIndex(InputDimension-1,4);
		
		break;
	      }
	      
	    default:
	      {
		DD("supportindex > 3 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
		
	  break;
	} // end case 7 shape

      case 7: 
	{
	  // ----------------------------------------------------------------------
	  // The diamond with 5 internal CP: 
	  // periodic, constrained to zero at the reference at position 3.5
	  // Coeff      row 0   1   2  BC1  3   4  BC2  5
	  // PaddedCoeff  R:0   1   2   3   4   5   6   7    
	  // BC1= -weights[3]/weights[1] ( R2+R5 ) - R4
	  // BC2= weights[2]/weights[0] ( R0+R2-R5-R7 ) + R1
	  // ---------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,3);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];	  
	      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,0);

		// BC2
		m_BC2Regions.resize(0);
		
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];	  
	      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,3);

		// BC2
		m_BC2Regions.resize(0);

		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,2);
		
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];	  	  
	      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,3);

		// BC2
		m_BC2Regions.resize(0);
		
		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];	  	  
	      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,3);

		// BC2
		m_BC2Regions.resize(5);
		m_BC2Values.resize(5);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]=1;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=m_WeightRatio[2][0];	  
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BC2Regions[3]=m_BCRegion;
		m_BC2Values[3]=-m_WeightRatio[2][0];	
		m_BCRegion.SetIndex(InputDimension-1,5);
		m_BC2Regions[4]=m_BCRegion;
		m_BC2Values[4]=-m_WeightRatio[2][0];	
		
		break;
	      }
	      
	    case 4:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,3);
		
		// BC
		m_BCRegions.resize(5);
		m_BCValues.resize(5);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=m_WeightRatio[2][0];
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]=1;
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=m_WeightRatio[2][0];	  
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BCRegions[3]=m_BCRegion;
		m_BCValues[3]=-m_WeightRatio[2][0];	
		m_BCRegion.SetIndex(InputDimension-1,5);
		m_BCRegions[4]=m_BCRegion;
		m_BCValues[4]=-m_WeightRatio[2][0];	
	      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,5);

		// BC2
		m_BC2Regions.resize(0);
			
		break;
	      }

	    default:
	      {
		DD("supportindex > 4 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
		
	  break;
	} // end case 7 shape

      case 9:
	{
	  // ----------------------------------------------------------------------
	  // The sputnik with 5 internal CP
	  // Periodic, constrained to zero at the reference 
	  // at position 3 and one indepent extreme
	  // Coeff     row BC1  0   1  BC2  2   3  BC3  4
	  // PaddedCoeff R: 0   1   2   3   4   5   6   7   
	  // BC1= -R2+R5-R7
	  // BC2= -weights[3]/weights[1] ( R2+R5 ) - R4
	  // BC3= R1
	  // ---------------------------------------------------------------------
	  switch(m_SupportRegion.GetIndex(InputDimension-1)) 
	    {
	    case 0:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);

		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]=1;
		m_BCRegion.SetIndex(InputDimension-1,4);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-1;

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,0);		
		
		// BC2
		m_BC2Regions.resize(3);
		m_BC2Values.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BC2Regions[1]=m_BCRegion;
		m_BC2Values[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BC2Regions[2]=m_BCRegion;
		m_BC2Values[2]=-m_WeightRatio[3][1];
		      
		break;
	      }
	    case 1:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];
		      
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(0);
		      
		break;
	      }
	    case 2:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,1);
		m_FirstRegion.SetIndex(InputDimension-1,1);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,2);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC2
		m_BC2Regions.resize(0);
			   		      
		break;
	      }
	    case 3:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,0);
		      
		// BC
		m_BCRegions.resize(3);
		m_BCValues.resize(3);
		m_BCRegion.SetIndex(InputDimension-1,1);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=-m_WeightRatio[3][1];
		m_BCRegion.SetIndex(InputDimension-1,2);
		m_BCRegions[1]=m_BCRegion;
		m_BCValues[1]= -1;
		m_BCRegion.SetIndex(InputDimension-1,3);
		m_BCRegions[2]=m_BCRegion;
		m_BCValues[2]=-m_WeightRatio[3][1];

		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,2);
		      
		// BC
		m_BC2Regions.resize(1);
		m_BC2Values.resize(1);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BC2Regions[0]=m_BCRegion;
		m_BC2Values[0]=1;
	
		break;
	      }
	    case 4:
	      {
		// Lower end 
		m_FirstRegion.SetSize(InputDimension-1,2);
		m_FirstRegion.SetIndex(InputDimension-1,2);		
      
		// BC
		m_BCRegions.resize(1);
		m_BCValues.resize(1);
		m_BCRegion.SetIndex(InputDimension-1,0);
		m_BCRegions[0]=m_BCRegion;
		m_BCValues[0]=1;
		
		// Second part
		m_SecondRegion.SetSize(InputDimension-1,1);
		m_SecondRegion.SetIndex(InputDimension-1,4);
		      
		// BC2
		m_BC2Regions.resize(0);

		break;
	      }
	    default:
	      {
		DD("supportindex > 4 ???");
		DD(m_SupportRegion.GetIndex(InputDimension-1));
	      }
	    } // end switch index
		
	  break;
	} // end case 9 shape
       

      default:
	{
	  DD ("Other shapes currently not implemented");
	}
	      
      } // end switch shape
  } // end wrap region
	
	

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  ShapedBLUTSpatioTemporalDeformableTransform<TCoordRep, NInputDimensions,NOutputDimensions>
  ::TransformPointToContinuousIndex( const InputPointType & point, ContinuousIndexType & index ) const
  {
    unsigned int j;
    
    itk::Vector<double, OutputDimension> tvector;
    
    for ( j = 0; j < OutputDimension; j++ )
      {
	//JV find the index in the PADDED version
	tvector[j] = point[j] - this->m_PaddedGridOrigin[j];
      }
    
    itk::Vector<double, OutputDimension> cvector;
    
    cvector = m_PointToIndex * tvector;
    
    for ( j = 0; j < OutputDimension; j++ )
      {
	index[j] = static_cast< typename ContinuousIndexType::CoordRepType >( cvector[j] );
      }
  }
  
  
} // namespace

#endif

