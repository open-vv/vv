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

// clitk includes
#include "clitkDicom2Image_ggo.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "vvImageReader.h"
#include "vvImageWriter.h"
#include <gdcmFile.h>
#include <vtkImageChangeInformation.h>
#if GDCM_MAJOR_VERSION == 2
  #include <gdcmImageHelper.h>
  #include <gdcmAttribute.h>
  #include <gdcmReader.h>
#endif

//====================================================================
int main(int argc, char * argv[])
{
  // init command line
  GGO(clitkDicom2Image, args_info);
  std::vector<std::string> input_files;
  ///if std_input is given, read the input files from stdin
  if (args_info.std_input_given) {
    while (true) {
      std::string tmp;
      std::cin >> tmp;
      if(std::cin.good())
        input_files.push_back(tmp);
      else break;
    }
    args_info.inputs_num=input_files.size();
  } else for (unsigned int i=0; i<args_info.inputs_num; i++)
      input_files.push_back(args_info.inputs[i]);


  //===========================================
  /// Get slices locations ...
  std::vector<double> theorigin(3);
  std::vector<double> sliceLocations;
  for(unsigned int i=0; i<args_info.inputs_num; i++) {
    //std::cout << "Reading <" << input_files[i] << std::endl;
#if GDCM_MAJOR_VERSION == 2
    if (args_info.verbose_flag)
      std::cout << "Using GDCM-2.x" << std::endl;
    gdcm::Reader hreader;
    hreader.SetFileName(input_files[i].c_str());
    hreader.Read();
    theorigin = gdcm::ImageHelper::GetOriginValue(hreader.GetFile());
    sliceLocations.push_back(theorigin[2]);
    gdcm::Attribute<0x28, 0x100> pixel_size;
    gdcm::DataSet& ds = hreader.GetFile().GetDataSet();
    pixel_size.SetFromDataSet(ds);
    if (pixel_size.GetValue() != 16)
    {
      std::cerr << "Pixel type 2 bytes ! " << std::endl;
      std::cerr << "In file " << input_files[i] << std::endl;
      exit(0);
    }
#else
    if (args_info.verbose_flag)
      std::cout << "Not using GDCM-2.x" << std::endl;
  gdcm::File *header = new gdcm::File();
  header->SetFileName(input_files[i]);
  header->SetMaxSizeLoadEntry(16384); // required ?
  header->Load();
  theorigin[0] = header->GetXOrigin();
  theorigin[1] = header->GetYOrigin();
  theorigin[2] = header->GetZOrigin();
  sliceLocations.push_back(theorigin[2]);
  if (header->GetPixelSize() != 2) {
    std::cerr << "Pixel type 2 bytes ! " << std::endl;
    std::cerr << "In file " << input_files[i] << std::endl;
    exit(0);
  }
#endif
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
    } else dist = currentDist;
    previous = sliceLocations[sliceIndex[i]];
  }

  //===========================================
  // Create ordered vector of filenames
  std::vector<std::string> sorted_files;
  sorted_files.resize(sliceIndex.size());
  for(unsigned int i=0; i<sliceIndex.size(); i++)
    sorted_files[i] = input_files[ sliceIndex[i] ];

  //===========================================
  // Read write serie
  vvImageReader::Pointer reader = vvImageReader::New();
  reader->SetInputFilenames(sorted_files);
  reader->Update(vvImageReader::DICOM);
  if (reader->GetLastError().size() != 0) {
    std::cerr << reader->GetLastError() << std::endl;
    return 1;
  }
  
  vvImage::Pointer image = reader->GetOutput();
  vtkImageData* vtk_image = image->GetFirstVTKImageData();
  vtkImageChangeInformation* modifier = vtkImageChangeInformation::New();
  if  (args_info.focal_origin_given) {
    std::vector<double> spacing = image->GetSpacing();
    std::vector<int> size = image->GetSize();
    theorigin[0] = -spacing[0]*size[0]/2.0;
    theorigin[1] = -spacing[1]*size[1]/2.0;
    modifier->SetInput(vtk_image);
    modifier->SetOutputOrigin(theorigin[0], theorigin[1], sliceLocations[sliceIndex[0]]);
    modifier->Update();
    vvImage::Pointer focal_image = vvImage::New();
    focal_image->AddVtkImage(modifier->GetOutput());
    image = focal_image;
  }

  vvImageWriter::Pointer writer = vvImageWriter::New();
  writer->SetInput(image);
  writer->SetOutputFileName(args_info.output_arg);
  writer->Update();

  modifier->Delete();

  return 0;
}
