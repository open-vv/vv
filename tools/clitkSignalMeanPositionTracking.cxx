/*=========================================================================
                                                                                
  Program:   clitk
  Language:  C++
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

#ifndef CLITKSIGNALMEANPOSITIONTRACKING_CXX
#define CLITKSIGNALMEANPOSITIONTRACKING_CXX
/**
   =================================================
   * @file   clitkSignalMeanPositionTracking.cxx
   * @author David Sarrut <david.sarrut@creatis.insa-lyon.fr>
   * @date   Sept 2009
   * 
   * @brief  "See Ruan 2007, compute mean position from a signal"                  
   =================================================*/

// clitk include
#include "clitkSignalMeanPositionTracking_ggo.h"
#include "clitkSignalMeanPositionFilter.h"
#include "clitkIO.h"
#include "clitkImageCommon.h"

int main(int argc, char * argv[]) {

  // Init command line
  GGO(clitkSignalMeanPositionTracking,args_info);
  CLITK_INIT;

  // Init filter
  clitk::SignalMeanPositionFilter filter;
  filter.SetParameters(args_info);

  // Run
  filter.Update();

  // This is the end my friend
  return EXIT_SUCCESS;  
}

#endif
