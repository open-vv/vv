/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkSignalMeanPositionFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/03 10:47:48 $
  Version:   $Revision: 1.6 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

#include "clitkSignalMeanPositionFilter.h"

//---------------------------------------------------------------------
void clitk::SignalMeanPositionFilter::SetParameters(args_info_clitkSignalMeanPositionTracking & args) {
  args_info = args;
  mEtaIsSet = false;
  mOutputFilenameIsSet = false;
  mOutputResidualFilenameIsSet = false;
  mInput.Read(args_info.input_arg);
  mAugmentationDelay = args_info.delay_arg;
  mIsAdaptiveMethod = false;
  mWindowLength = -1;
  mValidationWithRealPhase = false;
  mUseLearnedDeltaPhase = false;
  if (args_info.eta_given) {
    mEta = args_info.eta_arg;
    mEtaIsSet = true;
  }
  mMaxIteration = args_info.iter_arg;
  if (args_info.output_given) {
    mOutputFilenameIsSet = true;
    mOutputFilename = args_info.output_arg;
  }
  if (args_info.residual_given) {
    mOutputResidualFilenameIsSet = true;
    mOutputResidualFilename = args_info.residual_arg;
  }
  if (args_info.augmented_given) {
    mOutputAugmentedFilenameIsSet = true;
    mOutputAugmentedFilename = args_info.augmented_arg;
  }
  mVerbose = args_info.verbose_flag;
  mVerboseIteration = args_info.verbose_iteration_flag;
  if (args_info.L_given) {
    mIsAdaptiveMethod = true;
    mWindowLength = args_info.L_arg;
  }
  if (args_info.phase_given) {
    mValidationWithRealPhase = true;
    mInputPhaseFilename = args_info.phase_arg;
    mInputPhase.Read(mInputPhaseFilename);
  }
  mNumberOfIsoPhase = args_info.nbiso_arg;
  if (args_info.delta_given) {
    mUseLearnedDeltaPhase = true;
    mLearnIsoPhaseDelta.Read(args_info.delta_arg);
    mNumberOfIsoPhase = mLearnIsoPhaseDelta.size();
  }

  // DEBUG
  if (args_info.phase_given) {
    std::cout << "PLEASE DO NO USE THIS OPTION --phase YET ..." << std::endl;
    exit(0);
  }
  if (args_info.delta_given) {
    std::cout << "PLEASE DO NO USE THIS OPTION --delta YET ..." << std::endl;
    exit(0);
  }

}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalMeanPositionFilter::Update() {

  // Compute augmented space
  if (mVerbose) {
    std::cout << "Computing augmented space with delay = " << mAugmentationDelay
              << ", number of samples is " << mInput.size()-mAugmentationDelay 
              << std::endl;
  }
  mInput.ComputeAugmentedSpace(mAugmentedInputX, mAugmentedInputY, mAugmentationDelay);

  // Output augmented state
  if (mOutputAugmentedFilenameIsSet) {
    std::ofstream os;
    openFileForWriting(os, mOutputAugmentedFilename);
    for(unsigned int i=0; i<(unsigned int)mAugmentedInputX.size(); i++) {
      os << mAugmentedInputX[i] << " " << mAugmentedInputY[i] << std::endl;
    }
    os.close();
  }

  // Initial starting ellipse
  Ellipse An; // starting point
  if (!mEtaIsSet) mEta = -1;
  DD(mWindowLength);
  if (mWindowLength == -1) {
    mWindowLength = mAugmentedInputY.size();
  }
  An.InitialiseEllipseFitting(mEta, mWindowLength, mAugmentedInputX, mAugmentedInputY);
  mEta = An.GetEta();
  if (mVerbose) {
    std::cout << "Eta is " << mEta << std::endl;
  }
  Ellipse AnInitial(An);
  if (mVerbose) {
    std::cout << "Set initial ellipse to c=" << An.ComputeCenter() 
              << " and axes=" << An.ComputeSemiAxeLengths() << std::endl;
  }

  // Fitting method
  if (mIsAdaptiveMethod) {
    AdaptiveFitEllipse(An);
  }
  else FitEllipse(An);

  // Output
  if (mVerbose) {
    std::cout << "Result    is "  << An << std::endl;
    std::cout << "Center    is "  << An.ComputeCenter() << std::endl;
    std::cout << "SemiAxis are " << An.ComputeSemiAxeLengths() << std::endl;
    std::cout << "Angle     is " << rad2deg(An.ComputeAngleInRad()) << " deg" << std::endl;
  }
  if (mOutputFilenameIsSet) {
    std::ofstream os;
    openFileForWriting(os, mOutputFilename);
    
    std::vector<double> phase;
    ComputeIsoPhase(mListOfEllipses, phase, mCycles);
    
    //int currentCycle = 0;
    for (unsigned int i=0; i<mListOfEllipses.size(); i++) {
      clitk::Ellipse & An = *mListOfEllipses[i];
      int time = i+mAugmentationDelay+mWindowLength;
      os << time << " " // current 'time' in input
         << mInput[time] << " " // current value in input
         << An.ComputeCenter()[0] << " " << An.ComputeCenter()[1] << " " 
         << An.ComputeSemiAxeLengths()[0] << " " << An.ComputeSemiAxeLengths()[1] << " " 
         << An.ComputeAngleInRad() << " "
         << rad2deg(An.ComputeAngleInRad()) << " _Phase_ "
         << mIsoPhaseIndex[i] << " " ;

      /*
        if (mUseLearnedDeltaPhase) { 
        os << mIsoPhaseDelta[mIsoPhaseIndex[i]] << " " << phase[i] << " ";
        }
      */

      os << " _Ellipse_ ";
      for(int j=0; j<6; j++) os << An[j] << " ";
      os << std::endl;
    }
    os.close();
  }
  
  if (mOutputResidualFilenameIsSet) {
    std::ofstream os;
    openFileForWriting(os, mOutputResidualFilename);
    for(unsigned int i=0; i<mCurrentResidual.size(); i++) {
      os.precision(10);
      os << mCurrentResidual[i] << std::endl;
    }
  os.close();
  }
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalMeanPositionFilter::FitEllipse(clitk::Ellipse & An) {
  int n = 0;
  mCurrentResidual.clear();
  while (n<mMaxIteration) {
    double r = An.EllipseFittingNextIteration();
    mCurrentResidual.push_back(r);
    n++;
    if (mVerboseIteration) {
      std::cout.precision(3);
      std::cout << "A(" << n << ")=" << An
                << " c="  << An.ComputeCenter() 
                << " ax=" << An.ComputeSemiAxeLengths()
                << " theta=" << rad2deg(An.ComputeAngleInRad());
      std::cout.precision(10);
      std::cout << " R=" << r << std::endl;
    }
  }
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalMeanPositionFilter::AdaptiveFitEllipse(clitk::Ellipse & An) {  
  // Compute the total number of iteration : 
  //     = nb of inputs minus the delay, minus the windowL, minus the first one
  int maxN = std::min((unsigned int)args_info.t_arg, 
                      mInput.size()-mWindowLength-mAugmentationDelay);
  for(int t=0; t<maxN; t++) {
    An.UpdateSMatrix(t, mWindowLength, mAugmentedInputX, mAugmentedInputY);
    FitEllipse(An);
    clitk::Ellipse * B = new clitk::Ellipse(An);
    mListOfEllipses.push_back(B);
  }
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalMeanPositionFilter::ComputeIsoPhase(std::vector<clitk::Ellipse*> & l, 
                                                      std::vector<double> & phase, 
                                                      std::vector<int> & cycles) {
  double refphaseangle=0;
  double previousangle=0;
  phase.resize(mListOfEllipses.size());

  // DD(mListOfEllipses.size());
  mIsoPhaseIndex.resize(mListOfEllipses.size());
  // mIsoPhaseDelta.resize(mNumberOfIsoPhase);
  //   mIsoPhaseDeltaNb.resize(mNumberOfIsoPhase);
  mIsoPhaseRefAngle.resize(mNumberOfIsoPhase); 

  for (unsigned int i=0; i<mListOfEllipses.size(); i++) {
    clitk::Ellipse & An = *mListOfEllipses[i];
    
    // Current time accordint to initial input signal
    // int time = i+mAugmentationDelay+mWindowLength;   // not use yet
    
    // Compute current signed angle
    Vector2d x1(An.ComputeCenter());
    double a = mListOfEllipses[0]->ComputeSemiAxeLengths()[0]; 
    double theta = mListOfEllipses[0]->ComputeAngleInRad(); 
    Vector2d x2; x2[0] = x1[0]+a * cos(theta); x2[1] = x1[1]+a * sin(theta);
    Vector2d x3(x1);
    Vector2d x4; x4[0] = mAugmentedInputX[i+mWindowLength]; x4[1] = mAugmentedInputY[i+mWindowLength];
    Vector2d A(x2-x1);
    Vector2d B(x4-x3);
    double signed_angle = atan2(B[1], B[0]) - atan2(A[1], A[0]);
    // double signed_angle = atan2(B[1], B[0]) - atan2(0, 1);
    if (signed_angle<0) signed_angle = 2*M_PI+signed_angle;
    // http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm
    
    // First time : set the angle
    if (i==0) {
      refphaseangle = signed_angle;
      for(int a=0; a<mNumberOfIsoPhase; a++) {
        if (a==0) mIsoPhaseRefAngle[a] = signed_angle;
        else mIsoPhaseRefAngle[a] = (signed_angle
                                     + (2*M_PI)*a/mNumberOfIsoPhase);
        if (mIsoPhaseRefAngle[a] > 2*M_PI) mIsoPhaseRefAngle[a] -= 2*M_PI;
        if (mIsoPhaseRefAngle[a] < 0) mIsoPhaseRefAngle[a] = 2*M_PI-mIsoPhaseRefAngle[a];
        // DD(rad2deg(mIsoPhaseRefAngle[a]));
        // mIsoPhaseDelta[a] = 0.0;
        // mIsoPhaseDeltaNb[a] = 0;
      }
      int a=0;
      // DD(rad2deg(signed_angle));
      while ((a<mNumberOfIsoPhase) && (signed_angle>=mIsoPhaseRefAngle[a])) { a++; }
      // DD(a);
      mIsoPhaseIndex[i] = a-1;
      // DD(mIsoPhaseIndex[0]);
      cycles.push_back(0);
    }
    else {
      mIsoPhaseIndex[i] = mIsoPhaseIndex[i-1];
      
      // Check if angle cross a ref angle
      for(int a=0; a<mNumberOfIsoPhase; a++) {
        std::cout << "a=" << rad2deg(signed_angle) << " p=" << rad2deg(previousangle)
                  << " ref=" << rad2deg(mIsoPhaseRefAngle[a]) << std::endl;
        if (
            (((signed_angle > mIsoPhaseRefAngle[a]) && (previousangle < mIsoPhaseRefAngle[a]))) ||
            ((mIsoPhaseRefAngle[a]==0) && (signed_angle < previousangle)))
          {
            // if (mValidationWithRealPhase) {
            //             mIsoPhaseDelta[a] += mInputPhase[time];
            //             mIsoPhaseDeltaNb[a]++;
            //           }
            mIsoPhaseIndex[i] = a;
            // DD(a);
            cycles.push_back(i);
          }
      }
      // DD(mIsoPhaseIndex[i]);      
    }
    
    previousangle = signed_angle;
  }
  
  /*
  if (mValidationWithRealPhase) {
    // Mean of all deltas
    for(unsigned int a=0; a<mIsoPhaseDelta.size(); a++) {
      if (mIsoPhaseDelta[a] == 0) {
        std::cerr << "Error : less than one cyle ?" << std::endl;
        exit(0);
      }
      mIsoPhaseDelta[a] /= mIsoPhaseDeltaNb[a];
      DD(mIsoPhaseDelta[a]);
    }
    std::ofstream os;
    openFileForWriting(os, "delta.sig");
    for(unsigned int a=0; a<mIsoPhaseDelta.size(); a++) {
      os << mIsoPhaseDelta[a] << std::endl;
    }
    os.close();    
  }
  */

  /*
  if (mUseLearnedDeltaPhase) {
    for(unsigned int a=0; a<mIsoPhaseDelta.size(); a++) {
      mIsoPhaseDelta[a] = mLearnIsoPhaseDelta[a];
      DD(mIsoPhaseDelta[a]);
    }
  }
  */
  
  // DEBUG UNUSED
  /*
  int j=0;
  for (unsigned int i=0; i<cycles.size(); i++) {
    DD(cycles[i]);
    int n;
    if (i!=0) {
      n=cycles[i]-cycles[i-1];
      for(int index=0; index<n; index++) {
        
        int indexOfPhase1 = mIsoPhaseIndex[cycles[i-1]];
        int indexOfPhase2 = mIsoPhaseIndex[cycles[i]];
        DD(indexOfPhase1);
        DD(indexOfPhase2);
        double ph1 = mIsoPhaseDelta[indexOfPhase1];
        double ph2 = mIsoPhaseDelta[indexOfPhase2];
        DD(ph1);
        DD(ph2);
        
        phase[j] = (ph1+(double)index*(ph2-ph1)/(double)n);
        if (phase[j]>1.0) phase[j] = 1.0-phase[j];
        DD(phase[j]);
        j++;
      }
    }
    else j+=cycles[0];
  }  
  */
  for (unsigned int i=0; i<cycles.size(); i++) {
    DD(cycles[i]);
  }

}
//---------------------------------------------------------------------

