/*-------------------------------------------------------------------------
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkRigidRegistration.cxx,v $
  Language:  C++
  Date:      $Date: 2010/01/06 13:31:57 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
-------------------------------------------------------------------------*/

/**
 -------------------------------------------------
 * @file   clitkRigidRegistration.cxx
 * @author Jef Vandemeulebroucke <jef.Vandemeulebroucke@creatis.insa-lyon.fr>
 * @date   14 August 2007
 * 
 * @brief Perform a rigid registration between 2 images
 * 
 -------------------------------------------------*/


// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkRigidRegistration_ggo.h"
#include "clitkRigidRegistrationGenericFilter.h"

using namespace clitk;
using namespace std;


int main( int argc, char *argv[] )
{
  //init command line and check options
  GGO(args_info);  
  CLITK_INIT;
  
  //---------------------------------------------------------------------------
  //Set all the options passed through the commandline

  RigidRegistrationGenericFilter rigidRegistration;

  rigidRegistration.SetVerbose(args_info.verbose_flag);
  rigidRegistration.SetGradient(args_info.gradient_flag);
  rigidRegistration.SetZeroOrigin(args_info.zero_origin_flag);

 //Input
  rigidRegistration.SetFixedImageName(args_info.reference_arg);
  rigidRegistration.SetMovingImageName(args_info.object_arg);
  rigidRegistration.SetFixedImageMaskGiven(args_info.mask_given);
  if (args_info.mask_given)   rigidRegistration.SetFixedImageMaskName(args_info.mask_arg);

  //Output
  rigidRegistration.SetOutputGiven(args_info.output_given);
  if (args_info.output_given)  rigidRegistration.SetOutputName(args_info.output_arg);
  rigidRegistration.SetCheckerAfterGiven(args_info.checker_after_given);
  if (args_info.checker_after_given)  rigidRegistration.SetCheckerAfterName(args_info.checker_after_arg);
  rigidRegistration.SetCheckerBeforeGiven(args_info.checker_before_given);
  if (args_info.checker_before_given)  rigidRegistration.SetCheckerBeforeName(args_info.checker_before_arg);
  rigidRegistration.SetBeforeGiven(args_info.before_given);
  if (args_info.before_given)  rigidRegistration.SetBeforeName(args_info.before_arg);
  rigidRegistration.SetAfterGiven(args_info.after_given);
  if (args_info.after_given)  rigidRegistration.SetAfterName(args_info.after_arg);
  rigidRegistration.SetMatrixGiven(args_info.matrix_given);
  if (args_info.matrix_given)  rigidRegistration.SetMatrixName(args_info.matrix_arg);

  //Interp
  rigidRegistration.SetInterpType(args_info.interp_arg);

  //Transform
  rigidRegistration.SetRotX(args_info.rotX_arg);
  rigidRegistration.SetRotY(args_info.rotY_arg);
  rigidRegistration.SetRotZ(args_info.rotZ_arg);
  rigidRegistration.SetTransX(args_info.transX_arg);
  rigidRegistration.SetTransY(args_info.transY_arg);
  rigidRegistration.SetTransZ(args_info.transZ_arg);

  //Optimizer
  rigidRegistration.SetLevels(args_info.levels_arg); 
  rigidRegistration.SetIstep(args_info.Istep_arg); 
  rigidRegistration.SetFstep(args_info.Fstep_arg); 
  rigidRegistration.SetRelax(args_info.relax_arg);
  rigidRegistration.SetInc(args_info.inc_arg);
  rigidRegistration.SetDec(args_info.dec_arg);
  rigidRegistration.SetIter(args_info.iter_arg);
  rigidRegistration.SetRweight(args_info.Rweight_arg);
  rigidRegistration.SetTweight(args_info.Tweight_arg);

  //Metric
  rigidRegistration.SetMetricType(args_info.metric_arg);
  rigidRegistration.SetSamples(args_info.samples_arg);
  rigidRegistration.SetBins(args_info.bins_arg);
  rigidRegistration.SetRandom(args_info.random_flag);
  rigidRegistration.SetStdDev(args_info.stdDev_arg);

  //Preprocessing
  rigidRegistration.SetBlur(args_info.blur_arg); 
  rigidRegistration.SetNormalize(args_info.normalize_flag); 

  rigidRegistration.Update();
  
}
