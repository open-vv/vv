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
/**
   -------------------------------------------------
   * @file   clitkDicomInfo.cxx
   * @author Laurent ZAGNI <laurent.zagni@insa-lyon.fr>
   * @date   27 Jul 2006
   -------------------------------------------------*/

// clitk includes
#include "clitkDicomInfo_ggo.h"
#include "clitkCommon.h"

// itk (gdcm) include
#include "gdcmFile.h"

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // init command line
  GGO(clitkDicomInfo, args_info);

  // check arg
  if (args_info.inputs_num == 0) return 0;

  // Loop files
  for(unsigned int i=0; i<args_info.inputs_num; i++) {
    gdcm::File *header = new gdcm::File();
    header->SetFileName(args_info.inputs[i]);
    header->SetMaxSizeLoadEntry(163840);
    header->Load();
    header->Print();
  }

  // this is the end my friend
  return 0;
}
//--------------------------------------------------------------------
