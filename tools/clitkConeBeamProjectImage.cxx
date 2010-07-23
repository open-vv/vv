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
#ifndef CLITKCONEBEAMPROJECTIMAGE_CXX
#define CLITKCONEBEAMPROJECTIMAGE_CXX
/**
   =================================================
   * @file   clitkConeBeamProjectImage.cxx
   * @author Jef Vandemeulebroucke <jefvdmb@gmail.com>
   * @date   30 April 2008
   * 
   * @brief  "Project a 3D image using a Cone-Beam geometry"                  
   =================================================*/

// clitk include
#include "clitkConeBeamProjectImage_ggo.h"
#include "clitkConeBeamProjectImageGenericFilter.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkTransformUtilities.h"

int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkConeBeamProjectImage, args_info);
  CLITK_INIT;

  // Creation of a generic filter
  clitk::ConeBeamProjectImageGenericFilter::Pointer genericFilter = clitk::ConeBeamProjectImageGenericFilter::New();

  //Passing the arguments to the generic filter
  genericFilter->SetArgsInfo(args_info);

  //   //Passing the arguments to the generic filter
  //   genericFilter->SetVerbose(args_info.verbose_flag);
  //   genericFilter->SetInput(args_info.input_arg);
  //   genericFilter->SetOutput(args_info.output_arg);
  //   //genericFilter->SetMask(args_info.mask_arg);

  //   //set the three components of the isocenter
  //   std::vector<double> iso(3);
  //   for (unsigned int i=0 ; i<3 ; i++) iso[i]=args_info.iso_arg[i];
  //   genericFilter->SetIsoCenter(iso);
  //   genericFilter->SetSourceToScreen(args_info.screen_arg);
  //   genericFilter->SetSourceToAxis(args_info.axis_arg);
  //   genericFilter->SetProjectionAngle(args_info.angle_arg);
  //   if (args_info.matrix_given) genericFilter->SetRigidTransformMatrix(clitk::ReadMatrix3D(args_info.matrix_arg));
  //   genericFilter->SetEdgePaddingValue(args_info.pad_arg);

  //update
  genericFilter->Update();

  return 0;
}

#endif


