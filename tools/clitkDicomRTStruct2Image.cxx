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

#include "clitkDicomRTStruct2ImageFilter.h"
#include "clitkDicomRT_StructureSet.h"
#include "clitkDicomRTStruct2Image_ggo.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkDicomRTStruct2Image, args_info);

  // Read and display information
  clitk::DicomRT_StructureSet::Pointer s = clitk::DicomRT_StructureSet::New();
  s->Read(args_info.input_arg);
  if (args_info.verboseFile_flag) {
    s->Print(std::cout);
  }
  
  // New filter to convert to binary image
  clitk::DicomRTStruct2ImageFilter filter;
  filter.SetCropMaskEnabled(args_info.crop_flag);
  filter.SetImageFilename(args_info.image_arg);  // Used to get spacing + origin
  if (args_info.roi_arg != -1) {
    filter.SetROI(s->GetROIFromROINumber(args_info.roi_arg)); 
    filter.SetOutputImageFilename(args_info.output_arg);
    filter.Update();  
  }
  else {
    clitk::DicomRT_StructureSet::ROIConstIteratorType iter;
    for(iter = s->GetROIs().begin(); iter != s->GetROIs().end(); iter++) {
      clitk::DicomRT_ROI::Pointer roi = iter->second;
      // Create the filter
      clitk::DicomRTStruct2ImageFilter filter;
      filter.SetCropMaskEnabled(args_info.crop_flag);
      filter.SetImageFilename(args_info.image_arg);  // Used to get spacing + origin
      std::string name = roi->GetName();
      int num = roi->GetROINumber();
      filter.SetROI(roi); 
      name.erase(remove_if(name.begin(), name.end(), isspace), name.end());
      std::string n = std::string(args_info.output_arg).append
        (clitk::toString(num)).append
        ("_").append
        (name).append
        (".mhd");
      if (args_info.verbose_flag) {
        std::cout << num << " " << roi->GetName() << " num=" << num << " : " << n << std::endl;
      }
      filter.SetOutputImageFilename(n);
      filter.Update();
    }

    /*
    for(unsigned int i=0; i<s->GetListOfROI().size(); i++) {
      clitk::DicomRTStruct2ImageFilter filter;
      filter.SetCropMaskEnabled(args_info.crop_flag);
      filter.SetImageFilename(args_info.image_arg);  // Used to get spacing + origin
      std::string name = s->GetListOfROI()[i]->GetName();
      int num = s->GetListOfROI()[i]->GetROINumber();
      filter.SetROI(s->GetListOfROI()[i]); 
      name.erase(remove_if(name.begin(), name.end(), isspace), name.end());
      std::string n = std::string(args_info.output_arg).append
        (clitk::toString(num)).append
        ("_").append
        (name).append
        (".mhd");
      if (args_info.verbose_flag) {
        std::cout << i << " " << s->GetListOfROI()[i]->GetName() << " num=" << num << " : " << n << std::endl;
      }
      filter.SetOutputImageFilename(n);
      filter.Update();  
      }*/
  }

  // This is the end my friend 
  return 0;
}
