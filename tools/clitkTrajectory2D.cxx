/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkTrajectory2D.cxx,v $
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

#include "clitkTrajectory2D.h"

//---------------------------------------------------------------------
void clitk::Trajectory2D::ResampleWithTimeWarpTo(int begin, int end, Trajectory2D & output) const {
  DD(begin);
  DD(end);
  int n = (int)lrint((end-begin)*GetSamplingPeriod()/output.GetSamplingPeriod());
  output.resize(n);
  DD(n);
  double duration = (end-begin)*GetSamplingPeriod();
  DD(duration);
  double sp = output.GetSamplingPeriod();
  DD(output.GetTotalTimeDuration());
  //   DD(sp);
  for(int i=0; i<n; i++) {
    Vector2d p;
    // DD(i);
    GetValueAtLin((i*sp)+(begin*GetSamplingPeriod()), p);
    // DD(p);
    output.SetPoint(i, p);
  }
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
bool clitk::Trajectory2D::Read(std::string filename) {
  bool b = mU.Read(filename, 0);
  if (!b) return false;
  b = mV.Read(filename, 1);
  return b;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::Trajectory2D::GetValueAtLin(double t, Vector2d & p) const {
  // Can be faster, I know ...
  p[0] = mU.GetValueAtLin(t);
  p[1] = mV.GetValueAtLin(t);
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::Trajectory2D::ResampleWithTimeWarpTo(Trajectory2D & output) const {
  ResampleWithTimeWarpTo(0, size(), output);
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------        
void clitk::Trajectory2D::Shift(double s) {
  std::cout << "Shift s=" << s << std::endl;
  int d = (int)lrint(s/GetSamplingPeriod()); // closest integer delta
  DD(d);
  clitk::Signal tempU((uint)size());
  clitk::Signal tempV((uint)size());
  for(int i=0; i<size(); i++) {
    tempU[0] = mU[(i+d)%size()];
    tempV[0] = mV[(i+d)%size()];
  }
  for(int i=0; i<size(); i++) {
    mU[i] = tempU[i];
    mV[i] = tempV[i];
  }
}
//---------------------------------------------------------------------        


//---------------------------------------------------------------------        
void clitk::Trajectory2D::GetPoint(const int index, Vector2d & p) {
  p[0] = mU[index];
  p[1] = mV[index];
}
//---------------------------------------------------------------------        


//---------------------------------------------------------------------        
clitk::Trajectory2D & clitk::Trajectory2D::operator+(Trajectory2D & d) {
  for(int i=0; i<size(); i++) {
    mU[i] += d.GetU(i);
    mV[i] += d.GetV(i);
  }
  return  *this;
}
//---------------------------------------------------------------------        


//---------------------------------------------------------------------        
clitk::Trajectory2D & clitk::Trajectory2D::operator-(Trajectory2D & d) {
  for(int i=0; i<size(); i++) {
    mU[i] -= d.GetU(i);
    mV[i] -= d.GetV(i);
  }
  return  *this;
}
//---------------------------------------------------------------------        


//---------------------------------------------------------------------        
void clitk::Trajectory2D::GetMean(Vector2d & m) {
  m[0] = 0.0;
  m[1] = 0.0;
  for(int i=0; i<size(); i++) {
    m[0] += mU[i];
    m[1] += mV[i];
  }
  m[0] /= size();
  m[1] /= size();
}
//---------------------------------------------------------------------        


//---------------------------------------------------------------------        
double clitk::Trajectory2D::DistanceTo(int delta, const clitk::Trajectory2D & B) const {
  double d = 0.0;
  for(int n=0; n<size(); n++) {
    int i = n;//(n)%size();
    int j = (n+delta)%B.size();
    // DD(j);
//     DD(i);
//     DD(n);
    d += pow(mU[i] - B.GetU(j), 2) + pow(mV[i] - B.GetV(j), 2);
  }
  return d;
}
//---------------------------------------------------------------------        


//---------------------------------------------------------------------        
void clitk::Trajectory2D::Print(const std::string & name) const {
  Print(name, 0, size());
}
//---------------------------------------------------------------------        


//---------------------------------------------------------------------        
void clitk::Trajectory2D::Print(const std::string & name, int begin, int end) const {
  std::cout << "Traj " << name << " size = " << size() 
            << " from " << begin << " to " << end << std ::endl;
  for(int i=begin; i<end; i++) {
    std::cout << mU[i] << " " << mV[i] << std::endl;
  }
}
//---------------------------------------------------------------------        


//---------------------------------------------------------------------        
void clitk::Trajectory2D::Substract(const Vector2d & m) {
  mU.AddValue(-m[0]);
  mV.AddValue(-m[1]);
}
//---------------------------------------------------------------------        
