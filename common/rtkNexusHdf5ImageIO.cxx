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
// Read Image Information
void rtk::NexusHdf5ImageIO::ReadImageInformation()
{
  try {
    // Open the existing file
    H5::H5File file(m_FileName, H5F_ACC_RDONLY);
    
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
    H5::Group entry_group = file.openGroup("/" + entry);
    H5::Attribute attr = entry_group.openAttribute("definition");
    H5::DataType dtype = attr.getDataType();
    std::string entry_type;
    attr.read(dtype, entry_type);
    float spacing_x = 0.0;
    float spacing_y = 0.0;
    
    // Fetch spacing information
    if (entry_type == "NXtomo") {
        // Get projection spacing
        H5::DataSet spacing_x_ds = file.openDataSet(
            "/" + entry + "/instrument/detector/x_pixel_size"
        );
        spacing_x_ds.read(&spacing_x, H5::PredType::NATIVE_FLOAT);
        SetSpacing(0, spacing_x);

        H5::DataSet spacing_y_ds = file.openDataSet(
            "/" + entry + "/instrument/detector/y_pixel_size"
        );
        spacing_y_ds.read(&spacing_y, H5::PredType::NATIVE_FLOAT);
        SetSpacing(1, spacing_y);
        
        // Spacing on the rotation angle axis is irrelevant
        SetSpacing(2, 1.0); //

    } else if (entry_type == "NXentry") {
        H5::DataSet voxel_size_ds = file.openDataSet(
            "/" + entry + "/reconstruction/configuration/processing_options/reconstruction/voxel_size_cm"
        );
        float voxel_size_mm = 0.0;
        voxel_size_ds.read(&voxel_size_mm, H5::PredType::NATIVE_FLOAT);
        voxel_size_mm *= 10.0; // convert cm 
        SetSpacing(0, voxel_size_mm);
        SetSpacing(1, voxel_size_mm);
        SetSpacing(2, voxel_size_mm);
    } else {
        itkGenericExceptionMacro(<< entry_type 
                                 <<"is an invalid entry type: "
                                 << "Valid types are NXtomo and NXentry"
                                 <<" \"");
    }

    // Get datatype and dataspace
    H5::DataSet dataset;
    if (entry_type == "NXtomo") {
        m_PathToData = "/" + entry + "/instrument/detector/data";
        dataset = file.openDataSet(
            m_PathToData
        );

    } else if (entry_type == "NXentry") {
        m_PathToData = "/" + entry + "/reconstruction/results/data";
        dataset = file.openDataSet(
            m_PathToData
        );
    }

    // Get dimensions
    H5::DataSpace dataspace = dataset.getSpace();
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
    H5::DataType dtype2 = dataset.getDataType();
    H5T_class_t type_class = dtype2.getClass();

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

    } catch (H5::FileIException &error) {
      itkExceptionMacro("Cannot open HDF5 file: " + m_FileName);
    } catch (H5::DataSetIException &error) {
      itkExceptionMacro("Dataset error: " + std::string(error.getCDetailMsg()) + m_FileName);
    } catch (H5::DataSpaceIException &error) {
      itkExceptionMacro("Dataspace error: " + std::string(error.getCDetailMsg()));
    }   
} ////

//--------------------------------------------------------------------
// Read Image Information
bool rtk::NexusHdf5ImageIO::CanReadFile(const char* FileNameToRead)
{
  m_FileName = FileNameToRead;
  const std::string::size_type it = m_FileName.find_last_of( "." );
  std::string                  fileExt( m_FileName, it+1, m_FileName.length() );

  if (
    fileExt != std::string("h5") &&
    fileExt != std::string("hdf5") &&
     fileExt != std::string("nx")
    ) return false;
  
  // Try opening with HDF5 to verify
  try {
    H5::H5File file(m_FileName, H5F_ACC_RDONLY);
  } catch (H5::FileIException &error) {
    return false;
  } 
  return true;
} ////

//--------------------------------------------------------------------
// Read Image Content
void rtk::NexusHdf5ImageIO::Read(void * buffer)
{
  try {
    // Open the existing file
    H5::H5File file(m_FileName, H5F_ACC_RDONLY);

    H5::DataSet dataset = file.openDataSet(
      m_PathToData
    );
  
    H5::DataSpace dataspace = dataset.getSpace();
    int rank = dataspace.getSimpleExtentNdims();
    hsize_t dims[3];
    dataspace.getSimpleExtentDims(dims, nullptr);
    
    // Get the datatype
    H5::DataType readType = dataset.getDataType();
    H5T_class_t type_class = readType.getClass();
    dataset.read(buffer, type_class);

    dataspace.close();
    dataset.close();
    file.close();
  
  } catch (H5::FileIException &error) {
    itkExceptionMacro("Cannot open HDF5 file: " + m_FileName);
  } catch (H5::DataSetIException &error) {
    itkExceptionMacro("Dataset error: " + std::string(error.getCDetailMsg()) + m_FileName);
  } catch (H5::DataSpaceIException &error) {
    itkExceptionMacro("Dataspace error: " + std::string(error.getCDetailMsg()));
  }
}
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
