/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkSignalApparentMotionTracking.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/03 12:47:31 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

#ifndef CLITKSIGNALAPPARENTMOTIONTRACKING_CXX
#define CLITKSIGNALAPPARENTMOTIONTRACKING_CXX
/**
   =================================================
   * @file   clitkSignalApparentMotionTracking.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   Sept 2009
   * 
   =================================================*/

// clitk include
#include "clitkSignalApparentMotionTracking_ggo.h"
#include "clitkSignalApparentMotionTrackingFilter.h"
#include "clitkCommon.h"

int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkSignalApparentMotionTracking,args_info);
  CLITK_INIT;

  // Init filter
  clitk::SignalApparentMotionTrackingFilter filter;
  filter.SetParameters(args_info);

  // Run
  filter.Update();

  // This is the end my friend
  return EXIT_SUCCESS;  
}

#endif
