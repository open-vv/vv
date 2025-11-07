/*=========================================================================
 *
 *  Copyright RTK Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "rtkNexusHdf5ImageIO.h"

// itk include (for itkReadRawBytesAfterSwappingMacro)
#include <itkRawImageIO.h>
#include <itk_zlib.h>

//--------------------------------------------------------------------
/* Find value_ptr as pointer to the parameter of the given key in the header.
 * Returns NULL on success.
 */
char*
rtk::NexusHdf5ImageIO::edf_findInHeader( char* header, const char* key )
{
  char *value_ptr = strstr( header, key );

  if (!value_ptr) return NULL;
  /* an edf line is "key     = value ;" */
  value_ptr = 1 + strchr( value_ptr + strlen(key), '=' );
  while (isspace(*value_ptr) ) value_ptr++;
  return value_ptr;
}

//--------------------------------------------------------------------
// Read Image Information
void rtk::NexusHdf5ImageIO::ReadImageInformation()
{
  try {
    // Open the existing file
    H5File file(filename, H5F_ACC_RDONLY);
    
    // List of all entries
    hsize_t n_objs = file.getNumObjs();
    std::vector<std::string> groups;
    for (hsize_t i = 0; i < n_objs; ++i) {
        std::string name = file.getObjnameByIdx(i);
        H5G_obj_t type = file.getObjTypeByIdx(i);

        if (type == H5G_GROUP) {
            groups.push_back(name);
        }
    }

    // Take the first one
    std::string entry = groups[0];
    m_Entry = entry;
    Group entry_group = file.openGroup("/" + entry);
    Attribute attr = entry_group.openAttribute("definition");
    DataType dtype = attr.getDataType();
    std::string entry_type;
    attr.read(dtype, entry_type);
    float spacing_x = 0.0;
    float spacing_y = 0.0;
    
    // Fetch spacing information
    if entry_type == "NXtomo" {
        // Get projection spacing
        DataSet spacing_x_ds = file.openDataSet(
            "/" + entry + "/instrument/detector/x_pixel_size"
        );
        spacing_x_ds.read(&spacing_x, PredType::NATIVE_FLOAT);
        SetSpacing(0, spacing_x);

        DataSet spacing_y_ds = file.openDataSet(
            "/" + entry + "/instrument/detector/y_pixel_size"
        );
        spacing_y_ds.read(&spacing_y, PredType::NATIVE_FLOAT);
        SetSpacing(1, spacing_y);
        
        // Spacing on the rotation angle axis is irrelevant
        SetSpacing(2, 1.0); //

    } else if entry_type == "NXentry" {
        DataSet voxel_size_ds = file.openDataSet(
            "/" + entry + "/reconstruction/configuration/processing_options/reconstruction/voxel_size_cm"
        );
        float voxel_size_mm = 0.0;
        voxel_size_ds.read(&voxel_size_mm, PredType::NATIVE_FLOAT);
        voxel_size_mm *= 10.0; // convert cm 
        SetSpacing(0, voxel_size_mm);
        SetSpacing(1, voxel_size_mm);
        SetSpacing(2, voxel_size_mm);
    } else {
        itkGenericExceptionMacro(<< entry_type 
                                 <<"is an invalid entry type: "
                                 << "Valid types are NXtomo and NXentry"
                                 <<" \"");
        return 1;
    }

    // Get datatype and dataspace
    if (entry_type == "NXtomo") {
        m_PathToData = "/" + entry + "/instrument/detector/data"
        DataSet dataset = file.openDataSet(
            m_PathToData
        );

    } else if (entry_type == "NXentry") {
        m_PathToData = "/" + entry + "/reconstruction/results/data"
        DataSet dataset = file.openDataSet(
            m_PathToData
        );
    }

    // Get dimensions
    DataSpace dataspace = dataset.getSpace();
    int rank = dataspace.getSimpleExtentNdims();
    hsize_t dims[3];
    dataspace.getSimpleExtentDims(dims, nullptr);

    SetNumberOfDimensions(3);
    SetDimensions(0, dims[0]);
    SetDimensions(1, dims[1]);
    SetDimensions(2, dims[2]);

    // Set origin to zero
    SetOrigin(0, 0.);
    SetOrigin(1, 0.);
    SetOrigin(2, 0.);

    // Get the datatype
    DataType dtype = dataset.getDataType();
    H5T_class_t type_class = dtype.getClass();

    switch(type_class) {
      case H5T_NO_CLASS:
        itkGenericExceptionMacro( <<"H5T_NO_CLASS is incorrect datatype \"");
        break;
      case H5T_INTEGER:
        SetComponentType(itk::ImageIOBase::INT);
        break;
      case H5T_FLOAT:
        SetComponentType(itk::ImageIOBase::FLOAT);
        break;
      case H5T_TIME:
        itkGenericExceptionMacro( <<"TIME data type is not handled\"");
        break;
      case H5T_STRING:
        itkGenericExceptionMacro( <<"STRING data type is not handled\"");
        break;
      case H5T_BITFIELD:
        itkGenericExceptionMacro( <<"BITFIELD data type is not handled\"");
        break;
      case H5T_OPAQUE:
        itkGenericExceptionMacro( <<"BITFIELD data type is not handled\"");
        break;
      case H5T_COMPOUND:
        itkGenericExceptionMacro( <<"COMPOUND data type is not handled\"");
        break;
      case H5T_REFERENCE:
        itkGenericExceptionMacro( <<"REFERENCE data type is not handled\"");
        break;
      case H5T_ENUM:
        itkGenericExceptionMacro( <<"ENUM data type is not handled\"");
        break;
      case H5T_VLEN:
        itkGenericExceptionMacro( <<"VLEN data type is not handled\"");
        break;
      case H5T_ARRAY:
        itkGenericExceptionMacro( <<"ARRAY data type is not handled\"");
        break;
    }

    dataspace.close();
    dataset.close();
    file.close();

    } catch (FileIException &error) {
      itkExceptionMacro("Cannot open HDF5 file: " << filename);
      return 1;
    } catch (DataSetIException &error) {
      itkExceptionMacro("Dataset error: " << error.getCDetailMsg() << filename);
      return 1;
    } catch (DataSpaceIException &error) {
      itkExceptionError("Dataspace error: " << error.getCDetailMsg());
        return 1;
    }   
} ////

//--------------------------------------------------------------------
// Read Image Information
bool rtk::NexusHdf5ImageIO::CanReadFile(const char* FileNameToRead)
{
  std::string                  filename(FileNameToRead);
  const std::string::size_type it = filename.find_last_of( "." );
  std::string                  fileExt( filename, it+1, filename.length() );

  if (
    fileExt != std::string("h5") &&
    fileExt != std::string("hdf5") &&
     fileExt != std::string("nx")
    ) return false;
  
  // Try opening with HDF5 to verify
  hid_t file_id = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  if (file_id < 0)
    return false;

  H5Fclose(file_id);
  return true;
} ////

//--------------------------------------------------------------------
// Read Image Content
void rtk::NexusHdf5ImageIO::Read(void * buffer)
{
  try {
    DataSet dataset = file.openDataSet(
      m_PathToData
    );
  
    DataSpace dataspace = dataset.getSpace();
    int rank = dataspace.getSimpleExtentNdims();
    hsize_t dims[3];
    dataspace.getSimpleExtentDims(dims, nullptr);
    
    // Get the datatype
    DataType readType = dataset.getDataType();
    H5T_class_t type_class = dtype.getClass();
    dataset.read(buffer, type_class);

    dataspace.close();
    dataset.close();
    file.close();
  
  } catch (FileIException &error) {
    itkExceptionMacro("Cannot open HDF5 file: " << filename);
    return 1;
  } catch (DataSetIException &error) {
    itkExceptionMacro("Dataset error: " << error.getCDetailMsg() << filename);
    return 1;
  } catch (DataSpaceIException &error) {
    itkExceptionError("Dataspace error: " << error.getCDetailMsg());
      return 1;
  }

const std::string filename = this->GetFileName();
  hid_t file_id = H5Fopen(m_FileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  if (file_id < 0)
    itkExceptionMacro("Cannot open HDF5 file for reading");

  hid_t dataset_id = H5Dopen(file_id, "image", H5P_DEFAULT);
  if (dataset_id < 0)
    itkExceptionMacro("Cannot open dataset 'image'");

  herr_t status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer);
  if (status < 0)
    itkExceptionMacro("Error reading HDF5 dataset");

  H5Dclose(dataset_id);
  H5Fclose(file_id);}

//--------------------------------------------------------------------
// Write Image Information
void rtk::NexusHdf5ImageIO::WriteImageInformation( bool itkNotUsed(keepOfStream) )
{
}

//--------------------------------------------------------------------
// Write Image Information
bool rtk::NexusHdf5ImageIO::CanWriteFile( const char* itkNotUsed(FileNameToWrite) )
{
  return false;
}

//--------------------------------------------------------------------
// Write Image
void rtk::NexusHdf5ImageIO::Write( const void * itkNotUsed(buffer) )
{
} ////
