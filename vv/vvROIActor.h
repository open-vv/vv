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

  void SetROI(const clitk::DicomRT_ROI * r);
  void SetSlicerManager(vvSlicerManager * s);
  void Update();
  void Initialize();
  void SetVisible(bool b);
  bool IsVisible();
  void SetOpacity(double x);

public slots:
  void UpdateSlice(int slicer, int slices);

 protected:
  const clitk::DicomRT_ROI * mROI;
  vvSlicerManager * mSlicerManager;
  std::vector<vvImageContour *> mImageContour;
  std::vector<vvBinaryImageOverlayActor *> mOverlayActors;
  
  bool mIsVisible;
  double mOpacity;

}; // end class vvROIActor
//------------------------------------------------------------------------------

#endif

