/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkTrajectory2D.h,v $
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

#ifndef CLITKTRAJECTORY2D_H
#define CLITKTRAJECTORY2D_H

#include "clitkCommon.h"
#include "clitkSignal.h"
#include "itkVector.h"
#include <vnl/algo/vnl_generalized_eigensystem.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_real_eigensystem.h>

namespace clitk {

  //---------------------------------------------------------------------
  class Trajectory2D {
  public:
    typedef itk::Vector<double,2> Vector2d;

    void ResampleWithTimeWarpTo(int begin, int end, Trajectory2D & output) const;
    void ResampleWithTimeWarpTo(Trajectory2D & output) const; 
    void Shift(double s);
    
    bool Read(std::string filename);

    void GetPoint(const int index, Vector2d & p);
    void GetMean(Vector2d & m);
    double GetU(const int index) const { return mU[index]; }
    double GetV(const int index) const { return mV[index]; }
    clitk::Signal & GetU() { return mU; }
    clitk::Signal & GetV() { return mV; }
    
    int size() const { return mU.size(); }
    void resize(int n) { mU.resize(n); mV.resize(n); }

    void SetPoint(const int i, Vector2d & p) { mU[i] = p[0]; mV[i] = p[1]; }
    void SetPoint(const int i, double u, double v) { mU[i] = u; mV[i] = v; }
    void GetValueAtLin(double t, Vector2d & p) const;

    double GetSamplingPeriod() const { return mU.GetSamplingPeriod(); }
    void SetSamplingPeriod(double sp){ mU.SetSamplingPeriod(sp); mV.SetSamplingPeriod(sp); }
    double DistanceTo(int delta, const clitk::Trajectory2D & B) const;

    Trajectory2D & operator+(Trajectory2D & d);
    Trajectory2D & operator-(Trajectory2D & d);
    
    void Print(const std::string & name) const;
    void Print(const std::string & name, int begin, int end) const;

    void push_back(double u, double v) { mU.push_back(u); mV.push_back(v); }
    void Substract(const Vector2d & m);
    double GetTotalTimeDuration() const { return mU.GetTotalTimeDuration(); }

  protected:
    clitk::Signal mU;
    clitk::Signal mV;
    
  };
  //---------------------------------------------------------------------


} // end namespace

#endif
