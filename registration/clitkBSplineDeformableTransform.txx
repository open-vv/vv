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
#ifndef __clitkBSplineDeformableTransform_txx
#define __clitkBSplineDeformableTransform_txx
#include "clitkBSplineDeformableTransform.h"

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
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
#if ITK_VERSION_MAJOR >= 4
  ::BSplineDeformableTransform():Superclass(0)
#else
  ::BSplineDeformableTransform():Superclass(OutputDimension,0)
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

    // Default grid size is zero
    typename RegionType::SizeType size;
    typename RegionType::IndexType index;
    size.Fill( 0 );
    index.Fill( 0 );
    m_GridRegion.SetSize( size );
    m_GridRegion.SetIndex( index );

    m_GridOrigin.Fill( 0.0 );  // default origin is all zeros
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
    m_CoefficientImage = NULL;
  
    // Variables for computing interpolation
    for (i=0; i <InputDimension;i++)
      {
	m_Offset[i] = m_SplineOrders[i] / 2;
        m_SplineOrderOdd[i] = m_SplineOrders[i] % 2;
      }
    m_ValidRegion = m_GridRegion;
	
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
    this->m_FixedParameters.SetSize ( NInputDimensions * (NInputDimensions + 5)+2 );
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


    // Calculate the PointToIndex matrices
    DirectionType scale;
    for( unsigned int i=0; i<OutputDimension; i++)
      {
	scale[i][i] = m_GridSpacing[i];
      }

    m_IndexToPoint = m_GridDirection * scale;
    m_PointToIndex = m_IndexToPoint.GetInverse();

    // Jacobian
    m_LastJacobianIndex = m_ValidRegion.GetIndex();

    //JV initialize some variables for jacobian calculation
    m_SupportRegion.SetSize(m_SupportSize);
    m_SupportIndex.Fill(0);
    m_SupportRegion.SetIndex(m_SupportIndex);
    for (  i = 0; i < OutputDimension; i++ ) 
      m_ZeroVector[i]=itk::NumericTraits<JacobianValueType>::Zero;
  }
    

  // Destructor
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::~BSplineDeformableTransform()
  {

  }


  // JV set Spline Order
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetSplineOrder(const unsigned int & splineOrder)
  {
    SizeType splineOrders;
    for (unsigned int i=0;i<InputDimension;i++)splineOrders[i]=splineOrder;

    this->SetSplineOrders(splineOrders);
  }
   

  // JV set Spline Orders
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetSplineOrders(const SizeType & splineOrders)
  {
    if (m_SplineOrders!=splineOrders)
      {
	m_SplineOrders=splineOrders;
	
	//update the interpolation function
	m_VectorInterpolator->SetSplineOrders(m_SplineOrders);
	
	//update the variables for computing interpolation
	for (unsigned int i=0; i <InputDimension;i++)
        {
          m_SupportSize[i] = m_SplineOrders[i]+1;
          m_Offset[i] = m_SplineOrders[i] / 2;
          m_SplineOrderOdd[i] = m_SplineOrders[i] % 2;
        }
	this->Modified();
      }
  }


  // JV set sampling factor
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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
#if ITK_VERSION_MAJOR >= 4
  typename BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>::NumberOfParametersType
#else
  unsigned int
#endif
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetNumberOfParameters(void) const
  {
    // The number of parameters equal OutputDimension * number of
    // of pixels in the grid region.
    return ( static_cast<unsigned int>( OutputDimension ) *
	     static_cast<unsigned int>( m_GridRegion.GetNumberOfPixels() ) );

  }


  // Get the number of parameters per dimension
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  unsigned int
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetNumberOfParametersPerDimension(void) const
  {
    // The number of parameters per dimension equal number of
    // of pixels in the grid region.
    return ( static_cast<unsigned int>( m_GridRegion.GetNumberOfPixels() ) );

  }


  // Set the grid region
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridRegion( const RegionType & region )
  {
    if ( m_GridRegion != region )
      {
	m_GridRegion = region;

	// set regions for each coefficient and jacobian image
	m_WrappedImage->SetRegions( m_GridRegion );
	for (unsigned int j=0; j <OutputDimension;j++)
	  m_JacobianImage[j]->SetRegions( m_GridRegion );

	// Set the valid region
	// If the grid spans the interval [start, last].
	// The valid interval for evaluation is [start+offset, last-offset]
	// when spline order is even.
	// The valid interval for evaluation is [start+offset, last-offset)
	// when spline order is odd.
	// Where offset = vcl_floor(spline / 2 ).
	// Note that the last pixel is not included in the valid region
	// with odd spline orders.
	typename RegionType::SizeType size = m_GridRegion.GetSize();
	typename RegionType::IndexType index = m_GridRegion.GetIndex();
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
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridSpacing( const SpacingType & spacing )
  {
    if ( m_GridSpacing != spacing )
      {
	m_GridSpacing = spacing;

	// Set spacing for each coefficient and jacobian image
	m_WrappedImage->SetSpacing( m_GridSpacing.GetDataPointer() );
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
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridDirection( const DirectionType & direction )
  {
    if ( m_GridDirection != direction )
      {
	m_GridDirection = direction;

	// Set direction for each coefficient and jacobian image
	m_WrappedImage->SetDirection( m_GridDirection );
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
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetGridOrigin( const OriginType& origin )
  {
    if ( m_GridOrigin != origin )
      {
	m_GridOrigin = origin;

	// Set origin for each coefficient and jacobianimage
	m_WrappedImage->SetOrigin( m_GridOrigin.GetDataPointer() );
	for (unsigned int j=0; j <OutputDimension; j++) m_JacobianImage[j]->SetOrigin( m_GridOrigin.GetDataPointer() );
	
	this->Modified();
      }

  }


  // Set the parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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

    //Set input to vector interpolator
    m_VectorInterpolator->SetInputImage(this->GetCoefficientImage());

    // Modified is always called since we just have a pointer to the
    // parameters and cannot know if the parameters have changed.
    this->Modified();
  }


  // Set the Fixed Parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetFixedParameters( const ParametersType & parameters )
  {
 
    // JV number should be exact, no defaults for spacing
    if ( parameters.Size() != NInputDimensions * (5 + NInputDimensions)+2 )
      {
	itkExceptionMacro(<< "Mismatched between parameters size "
			  << parameters.size() 
			  << " and number of fixed parameters "
			  << NInputDimensions * (5 + NInputDimensions)+2 );
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

    //Set the members
    this->SetGridSpacing( spacing );
    this->SetGridDirection( direction );
    this->SetGridOrigin( origin );
    this->SetGridRegion( bsplineRegion );

    //JV add the LUT parameters
    this->SetSplineOrders( splineOrders );
    this->SetLUTSamplingFactors( samplingFactors );
      
  }


  // Wrap flat parameters as images
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::WrapAsImages()
  {
    //JV Wrap parameter array in vectorial image, changed parameter order: A1x A1y A1z, A2x ....
    PixelType * dataPointer =reinterpret_cast<PixelType *>( const_cast<double *>(m_InputParametersPointer->data_block() )) ;
    unsigned int numberOfPixels = m_GridRegion.GetNumberOfPixels();
    
    m_WrappedImage->GetPixelContainer()->SetImportPointer( dataPointer,numberOfPixels);//InputDimension
    m_CoefficientImage = m_WrappedImage;
    
    //JV Wrap jacobian into OutputDimension X Vectorial images
#if ITK_VERSION_MAJOR >= 4
    this->m_SharedDataBSplineJacobian.set_size( OutputDimension, this->GetNumberOfParameters() );
#else
    this->m_Jacobian.set_size( OutputDimension, this->GetNumberOfParameters() );
#endif

    // Use memset to set the memory
#if ITK_VERSION_MAJOR >= 4
    JacobianPixelType * jacobianDataPointer = reinterpret_cast<JacobianPixelType *>(this->m_SharedDataBSplineJacobian.data_block());
#else
    JacobianPixelType * jacobianDataPointer = reinterpret_cast<JacobianPixelType *>(this->m_Jacobian.data_block());
#endif
    memset(jacobianDataPointer, 0,  OutputDimension*numberOfPixels*sizeof(JacobianPixelType));
    m_LastJacobianIndex = m_ValidRegion.GetIndex();
    m_NeedResetJacobian = false;

    for (unsigned int j=0; j<OutputDimension; j++)
      {
	m_JacobianImage[j]->GetPixelContainer()->
	  SetImportPointer( jacobianDataPointer, numberOfPixels );
	jacobianDataPointer += numberOfPixels;
     }
  }


  // Set the parameters by value
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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

    // Modified is always called since we just have a pointer to the
    // Parameters and cannot know if the parameters have changed.
    this->Modified();

  }

  // Get the parameters
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  const 
  typename BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::ParametersType &
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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
  typename BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::ParametersType &
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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
    
    return (this->m_FixedParameters);
  }

  
  // Set the B-Spline coefficients using input images
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void 
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::SetCoefficientImage( CoefficientImagePointer image )
  {
    
    this->SetGridRegion( image->GetBufferedRegion() );
    this->SetGridSpacing( image->GetSpacing() );
    this->SetGridDirection( image->GetDirection() );
    this->SetGridOrigin( image->GetOrigin() );
    m_CoefficientImage = image;

    // Update the interpolator    
    m_VectorInterpolator->SetInputImage(this->GetCoefficientImage());

    // Clean up buffered parameters
    m_InternalParametersBuffer = ParametersType( 0 );
    m_InputParametersPointer  = NULL;
      
  }  


  // Print self
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::InsideValidRegion( const ContinuousIndexType& index ) const
  {
    bool inside = true;

    if ( !m_ValidRegion.IsInside( index ) )
      {
	inside = false;
      }
    //JV verify for each input dimension
    if ( inside)
      {
	typedef typename ContinuousIndexType::ValueType ValueType;
	for( unsigned int j = 0; j < InputDimension; j++ )
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
  typename BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::OutputPointType
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
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
    if ( m_CoefficientImage )
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
	    return transformedPoint;
	  }
	
	// Call the vector interpolator
	itk::Vector<TCoordRep,OutputDimension> displacement=m_VectorInterpolator->EvaluateAtContinuousIndex(index);

	// Return the results
	outputPoint = transformedPoint+displacement;

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
  typename BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::OutputPointType
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::DeformablyTransformPoint(const InputPointType &inputPoint) const 
  {
    OutputPointType outputPoint;
    if ( m_CoefficientImage )
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
	     return inputPoint;
	     //return outputPoint;
	  }

	// Call the vector interpolator
	itk::Vector<TCoordRep,OutputDimension> displacement=m_VectorInterpolator->EvaluateAtContinuousIndex(index);
	
	// Return the results
	outputPoint = inputPoint+displacement;
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
#if ITK_VERSION_MAJOR >= 4
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::ComputeJacobianWithRespectToParameters(const InputPointType & point, JacobianType & jacobian) const
  {
    if (m_NeedResetJacobian)
      ResetJacobian();

    //========================================================
    // For each dimension, copy the weight to the support region
    //========================================================

    // Check if inside mask
    if (m_Mask &&  !(m_Mask->IsInside(point) ) )
      {
	// Outside: no (deformable) displacement
        jacobian = m_SharedDataBSplineJacobian;
	return;
      }

    //Get index
    this->TransformPointToContinuousIndex( point, m_Index );

    // NOTE: if the support region does not lie totally within the grid
    // we assume zero displacement and return the input point
    if ( !this->InsideValidRegion( m_Index ) )
      {
        jacobian = m_SharedDataBSplineJacobian;
	return;
      }

    //Compute interpolation weights
    const WeightsDataType *weights = NULL;
    m_VectorInterpolator->EvaluateWeightsAtContinuousIndex( m_Index, &weights, m_LastJacobianIndex);
    m_SupportRegion.SetIndex( m_LastJacobianIndex );

    //Reset the iterators
    unsigned int j = 0;
    for ( j = 0; j < OutputDimension; j++ )
      m_Iterator[j] = IteratorType( m_JacobianImage[j], m_SupportRegion);

    // For each dimension, copy the weight to the support region
    while ( ! (m_Iterator[0]).IsAtEnd() )
      {
	//copy weight to jacobian image
	for ( j = 0; j < OutputDimension; j++ )
	  {
	    m_ZeroVector[j]=*weights;
	    (m_Iterator[j]).Set( m_ZeroVector);
	    m_ZeroVector[j]=itk::NumericTraits<JacobianValueType>::Zero;
	    ++(m_Iterator[j]);
	  }
	// go to next coefficient in the support region
	weights++;
      }
    m_NeedResetJacobian = true;

    // Return the result
    jacobian = m_SharedDataBSplineJacobian;

  }
#else
  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  const 
  typename BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::JacobianType & 
  BSplineDeformableTransform<TCoordRep, NInputDimensions, NOutputDimensions>
  ::GetJacobian( const InputPointType & point ) const
  {
    // Can only compute Jacobian if parameters are set via
    // SetParameters or SetParametersByValue
    //     if( m_InputParametersPointer == NULL )
    //       {
    // 	itkExceptionMacro( <<"Cannot compute Jacobian: parameters not set" );
    //       }

    if (m_NeedResetJacobian)
      ResetJacobian();

    //========================================================
    // For each dimension, copy the weight to the support region
    //========================================================

    // Check if inside mask
    if(m_Mask &&  !(m_Mask->IsInside(point) ) )
      {
	// Outside: no (deformable) displacement
	return this->m_Jacobian;
      }	

    //Get index   
    this->TransformPointToContinuousIndex( point, m_Index );

    // NOTE: if the support region does not lie totally within the grid
    // we assume zero displacement and return the input point
    if ( !this->InsideValidRegion( m_Index ) )
      {
	return this->m_Jacobian;
      }

    //Compute interpolation weights
    const WeightsDataType *weights=NULL;
    m_VectorInterpolator->EvaluateWeightsAtContinuousIndex( m_Index, &weights, m_LastJacobianIndex);
    m_SupportRegion.SetIndex( m_LastJacobianIndex );

    //Reset the iterators
    unsigned int j = 0;
    for ( j = 0; j < OutputDimension; j++ ) 
      m_Iterator[j] = IteratorType( m_JacobianImage[j], m_SupportRegion);

    // For each dimension, copy the weight to the support region
    while ( ! (m_Iterator[0]).IsAtEnd() )
      {
	//copy weight to jacobian image
	for ( j = 0; j < OutputDimension; j++ )
	  {
	    m_ZeroVector[j]=*weights;
	    (m_Iterator[j]).Set( m_ZeroVector);
	    m_ZeroVector[j]=itk::NumericTraits<JacobianValueType>::Zero;
	    ++(m_Iterator[j]);
	  }
	// go to next coefficient in the support region
	weights++;
      }
    m_NeedResetJacobian = true;

    // Return the result
    return this->m_Jacobian;

  }
#endif


  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions,NOutputDimensions>
  ::TransformPointToContinuousIndex( const InputPointType & point, ContinuousIndexType & index ) const
  {
    unsigned int j;

    itk::Vector<double, OutputDimension> tvector;

    for ( j = 0; j < OutputDimension; j++ )
      {
	tvector[j] = point[j] - this->m_GridOrigin[j];
      }

    itk::Vector<double, OutputDimension> cvector;

    cvector = m_PointToIndex * tvector;

    for ( j = 0; j < OutputDimension; j++ )
      {
	index[j] = static_cast< typename ContinuousIndexType::CoordRepType >( cvector[j] );
      }
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  unsigned
  BSplineDeformableTransform<TCoordRep, NInputDimensions,NOutputDimensions>
  ::SetJacobianImageData(JacobianPixelType * jacobianDataPointer, unsigned dim)
  {
    unsigned int numberOfPixels = m_GridRegion.GetNumberOfPixels();
    m_JacobianImage[dim]->GetPixelContainer()->SetImportPointer(jacobianDataPointer, numberOfPixels);
    return numberOfPixels;
  }

  template<class TCoordRep, unsigned int NInputDimensions, unsigned int NOutputDimensions>
  void
  BSplineDeformableTransform<TCoordRep, NInputDimensions,NOutputDimensions>
  ::ResetJacobian() const
  {
    //========================================================
    // Zero all components of jacobian
    //========================================================
    // JV not thread safe (m_LastJacobianIndex), instantiate N transforms
    // NOTE: for efficiency, we only need to zero out the coefficients
    // that got fill last time this method was called.

    unsigned int j = 0;

    //Define the  region for each jacobian image
    m_SupportRegion.SetIndex(m_LastJacobianIndex);

    //Initialize the iterators
    for (j = 0; j < OutputDimension; j++)
      m_Iterator[j] = IteratorType(m_JacobianImage[j], m_SupportRegion);

    //Set the previously-set to zero
    while (!(m_Iterator[0]).IsAtEnd())
      {
	for (j = 0; j < OutputDimension; j++)
	  {
	    m_Iterator[j].Set(m_ZeroVector);
	    ++(m_Iterator[j]);
	  }
      }
    m_NeedResetJacobian = false;
  }
} // namespace

#endif
