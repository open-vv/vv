#ifndef _clitkVectorBSplineDecompositionImageFilterWithOBD_txx
#define _clitkVectorBSplineDecompositionImageFilterWithOBD_txx

#include "clitkVectorBSplineDecompositionImageFilterWithOBD.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"
#include "itkVector.h"

namespace clitk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::VectorBSplineDecompositionImageFilterWithOBD()
{
  m_SplineOrder = 0;
  int SplineOrder = 3;
  m_Tolerance = 1e-10;   // Need some guidance on this one...what is reasonable?
  m_IteratorDirection = 0;
  this->SetSplineOrder(SplineOrder);
}


/**
 * Standard "PrintSelf" method
 */
template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::PrintSelf(
  std::ostream& os, 
  itk::Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Spline Order: " << m_SplineOrder << std::endl;

}


template <class TInputImage, class TOutputImage>
bool
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::DataToCoefficients1D()
{ 

  // See Unser, 1993, Part II, Equation 2.5, 
  //   or Unser, 1999, Box 2. for an explaination. 

  double c0 = 1.0;  
  
  if (m_DataLength[m_IteratorDirection] == 1) //Required by mirror boundaries
    {
    return false;
    }

  // Compute overall gain
  for (int k = 0; k < m_NumberOfPoles; k++)
    {
    // Note for cubic splines lambda = 6 
    c0 = c0 * (1.0 - m_SplinePoles[k]) * (1.0 - 1.0 / m_SplinePoles[k]);
    }

  // apply the gain 
  for (unsigned int n = 0; n < m_DataLength[m_IteratorDirection]; n++)
    {
    m_Scratch[n] *= c0;
    }

  // loop over all poles 
  for (int k = 0; k < m_NumberOfPoles; k++) 
    {
    // causal initialization 
    this->SetInitialCausalCoefficient(m_SplinePoles[k]);
    // causal recursion 
    for (unsigned int n = 1; n < m_DataLength[m_IteratorDirection]; n++)
      {
      m_Scratch[n] += m_SplinePoles[k] * m_Scratch[n - 1];
      }

    // anticausal initialization 
    this->SetInitialAntiCausalCoefficient(m_SplinePoles[k]);
    // anticausal recursion 
    for ( int n = m_DataLength[m_IteratorDirection] - 2; 0 <= n; n--)
      {
      m_Scratch[n] = m_SplinePoles[k] * (m_Scratch[n + 1] - m_Scratch[n]);
      }
    }
  return true;

}


template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::SetSplineOrder(unsigned int SplineOrder)
{
  if (SplineOrder == m_SplineOrder)
    {
    return;
    }
  m_SplineOrder = SplineOrder;
  this->SetPoles();
  this->Modified();

}

//JV
template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::SetSplineOrders(SizeType SplineOrders)
{
  m_SplineOrders=SplineOrders;
}

template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::SetPoles()
{
  /* See Unser, 1997. Part II, Table I for Pole values */
  // See also, Handbook of Medical Imaging, Processing and Analysis, Ed. Isaac N. Bankman, 
  //  2000, pg. 416.
  switch (m_SplineOrder)
    {
    case 3:
      m_NumberOfPoles = 1;
      m_SplinePoles[0] = vcl_sqrt(3.0) - 2.0;
      break;
    case 0:
      m_NumberOfPoles = 0;
      break;
    case 1:
      m_NumberOfPoles = 0;
      break;
    case 2:
      m_NumberOfPoles = 1;
      m_SplinePoles[0] = vcl_sqrt(8.0) - 3.0;
      break;
    case 4:
      m_NumberOfPoles = 2;
      m_SplinePoles[0] = vcl_sqrt(664.0 - vcl_sqrt(438976.0)) + vcl_sqrt(304.0) - 19.0;
      m_SplinePoles[1] = vcl_sqrt(664.0 + vcl_sqrt(438976.0)) - vcl_sqrt(304.0) - 19.0;
      break;
    case 5:
      m_NumberOfPoles = 2;
      m_SplinePoles[0] = vcl_sqrt(135.0 / 2.0 - vcl_sqrt(17745.0 / 4.0)) + vcl_sqrt(105.0 / 4.0)
        - 13.0 / 2.0;
      m_SplinePoles[1] = vcl_sqrt(135.0 / 2.0 + vcl_sqrt(17745.0 / 4.0)) - vcl_sqrt(105.0 / 4.0)
        - 13.0 / 2.0;
      break;
    default:
      // SplineOrder not implemented yet.
      itk::ExceptionObject err(__FILE__, __LINE__);
      err.SetLocation( ITK_LOCATION);
      err.SetDescription( "SplineOrder must be between 0 and 5. Requested spline order has not been implemented yet." );
      throw err;
      break;
    }
}


template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::SetInitialCausalCoefficient(double z)
{
  /* begining InitialCausalCoefficient */
  /* See Unser, 1999, Box 2 for explaination */
  //JV
  itk::Vector<double, VectorDimension> sum;
  double  zn, z2n, iz; //sum
  unsigned long  horizon;

  /* this initialization corresponds to mirror boundaries */
  horizon = m_DataLength[m_IteratorDirection];
  zn = z;
  if (m_Tolerance > 0.0)
    {
    horizon = (long)vcl_ceil(log(m_Tolerance) / vcl_log(fabs(z)));
    }
  if (horizon < m_DataLength[m_IteratorDirection])
    {
    /* accelerated loop */
    sum = m_Scratch[0];   // verify this
    for (unsigned int n = 1; n < horizon; n++) 
      {
      sum += zn * m_Scratch[n];
      zn *= z;
      }
    m_Scratch[0] = sum;
    }
  else {
  /* full loop */
  iz = 1.0 / z;
  z2n = vcl_pow(z, (double)(m_DataLength[m_IteratorDirection] - 1L));
  sum = m_Scratch[0] + z2n * m_Scratch[m_DataLength[m_IteratorDirection] - 1L];
  z2n *= z2n * iz;
  for (unsigned int n = 1; n <= (m_DataLength[m_IteratorDirection] - 2); n++)
    {
    sum += (zn + z2n) * m_Scratch[n];
    zn *= z;
    z2n *= iz;
    }
  m_Scratch[0] = sum / (1.0 - zn * zn);
  }
}


template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::SetInitialAntiCausalCoefficient(double z)
{
  // this initialization corresponds to mirror boundaries 
  /* See Unser, 1999, Box 2 for explaination */
  //  Also see erratum at http://bigwww.epfl.ch/publications/unser9902.html
  m_Scratch[m_DataLength[m_IteratorDirection] - 1] =
    (z / (z * z - 1.0)) * 
    (z * m_Scratch[m_DataLength[m_IteratorDirection] - 2] + m_Scratch[m_DataLength[m_IteratorDirection] - 1]);
}


template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::DataToCoefficientsND()
{
  OutputImagePointer output = this->GetOutput();

  itk::Size<ImageDimension> size = output->GetBufferedRegion().GetSize();

  unsigned int count = output->GetBufferedRegion().GetNumberOfPixels() / size[0] * ImageDimension;

  itk::ProgressReporter progress(this, 0, count, 10);

  // Initialize coeffient array
  this->CopyImageToImage();   // Coefficients are initialized to the input data

  for (unsigned int n=0; n < ImageDimension; n++)
    {
    m_IteratorDirection = n;
    // Loop through each dimension

    //JV Set the correct order by dimension!
    SetSplineOrder(m_SplineOrders[n]);

    // Initialize iterators
    OutputLinearIterator CIterator( output, output->GetBufferedRegion() );
    CIterator.SetDirection( m_IteratorDirection );
    // For each data vector
    while ( !CIterator.IsAtEnd() )
      {
      // Copy coefficients to scratch
      this->CopyCoefficientsToScratch( CIterator );


      // Perform 1D BSpline calculations
      this->DataToCoefficients1D();
    
      // Copy scratch back to coefficients.
      // Brings us back to the end of the line we were working on.
      CIterator.GoToBeginOfLine();
      this->CopyScratchToCoefficients( CIterator ); // m_Scratch = m_Image;
      CIterator.NextLine();
      progress.CompletedPixel();
      }
    }
}


/**
 * Copy the input image into the output image
 */
template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::CopyImageToImage()
{

  typedef itk::ImageRegionConstIteratorWithIndex< TInputImage > InputIterator;
  typedef itk::ImageRegionIterator< TOutputImage > OutputIterator;
  typedef typename TOutputImage::PixelType OutputPixelType;

  InputIterator inIt( this->GetInput(), this->GetInput()->GetBufferedRegion() );
  OutputIterator outIt( this->GetOutput(), this->GetOutput()->GetBufferedRegion() );

  inIt = inIt.Begin();
  outIt = outIt.Begin();
  OutputPixelType v;
  while ( !outIt.IsAtEnd() )
    {
      for (unsigned int i=0; i< VectorDimension;i++) 
	{
	  v[i]= static_cast<typename OutputPixelType::ComponentType>( inIt.Get()[i] );
	}
      outIt.Set( v );
      ++inIt;
    ++outIt;
    }
 
}


/**
 * Copy the scratch to one line of the output image
 */
template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::CopyScratchToCoefficients(OutputLinearIterator & Iter)
{
  typedef typename TOutputImage::PixelType OutputPixelType;
  unsigned long j = 0;
  OutputPixelType v;
  while ( !Iter.IsAtEndOfLine() )
    {
      for(unsigned int i=0; i<VectorDimension; i++) v[i]=static_cast<typename OutputPixelType::ComponentType>( m_Scratch[j][i]);
    Iter.Set( v );
    ++Iter;
    ++j;
    }

}


/**
 * Copy one line of the output image to the scratch
 */
template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::CopyCoefficientsToScratch(OutputLinearIterator & Iter)
{
  unsigned long j = 0;
  itk::Vector<double, VectorDimension> v;
  while ( !Iter.IsAtEndOfLine() )
    {
      for(unsigned int i=0; i<VectorDimension; i++)v[i]=static_cast<double>( Iter.Get()[i] ); 
    m_Scratch[j] = v ;
    ++Iter;
    ++j;
    }
}


/**
 * GenerateInputRequestedRegion method.
 */
template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  // this filter requires the all of the input image to be in
  // the buffer
  InputImagePointer  inputPtr = const_cast< TInputImage * > ( this->GetInput() );
  if( inputPtr )
    {
    inputPtr->SetRequestedRegionToLargestPossibleRegion();
    }
}


/**
 * EnlargeOutputRequestedRegion method.
 */
template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::EnlargeOutputRequestedRegion( itk::DataObject *output )
{

  // this filter requires the all of the output image to be in
  // the buffer
  TOutputImage *imgData;
  imgData = dynamic_cast<TOutputImage*>( output );
  if( imgData )
    {
    imgData->SetRequestedRegionToLargestPossibleRegion();
    }

}

/**
 * Generate data
 */
template <class TInputImage, class TOutputImage>
void
VectorBSplineDecompositionImageFilterWithOBD<TInputImage, TOutputImage>
::GenerateData()
{
  DD("VectorBSplineDecompositionImageFilterWithOBD GenerateData()");
  // Allocate scratch memory
  InputImageConstPointer inputPtr = this->GetInput();
  m_DataLength = inputPtr->GetBufferedRegion().GetSize();

  unsigned long maxLength = 0;
  for ( unsigned int n = 0; n < ImageDimension; n++ )
    {
    if ( m_DataLength[n] > maxLength )
      {
      maxLength = m_DataLength[n];
      }
    }
  m_Scratch.resize( maxLength );

  // Allocate memory for output image
  OutputImagePointer outputPtr = this->GetOutput();
  outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
  outputPtr->Allocate();

  // Calculate actual output
  this->DataToCoefficientsND();

  // Clean up
  m_Scratch.clear();

}


} // namespace clitk

#endif
