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
#include <string>
#include <fstream>

#include "vtkVOXImageWriter.h"

#include "vtkCommand.h"
#include "vtkErrorCode.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDataSetAttributes.h"
#include <vtkVersion.h>
#include <vtkAlgorithm.h>

#include <sys/stat.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkVOXImageWriter);

//----------------------------------------------------------------------------
vtkVOXImageWriter::vtkVOXImageWriter()
{
  this->FileName = 0;
  this->FileLowerLeft = 1;
}

//----------------------------------------------------------------------------
vtkVOXImageWriter::~vtkVOXImageWriter()
{
  this->SetFileName(0);
}


//----------------------------------------------------------------------------
void vtkVOXImageWriter::Write( )
{
  this->SetErrorCode(vtkErrorCode::NoError);
#if VTK_MAJOR_VERSION <= 5
  this->GetInput()->UpdateInformation();
#else
  this->UpdateInformation();
#endif
  // Error checking
  if (this->GetInput() == NULL ) {
    vtkErrorMacro(<<"Write:Please specify an input!");
    return;
  }

  if ( this->FileName == 0) {
    vtkErrorMacro("Output file name not specified");
    return;
  }

  int nDims = 3;
#if VTK_MAJOR_VERSION <= 5
  int * ext = this->GetInput()->GetWholeExtent();
#else
  int * ext = this->GetInformation()->Get(vtkDataObject::DATA_EXTENT());
#endif
  if ( ext[4] == ext[5] ) {
    nDims = 2;
    if ( ext[2] == ext[3] ) {
      nDims = 1;
    }
  }

  double * origin = this->GetInput()->GetOrigin();
  double * spacingDouble = this->GetInput()->GetSpacing();

  float spacing[3];
  spacing[0] = spacingDouble[0];
  spacing[1] = spacingDouble[1];
  spacing[2] = spacingDouble[2];

  int dimSize[3];
  dimSize[0] = ext[1]-ext[0]+1;
  dimSize[1] = ext[3]-ext[2]+1;
  dimSize[2] = ext[5]-ext[4]+1;

  std::string elementType;

  int scalarType = this->GetInput()->GetScalarType();
  switch ( scalarType ) {
  case VTK_CHAR:
    elementType = "schar";
    break;
  case VTK_UNSIGNED_CHAR:
    elementType = "uchar";
    break;
  case VTK_SHORT:
    elementType = "sshort";
    break;
  case VTK_UNSIGNED_SHORT:
    elementType = "ushort";
    break;
  case VTK_INT:
    elementType = "int";
    break;
  case VTK_UNSIGNED_INT:
    elementType = "uint";
    break;
  case VTK_LONG:
    elementType = "slong";
    break;
  case VTK_UNSIGNED_LONG:
    elementType = "ulong";
    break;
  case VTK_FLOAT:
    elementType = "float";
    break;
  case VTK_DOUBLE:
    elementType = "double";
    break;
  default:
    vtkErrorMacro("Unknown scalar type." );
    return ;
  }

  origin[0] += ext[0] * spacing[0];
  origin[1] += ext[2] * spacing[1];
  origin[2] += ext[4] * spacing[2];

#if VTK_MAJOR_VERSION <= 5
  this->GetInput()->SetUpdateExtent(ext[0], ext[1],
                                    ext[2], ext[3],
                                    ext[4], ext[5]);
  this->GetInput()->UpdateData();
#elif VTK_MAJOR_VERSION >= 8 || (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1)
  this->UpdateExtent(ext);
  this->Update();
#else
  this->SetUpdateExtent(ext);
  this->Update();
#endif
  this->SetFileDimensionality(nDims);

  this->InvokeEvent(vtkCommand::StartEvent);
  this->UpdateProgress(0.0);
  //write here
  std::cout << "Writing to file " << this->GetFileName() << " ..." << std::endl;
  std::cout.flush();
  fstream out(this->GetFileName(),ios::out|ios::binary);
  out << "VOX v2\n# Size\n" << dimSize[0] << " " << dimSize[1] << " "
      << dimSize[2] << std::endl << "# Spacing" << std::endl
      << spacing[0] << " " << spacing[1] << " " << spacing[2] << std::endl
      << "# Image dim" << std::endl << nDims << std::endl
      << "# Image type" << std::endl << elementType << std::endl;
  out.write((char*)this->GetInput()->GetScalarPointer(),
            dimSize[0]*dimSize[1]*dimSize[2]*this->GetInput()->GetScalarSize());
  out.close();

  this->UpdateProgress(1.0);
  this->InvokeEvent(vtkCommand::EndEvent);
}

//----------------------------------------------------------------------------
void vtkVOXImageWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: " << (this->FileName==0?this->FileName:"(none)") << endl;
}
