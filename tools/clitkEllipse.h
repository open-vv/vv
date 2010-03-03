/*=========================================================================
                                                                                
  Program:   clitk
  Module:    $RCSfile: clitkEllipse.h,v $
  Language:  C++
  Date:      $Date: 2010/03/03 10:47:48 $
  Version:   $Revision: 1.3 $
                                                                                
  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
=========================================================================*/

#ifndef CLITKELLIPSE_H
#define CLITKELLIPSE_H

#include "clitkCommon.h"
#include "clitkSignal.h"
#include "itkVector.h"
#include <vnl/algo/vnl_generalized_eigensystem.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_real_eigensystem.h>

namespace clitk {

  //---------------------------------------------------------------------
  class Ellipse : public itk::Vector<double,6> {
  public:
    typedef itk::Vector<double,2> Vector2d;
    typedef itk::Vector<double,6> Vector6d;
    typedef itk::Matrix<double,6,6> Matrix6x6d;
    typedef itk::Matrix<double,3,3> Matrix3x3d;

    Ellipse();
    Ellipse(const Ellipse & e);
    void InitialiseEllipseFitting(double eta, unsigned int n, clitk::Signal & inputX, clitk::Signal & inputY);
    double EllipseFittingNextIteration();
    Vector2d ComputeCenter();
    Vector2d ComputeSemiAxeLengths();
    double ComputeAngleInRad();
    void SetCenterAndSemiAxes(double x0, double y0, double r1, double r2);
    void Copy(const Vector6d & a);
    double GetEta() { return mEta; }
    void UpdateSMatrix(unsigned int begin, unsigned int n, 
                       clitk::Signal & inputX, clitk::Signal & inputY);

  protected:
    double mEta;
    double & a;
    double & b;
    double & c;
    double & d;
    double & e;
    double & f;
    Matrix6x6d C;
    Matrix6x6d Ct;
    Matrix6x6d W;
    Matrix6x6d Wt;
    Matrix6x6d S;
    Matrix6x6d Sinv;
    Matrix6x6d St;
    std::vector<Vector6d> z;
    clitk::Signal * mInputX;
    clitk::Signal * mInputY;

    void CopyBlock(Matrix6x6d & out, const Matrix6x6d & in, 
                   int ox, int oy, int l, double factor=1.0);
    void CopyBlock(Matrix6x6d & out, const Matrix3x3d & in, 
                   int ox, int oy, double factor=1.0);
    Matrix3x3d GetBlock3x3(const Matrix6x6d & M, int x, int y);

  };
  //---------------------------------------------------------------------


  void ComputeIsoPhaseFromListOfEllipses(std::vector<clitk::Ellipse*> & l, 
                                         clitk::Signal & sx, 
                                         clitk::Signal & sy, 
                                         int nbIsoPhase, 
                                         int delay, 
                                         int L,
                                         clitk::Signal & phase);

    
} // end namespace

#endif
