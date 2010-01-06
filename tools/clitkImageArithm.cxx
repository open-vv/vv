/*-------------------------------------------------------------------------
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
                                                                             
  -------------------------------------------------------------------------*/

#ifndef CLITKIMAGEARITHM_CXX
#define CLITKIMAGEARITHM_CXX

/**
   -------------------------------------------------
   * @file   clitkImageArithm.cxx
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   23 Feb 2008 08:37:53
   -------------------------------------------------*/

// clitk include
#include "clitkImageArithm_ggo.h"
#include "clitkImageArithmGenericFilter.h"
#include "clitkIO.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkImageArithm, args_info);
  CLITK_INIT;

  // Check that we have either the 2nd image or the scalar 
  if ((args_info.input2_given) && (args_info.scalar_given)) {
    std::cerr << "ERROR : you cannot provide both --scalar and --input2 option" << std::endl;
    exit(-1);
  }
  if ((!args_info.input2_given) && (!args_info.scalar_given)) {
    if (args_info.operation_arg < 5) {
      std::cerr << "Such operation need the --scalar option." << std::endl;
      exit(-1);
    }
  }
  
  // Read input image header1 to check image dimension
  itk::ImageIOBase::Pointer header1 = clitk::readImageHeader(args_info.input1_arg);
  unsigned int dim1 = header1->GetNumberOfDimensions();
  std::string pixelTypeName = header1->GetComponentTypeAsString(header1->GetComponentType());

  // Options for arithm operation between 2 images
  if (args_info.input2_given) {

    itk::ImageIOBase::Pointer header2 = clitk::readImageHeader(args_info.input2_arg);
    unsigned int dim2 = header2->GetNumberOfDimensions();
    std::string pixelTypeName2 = header2->GetComponentTypeAsString(header1->GetComponentType());
    
    // Check dimension
    if (dim1 != dim2) {
      std::cerr << "Images should have the same dimension : " <<std::endl;
      std::cerr  << "Dim input1 = " << dim1 << std::endl;
      std::cerr  << "Dim input2 = " << dim2 << std::endl;
      exit(-1);
    }

    // Check same type of pixel in both images
    if (pixelTypeName2 != pixelTypeName) {
      std::cerr << "Input images do not have the same pixel type: " <<std::endl;
      std::cerr  << "Type input1 = " << pixelTypeName << std::endl;
      std::cerr  << "Type input2 = " << pixelTypeName2 << std::endl;
      exit(-1);
    }
    
    // Check same X,Y dimensions in both images
    for(int i=0; i<2; i++) {
      if ( header1->GetDimensions(i) != header2->GetDimensions(i) ) {
	std::cerr << "ERROR: input images should have same X and Y size. " <<  std::endl;
	std::cerr << "Dimensions X and Y of input1: = " << header1->GetDimensions(0) <<"  "<< header1->GetDimensions(1)<< std::endl;
	std::cerr << "Dimensions X and Y of input2: = " << header2->GetDimensions(0) <<"  "<< header2->GetDimensions(1)<< std::endl;
	exit(-1);
      }      
    }//end for
    if (dim1 == 3) {
      if (header1->GetDimensions(2) < header2->GetDimensions(2)) {
	std::cerr << "ERROR: Z size in input1 is greater than in input2. " <<  std::endl;
	std::cerr << "Size input1 : " << header1->GetDimensions(0) << " " << header1->GetDimensions(1)<< " " << header1->GetDimensions(2) << std::endl;
	std::cerr << "Size input2 : " << header2->GetDimensions(0) << " " << header2->GetDimensions(1)<< " " << header2->GetDimensions(2) << std::endl;
      }
    }
  } //end if operation between 2 images	
  
  // Creation of a generic filter
  clitk::ImageArithmGenericFilter::Pointer filter = clitk::ImageArithmGenericFilter::New();
  filter->AddInputFilename(args_info.input1_arg);
  if (args_info.input2_given) filter->AddInputFilename(args_info.input2_arg);
  else filter->SetScalar(args_info.scalar_arg);
  //if (args_info.binary_given) filter->SetBinaryMaskFilename(args_info.binary_arg);   
  filter->SetTypeOfOperation(args_info.operation_arg);   
  filter->SetDefaultPixelValue(args_info.pixelValue_arg);
  filter->SetOutputFilename(args_info.output_arg);

  // Go ! 
  filter->Update();

  // this is the end my friend  
  return 0;
} // end main

#endif //define CLITKIMAGEARITHM_CXX
