/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/

#ifndef VVROIACTOR_H
#define VVROIACTOR_H

#include "clitkDicomRT_ROI.h"
#include <QObject>

class vvSlicerManager;
class vvImageContour;
class vvBinaryImageOverlayActor;
class vtkActor;

//------------------------------------------------------------------------------
class vvROIActor: public QObject {
  Q_OBJECT
    public:
  vvROIActor();
  ~vvROIActor();

  void SetROI(clitk::DicomRT_ROI * r);
  clitk::DicomRT_ROI * GetROI() { return mROI; }
  void SetSlicerManager(vvSlicerManager * s);
  void Update();
  void Initialize();
  void SetVisible(bool b);
  void SetContourVisible(bool b);
  bool IsVisible();
  bool IsContourVisible();
  void SetOpacity(double x);
  double GetOpacity();
  void SetSelected(bool b);
  void SetContourWidth(int n);
  int GetContourWidth() { return mContourWidth; }
  void SetContourColor(double r, double v, double b);
  std::vector<double> & GetContourColor();

public slots:
  void UpdateSlice(int slicer, int slices);
  void UpdateColor();
  void UpdateImage();

 protected:
  clitk::DicomRT_ROI * mROI;
  vvSlicerManager * mSlicerManager;
  std::vector<vvImageContour *> mImageContour;
  std::vector<vvBinaryImageOverlayActor *> mOverlayActors;
  
  bool mIsVisible;
  bool mIsContourVisible;
  double mOpacity;
  bool mIsSelected;
  int mContourWidth;
  std::vector<double> mContourColor;

}; // end class vvROIActor
//------------------------------------------------------------------------------

#endif

