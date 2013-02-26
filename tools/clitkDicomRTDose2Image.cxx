// clitk includes
#include "clitkDicomRTDose2Image_ggo.h"
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <gdcmFile.h>
#if GDCM_MAJOR_VERSION == 2
  #include <gdcmImageHelper.h>
  #include <gdcmAttribute.h>
  #include <gdcmReader.h>
#endif

//====================================================================
int main(int argc, char * argv[])
{
  // init command line
  GGO(clitkDicomRTDose2Image, args_info);
  //===========================================
  // Read write serie

  const unsigned int Dim = 3;
  typedef unsigned int ImageValueType;
  typedef itk::Image<ImageValueType, Dim> ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(args_info.input_arg);
  reader->Update();
  
  ImageType::Pointer image = reader->GetOutput();

  typedef float DoseValueType;
  typedef itk::Image<DoseValueType, Dim> DoseImageType;
  DoseImageType::Pointer output_image = DoseImageType::New();
  output_image->SetOrigin(image->GetOrigin());
  output_image->SetSpacing(image->GetSpacing());
  output_image->SetDirection(image->GetDirection());
  output_image->SetRegions(image->GetLargestPossibleRegion());
  output_image->Allocate();
  
  typedef itk::ImageRegionConstIterator<ImageType> InputIteratorType;
  InputIteratorType it_in(image, image->GetLargestPossibleRegion());
  it_in.GoToBegin();

  typedef itk::ImageRegionIterator<DoseImageType> OutputIteratorType;
  OutputIteratorType it_out(output_image, output_image->GetLargestPossibleRegion());
  it_out.GoToBegin();
  
  double dose_scale = 1;
  std::string modality;
#if GDCM_MAJOR_VERSION == 2
    if (args_info.verbose_flag)
      std::cout << "Using GDCM-2.x" << std::endl;
    gdcm::Reader hreader;
    hreader.SetFileName(args_info.input_arg);
    hreader.Read();
    gdcm::DataSet& ds = hreader.GetFile().GetDataSet();
    gdcm::Attribute<0x0008, 0x0060> attr_modality;
    attr_modality.SetFromDataSet(ds);
    modality = attr_modality.GetValue();
    if (modality != "RTDOSE") {
      FATAL("Dicom modality (0x0008, 0x0060) must be RTDOSE.\n");
    }
    gdcm::Attribute<0x3004, 0x000e> attr_dose_grid_scaling;
    attr_dose_grid_scaling.SetFromDataSet(ds);
    dose_scale = attr_dose_grid_scaling.GetValue();
#else
    if (args_info.verbose_flag)
      std::cout << "Not using GDCM-2.x" << std::endl;
  gdcm::File *header = new gdcm::File();
  header->SetFileName(input_files[i]);
  header->SetMaxSizeLoadEntry(16384); // required ?
  header->Load();
  modality = atof(header->GetEntryValue(0x0008, 0x0060).c_str());
  if (modality != "RTDOSE") {
    FATAL("Dicom modality (0x0008, 0x0060) must be RTDOSE.\n");
  }
  dose_scale = atof(header->GetEntryValue(0x3004,0x000e).c_str());
#endif

  if (args_info.verbose_flag) {
    std::cout << "Dose Grid Scaling is " << dose_scale << std::endl;
  }
  
  while (!it_in.IsAtEnd()) {
    it_out.Set(it_in.Get() * dose_scale);
    ++it_in;
    ++it_out;
  }
  
  typedef itk::ImageFileWriter<DoseImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(output_image);
  writer->SetFileName(args_info.output_arg);
  writer->Update();
  
  return 0;
}