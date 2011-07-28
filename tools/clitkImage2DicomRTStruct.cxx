/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX

  Authors belongs to: 
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://www.centreleonberard.fr
  - CREATIS CNRS laboratory      http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence
  - BSD       http://www.opensource.org/licenses/bsd-license.php
  - CeCILL-B  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html

  =========================================================================*/

#include "clitkImage2DicomRTStructFilter.h"
#include "clitkDicomRT_StructureSet.h"
#include "clitkImage2DicomRTStruct_ggo.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkImage2DicomRTStruct, args_info);

  // Read initial 3D image
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  ImageType::Pointer input = clitk::readImage<ImageType>(args_info.input_arg, true);

  // Create a filter to convert image into dicomRTStruct
  clitk::Image2DicomRTStructFilter<PixelType> filter;
  filter.SetInput(input);
  filter.Update();
  
  // Write result
  clitk::DicomRT_StructureSet::Pointer s = filter.GetDicomRTStruct();
  //  s->Write(args_info.output_arg);

  // This is the end my friend 
  return 0;
}
//--------------------------------------------------------------------
