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
#ifndef __clitkLocallyAdaptiveThresholdConnectedImageFilter_txx
#define __clitkLocallyAdaptiveThresholdConnectedImageFilter_txx
#include "clitkLocallyAdaptiveThresholdConnectedImageFilter.h"
#include "clitkLocallyAdaptiveBinaryThresholdImageFunction.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkProgressReporter.h"

namespace clitk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
LocallyAdaptiveThresholdConnectedImageFilter<TInputImage, TOutputImage>
::LocallyAdaptiveThresholdConnectedImageFilter()
{
  m_Lower = itk::NumericTraits<InputImagePixelType>::NonpositiveMin();
  m_Upper = itk::NumericTraits<InputImagePixelType>::max();
  m_LowerBorderIsGiven=true;
  m_UpperBorderIsGiven=true;
  m_MaximumSDIsGiven=true;
  m_Multiplier=1.0;
  m_ReplaceValue = itk::NumericTraits<OutputImagePixelType>::One;
  m_Radius.Fill(1);
  m_MaximumSD=100.0;
}

template <class TInputImage, class TOutputImage>
void
LocallyAdaptiveThresholdConnectedImageFilter<TInputImage, TOutputImage>
::ClearSeeds()
{
  if( this->m_Seeds.size() > 0 )
    {
    this->m_Seeds.clear();
    this->Modified();
    }
}

template <class TInputImage, class TOutputImage>
void
LocallyAdaptiveThresholdConnectedImageFilter<TInputImage, TOutputImage>
::SetSeed(const IndexType & seed)
{
  this->ClearSeeds();
  this->AddSeed ( seed );
}

template <class TInputImage, class TOutputImage>
void
LocallyAdaptiveThresholdConnectedImageFilter<TInputImage, TOutputImage>
::AddSeed ( const IndexType & seed )
{
  this->m_Seeds.push_back ( seed );
  this->Modified();
}

/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
LocallyAdaptiveThresholdConnectedImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Upper: "
     << static_cast<typename itk::NumericTraits<InputImagePixelType>::PrintType>(m_UpperBorderIsGiven)
     << std::endl;
  os << indent << "Lower: "
     << static_cast<typename itk::NumericTraits<InputImagePixelType>::PrintType>(m_LowerBorderIsGiven)
     << std::endl;
  os << indent << "ReplaceValue: "
     << static_cast<double>(m_Multiplier)
     << std::endl;
  os << indent << "ReplaceValue: "
     << static_cast<typename itk::NumericTraits<OutputImagePixelType>::PrintType>(m_ReplaceValue)
     << std::endl;
  os << indent << "Radius: " << m_Radius << std::endl;
}

template <class TInputImage, class TOutputImage>
void 
LocallyAdaptiveThresholdConnectedImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  if ( this->GetInput() )
    {
    InputImagePointer image =
      const_cast< InputImageType * >( this->GetInput() );
    image->SetRequestedRegionToLargestPossibleRegion();
    }
}

template <class TInputImage, class TOutputImage>
void 
LocallyAdaptiveThresholdConnectedImageFilter<TInputImage,TOutputImage>
::EnlargeOutputRequestedRegion(itk::DataObject *output)
{
  Superclass::EnlargeOutputRequestedRegion(output);
  output->SetRequestedRegionToLargestPossibleRegion();
}

template <class TInputImage, class TOutputImage>
void 
LocallyAdaptiveThresholdConnectedImageFilter<TInputImage,TOutputImage>
::GenerateData()
{
  typename Superclass::InputImageConstPointer inputImage  = this->GetInput();
  typename Superclass::OutputImagePointer     outputImage = this->GetOutput();

  // Zero the output
  outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
  outputImage->Allocate();
  outputImage->FillBuffer ( itk::NumericTraits<OutputImagePixelType>::Zero );
  
  typedef LocallyAdaptiveBinaryThresholdImageFunction<InputImageType> FunctionType;
  typedef itk::FloodFilledImageFunctionConditionalIterator<OutputImageType, FunctionType> IteratorType;

  typename FunctionType::Pointer function = FunctionType::New();
  function->SetInputImage ( inputImage );
  function->SetLowerBorderIsGiven ( m_LowerBorderIsGiven );
  function->SetUpperBorderIsGiven ( m_UpperBorderIsGiven );
  function->SetMaximumSDIsGiven ( m_MaximumSDIsGiven );
  function->ThresholdBetween ( m_Lower, m_Upper );
  function->SetMultiplier ( m_Multiplier );
  function->SetMaximumSD ( m_MaximumSD );
  function->SetRadius (m_Radius);
 
  IteratorType it = IteratorType ( outputImage, function, m_Seeds );
    itk::ProgressReporter progress( this, 0, outputImage->GetRequestedRegion().GetNumberOfPixels());
  
  while( !it.IsAtEnd())
    {
    it.Set(m_ReplaceValue);
    ++it;
    progress.CompletedPixel();
    }
}


} // end namespace clitk

#endif
