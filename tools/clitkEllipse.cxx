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

#include "clitkEllipse.h"

typedef itk::Vector<double,2> Vector2d;
typedef itk::Vector<double,6> Vector6d;
typedef itk::Matrix<double,6,6> Matrix6x6d;
typedef itk::Matrix<double,3,3> Matrix3x3d;

//---------------------------------------------------------------------
clitk::Ellipse::Ellipse():a((*this)[0]), b((*this)[1]), 
                          c((*this)[2]), d((*this)[3]), 
                          e((*this)[4]), f((*this)[5]) {
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
clitk::Ellipse::Ellipse(const Ellipse & e):a((*this)[0]), b((*this)[1]), 
                                           c((*this)[2]), d((*this)[3]), 
                                           e((*this)[4]), f((*this)[5]) {
  for(int i=0; i<7; i++) (*this)[i] = e[i];
}

//---------------------------------------------------------------------


//---------------------------------------------------------------------
Vector2d clitk::Ellipse::ComputeCenter() {
  // See http://mathworld.wolfram.com/Ellipse.html
  // see Ruan 2008
  Vector2d center;
  center[0] = (2*c*d - b*e)/(b*b-4*a*c);
  center[1] = (2*a*e - b*d)/(b*b-4*a*c);
  return center;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
Vector2d clitk::Ellipse::ComputeSemiAxeLengths() {
  // See http://www.geometrictools.com/Documentation/InformationAboutEllipses.pdf
  Vector2d axis;
  Vector2d center = ComputeCenter();
  double & k1 = center[0];
  double & k2 = center[1];
  double mu = 1.0/(a*k1*k1 + b*k1*k2 + c*k2*k2 - f);
  double m11 = mu * a;
  double m12 = mu * 0.5 * b;
  double m22 = mu * c;
  double l1 = ( (m11+m22) + sqrt((m11-m22)*(m11-m22)+4*m12*m12) )/2.0;
  assert(l1>0.0);
  axis[1] = 1.0/sqrt(l1);
  double l2 = ((m11+m22)-sqrt((m11-m22)*(m11-m22)+4*m12*m12))/2.0;
  assert(l2>0.0);
  axis[0] = 1.0/sqrt(l2);
  return axis;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
double clitk::Ellipse::ComputeAngleInRad() {
  // See http://www.geometrictools.com/Documentation/InformationAboutEllipses.pdf
  double theta;
  if (b==0) {
    if (a<c) { theta = 0; }
    else     { theta = 0.5*M_PI; }      
  }
  else {
    if (a<c) {
      theta = 0.5*atan(-b/(c-a));
    }
    else {
      theta = M_PI/2+0.5*atan(-b/(c-a));
    }
  }
  return theta;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::Ellipse::InitialiseEllipseFitting(double eta, unsigned int n, clitk::Signal & inputX, clitk::Signal & inputY) {
  // Store data
  mEta = eta;
  mInputX = &inputX;
  mInputY = &inputY;

  // Initialise ellipse to global fit
  std::vector<double> extremaX(2);
  std::vector<double> extremaY(2);
  double x0 = 0.0;
  double y0 = 0.0;
  extremaX[0] = extremaY[0] = numeric_limits<double>::max();
  extremaX[1] = extremaY[1] = -numeric_limits<double>::max();
  for(unsigned int i=0; i<n; i++) {
    if (inputX[i] < extremaX[0]) extremaX[0] = inputX[i];
    if (inputX[i] > extremaX[1]) extremaX[1] = inputX[i];
    if (inputY[i] < extremaY[0]) extremaY[0] = inputY[i];
    if (inputY[i] > extremaY[1]) extremaY[1] = inputY[i];
    x0 += inputX[i];
    y0 += inputY[i];
  }
  x0 /= n;
  y0 /= n;

  // initialisation with an ellipse more small than real points extends
  double ax1 = (extremaX[1]-extremaX[0])/2.0;
  double ax2 = (extremaY[1]-extremaY[0])/2.0;
  if (ax2 >= ax1) ax2 = 0.99*ax1;
  SetCenterAndSemiAxes(x0, y0, ax1, ax2);

  // Initialisation of C
  C.Fill(0.0);
  C(0,0) = 0; C(0,1) = 0;  C(0,2) = 2;
  C(1,0) = 0; C(1,1) = -1; C(1,2) = 0;
  C(2,0) = 2; C(2,1) = 0;  C(2,2) = 0;
  Ct = C.GetVnlMatrix().transpose();
  
  // Compute initial S
  UpdateSMatrix(0,n, inputX, inputY);
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::Ellipse::CopyBlock(Matrix6x6d & out, const Matrix6x6d & in, 
                               int ox, int oy, int l, double factor) {
  for(int x=ox; x<ox+l; x++)
    for(int y=oy; y<oy+l; y++) {
      out(x,y) = factor * in(x,y);
    }
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::Ellipse::CopyBlock(Matrix6x6d & out, const Matrix3x3d & in, 
                               int ox, int oy, double factor) {
  for(int x=0; x<3; x++)
    for(int y=0; y<3; y++) {
      out(ox+x,oy+y) = factor * in(x,y);
    }
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
Matrix3x3d clitk::Ellipse::GetBlock3x3(const Matrix6x6d & M, int x, int y) {
  Matrix3x3d B;
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) {
      B(i,j) = M(x+i,y+j);
    }
  }
  return B;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
double clitk::Ellipse::EllipseFittingNextIteration() {  
  Vector6d & current = (*this);

  // Normalize current vector. This allow to have a decreasing
  // residual r (no need to optimize)
  GetVnlVector().normalize();
  double r = (St*current)*current;

  // Temporary parameters
  Vector6d an;

  // Iterative update
  an = Sinv * C * current;
  double num = (Wt*current)*current; // anT W an = (WT an) an
  double denom = (Ct*current)*current;
  an = (mEta * num/denom) * an;
  an += (1.0-mEta)*current;
  SetVnlVector(an.GetVnlVector());

  // return residual
  return r;
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::Ellipse::SetCenterAndSemiAxes(double x0, double y0, 
                                          double r1, double r2) {
  a = 1.0/(r1*r1);
  b = 0;
  c = 1.0/(r2*r2);
  if (a>c) {
    std::cerr << "Error major axis should be r1, not r2 (r1=" << r1
              << " r2=" << r2 << ")" << std::endl;
    exit(0);
  }
  d = (-2.0*x0)/(r1*r1);
  e = (-2.0*y0)/(r2*r2); 
  f = (x0*x0)/(r1*r1) + (y0*y0)/(r2*r2) - 1.0;
  GetVnlVector().normalize();
}
//---------------------------------------------------------------------


//---------------------------------------------------------------------
void clitk::Ellipse::UpdateSMatrix(unsigned int begin, unsigned int n, 
                                   clitk::Signal & inputX, clitk::Signal & inputY) {
  // Initialisation of z
  z.resize(n);
  int j = 0;
  for(unsigned int i=begin; i<begin+n; i++) {
    z[j][0] = inputX[i]*inputX[i];
    z[j][1] = inputX[i]*inputY[i];
    z[j][2] = inputY[i]*inputY[i];
    z[j][3] = inputX[i];
    z[j][4] = inputY[i];
    z[j][5] = 1;
    j++;
  }

  // Initialisation of S
  S.Fill(0.0);
  j = 0;
  for(unsigned int i=begin; i<begin+n; i++) {
    for(unsigned int x=0; x<6; x++)
      for(unsigned int y=0; y<6; y++) 
        S(x,y) += z[j][x]*z[j][y];
    j++;
  }
  Sinv = S.GetInverse();
  St = S.GetVnlMatrix().transpose();
  
  // Initialisation of W
  W.Fill(0.0);
  CopyBlock(W, S, 0, 0, 3);
  CopyBlock(W, S, 3, 3, 3, -1);
  Wt = W.GetVnlMatrix().transpose();

  // Automated computation of mEta
  if (mEta<0) {
    Matrix3x3d E  = GetBlock3x3(S, 0, 0);
    Matrix3x3d B  = GetBlock3x3(S, 0, 3);
    Matrix3x3d Bt = GetBlock3x3(S, 3, 0);
    Matrix3x3d D  = GetBlock3x3(S, 3, 3);
    Matrix3x3d Dinv(D.GetInverse());    
    Matrix3x3d Stilde;
    Stilde = E - B*Dinv*Bt;
    
    Matrix3x3d Ctilde;
    Ctilde(0,0) = 0; Ctilde(0,1) = 0;  Ctilde(0,2) = 2;
    Ctilde(1,0) = 0; Ctilde(1,1) = -1; Ctilde(1,2) = 0;
    Ctilde(2,0) = 2; Ctilde(2,1) = 0;  Ctilde(2,2) = 0;
    
    // Ctilde is not inonneg-definite, so vnl print error on std cerr
    // the following "disableStdCerr" disable it ...
    disableStdCerr();
    vnl_generalized_eigensystem solver(Stilde.GetVnlMatrix(), Ctilde.GetVnlMatrix());
    //vnl_generalized_eigensystem solver(Ctilde.GetVnlMatrix(), Stilde.GetVnlMatrix());
    enableStdCerr();
    double dmax=0.0, dmin=9999999.9;
    dmax = solver.D(2);
    dmin = solver.D(1);
    mEta = 2.0/(fabs(dmax/dmin)+1);
    assert(mEta<1.0);
  }  
}
//---------------------------------------------------------------------
