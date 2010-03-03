/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkSignalApparentMotionTrackingFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2010/03/03 12:41:27 $
  Version:   $Revision: 1.1 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

#include "clitkSignalApparentMotionTrackingFilter.h"
#include <limits>
#include <fstream>

//---------------------------------------------------------------------
void clitk::SignalApparentMotionTrackingFilter::SetParameters(args_info_clitkSignalApparentMotionTracking & a) {
  args_info = a;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalApparentMotionTrackingFilter::ComputeMeanAndIsoPhase(clitk::Signal & s, 
                                                                       int delay, 
                                                                       int L,
                                                                       int nbIsoPhase, 
                                                                       clitk::Signal & mean, 
                                                                       clitk::Signal & isoPhase) {
  int mMaxIteration = 1;

  // Compute augmented signal
  clitk::Signal sx;
  clitk::Signal sy;
  s.ComputeAugmentedSpace(sx, sy, delay);
  
  // DEBUG augmented space
  static int tt=0;
  ofstream os;
  if (tt==0) os.open("augmentedU.sig");
  else os.open("augmentedV.sig");
  tt++;
  for(uint i=0; i<sx.size(); i++) {
    os << i << " " << sx[i] << " " << sy[i] << std::endl;
  }
  os.close();

  // Compute starting ellipse
  Ellipse An;
  An.InitialiseEllipseFitting(-1, L, sx, sy); // -1 is auto eta
  DD(An.ComputeSemiAxeLengths());
  
  // Fit successive ellipses
  std::vector<clitk::Ellipse*> e;
  int n = s.size()-L-delay;
  for(int t=0; t<n; t++) {
    // An.InitialiseEllipseFitting(-1, L, sx, sy); // -1 is auto eta
    An.UpdateSMatrix(t, L, sx, sy);
    // DD(An.GetEta());
//     DD(An);
//     DD(An.ComputeSemiAxeLengths());
    int nn = 0;
    while (nn<mMaxIteration) {
      An.EllipseFittingNextIteration();
      // DD(nn);
//       DD(An);
//       DD(An.ComputeSemiAxeLengths());
      nn++;
    }
    clitk::Ellipse * B = new clitk::Ellipse(An);
    e.push_back(B);
    DD(t);
    DD(B->ComputeCenter());
//      DD(B->ComputeSemiAxeLengths());
  }

  // Get mean and isoPhase
  //mean.resize(n);
  isoPhase.resize(n);
  ComputeIsoPhaseFromListOfEllipses(e, sx, sy, nbIsoPhase, delay, L, isoPhase);
  for(unsigned int i=0; i<e.size(); i++) {
    clitk::Ellipse & An = *e[i];
    mean[i+delay+L] = An.ComputeCenter()[0];
    //    DD(An.ComputeCenter());
  }
  DD("------------------------");
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalApparentMotionTrackingFilter::ComputeMeanAndIsoPhase(clitk::Trajectory2D & s, 
                                                                       int delay, 
                                                                       int L,
                                                                       int nbIsoPhase, 
                                                                       clitk::Trajectory2D & mean, 
                                                                       std::vector<int> & isoPhase) {
  // Extract signal (slow I know)
  clitk::Signal U(s.GetU());
  clitk::Signal V(s.GetV());
  
  clitk::Signal & meanU = mean.GetU();
  clitk::Signal & meanV = mean.GetV();

  meanU.resize(s.size());
  meanV.resize(s.size());

  clitk::Signal isoPhaseU;
  clitk::Signal isoPhaseV;
  
  DD(U.size());
  DD(V.size());
  
  // Compute mean and iso independently
  ComputeMeanAndIsoPhase(U, delay, L, nbIsoPhase, meanU, isoPhaseU);
  ComputeMeanAndIsoPhase(V, delay, L, nbIsoPhase, meanV, isoPhaseV);

  // Check isphase ??? ???
  isoPhase.resize(s.size());
  for(unsigned int i=0; i<isoPhaseU.size(); i++) {

    // ********************* U OR V ??????????

    isoPhase[i+delay+L] = isoPhaseV[i]; 
    //isoPhase[i+delay+L] = isoPhaseU[i]; 

    //    mean[i+delay+L] = mean[i];
    //DD(isoPhase[i]);
  }
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalApparentMotionTrackingFilter::ReadDB(std::string filename, 
                                                       std::vector<Trajectory2D> & bd) {
  // File format : 2 columns by point (U,V), time by line
  std::ifstream is(filename.c_str());
  skipComment(is);
  std::string line;
  std::getline(is, line);
  
  // Compute nb of column in this first line
  std::istringstream iss(line);
  std::string s;
  int nb = 0;
  while (iss>>s) {
    nb++;
  }
  DD(nb);

  if (nb%2 == 1) {
    std::cout << "ERROR in the file '" << filename << "', I read " << nb 
              << " columns, which is not odd." << std::endl;
  }
  nb = nb/2;
  DD(nb);

  // Read trajectories
  bd.resize(nb);
  while (is) {
    std::istringstream iss(line);
    std::string s;
    int i=0;
    while (iss>>s) {
      double u = atof(s.c_str());
      iss >> s;
      double v = atof(s.c_str());
      bd[i].push_back(u,v);
      i++;
    }
    std::getline(is, line);
  }
  DD(bd[0].size());
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalApparentMotionTrackingFilter::Update() {

  // -----------------------------------------------------
  // Read and build initial DB
  ReadDB(args_info.ref_arg, mReferenceTrajectoriesBD);
  mReferencePhase.Read(args_info.refphase_arg);
  mReferencePhase.SetSamplingPeriod(args_info.refsampling_arg);

  DD(mReferencePhase.GetSamplingPeriod());
  DD(mReferencePhase.size());  
  DD(mReferenceTrajectoriesBD.size());

  mReferenceMean.resize(mReferenceTrajectoriesBD.size());
  for(unsigned int i=0; i<mReferenceMean.size(); i++) {
    mReferenceTrajectoriesBD[i].GetMean(mReferenceMean[i]);
    // DD(mReferenceMean[i]);
  }
  
  for(uint i=0; i<mReferenceTrajectoriesBD.size(); i++)
    mReferenceTrajectoriesBD[i].SetSamplingPeriod(mReferencePhase.GetSamplingPeriod());

  // Relative reference motion
  for(uint i=0; i<mReferenceTrajectoriesBD.size(); i++) {
    // DD(i);
    // mReferenceTrajectoriesBD[i].Print();
    mReferenceTrajectoriesBD[i].Substract(mReferenceMean[i]);
    // mReferenceTrajectoriesBD[i].Print();
  }

  // -----------------------------------------------------
  // Read trajectory
  mInput.Read(args_info.input_arg);
  DD(mInput.size());
  mInput.SetSamplingPeriod(args_info.inputsampling_arg);
  mInputMean.SetSamplingPeriod(args_info.inputsampling_arg);
  mInputRelative.SetSamplingPeriod(args_info.inputsampling_arg);
  DD(mInput.GetSamplingPeriod());

  // -----------------------------------------------------
  // Compute input mean and isophase
  std::cout << "Compute Mean and IsoPhase" << std::endl;
  std::vector<int> isophase;
  double d = args_info.delay_arg;
  int L = args_info.windowLength_arg;
  int nbiso = args_info.nbiso_arg;
  isophase.resize(mInput.size());
  ComputeMeanAndIsoPhase(mInput, d, L, nbiso, mInputMean, isophase);
  
  // -----------------------------------------------------
  // Substract mean to trajectory
  std::cout << "Build relative traj" << std::endl;
  mInputRelative.resize(mInput.size());
  for(int i=0; i<mInput.size(); i++) {
     // DD(i);
//      DD(mInput.GetU(i));
//      DD(mInput.GetV(i));
//      DD(mInputMean.GetU(i));
//      DD(mInputMean.GetV(i));
     mInputRelative.SetPoint(i, mInput.GetU(i) - mInputMean.GetU(i),
                            mInput.GetV(i) - mInputMean.GetV(i));
    // DD(mInputRelative.GetU(i));
//     DD(mInputRelative.GetV(i));
  }

  // DEBUG : output mean
  ofstream osm("mean-phase.sig");
  for(int i=0; i<mInput.size(); i++) {
    osm << mInputMean.GetU(i) << " " << mInputMean.GetV(i) << " " << isophase[i] << std::endl;
  }
  osm.close();

  // -----------------------------------------------------
  // Loop on each iso-phase segment
  std::cout << "Loop on each isophase" << std::endl;
  clitk::Signal mEstimatedPhase;
  mEstimatedPhase.resize(mInput.size());

  std::vector<int> isophaseIndex;
  int previous = isophase[0];
  for(uint i=0; i<isophase.size(); i++) {
    if (isophase[i] != previous) {
      isophaseIndex.push_back(i);
      previous = isophase[i];
    }
  }
  DDV(isophaseIndex, isophaseIndex.size());

  for(uint ips=0; ips<isophaseIndex.size()-1; ips++) {  
    DD(ips);
    int begin = isophaseIndex[ips];
    int end = isophaseIndex[ips+1];
    DD(begin);
    DD(end);


    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Find traj from " << begin << " to " << end << std::endl;
    mInput.Print("mInput", begin, end);
    mInputRelative.Print("mInputRelativ", begin, end);
    

    // Get current mean position
    Vector2d currentMeanPosition;
    mInputMean.GetPoint(begin, currentMeanPosition);
    DD(currentMeanPosition);

    // Find closest ref trajectory
    int index = FindClosestTrajectory(mReferenceMean, currentMeanPosition);
    DD(index);
    clitk::Trajectory2D & currentClosestReferenceTrajectory = mReferenceTrajectoriesBD[index];
    
    DD(mReferenceMean[index]);
    currentClosestReferenceTrajectory.Print("currentClosestReferenceTrajectory");

    // Estimate phase delta
    clitk::Signal tempPhase;
    EstimatePhase(mInputRelative, begin, end, 
                  currentClosestReferenceTrajectory, 
                  mReferencePhase, tempPhase);

    // Cat current phase
    int n=0;
    for(int i=begin; i<end; i++) {
      mEstimatedPhase[i] = tempPhase[n];
      n++;
    }
  }
  
  // -----------------------------------------------------
  // Output time - phase
  // DEBUG OUTPUT
  ofstream os(args_info.output_arg);
  for(int t=0; t<mInput.size(); t++) {
    os << t << " " << t*mInput.GetSamplingPeriod() << " " << mEstimatedPhase[t] << std::endl;
  }
  os.close();

}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::SignalApparentMotionTrackingFilter::EstimatePhase(const clitk::Trajectory2D & input, 
                                                              const int begin, const int end, 
                                                              const clitk::Trajectory2D & ref, 
                                                              const clitk::Signal & phaseref, 
                                                              clitk::Signal & phase) {

  // Create time-warped resampled trajectory
  clitk::Trajectory2D T;
  T.SetSamplingPeriod(ref.GetSamplingPeriod());
  DD(input.size());
  input.ResampleWithTimeWarpTo(begin, end, T);
  DD(input.GetSamplingPeriod());
  DD(ref.GetSamplingPeriod());
  ref.Print("ref");
  input.Print("input", begin, end);
  T.Print("T");

  // Find optimal delta
  int delta = FindPhaseDelta(T, ref);
  DD(delta);

  // Shift phase
  clitk::Signal temp(phaseref);
  DD(temp.GetSamplingPeriod());
  DDV(temp, temp.size());
  temp.Shift(delta, (int)lrint((end-begin)*input.GetSamplingPeriod()/phaseref.GetSamplingPeriod()));
  DD(temp.GetSamplingPeriod());
  DDV(temp, temp.size());
  

  // Output result with time-unwarp
  phase.resize(end-begin);
  phase.SetSamplingPeriod(input.GetSamplingPeriod());
  DD(phase.GetSamplingPeriod());
  DD(phaseref.GetSamplingPeriod());
  DD(temp.GetSamplingPeriod());
  DD(temp.size());
  DD(phase.size());
  temp.ResampleWithTimeWarpTo(phase, true); // linear
  DDV(phase, phase.size());
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
int clitk::SignalApparentMotionTrackingFilter::FindPhaseDelta(const clitk::Trajectory2D & A, 
                                                                 const clitk::Trajectory2D & B) const {
  assert(A.GetSamplingPeriod() == B.GetSamplingPeriod());
  
  DD("A");
  A.Print("A");
  DD("B");
  B.Print("B");

  // exhaustive search
  double min=numeric_limits<double>::max();
  int imin =0;
  for(int i=0; i<B.size(); i++) {
    double d = A.DistanceTo(i, B);
    std::cout.precision(10);
    std::cout << "i=" << i << " d=" << d << " (min=" << min << " imin=" << imin << ")" << std::endl;
    if (d < min) {
      imin = i;
      min = d;
    }
  }
  DD(imin);
  return imin;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
int clitk::SignalApparentMotionTrackingFilter::FindClosestTrajectory(const std::vector<Vector2d> & meanlist, 
                                                                     const Vector2d & mean) {
  // Brute force for the moment
  double dmin = numeric_limits<double>::max();
  double imin=0;
  for(uint i=0; i<meanlist.size(); i++) {
    double d = (meanlist[i]-mean).GetNorm();
    if (d< dmin) {
      dmin = d;
      imin = i;
    }
  }
  DD(dmin);
  DD(imin);
  return imin;
}
//---------------------------------------------------------------------
