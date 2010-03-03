/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkSignalApparentMotionTrackingFilter.h,v $
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

#ifndef SIGNALAPPARENTMOTIONTRACKINGFILTER_H
#define SIGNALAPPARENTMOTIONTRACKINGFILTER_H

#include "clitkSignalApparentMotionTracking_ggo.h"
#include "clitkSignal.h"
#include "clitkEllipse.h"
#include "clitkTrajectory2D.h"
#include "itkVector.h"
#include <math.h>

namespace clitk {

  //---------------------------------------------------------------------
  class SignalApparentMotionTrackingFilter {
  public:
    typedef itk::Vector<double,2> Vector2d;

    void SetParameters(args_info_clitkSignalApparentMotionTracking & args_info);
    void Update();
    
  protected:    
    args_info_clitkSignalApparentMotionTracking args_info;
    clitk::Signal mRefU;
    clitk::Signal mRefV;
    clitk::Signal mRefPhase;
    clitk::Signal mInputU;
    clitk::Signal mInputV;
    clitk::Signal mMeanU;
    clitk::Signal mMeanV;
    clitk::Signal mIsoPhaseU;
    clitk::Signal mIsoPhaseV;
    double mRefSampling;

    std::vector<Trajectory2D> mReferenceTrajectoriesBD;
    clitk::Signal mReferencePhase;
    clitk::Trajectory2D mInput;
    clitk::Trajectory2D mInputRelative;
    clitk::Trajectory2D mInputMean;
    std::vector<Vector2d> mReferenceMean;

    void ComputeMeanAndIsoPhase(clitk::Signal & s, int delay, int L, int nbIso,
                                clitk::Signal & mean, clitk::Signal & isoPhase);
    void ComputeMeanAndIsoPhase(clitk::Trajectory2D & s, int delay, int L,
                                int nbIsoPhase, clitk::Trajectory2D & mean, 
                                std::vector<int> & isoPhase);
    void EstimatePhase(const clitk::Trajectory2D & input, 
                       const int begin, const int end, 
                       const clitk::Trajectory2D & ref, 
                       const clitk::Signal & phaseref, 
                       clitk::Signal & phase);
    int FindPhaseDelta(const clitk::Trajectory2D & A, const clitk::Trajectory2D & B) const;
    int FindClosestTrajectory(const std::vector<Vector2d> & meanlist, const Vector2d & mean);
    void ReadDB(std::string filename, std::vector<Trajectory2D> & bd);
  };
  //---------------------------------------------------------------------
  
} // end namespace

#endif
