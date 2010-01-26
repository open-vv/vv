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

#include "clitkSignalMeanPositionFilter.h"

//---------------------------------------------------------------------
void clitk::SignalMeanPositionFilter::SetParameters(gengetopt_args_info_clitkSignalMeanPositionTracking & args) {
  args_info = args;
  mEtaIsSet = false;
  mOutputFilenameIsSet = false;
  mOutputResidualFilenameIsSet = false;
  mInput.Read(args_info.input_arg);
  mAugmentationDelay = args_info.delay_arg;
  mIsAdaptiveMethod = false;
  mWindowLength = -1;
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
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalMeanPositionFilter::Update() {

  // DEBUG
//   int e = 5;
//   clitk::Signal temp;
//   temp.resize(mInput.size()*e);
//   for(unsigned int i=0; i<temp.size(); i++) {
//     int index = lrint(floor((double)i/e));
//     double res = (double)i/e - index;
//     //    DD(index);
//     //DD(res);
//     temp[i] = mInput[index] + res*(mInput[index+1]-mInput[index])/e;
//   }
//   mInput.resize(temp.size());
  for  (unsigned int i=0; i<mInput.size(); i++) mInput[i] = mInput[i]+(double)i/(double)mInput.size();
  srand ( time(0) );
  for  (unsigned int i=0; i<mInput.size(); i++) {
    mInput[i] = (0.8+((double)rand()/ (double)RAND_MAX)*0.4) * mInput[i];
  }

   {
     std::ofstream os;
     openFileForWriting(os, "input.dat");
     for(unsigned int i=0; i<mInput.size(); i++) os << mInput[i] << std::endl;
     os.close();
   }


  // Compute augmented space
  if (mVerbose) {
    std::cout << "Computing augmented space with delay = " << mAugmentationDelay
              << ", number of samples is " << mInput.size()-mAugmentationDelay 
              << std::endl;
  }
  ComputeAugmentedSpace(mInput, mAugmentedInputX, mAugmentedInputY, mAugmentationDelay);

  // Output augmented state
  if (mOutputAugmentedFilenameIsSet) {
    std::ofstream os;
    openFileForWriting(os, mOutputAugmentedFilename);
    for(unsigned int i=0; i<(unsigned int)mWindowLength; i++) {
      os << mAugmentedInputX[i] << " " << mAugmentedInputY[i] << std::endl;
    }
    os.close();
  }

  // Initial starting ellipse
  Ellipse An; // starting point
  if (!mEtaIsSet) mEta = -1;
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
    //FitEllipse(An);   
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
    os << AnInitial.ComputeCenter()[0] << " " << AnInitial.ComputeCenter()[1] << " " 
       << AnInitial.ComputeSemiAxeLengths()[0] << " " << AnInitial.ComputeSemiAxeLengths()[1] << " " 
       << AnInitial.ComputeAngleInRad();
    for(int i=0; i<6; i++) os << AnInitial[i] << " " ;
    os << std::endl;
    
    os << An.ComputeCenter()[0] << " " << An.ComputeCenter()[1] << " " 
       << An.ComputeSemiAxeLengths()[0] << " " << An.ComputeSemiAxeLengths()[1] << " " 
       << An.ComputeAngleInRad();
    for(int i=0; i<6; i++) os << An[i] << " ";
    os << std::endl;
    os.close();

    openFileForWriting(os, "centers.dat");
    for(unsigned int i=0; i<mCenters.size(); i++) {
      os << mCenters[i][0] << " " << mCenters[i][1] << std::endl;
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
void clitk::SignalMeanPositionFilter::ComputeAugmentedSpace(const clitk::Signal & input, 
                                                            clitk::Signal & outputX, 
                                                            clitk::Signal & outputY, 
                                                            unsigned int delay) {
  if (input.size() <= delay) {
    std::cerr << "Error in signal length is " << input.size()
              << " while delay is " << delay << " : too short. Abort." << std::endl;
    exit(0);
  }
  outputX.resize(input.size()-delay);
  outputY.resize(input.size()-delay);
  for(unsigned int i=0; i<outputX.size(); i++) {
    outputX[i] = input[i+delay];
    outputY[i] = input[i];
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
  for(unsigned int t=0; t<(unsigned int)args_info.t_arg; t++) {
    DD(t);
    An.UpdateSMatrix(t, mWindowLength, mAugmentedInputX, mAugmentedInputY);
    mCenters.push_back(An.ComputeCenter());
    FitEllipse(An);
  }
}
//---------------------------------------------------------------------
