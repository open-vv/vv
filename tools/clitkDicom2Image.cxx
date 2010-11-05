/*=========================================================================
                                                                                
Program:   clitk
Module:    $RCSfile: clitkDicom2Image.cxx,v $
Language:  C++
Date:      $Date: 2010/11/05 00:05:00 $
Version:   $Revision: 1.1 $
                                                                                
Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
l'Image). All rights reserved. See Doc/License.txt or
http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

/**
   =================================================
   * @file   clitkDicom2Image.cxx
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   26 Oct 2006
   * 
   * @brief  
   * 
   =================================================*/

// clitk includes
#include "clitkDicom2Image_ggo.h"
#include <clitkCommon.h>
#include "clitkImageCommon.h"

// itk include
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

// Why this is needed ???
//const double itk::NumericTraits<double>::Zero = 0.0;

//====================================================================
int main(int argc, char * argv[]) {

  // init command line
  GGO(clitkDicom2Image, args_info);
  std::vector<std::string> input_files;
  ///if std_input is given, read the input files from stdin
  if (args_info.std_input_given)
  {
      while (true)
      {
          std::string tmp;
          std::cin >> tmp;
          if(std::cin.good())
              input_files.push_back(tmp);
          else break;
      }
      args_info.inputs_num=input_files.size();
  }
  else for (unsigned int i=0;i<args_info.inputs_num;i++)
      input_files.push_back(args_info.inputs[i]);

  
  //===========================================
  /// Get slices locations ...
    std::vector<double> sliceLocations;
    for(unsigned int i=0; i<args_info.inputs_num; i++) {
      //std::cout << "Reading <" << input_files[i] << std::endl;	
      gdcm::File * header = clitk::readDicomHeader(input_files[i]);
      sliceLocations.push_back(header->GetZOrigin());
      // check
      /*if (!header->IsSignedPixelData()) {
	std::cerr << "Pixel type not signed ! " << std::endl;
	std::cerr << "In file " << input_files[i] << std::endl;
	exit(0);
	}*/
      if (header->GetPixelSize() != 2) {
	std::cerr << "Pixel type 2 bytes ! " << std::endl;
	std::cerr << "In file " << input_files[i] << std::endl;
	exit(0);
      }
    }

    //===========================================
    // Sort slices locations ...
    std::vector<int> sliceIndex;
    clitk::GetSortedIndex(sliceLocations, sliceIndex);
    if (args_info.verboseSliceLocation_flag) {
      std::cout << sliceLocations[sliceIndex[0]] << " -> " 
		<< sliceIndex[0] << " / " << 0 << " => " 
		<< "0 mm " 
		<< input_files[sliceIndex[0]]
		<< std::endl;
      for(unsigned int i=1; i<sliceIndex.size(); i++) {
	std::cout << sliceLocations[sliceIndex[i]] << " -> " 
		  << sliceIndex[i] << " / " << i << " => " 
		  << sliceLocations[sliceIndex[i]] - sliceLocations[sliceIndex[i-1]] 
		  << "mm " 
		  << input_files[sliceIndex[i]]
		  << std::endl;
      }
    }

    //===========================================
    // Analyze slices locations ...
    double currentDist;
    double dist=0;
    double tolerance = args_info.tolerance_arg;
    double previous = sliceLocations[sliceIndex[0]];
    for(unsigned int i=1; i<sliceIndex.size(); i++) {
      currentDist = sliceLocations[sliceIndex[i]]-previous;
      if (i!=1) {
	if (fabs(dist-currentDist) > tolerance) {
	  std::cout << "ERROR : " << std::endl
		    << "Current slice pos is  = " << sliceLocations[sliceIndex[i]] << std::endl
		    << "Previous slice pos is = " << previous << std::endl
		    << "Current file is       = " << input_files[sliceIndex[i]] << std::endl
		    << "Current index is      = " << i << std::endl
		    << "Current sortindex is  = " << sliceIndex[i] << std::endl
		    << "Current slice diff    = " << dist << std::endl
		    << "Current error         = " << fabs(dist-currentDist) << std::endl;
	  exit(1);
	}
      }
      else dist = currentDist;
      previous = sliceLocations[sliceIndex[i]];
    }

    //===========================================
    // Create image  
    gdcm::File * header = clitk::readDicomHeader(input_files[sliceIndex.front()]);
    typedef itk::Image<float, 3> ImageType;
    typedef itk::Image<signed short, 3> ImageTypeOutput;
    ImageType::SpacingType spacing;
    ImageType::SizeType    size;
    ImageType::PointType    origin;
    spacing[0] = header->GetXSpacing();
    spacing[1] = header->GetYSpacing();
    spacing[2] = dist;
    size[0]    = header->GetXSize();
    size[1]    = header->GetYSize();
    size[2]    = sliceIndex.size();
    ///Optional use of special origin scheme used at CLB
    if (args_info.focal_origin_flag)
    {
        origin[0]  = -spacing[0]*size[0]/2;
        origin[1]  = -spacing[1]*size[1]/2;
    }
    else
    {
        origin[0]  = header->GetXOrigin();
        origin[1]  = header->GetYOrigin();
    }
    origin[2]  = header->GetZOrigin();
    ImageTypeOutput::Pointer output = ImageTypeOutput::New();
    itk::ImageRegion<3> region;
    region.SetSize(size);
    output->SetRegions(region);
    output->SetOrigin(origin);
    output->SetSpacing(spacing);
    output->Allocate();
  
    //===========================================
    // Fill image
    ImageType::Pointer slice;
    typedef itk::ImageRegionIterator<ImageTypeOutput> IteratorType;
    typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
    IteratorType po(output, output->GetLargestPossibleRegion());
    po.Begin();
    for(unsigned int i=0; i<sliceIndex.size(); i++) {
      slice = clitk::readImage<ImageType>(input_files[sliceIndex[i]]);
      ConstIteratorType pi(slice, slice->GetLargestPossibleRegion());
      while ( !pi.IsAtEnd() ) {
	po.Set((signed short)pi.Get());
	++po;
	++pi;
      }
    }

    //===========================================
    // Write image
    clitk::writeImage<ImageTypeOutput>(output, 
				       args_info.output_arg, 
				       args_info.verbose_flag);
  
    // this is the end my friend 
    return 0;
}
