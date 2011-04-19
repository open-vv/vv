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
===========================================================================*/
#ifndef __clitkExplosionControlledThresholdConnectedImageFilter_txx
#define __clitkExplosionControlledThresholdConnectedImageFilter_txx

#include "clitkExplosionControlledThresholdConnectedImageFilter.h"
#include "itkBinaryThresholdImageFunction.h"
#include "itkFloodFilledImageFunctionConditionalIterator.h"
#include "itkShapedFloodFilledImageFunctionConditionalIterator.h"
#include "itkProgressReporter.h"

namespace clitk
{

  //--------------------------------------------------------------------
  /**
   * Constructor
   */
  template <class TInputImage, class TOutputImage>
  ExplosionControlledThresholdConnectedImageFilter<TInputImage, TOutputImage>
  ::ExplosionControlledThresholdConnectedImageFilter()
  {
    m_Lower = itk::NumericTraits<InputImagePixelType>::NonpositiveMin();
    m_Upper = itk::NumericTraits<InputImagePixelType>::max();
    m_FinalLower = itk::NumericTraits<InputImagePixelType>::NonpositiveMin();
    m_FinalUpper = itk::NumericTraits<InputImagePixelType>::max();
    m_AdaptLowerBorder=false;
    m_AdaptUpperBorder=false;
    m_MaximumUpperThreshold=itk::NumericTraits<InputImagePixelType>::max();
    m_MinimumLowerThreshold=itk::NumericTraits<InputImagePixelType>::NonpositiveMin();
    m_Multiplier=2.0;
    m_ReplaceValue = itk::NumericTraits<OutputImagePixelType>::One;
    m_MinimumThresholdStepSize=itk::NumericTraits<InputImagePixelType>::One;
    m_ThresholdStepSize=64;
    m_Verbose=false;
    m_FullyConnected=false;
    m_MinimumSize=5000;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage, class TOutputImage>
  void
  ExplosionControlledThresholdConnectedImageFilter<TInputImage, TOutputImage>
  ::ClearSeeds()
  {
    if( this->m_Seeds.size() > 0 )
      {
	this->m_Seeds.clear();
	this->Modified();
      }
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage, class TOutputImage>
  void
  ExplosionControlledThresholdConnectedImageFilter<TInputImage, TOutputImage>
  ::SetSeed(const IndexType & seed)
  {
    this->ClearSeeds();
    this->AddSeed ( seed );
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage, class TOutputImage>
  void
  ExplosionControlledThresholdConnectedImageFilter<TInputImage, TOutputImage>
  ::AddSeed ( const IndexType & seed )
  {
    this->m_Seeds.push_back ( seed );
    this->Modified();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  /**
   * Standard PrintSelf method.
   */
  template <class TInputImage, class TOutputImage>
  void
  ExplosionControlledThresholdConnectedImageFilter<TInputImage, TOutputImage>
  ::PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Upper: "
       << static_cast<typename itk::NumericTraits<InputImagePixelType>::PrintType>(m_Upper)
       << std::endl;
    os << indent << "Lower: "
       << static_cast<typename itk::NumericTraits<InputImagePixelType>::PrintType>(m_Lower)
       << std::endl;
    os << indent << "ReplaceValue: "
       << static_cast<double>(m_Multiplier)
       << std::endl;
    os << indent << "ReplaceValue: "
       << static_cast<typename itk::NumericTraits<OutputImagePixelType>::PrintType>(m_ReplaceValue)
       << std::endl;
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage, class TOutputImage>
  void 
  ExplosionControlledThresholdConnectedImageFilter<TInputImage,TOutputImage>
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
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage, class TOutputImage>
  void 
  ExplosionControlledThresholdConnectedImageFilter<TInputImage,TOutputImage>
  ::EnlargeOutputRequestedRegion(itk::DataObject *output)
  {
    Superclass::EnlargeOutputRequestedRegion(output);
    output->SetRequestedRegionToLargestPossibleRegion();
  }
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template <class TInputImage, class TOutputImage>
  void 
  ExplosionControlledThresholdConnectedImageFilter<TInputImage,TOutputImage>
  ::GenerateData()
  {
    typename Superclass::InputImageConstPointer inputImage  = this->GetInput();
    typename Superclass::OutputImagePointer     outputImage = this->GetOutput();

    // Zero the output
    outputImage->SetBufferedRegion( outputImage->GetRequestedRegion() );
    outputImage->Allocate();
    outputImage->FillBuffer ( itk::NumericTraits<OutputImagePixelType>::Zero );
  

    //--------------------------------------------------
    // Get initial region size
    //--------------------------------------------------
    typedef itk::BinaryThresholdImageFunction<InputImageType> FunctionType;
    //typedef itk::ShapedFloodFilledImageFunctionConditionalIterator<OutputImageType, FunctionType> IteratorType;
    typedef itk::FloodFilledImageFunctionConditionalIterator<OutputImageType, FunctionType> IteratorType;
    typename FunctionType::Pointer function = FunctionType::New();
    function->SetInputImage ( inputImage );
    function->ThresholdBetween ( m_Lower, m_Upper );
    IteratorType it = IteratorType ( outputImage, function, m_Seeds );
    //it.SetFullyConnected(m_FullyConnected);
    unsigned int initialSize=0; 

    it.GoToBegin();
    while( !it.IsAtEnd())
      {
	++initialSize;
	++it;
      }
    if (m_Verbose)  std::cout<<"Initial region size using thresholds ["<<m_Lower<<", "<<m_Upper<<"], is "<<initialSize<<"..."<<std::endl;  
  
  
    //--------------------------------------------------
    // Decrease lower threshold
    //--------------------------------------------------
    m_FinalLower=m_Lower;
    if (m_AdaptLowerBorder)
      {
	// Init
	InputImagePixelType currentLower=m_Lower;
	unsigned int currentSize=initialSize;
	unsigned int previousSize=initialSize;
      
	// Lower the threshold till explosion 
	while ( (previousSize<m_MinimumSize) || (  (currentLower> m_MinimumLowerThreshold) && ( (unsigned int) currentSize <= (unsigned int )m_Multiplier* previousSize)  ) )
	  {
	    // Update threshold
	    currentLower-=m_ThresholdStepSize;
	    function->ThresholdBetween(currentLower, m_Upper);

	    // Get current region size
	    previousSize=currentSize;
	    currentSize=0; 
	    IteratorType it = IteratorType ( outputImage, function, m_Seeds );
	    it.GoToBegin();
	    while( !it.IsAtEnd())
	      {
		++currentSize;
		++it;
	      }
	    if (m_Verbose)  std::cout<<"Decreasing lower threshold to "<<currentLower<<", size is "<<currentSize  <<"(previously "<<previousSize<<") ..."<<std::endl;  
	  }


	// Explosion occured, increase lower theshold
	if ( (double)currentSize > m_Multiplier*(double) previousSize)
	  {
	    // Raise lower threshold
	    if (m_Verbose)  std::cout<<"Explosion detected, adapting lower threshold ..."<<std::endl;
	    currentLower+=m_ThresholdStepSize;
	    InputImagePixelType currentStepSize=m_ThresholdStepSize;
	   	  
	    while (currentStepSize>m_MinimumThresholdStepSize)
	      {
		currentStepSize/=2;
		currentLower-=currentStepSize;
	      
		// Get current region size
		currentSize=0; 
		IteratorType it = IteratorType ( outputImage, function, m_Seeds );
		it.GoToBegin();
		while( !it.IsAtEnd())
		  {
		    ++currentSize;
		    ++it;
		  }
	      
		if (m_Verbose)  std::cout<<"Adapting lower threshold to "<<currentLower<<", size is "<<currentSize  <<"(previously "<<previousSize<<") ..."<<std::endl;  
		
		// Explosion: go back
		if ((double)currentSize > m_Multiplier* (double) previousSize)
		  currentLower+=currentStepSize;

		// No explosion: adapt previous
		else previousSize=currentSize;
	      } 
	  
	    // Converged
	    m_FinalLower=currentLower;
	    if (m_Verbose)  std::cout<<"Final lower threshold  (precision="<<m_MinimumThresholdStepSize<<") is set to "<<m_FinalLower<<"..."<<std::endl;
	  }
	else {
          if (m_Verbose) std::cout<<"No explosion before minimum lower threshold reached!"<<std::endl;
        }
      
      } // end update lower
	
  
    //--------------------------------------------------
    // Increase upper threshold
    //--------------------------------------------------
    m_FinalUpper=m_Upper;
    if (m_AdaptUpperBorder)
      {
	// Init
	InputImagePixelType currentUpper=m_Upper;
	unsigned int currentSize=initialSize;
	unsigned int previousSize=initialSize;
      
	// Upper the threshold till explosion 
	while ((previousSize<m_MinimumSize) || ( (currentUpper< m_MaximumUpperThreshold) && ((double)currentSize <= m_Multiplier* (double) previousSize) )  )
	  {
	    // Update threshold
	    currentUpper+=m_ThresholdStepSize;
	    function->ThresholdBetween(m_Lower,currentUpper);

	    // Get current region size
	    previousSize=currentSize;
	    currentSize=0; 
	    IteratorType it = IteratorType ( outputImage, function, m_Seeds );
	    it.GoToBegin();
	    while( !it.IsAtEnd())
	      {
		++currentSize;
		++it;
	      }
	    if (m_Verbose)  std::cout<<"Increasing upper threshold to "<<currentUpper<<", size is "<<currentSize  <<"(previously "<<previousSize<<") ..."<<std::endl;  

	  }

	// Explosion occured, decrease upper theshold
	if ((double)currentSize > m_Multiplier* (double) previousSize)
	  {
	    // Lower upper threshold
	    if (m_Verbose)  std::cout<<"Explosion detected, adapting upper threshold ..."<<std::endl;
	    currentUpper-=m_ThresholdStepSize;
	    InputImagePixelType currentStepSize=m_ThresholdStepSize;
	   	  
	    while (currentStepSize>m_MinimumThresholdStepSize)
	      {
		currentStepSize/=2;
		currentUpper+=currentStepSize;
	      
		// Get current region size
		currentSize=0; 
		function->ThresholdBetween(m_Lower,currentUpper);
		IteratorType it = IteratorType ( outputImage, function, m_Seeds );
		it.GoToBegin();
		while( !it.IsAtEnd())
		  {
		    ++currentSize;
		    ++it;
		  }
		if (m_Verbose)  std::cout<<"Adapting upper threshold to "<<currentUpper<<", size is "<<currentSize  <<"(previously "<<previousSize<<") ..."<<std::endl;  
	
		// Explosion: go back
		if ((double)currentSize > m_Multiplier* (double) previousSize)
		  currentUpper-=currentStepSize;

		// No explosion: adapt previous
		else previousSize=currentSize;
	      } 
	    
	    // Converged
	    m_FinalUpper=currentUpper;
	    if (m_Verbose)  std::cout<<"Final upper threshold  (precision="<<m_MinimumThresholdStepSize<<") is set to "<<m_FinalUpper<<"..."<<std::endl;
	    
	  }
	else {  if (m_Verbose)  std::cout<<"No explosion before maximum upper threshold reached!"<<std::endl; }
	
      } // end update upper



	//--------------------------------------------------
	// Update the output with final thresholds
	//--------------------------------------------------
    function->ThresholdBetween(m_FinalLower, m_FinalUpper);
    IteratorType it2 = IteratorType ( outputImage, function, m_Seeds );
    it2.GoToBegin();
    while( !it2.IsAtEnd())
      {
	it2.Set(m_ReplaceValue);
	++it2;
      }      

  }
  //--------------------------------------------------------------------


} // end namespace clitk

#endif
