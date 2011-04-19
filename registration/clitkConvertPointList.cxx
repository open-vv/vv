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

/* =================================================
 * @file   clitkConvertPointList.cxx
 * @author 
 * @date   
 * 
 * @brief 
 * 
 ===================================================*/


// clitk
#include "clitkConvertPointList_ggo.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkList.h"
#include "clitkLists.h"


//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkConvertPointList, args_info);
  CLITK_INIT;

  // List
  typedef itk::Point<double, 3> PointType;
  typedef clitk::List<PointType> ListType;
  typedef clitk::Lists<PointType> ListsType;
  ListType refList;
  ListsType targetLists;

  // Image 
  typedef itk::Image<signed short, 3> ImageType; 
  ImageType::Pointer referenceImage=clitk::readImage<ImageType>(args_info.refIm_arg, args_info.verbose_flag);
  
  // Filenames
  std::vector< std::string> fileNames;
  for(unsigned int i=0; i< args_info.lists_given; i++)
    {
      fileNames.push_back(args_info.lists_arg[i]);
      if (args_info.verbose_flag) std::cout<<"Adding input point list "<<fileNames[i]<<std::endl;
    }

  // Read and convert
  targetLists.ReadAndConvertPointPairs(fileNames, refList, referenceImage, args_info.verbose_flag );

  // Write  
  if (args_info.ref_given)  refList.Write(args_info.ref_arg, args_info.verbose_flag); 
  if (args_info.targets_given ==  args_info.lists_given) 
    {
      // output filenames
      std::vector<std::string> oFileNames;
      for(unsigned int i=0; i< args_info.targets_given; i++)
	oFileNames.push_back(args_info.targets_arg[i]);
      targetLists.Write(oFileNames, args_info.verbose_flag); 
    }
  
  return EXIT_SUCCESS;

}// end main

//--------------------------------------------------------------------
