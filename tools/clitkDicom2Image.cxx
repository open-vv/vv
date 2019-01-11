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
#include "clitkIO.h"
#include "clitkDicom2Image_ggo.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "vvImageReader.h"
#include "vvImageWriter.h"
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <gdcmFile.h>
#include <vtkVersion.h>
#include <vtkImageChangeInformation.h>
#if GDCM_MAJOR_VERSION >= 2
  #include <gdcmImageHelper.h>
  #include <gdcmAttribute.h>
  #include <gdcmReader.h>
#endif

#include <set>

//====================================================================
int main(int argc, char * argv[])
{
  // init command line
  GGO(clitkDicom2Image, args_info);
  CLITK_INIT;

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

  //Get GDCMSeriesFileNames order to sort filenames
  typedef itk::GDCMSeriesFileNames NamesGeneratorType;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
  nameGenerator->SetUseSeriesDetails(false);
  std::string folderName=".";
  const size_t last_slash_idx = input_files[0].rfind('/');
  if (std::string::npos != last_slash_idx)
    folderName = input_files[0].substr(0, last_slash_idx);
  nameGenerator->SetInputDirectory(folderName);

  //===========================================
  /// Get slices locations ...
  std::string series_UID = "";
  std::set<std::string> series_UIDs;
  std::map< std::string, std::vector<double> > theorigin;
  std::map< std::string, std::vector<double> > theorientation;
  std::map< std::string, std::vector<double> > sliceLocations;
  std::map< std::string, std::vector<std::string> > seriesFiles;
#if GDCM_MAJOR_VERSION >= 2
  if (args_info.verbose_flag)
    std::cout << "Using GDCM-2.x" << std::endl;
#else
  if (args_info.verbose_flag) {
    std::cout << "Not using GDCM-2.x" << std::endl;
    std::cout<< "The image orientation is not supported with this version of GDCM" <<std::endl;
  }
#endif
  for(unsigned int i=0; i<args_info.inputs_num; i++) {
    if (args_info.verbose_flag)
        std::cout << "Reading <" << input_files[i] << std::endl;
#if GDCM_MAJOR_VERSION >= 2
    gdcm::Reader hreader;
    hreader.SetFileName(input_files[i].c_str());
    hreader.Read();
    gdcm::DataSet& ds = hreader.GetFile().GetDataSet();

    gdcm::Attribute<0x20,0x000e> series_UID_att;
    series_UID_att.SetFromDataSet(ds);
    series_UID = series_UID_att.GetValue().c_str();

    series_UIDs.insert(series_UID);
    theorigin[series_UID] = gdcm::ImageHelper::GetOriginValue(hreader.GetFile());
    theorientation[series_UID] = gdcm::ImageHelper::GetDirectionCosinesValue(hreader.GetFile());
    if (args_info.patientSystem_flag) {
      double n1 = theorientation[series_UID][1]*theorientation[series_UID][5]-
                  theorientation[series_UID][2]*theorientation[series_UID][4];
      double n2 = theorientation[series_UID][3]*theorientation[series_UID][2]-
                  theorientation[series_UID][5]*theorientation[series_UID][0];
      double n3 = theorientation[series_UID][0]*theorientation[series_UID][4]-
                  theorientation[series_UID][1]*theorientation[series_UID][3];
      double sloc = theorigin[series_UID][0]*n1+
                    theorigin[series_UID][1]*n2+
                    theorigin[series_UID][2]*n3;
      sliceLocations[series_UID].push_back(sloc);
    } else
      sliceLocations[series_UID].push_back(theorigin[series_UID][2]);
    seriesFiles[series_UID].push_back(input_files[i]);

    gdcm::Attribute<0x28, 0x100> pixel_size;
    pixel_size.SetFromDataSet(ds);
    /* if (pixel_size.GetValue() != 16)
       {
       std::cerr << "Pixel type not 2 bytes ! " << std::endl;
       std::cerr << "In file " << input_files[i] << std::endl;
       exit(0);
       }
    */
#else
  gdcm::File *header = new gdcm::File();
  header->SetFileName(input_files[i]);
  header->SetMaxSizeLoadEntry(16384); // required ?
  header->Load();

  series_UID = header->GetEntryValue(0x20,0x000e).c_str();

  series_UIDs.insert(series_UID);
  theorigin[series_UID].resize(3);
  theorigin[series_UID][0] = header->GetXOrigin();
  theorigin[series_UID][1] = header->GetYOrigin();
  theorigin[series_UID][2] = header->GetZOrigin();
  sliceLocations[series_UID].push_back(theorigin[series_UID][2]);
  seriesFiles[series_UID].push_back(input_files[i]);
  /*if (header->GetPixelSize() != 2) {
    std::cerr << "Pixel type 2 bytes ! " << std::endl;
    std::cerr << "In file " << input_files[i] << std::endl;
    exit(0);
  }
  */
#endif
  }

  //===========================================
  // Sort slices locations ...
  std::set<std::string>::iterator sn = series_UIDs.begin();
  while ( sn != series_UIDs.end() ) {
    std::vector<double> locs = sliceLocations[*sn];
    std::vector<double> origin = theorigin[*sn];
    std::vector<std::string> files = seriesFiles[*sn];
    std::vector<int> sliceIndex(files.size());
    //clitk::GetSortedIndex(locs, sliceIndex);
    //Look for files into GDCMSeriesFileNames, because it sorts files correctly and take the order
    const std::vector<std::string> & temp = nameGenerator->GetFileNames(*sn);
    for(unsigned int i=0; i<files.size(); i++) {
      int j(0);
      bool found(false);
      while (!found && j<temp.size()) {
        const size_t last_slash_idx2 = temp[j].rfind('/');
        std::string tempFilename(temp[j]);
        if (temp[j][0] == '.' && temp[j][1] == '/')
          tempFilename = temp[j].substr(2, temp[j].size()-1);
        if (tempFilename == files[i]) {
          sliceIndex[j] = i;
          found = true;
        }
        ++j;
      }
    }
    if (sliceIndex.size() == 0) { //ie. sn is not a serie present in files
      sn++;
      continue;
    }

    if (args_info.verbose_flag) {
      std::cout << locs[sliceIndex[0]] << " -> "
                << sliceIndex[0] << " / " << 0 << " => "
                << "0 mm "
                << files[sliceIndex[0]]
                << std::endl;
      for(unsigned int i=1; i<sliceIndex.size(); i++) {
        std::cout << locs[sliceIndex[i]] << " -> "
                  << sliceIndex[i] << " / " << i << " => "
                  << locs[sliceIndex[i]] - locs[sliceIndex[i-1]]
                  << "mm "
                  << files[sliceIndex[i]]
                  << std::endl;
      }
    }

    //===========================================
    // Analyze slices locations ...
    double currentDist;
    double dist=0;
    double tolerance = args_info.tolerance_arg;
    double previous = locs[sliceIndex[0]];
    for(unsigned int i=1; i<sliceIndex.size(); i++) {
      currentDist = locs[sliceIndex[i]]-previous;
      if (i!=1) {
        if (fabs(dist-currentDist) > tolerance) {
          std::cout << "ERROR : " << std::endl
                    << "Current slice pos is  = " << locs[sliceIndex[i]] << std::endl
                    << "Previous slice pos is = " << previous << std::endl
                    << "Current file is       = " << files[sliceIndex[i]] << std::endl
                    << "Current index is      = " << i << std::endl
                    << "Current sortindex is  = " << sliceIndex[i] << std::endl
                    << "Current slice diff    = " << dist << std::endl
                    << "Current error         = " << fabs(dist-currentDist) << std::endl;
          exit(1);
        }
      } else dist = currentDist;
      previous = locs[sliceIndex[i]];
    }

    //===========================================
    // Create ordered vector of filenames
    std::vector<std::string> sorted_files;
    sorted_files.resize(sliceIndex.size());
    for(unsigned int i=0; i<sliceIndex.size(); i++)
      sorted_files[i] = files[ sliceIndex[i] ];

    //===========================================
    // Read write serie
    vvImageReader::Pointer reader = vvImageReader::New();
    reader->SetInputFilenames(sorted_files);
    reader->SetPatientCoordinateSystem(args_info.patientSystem_flag);
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
      origin[0] = -spacing[0]*size[0]/2.0;
      origin[1] = -spacing[1]*size[1]/2.0;
#if VTK_MAJOR_VERSION <= 5
      modifier->SetInput(vtk_image);
#else
      modifier->SetInputData(vtk_image);
#endif
      modifier->SetOutputOrigin(origin[0], origin[1], locs[sliceIndex[0]]);
      modifier->Update();
      vvImage::Pointer focal_image = vvImage::New();
      focal_image->AddVtkImage(modifier->GetOutput());
      image = focal_image;
    }

    std::string outfile;
    if (series_UIDs.size() == 1)
      outfile = args_info.output_arg;
    else {
      std::ostringstream name;
      std::vector<std::string> directory = clitk::SplitFilename(args_info.output_arg);
      if (directory.size() == 2)
        name << directory[0] << "/" << *sn << "_" << directory[1];
      else
        name << *sn << "_" << args_info.output_arg;
      outfile = name.str();
    }
    vvImageWriter::Pointer writer = vvImageWriter::New();
    writer->SetInput(image);
    if (args_info.patientSystem_flag && !image->GetTransform().empty())
      writer->SetSaveTransform(true);
    writer->SetOutputFileName(outfile);
    writer->Update();

    modifier->Delete();

    sn++;
  }

  return 0;
}
