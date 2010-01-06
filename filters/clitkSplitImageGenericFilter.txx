/*-------------------------------------------------------------------------
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  -------------------------------------------------------------------------*/

#ifndef clitkSplitImageGenericFilter_TXX
#define clitkSplitImageGenericFilter_TXX

//This is where you put the actual implementation

#include <sstream>
#include <itkExtractImageFilter.h>


//--------------------------------------------------------------------
template<unsigned int Dimension>
void clitk::SplitImageGenericFilter::UpdateWithDim(std::string PixelType, int Components) { 

  if (m_Verbose) std::cout << "Image was detected to be "<<Dimension<<"D and "<<Components<<" component(s) of "<<  PixelType<<"..."<<std::endl;

    if (Components==1)
      {
	if(PixelType == "short"){  
	  if (m_Verbose) std::cout << "Launching filter in "<< Dimension <<"D and signed short..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, signed short>(); 
	}
	    else if(PixelType == "unsigned_short"){  
	       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_short..." << std::endl;
	       UpdateWithDimAndPixelType<Dimension, unsigned short>(); 
	     }
	
	else if (PixelType == "unsigned_char"){ 
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and unsigned_char..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, unsigned char>();
	}
	
	     else if (PixelType == "char"){ 
	       if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and signed_char..." << std::endl;
	       UpdateWithDimAndPixelType<Dimension, signed char>();
	     }
	else {
	  if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and float..." << std::endl;
	  UpdateWithDimAndPixelType<Dimension, float>();
	}
      }

    else if (Components==3)
      {
	if (m_Verbose) std::cout  << "Launching filter in "<< Dimension <<"D and 3D float (DVF)" << std::endl;
	UpdateWithDimAndPixelType<Dimension, itk::Vector<float, Dimension> >();
      }
    else std::cerr<<"Number of components is "<<Components<<", not supported!"<<std::endl;

}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template<unsigned int Dimension, class PixelType>
void clitk::SplitImageGenericFilter::UpdateWithDimAndPixelType() {

  // Read input
  typedef itk::Image<PixelType,Dimension> ImageType;
  typedef itk::Image<PixelType,Dimension-1> OutputImageType;
  typename ImageType::Pointer input = clitk::readImage<ImageType>(mInputFilenames[0], mIOVerbose);
  typedef itk::ExtractImageFilter<ImageType,OutputImageType> FilterType;
  typename FilterType::Pointer filter= FilterType::New();

  filter->SetInput(input);
  typename ImageType::SizeType size=input->GetLargestPossibleRegion().GetSize();
  size[mSplitDimension]=0;
  typename ImageType::RegionType extracted_region;
  extracted_region.SetSize(size);
  filter->SetExtractionRegion(extracted_region);
  filter->Update();

  typename ImageType::IndexType index=input->GetLargestPossibleRegion().GetIndex();
  std::string base_filename=GetOutputFilename();
  unsigned int number_of_output_images=input->GetLargestPossibleRegion().GetSize()[mSplitDimension];
  for (unsigned int i=0;i<number_of_output_images;i++)
  {
      std::ostringstream ss;
      ss << i;
      index[mSplitDimension]=i;
      extracted_region.SetIndex(index);
      filter->SetExtractionRegion(extracted_region);
      filter->Update();
      SetOutputFilename(base_filename+"_"+ss.str()+".mhd");
      typename OutputImageType::Pointer output=filter->GetOutput();
      SetNextOutput<OutputImageType>(output);
  }
}
//--------------------------------------------------------------------

#endif  //#define clitkSplitImageGenericFilter_TXX
