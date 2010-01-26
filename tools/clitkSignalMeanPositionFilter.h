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

#ifndef CLITKSIGNALMEANPOSITIONFILTER_H
#define CLITKSIGNALMEANPOSITIONFILTER_H

#include "clitkSignalMeanPositionTracking_ggo.h"
#include "clitkSignal.h"
#include "clitkEllipse.h"
#include "itkVector.h"

namespace clitk {

  //---------------------------------------------------------------------
  class SignalMeanPositionFilter {
  public:
    typedef itk::Vector<double,2> Vector2d;

    void SetParameters(gengetopt_args_info_clitkSignalMeanPositionTracking & args_info);
    void Update();
    
  protected:    
    gengetopt_args_info_clitkSignalMeanPositionTracking args_info;
    clitk::Signal mInput;
    clitk::Signal mAugmentedInputX;
    clitk::Signal mAugmentedInputY;
    int mAugmentationDelay;
    int mMaxIteration;
    double mEta;
    bool mEtaIsSet;
    bool mOutputFilenameIsSet;
    bool mOutputResidualFilenameIsSet;
    bool mOutputAugmentedFilenameIsSet;
    std::string mOutputFilename;
    std::string mOutputResidualFilename;
    std::string mOutputAugmentedFilename;
    bool mVerbose;
    bool mVerboseIteration;
    bool mIsAdaptiveMethod;
    std::vector<double> mCurrentResidual;
    int mWindowLength;
    std::vector<Vector2d> mCenters;

    void FitEllipse(clitk::Ellipse & An);
    void AdaptiveFitEllipse(clitk::Ellipse & An);

    void ComputeAugmentedSpace(const clitk::Signal & input, 
                               clitk::Signal & outputX, 
                               clitk::Signal & outputY, 
                               unsigned int delay);
  };
  //---------------------------------------------------------------------
  
} // end namespace

#endif
