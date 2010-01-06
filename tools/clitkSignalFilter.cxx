/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkSignalFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2010/01/06 13:31:56 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

#ifndef CLITKSIGNALFILTER_CXX
#define CLITKSIGNALFILTER_CXX
/**
   =================================================
   * @file   clitkSignalFilter.cxx
   * @author Jef Vandemeulebroucke <jefvdmb@gmail.com>
   * @date   23 April 2008
   * 
   * @brief  "Apply a filter to the signal"                  
   =================================================*/

// clitk include
#include "clitkSignalFilter_ggo.h"
#include "clitkSignal.h"
#include "clitkIO.h"
#include "clitkIOCommon.h"

int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkSignalFilter, args_info);
  CLITK_INIT;

  //input
  clitk::Signal input;
  input.Read (args_info.input_arg);
  clitk::Signal input2;
  if (args_info.input2_given) input2.Read(args_info.input2_arg);

  double p1 = args_info.p1_arg;
  double p2 = args_info.p2_arg;

  //process

  if( args_info.multiply_flag) input*=input2;
  if( args_info.divide_flag) input/=input2;
  if( args_info.norm_flag) input=input.Normalize (p1, p2);
  if( args_info.highPass_flag) input=input.HighPassFilter(p1,p2);
  if( args_info.lowPass_flag) input=input.LowPassFilter(p1,p2);
  if( args_info.detect_flag) input=input.DetectLocalExtrema(static_cast<unsigned int >(p1));
  if( args_info.limPhase_flag) input=input.LimPhase();
  if( args_info.monPhase_flag) input=input.MonPhase();
  if( args_info.monPhaseDE_flag) input=input.MonPhaseDE(p1,p2);
  if( args_info.average_flag) input=input.MovingAverageFilter(static_cast<unsigned int> (p1));
  if( args_info.ssd_flag) std::cout<<"The sqrt of the mean SSD is "<< input.SSD(input2)<<std::endl;
  if (args_info.gauss_flag)input=input.GaussLikeFilter();
  if (args_info.rescale_flag)input=input.NormalizeMeanStdDev(p1, p2);
  if (args_info.interp_flag)input=input.LinearlyInterpolateScatteredValues();
  // if( args_info.approx_flag) input=input.ApproximateScatteredValuesWithBSplines(static_cast<unsigned int>(p1),static_cast<unsigned int>(p2));
  if( args_info.limit_flag) input=input.LimitSignalRange();
//   if( args_info._flag) input=input;
//   if( args_info._flag) input=input;
//   if( args_info._flag) input=input;
//   if( args_info._flag) input=input;
  // if( args_info._flag) input=input;
  // if( args_info._flag) input=input;
  // if( args_info._flag) input=input;

  if (args_info.output_given) input.Write(args_info.output_arg);

  return EXIT_SUCCESS;
  
}

#endif
