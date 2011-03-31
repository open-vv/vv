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

// clitk include
#include "clitkDicomRTDoseIO.h"
#include "clitkCommon.h"

// itk include
#if GDCM_MAJOR_VERSION == 2
// IMPLEMENTATION NOTE:
// The following has been done without the use of vtkGDCMImageReader which directly
// handle RTDOSE image. Another approach would have been to use gdcm::ImageReader
// which also properly recognize RTDOSE
  #include <gdcmReader.h>
  #include <gdcmAttribute.h>
#else
  #include <gdcmFileHelper.h>
#endif

#define GFOV_SPACING_TOL (1e-1)

//--------------------------------------------------------------------
// Read Image Information
void clitk::DicomRTDoseIO::ReadImageInformation()
{
#if GDCM_MAJOR_VERSION < 2
  if(m_GdcmFile)
    delete m_GdcmFile;
  m_GdcmFile = new gdcm::File;
#endif

  int i;
  std::string tmp;
  float ipp[3];
  int dim[3];
  float spacing[3];
  double *gfov;    /* gfov = GridFrameOffsetVector */
  int gfov_len;
#if GDCM_MAJOR_VERSION < 2
  const char *gfov_str;
  int rc;
#endif

#if GDCM_MAJOR_VERSION == 2
  m_GdcmImageReader.SetFileName(m_FileName.c_str());
  m_GdcmImageReader.Read();
  gdcm::File* m_GdcmFile = &m_GdcmImageReader.GetFile();
#else
  m_GdcmFile->SetMaxSizeLoadEntry (0xffff);
  m_GdcmFile->SetFileName (m_FileName.c_str());
  m_GdcmFile->SetLoadMode (0);
  m_GdcmFile->Load();
#endif

  /* Modality -- better be RTSTRUCT */
#if GDCM_MAJOR_VERSION == 2
  gdcm::DataSet &ds = m_GdcmFile->GetDataSet();

  gdcm::Attribute<0x8,0x60> at1;
  at1.SetFromDataSet(ds);
  tmp = at1.GetValue();
#else
  tmp = m_GdcmFile->GetEntryValue (0x0008, 0x0060);
#endif
  if (strncmp (tmp.c_str(), "RTDOSE", strlen("RTDOSE"))) {
    itkExceptionMacro(<< "Error.  Input file not RTDOSE: " << m_FileName);
  }

  /* ImagePositionPatient */
#if GDCM_MAJOR_VERSION == 2
  gdcm::Attribute<0x20,0x32> at2;
  at2.SetFromDataSet(ds);
  ipp[0] = at2.GetValue(0);
  ipp[1] = at2.GetValue(1);
  ipp[2] = at2.GetValue(2);
#else
  tmp = m_GdcmFile->GetEntryValue (0x0020, 0x0032);
  rc = sscanf (tmp.c_str(), "%f\\%f\\%f", &ipp[0], &ipp[1], &ipp[2]);
  if (rc != 3) {
    itkExceptionMacro(<< "Error parsing RTDOSE ipp.");
  }
#endif

  /* Rows */
#if GDCM_MAJOR_VERSION == 2
  gdcm::Attribute<0x28,0x10> at3;
  at3.SetFromDataSet(ds);
  dim[1] = at3.GetValue();
#else
  tmp = m_GdcmFile->GetEntryValue (0x0028, 0x0010);
  rc = sscanf (tmp.c_str(), "%d", &dim[1]);
  if (rc != 1) {
    itkExceptionMacro(<< "Error parsing RTDOSE rows.");
  }
#endif

  /* Columns */
#if GDCM_MAJOR_VERSION == 2
  gdcm::Attribute<0x28,0x11> at4;
  at4.SetFromDataSet(ds);
  dim[0] = at4.GetValue();
#else
  tmp = m_GdcmFile->GetEntryValue (0x0028, 0x0011);
  rc = sscanf (tmp.c_str(), "%d", &dim[0]);
  if (rc != 1) {
    itkExceptionMacro(<< "Error parsing RTDOSE columns.");
  }
#endif

  /* PixelSpacing */
#if GDCM_MAJOR_VERSION == 2
  gdcm::Attribute<0x28,0x30> at5;
  at5.SetFromDataSet(ds);
  spacing[0] = at5.GetValue(0);
  spacing[1] = at5.GetValue(1);
#else
  tmp = m_GdcmFile->GetEntryValue (0x0028, 0x0030);
  rc = sscanf (tmp.c_str(), "%g\\%g", &spacing[0], &spacing[1]);
  if (rc != 2) {
    itkExceptionMacro(<< "Error parsing RTDOSE pixel spacing.");
  }
#endif

  /* GridFrameOffsetVector */
#if GDCM_MAJOR_VERSION == 2
  gdcm::Attribute<0x3004,0x000C> at6;
  const gdcm::DataElement& de6 = ds.GetDataElement( at6.GetTag() );
  at6.SetFromDataElement(de6);
  gfov = (double*) at6.GetValues();
  gfov_len = at6.GetNumberOfValues();
#else
  tmp = m_GdcmFile->GetEntryValue (0x3004, 0x000C);
  gfov = 0;
  gfov_len = 0;
  gfov_str = tmp.c_str();
  while (1) {
    int len;
    gfov = (double*) realloc (gfov, (gfov_len + 1) * sizeof(double));
    rc = sscanf (gfov_str, "%g%n", &gfov[gfov_len], &len);
    if (rc != 1) {
      break;
    }
    gfov_len ++;
    gfov_str += len;
    if (gfov_str[0] == '\\') {
      gfov_str ++;
    }
  }
#endif

  dim[2] = gfov_len;
  if (gfov_len == 0) {
    itkExceptionMacro(<< "Error parsing RTDOSE gfov.");
  }

  /* --- Analyze GridFrameOffsetVector --- */

  /* (1) Make sure first element is 0. */
  if (gfov[0] != 0.) {
    itkExceptionMacro(<< "Error RTDOSE gfov[0] is not 0.");
  }

  /* (2) Handle case where gfov_len == 1 (only one slice). */
  if (gfov_len == 1) {
    spacing[2] = spacing[0];
  }

  /* (3) Check to make sure spacing is regular. */
  for (i = 1; i < gfov_len; i++) {
    if (i == 1) {
      spacing[2] = gfov[1] - gfov[0];
    } else {
      double sp = gfov[i] - gfov[i-1];
      if (fabs(sp - spacing[2]) > GFOV_SPACING_TOL) {
        itkExceptionMacro(<< "Error RTDOSE grid has irregular spacing:"
                          << sp << " vs " << spacing[2]);
      }
    }
  }

#if GDCM_MAJOR_VERSION < 2
  free (gfov);
#endif

  /* DoseGridScaling */
#if GDCM_MAJOR_VERSION == 2
  gdcm::Attribute<0x3004,0x000E> at7 = { 1. } ;
  at7.SetFromDataSet(ds);
  m_DoseScaling = at7.GetValue();
#else
  m_DoseScaling = 1.0;
  tmp = m_GdcmFile->GetEntryValue (0x3004, 0x000E);
  rc = sscanf (tmp.c_str(), "%f", &m_DoseScaling);
#endif
  /* If element doesn't exist, scaling is 1.0 */

  // set dimension values
  SetNumberOfDimensions(3);
  for(int i=0; i<3; i++) {
    SetDimensions(i, dim[i]);
    SetSpacing(i, spacing[i]);
    SetOrigin(i, ipp[i]);
  }

  // set other information
  SetByteOrderToLittleEndian();
  SetPixelType(itk::ImageIOBase::SCALAR);
  SetNumberOfComponents(1);
  SetComponentType(itk::ImageIOBase::FLOAT);
}

//--------------------------------------------------------------------
// Read Image Information
bool clitk::DicomRTDoseIO::CanReadFile(const char* FileNameToRead)
{
#if GDCM_MAJOR_VERSION == 2
  gdcm::Reader creader;
  creader.SetFileName(FileNameToRead);
  if (!creader.Read())
    return false;

  gdcm::DataSet &ds = creader.GetFile().GetDataSet();
  gdcm::Attribute<0x8,0x60> at1;
  at1.SetFromDataSet(ds);
  std::string tmp = at1.GetValue();
#else
  // opening dicom input file
  gdcm::File dcmFile;
  dcmFile.SetFileName(FileNameToRead);
  if (!dcmFile.OpenFile())
    return false;

  //Taken from plastimatch
  dcmFile.SetMaxSizeLoadEntry (0xffff);
  dcmFile.SetLoadMode (0);
  dcmFile.Load();

  /* Modality -- better be RTDOSE */
  std::string tmp = dcmFile.GetEntryValue (0x0008, 0x0060);
#endif
  if (!strncmp (tmp.c_str(), "RTDOSE", strlen("RTDOSE"))) {
    return true;
  }

  return false;
}

//--------------------------------------------------------------------
template <class T>
void
clitk::DicomRTDoseIO::dose_copy_raw (float *img_out, T *img_in, int nvox, float scale)
{
  for (int i = 0; i < nvox; i++) {
    img_out[i] = img_in[i] * scale;
  }
}

//--------------------------------------------------------------------
// Read Image Content
void clitk::DicomRTDoseIO::Read(void * buffer)
{
  unsigned long npix = 1;
  for(unsigned int i=0; i<GetNumberOfDimensions(); i++)
    npix *= GetDimensions(i);

  /* PixelData */
#if GDCM_MAJOR_VERSION == 2
  gdcm::Image &i = m_GdcmImageReader.GetImage();
  i.GetBuffer((char*)buffer);
  // WARNING: GetBuffer return the pixel values as stored on disk not the real pixel value
  // we still need to multiply by m_DoseScaling
  // An alternate solution would be to use vtkGDCMImageReader...
#else
  float *img = (float*) buffer;

  gdcm::FileHelper m_GdcmFile_helper (m_GdcmFile);

  //size_t image_data_size = m_GdcmFile_helper.GetImageDataSize();
  if (strcmp (m_GdcmFile->GetPixelType().c_str(), "16U")==0) {
    unsigned short* image_data = (unsigned short*) m_GdcmFile_helper.GetImageData();
    dose_copy_raw (img, image_data, npix, m_DoseScaling);
  } else if (strcmp(m_GdcmFile->GetPixelType().c_str(),"32U")==0) {
    unsigned long* image_data = (unsigned long*) m_GdcmFile_helper.GetImageData ();
    dose_copy_raw (img, image_data, npix, m_DoseScaling);
  } else {
    itkExceptionMacro(<< "Error RTDOSE not type 16U and 32U (type="
                      << m_GdcmFile->GetPixelType().c_str() << ")");
  }
  delete m_GdcmFile;
#endif

  /* GCS FIX: Do I need to do something about endian-ness? */



  /* Copy data to volume */
//   float *img = (float*) vol->img;
//   for (i = 0; i < vol->npix; i++) {
  //img[i] = image_data[i] * m_DoseScaling;
//   }
}

//--------------------------------------------------------------------
// Write Image Information
void clitk::DicomRTDoseIO::WriteImageInformation(bool keepOfStream)
{
}

//--------------------------------------------------------------------
// Write Image Information
bool clitk::DicomRTDoseIO::CanWriteFile(const char* FileNameToWrite)
{
  return false;
}

//--------------------------------------------------------------------
// Write Image
void clitk::DicomRTDoseIO::Write(const void * buffer)
{
}

