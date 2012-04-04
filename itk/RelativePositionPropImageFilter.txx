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
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: RelativePositionPropImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2010/07/12 06:57:25 $
  Version:   $Revision: 1.2 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/
#ifndef _RelativePositionPropImageFilter_txx
#define _RelativePositionPropImageFilter_txx

#include "RelativePositionPropImageFilter.h"

#include "itkNeighborhoodOperatorImageFilter.h"
#include "itkDerivativeOperator.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkProgressAccumulator.h"
#include "itkImageFileWriter.h"
#include "itkSimpleContourExtractorImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

namespace itk
{

  template <class TInputImage, class TOutputImage,class TtNorm>
  void 
  RelativePositionPropImageFilter<TInputImage,TOutputImage,TtNorm>
  ::GenerateInputRequestedRegion() throw (InvalidRequestedRegionError)
  {
    // call the superclass' implementation of this method
    Superclass::GenerateInputRequestedRegion ();

    // We need all the input.
    typename InputImageType::Pointer input =
      const_cast < InputImageType * >(this->GetInput ());

    input->SetRequestedRegion (input->GetLargestPossibleRegion ());
  }

  /**
   * Generate all the output data
   */
  template < class TInputImage, class TOutputImage,class TtNorm>
  void RelativePositionPropImageFilter < TInputImage,
                                         TOutputImage ,TtNorm>::EnlargeOutputRequestedRegion (DataObject * output)
  {
    Superclass::EnlargeOutputRequestedRegion (output);
    output->SetRequestedRegionToLargestPossibleRegion ();
  }
  
  
  inline float min(float a,float b)
  {
    return (a<b?a:b);
  }  
  
  template< class TInputImage, class TOutputImage ,class TtNorm>
  void
  RelativePositionPropImageFilter< TInputImage, TOutputImage ,TtNorm>
  //::GenerateThreadedData(const typename TOutputImage::RegionType& outputRegionForThread, int threadId)
  ::GenerateData()
  {

    this->AllocateOutputs();
    computeDirection();
    typename InputImageType::ConstPointer input = this->GetInput();
  
    typename CorrespondanceMapType::Pointer m_CorrespondanceMap = InitCorrespondanceMap();

    typename InputImageType::IndexType nullIndex;
    nullIndex.Fill(-1);
  
    typedef itk::ImageRegionConstIteratorWithIndex<InputImageType> InputConstIteratorWithIndexType;  
    typedef itk::ImageRegionIteratorWithIndex<OutputImageType> OutputIteratorType;
  
    typedef itk::NeighborhoodIterator< CorrespondanceMapType > NeighborhoodIteratorType;

   
    typename NeighborhoodIteratorType::RadiusType radius;
    radius.Fill(m_Radius);
    NeighborhoodIteratorType  it( radius,
                                  m_CorrespondanceMap,
                                  m_CorrespondanceMap->GetLargestPossibleRegion());  
    OutputIteratorType outputIt( this->GetOutput(),
                                 this->GetOutput()->GetLargestPossibleRegion() );    

    typename InputImageType::SizeType size = input->GetLargestPossibleRegion().GetSize();
    unsigned int totalSize = 1;
    for(int i=0;i<ImageDimension;i++)
      {
        totalSize *= size[i];
      }	
    const typename InputImageType::OffsetValueType	* offsetTable = input->GetOffsetTable();
  
  
  
    VectorType vecttemp;  
    OutputPixelType max;  
    int indmax;
    OutputPixelType tmp;
    typename InputImageType::IndexType ind1;
    bool in=false;
    //unsigned int centeroffset = (int)(it.Size() / 2);
  
    if(!m_Fast)
      {  
        for(int i=0;i<pow((double)2,(int)ImageDimension);i++)
          {
            int orient[ImageDimension];
            int init[ImageDimension];
            typename InputImageType::OffsetType tempOffset;
            tempOffset.Fill(0);
   	
            it.GoToBegin();
            outputIt.GoToBegin();
            for(int j=0;j<ImageDimension;j++)
              {
                orient[j] = ((int)(i/pow((double)2,j)))%2;
                tempOffset[j] = (orient[j])*(size[j]-1);
              }
            it += tempOffset;
  	
            outputIt.SetIndex(it.GetIndex());
  	
            for(int j=0;j<ImageDimension;j++)
              {
                if(orient[j]==0)
                  init[j] = -size[j]+1;
                else
                  init[j] = size[j]-1;
              }

            // DD(totalSize);
            for(unsigned int p=1;p<=totalSize;p++)
              {
                // DD(p);
                if ((m_VerboseProgress && (p % totalSize/100) == 0)) {
                  DD(p);
                }
                max = -100;
                indmax = -1;
                for (unsigned i = 0; i < it.Size(); i++)
                  {
                    if( it.GetPixel(i,in)!=nullIndex && in )
                      {
                        for(int j=0;j<ImageDimension;j++)
                          {	
                            ind1[j] = it.GetPixel(i)[j];
                            vecttemp[j] = it.GetPixel(i)[j]-outputIt.GetIndex()[j];
                          }
		
                        tmp = static_cast<double>(sqrt(vecttemp*vecttemp)) ;
                        if(tmp!=0)	
                          {
                            if((m_DirectionVector*vecttemp)/tmp>=1)
                              tmp = 0;
                            else
                              if((m_DirectionVector*vecttemp)/tmp<=-1)
                                tmp = 4.*atan(1.0);
                              else
                                tmp = acos((m_DirectionVector*vecttemp)/tmp);
                          }
                        tmp=std::max(0.,1.-(tmp/m_K1));
                        // tmp= (m_K2 - tmp)/(m_K2-m_K1); //1-tmp/m_K;
	  					
                        tmp = m_TNorm( input->GetPixel(ind1), tmp);
                        if ( tmp > max)
                          {
                            max = tmp;
                            indmax = i;
                          }
                      }
                  }
		
                if(indmax!=-1 )
                  {
                    it.SetCenterPixel(it.GetPixel(indmax));
                    if( max>0)
                      outputIt.Set( max );
                    else outputIt.Set( 0 );
                  }
                else 
                  {
                    outputIt.Set( 0 );
                  }
		
                tempOffset.Fill(0);
                for(int j=0;j<ImageDimension;j++)
                  {
                    if(p%offsetTable[j]==0)
                      {
                        if(orient[j]==0)
                          {
                            tempOffset[j] = 1;
                            for(int k=0;k<j;k++)
                              tempOffset[k] = init[k];
                          }
                        else
                          {
                            tempOffset[j] = -1;
                            for(int k=0;k<j;k++)
                              tempOffset[k] = init[k];
                          }
                      }	
                  }
                it += tempOffset;
                outputIt.SetIndex(it.GetIndex());
              }
          }
      }

    // ==================================================================================================
    else  //if fast compute in two pass
      {
        // 	std::cout<<"pass 1"<<std::endl;

        //         DD(it.Size());
        
        //pass 1
        long i=0;
         long nb = input->GetLargestPossibleRegion().GetNumberOfPixels( );
        for (it.GoToBegin(), outputIt.GoToBegin(); ! it.IsAtEnd(); ++it, ++outputIt)
          {   
            if (m_VerboseProgress && (i%(nb/10)) == 0) {
              //DD(i);     
              std::cout << i << " / " << nb << std::endl;
            }
            i++;

            max = -100;
            indmax = -1;
            for (unsigned i = 0; i < it.Size(); i++) {
              if( it.GetPixel(i,in)!=nullIndex && in )
                {

                  // DS : can be precomputed ??? no
                  for(int j=0;j<ImageDimension;j++)
                    vecttemp[j] = it.GetPixel(i)[j]-outputIt.GetIndex()[j];
                  // DD(vecttemp);

                  tmp = vecttemp.GetNorm();
                  if(tmp!=0)	
                    tmp = acos((m_DirectionVector*vecttemp)/tmp);
                  //tmp= (m_K2 - tmp)/(m_K2-m_K1); //1-tmp/m_K;
                  tmp=std::max(0.,1.-tmp/m_K1);
                  tmp = min( input->GetPixel(it.GetPixel(i)), tmp);
                  if ( tmp > max)
                    {
                      max = tmp;
                      indmax = i;
                    }
                }
            }

            if(max>0)
              {
                it.SetCenterPixel(it.GetPixel(indmax));
                outputIt.Set( max );
              }
            else 
              outputIt.Set( 0 );

          }

        //  std::cout<<"pass 2"<<std::endl;
        //pass2
        it.GoToEnd();
        --it;
        
        i=0;
        nb = this->GetOutput()->GetLargestPossibleRegion().GetNumberOfPixels( );

        for ( outputIt.GoToReverseBegin(); ! outputIt.IsAtReverseEnd(); --it, --outputIt)
          {    
             if (m_VerboseProgress && (i%(nb/10)) == 0) {
              //DD(i);     
              std::cout << i << " / " << nb << std::endl;
            }
            i++;

            max = -100;
            indmax = -1;
	
            for (unsigned i = 0; i < it.Size(); i++)
              if( it.GetPixel(i,in)!=nullIndex && in )
                {
                  for(int j=0;j<ImageDimension;j++)
                    vecttemp[j] = it.GetPixel(i)[j]-outputIt.GetIndex()[j];
		
                  tmp = vecttemp.GetNorm();
			
                  if(tmp!=0)
                    tmp = acos((m_DirectionVector*vecttemp)/tmp);
                  //tmp= (m_K2 - tmp)/(m_K2-m_K1); //1-tmp/m_K;
                  tmp=std::max(0.,1. - tmp/m_K1);
                  tmp = min( input->GetPixel(it.GetPixel(i)),tmp);
      		
                  if ( tmp > max)
                    {
                      max = tmp;
                      indmax = i;
                    }
                }
			
            if(max>0)
              {
                it.SetCenterPixel(it.GetPixel(indmax));
                outputIt.Set( max );
              }
            else 
              outputIt.Set( 0 );
          }	
      }

  }

  template< class TInputImage, class TOutputImage, class TtNorm >
  void
  RelativePositionPropImageFilter< TInputImage, TOutputImage, TtNorm >::
  PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);

    os << indent << "First Angle: " << m_Alpha1 << std::endl;
    os << indent << "Second Angle: " << m_Alpha2 << std::endl;
    os << indent << "K1: " << m_K1 << std::endl;
  }

  template< class TInputImage, class TOutputImage, class TtNorm >
  typename RelativePositionPropImageFilter< TInputImage, TOutputImage, TtNorm >::TabulationImageType::Pointer  
  RelativePositionPropImageFilter< TInputImage, TOutputImage, TtNorm >::
  ComputeAngleTabulation(){
    computeDirection();
    typename TabulationImageType::Pointer m_AngleTabulation; 
    m_AngleTabulation = TabulationImageType::New();
	
    typename TabulationImageType::IndexType start;
	
    for(register int i=0;i<ImageDimension;i++)
      start[i]=0;
	
    typename TabulationImageType::SizeType size = this->GetInput()->GetLargestPossibleRegion().GetSize();
	
    for(register int i=0;i<ImageDimension;i++)
      size[i]*=2;
	
    typename TabulationImageType::RegionType region;
	
    region.SetSize(size);
    region.SetIndex(start);
	
    m_AngleTabulation->SetRegions(region);
	
    m_AngleTabulation->Allocate();
    m_AngleTabulation->SetRegions(m_AngleTabulation->GetLargestPossibleRegion());
    typedef typename itk::ImageRegionIteratorWithIndex<TabulationImageType>
      InputIteratorType;
    InputIteratorType inputIt = InputIteratorType(m_AngleTabulation, m_AngleTabulation->GetRequestedRegion());
	
    typename TabulationImageType::IndexType requestedIndex =
      m_AngleTabulation->GetRequestedRegion().GetIndex();
	
    typename TabulationImageType::SizeType center = this->GetInput()->GetLargestPossibleRegion().GetSize();
    for(register int i=0;i<ImageDimension;i++)
      center[i]-=1;
		
    VectorType vecttemp;
	
    for(inputIt.GoToBegin();!inputIt.IsAtEnd();++inputIt)
      {
        typename TabulationImageType::IndexType idx = inputIt.GetIndex();
        typename TabulationImageType::IndexType diff = idx - center;
        for(int i=0;i<ImageDimension;i++)
          vecttemp[i]=diff[i];
        double tmp = static_cast<double>(sqrt(vecttemp*vecttemp)) ;
        if(tmp==0)
          {//std::cout<<"tem"<<std::endl;
            inputIt.Set(0);}
        else
          {
            double cos_angle = acos((m_DirectionVector*vecttemp)/tmp);
            inputIt.Set(cos_angle);
          }
      }
	
    typedef itk::ImageFileWriter<TabulationImageType> WT;
    typename WT::Pointer wt = WT::New();
    wt->SetFileName("testangle.nii");
    wt->SetInput(m_AngleTabulation);
    wt->Write();
    std::cout<<"end compute angle"<<std::endl;

    return m_AngleTabulation;
  }

  template< class TInputImage, class TOutputImage, class TtNorm>
  typename RelativePositionPropImageFilter< TInputImage, TOutputImage, TtNorm >::CorrespondanceMapType::Pointer
  RelativePositionPropImageFilter< TInputImage, TOutputImage, TtNorm >::
  InitCorrespondanceMap()
  {
    typename CorrespondanceMapType::Pointer m_CorrespondanceMap;
    m_CorrespondanceMap = CorrespondanceMapType::New();
    m_CorrespondanceMap->SetRegions(this->GetInput()->GetLargestPossibleRegion());	
    m_CorrespondanceMap->Allocate();

    typename InputImageType::IndexType nullIndex;
    nullIndex.Fill(-1);

    typedef itk::ImageRegionIterator< CorrespondanceMapType > CorrIteratorType;
    CorrIteratorType it = CorrIteratorType(m_CorrespondanceMap,
                                           m_CorrespondanceMap->GetLargestPossibleRegion());

    typedef itk::ImageRegionConstIteratorWithIndex< InputImageType>
      InputIteratorType;
    InputIteratorType inputIt = InputIteratorType(this->GetInput(),
                                                  this->GetInput()->GetLargestPossibleRegion());
	
    for(it.GoToBegin(),inputIt.GoToBegin();!it.IsAtEnd();++it,++inputIt)
      if(inputIt.Get()>0)
        it.Set(inputIt.GetIndex());
      else
        it.Set(nullIndex);


 

    return m_CorrespondanceMap;
  }









} // end namespace itk

#endif
