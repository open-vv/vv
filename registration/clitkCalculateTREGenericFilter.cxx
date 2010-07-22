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
#ifndef clitkCalculateTREGenericFilter_cxx
#define clitkCalculateTREGenericFilter_cxx

/* =================================================
 * @file   clitkCalculateTREGenericFilter.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/

#include "clitkCalculateTREGenericFilter.h"


namespace clitk
{


  //-----------------------------------------------------------
  // Constructor
  //-----------------------------------------------------------
  CalculateTREGenericFilter::CalculateTREGenericFilter()
  {
    m_Verbose=false;
    m_InputFileName="";
  }


  //-----------------------------------------------------------
  // Update
  //-----------------------------------------------------------
  void CalculateTREGenericFilter::Update()
  {
    // Read the Dimension and PixelType
    int Dimension, Components;
    std::string PixelType;
    ReadImageDimensionAndPixelType(m_InputFileName, Dimension, PixelType, Components);

    
    // Call UpdateWithDim
    if(Dimension==2) ReadVectorFields<2,2>();
    else if(Dimension==3) ReadVectorFields<3,3>();
    else if (Dimension==4)ReadVectorFields<4,3>(); 
    else 
      {
	std::cout<<"Error, Only for 2, 3 or 4  Dimensions!!!"<<std::endl ;
	return;
      }
  }



  //-----------------------------
  // Process DVF
  //-----------------------------
  template< >
  void 
  CalculateTREGenericFilter::ProcessVectorFields<4,3>(std::vector<itk::Image<itk::Vector<float, 3>, 4>::Pointer > dvfs,  char** filenames )
  {
    // Typedefs
    typedef itk::Vector<float,3> PixelType;
    typedef itk::Image<PixelType, 4> InputImageType;
    typedef itk::Image<PixelType, 3> OutputImageType;

    // IO
    InputImageType::Pointer input=dvfs[0];
    std::vector<itk::Image<itk::Vector<float, 3>, 3>::Pointer > new_dvfs;
    
    // Split vector field
    typedef itk::ExtractImageFilter<InputImageType,OutputImageType> FilterType;
    unsigned int splitDimension=3;

    // Make new file names
    std::vector<std::string> new_filenames;
    std::string base = filenames[0];

    // Set the extract region
    InputImageType::SizeType size=input->GetLargestPossibleRegion().GetSize();
    size[splitDimension]=0;
    InputImageType::RegionType extracted_region;
    extracted_region.SetSize(size);
    InputImageType::IndexType index=input->GetLargestPossibleRegion().GetIndex();
    
  
    // Loop
    for (unsigned int i=0;i<input->GetLargestPossibleRegion().GetSize()[splitDimension];i++)
      {
	
	// Skip?
	if (m_ArgsInfo.skip_given && i==(unsigned int) m_ArgsInfo.skip_arg) continue;

	// extract dvf
	 FilterType::Pointer filter= FilterType::New();
	filter->SetInput(input);
	index[splitDimension]=i;
	extracted_region.SetIndex(index);
	filter->SetExtractionRegion(extracted_region);
	filter->Update();
	new_dvfs.push_back(filter->GetOutput());
	
	// make name
	std::ostringstream number_dvf;
	number_dvf << i;
	std::string number =  number_dvf.str();
	new_filenames.push_back(base+"_"+number);
      }

    // Update
    this->UpdateWithDim<3>(new_dvfs, new_filenames); 

  }




  
} //end clitk

#endif  //#define clitkCalculateTREGenericFilter_cxx
