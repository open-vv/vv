/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkSignalMeanPositionFilter.h,v $
  Language:  C++
  Date:      $Date: 2010/02/10 14:55:00 $
  Version:   $Revision: 1.4 $
                                                                                
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
#include <math.h>

namespace clitk {

  //---------------------------------------------------------------------
  class SignalMeanPositionFilter {
  public:
    typedef itk::Vector<double,2> Vector2d;

    void SetParameters(args_info_clitkSignalMeanPositionTracking & args_info);
    void Update();
    
  protected:    
    args_info_clitkSignalMeanPositionTracking args_info;
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
    std::vector<clitk::Ellipse*> mListOfEllipses;
    
    bool mValidationWithRealPhase;
    std::string mInputPhaseFilename;
    clitk::Signal mInputPhase;
    std::vector<int> mCycles;

    std::vector<int> mIsoPhaseIndex;
    std::vector<double> mIsoPhaseDelta;
    std::vector<int> mIsoPhaseDeltaNb;
    std::vector<double> mIsoPhaseRefAngle;

    bool mUseLearnedDeltaPhase;
    clitk::Signal mLearnIsoPhaseDelta;
    int mNumberOfIsoPhase;

    void FitEllipse(clitk::Ellipse & An);
    void AdaptiveFitEllipse(clitk::Ellipse & An);

    void ComputeIsoPhase(std::vector<clitk::Ellipse*> & l, 
                         std::vector<double> & phase,
                         std::vector<int> & cycles);
  };
  //---------------------------------------------------------------------
  
} // end namespace

#endif
