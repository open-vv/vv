/*=========================================================================

  Program:   vv
  Module:    $RCSfile: vvToolBinarize.h,v $
  Language:  C++
  Date:      $Date: 2010/02/07 08:49:42 $
  Version:   $Revision: 1.4 $
  Author :   David Sarrut (david.sarrut@creatis.insa-lyon.fr)

  Copyright (C) 2010
  Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
  CREATIS                   http://www.creatis.insa-lyon.fr

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  =========================================================================*/

#ifndef VVTOOLBINARIZE_H
#define VVTOOLBINARIZE_H

#include <QtDesigner/QDesignerExportWidget>
#include <QDialog>

#include "vvToolBase.h"
#include "vvImageContour.h"
#include "ui_vvToolBinarize.h"
#include "clitkBinarizeImage_ggo.h"

#include "vtkMarchingSquares.h"
#include "vtkImageClip.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
class vvToolBinarize:
  public QDialog, public vvToolBase<vvToolBinarize>, private Ui::vvToolBinarize 
{
  Q_OBJECT
    public:
  vvToolBinarize(QWidget * parent=0, Qt::WindowFlags f=0);
  ~vvToolBinarize();

  //-----------------------------------------------------
  typedef vvToolBinarize Self;

  //-----------------------------------------------------
  static void Initialize() {
    SetToolName("Binarize");
    SetToolMenuName("Binarize");
    SetToolIconFilename(":/new/prefix1/icons/binarize.png");
    SetToolTip("Image interactive binarization with thresholds.");
  }

  void InputIsSet(bool b);
  void GetArgsInfoFromGUI();

  //-----------------------------------------------------
 // public slots:
//   void InputUpdate();

  public slots:
  void apply();
  // void InputChange(int index);
  void valueChangedT1(double v);
  void valueChangedT2(double v);
  void UpdateSlice(int slicer,int slices);
  void InputIsSelected();
  void enableLowerThan(bool b);
  void useFGBGtoggled(bool);

 protected:
  Ui::vvToolBinarize ui;
  // std::vector<vvSlicerManager*> mSlicerManagers;
//   std::vector<vvSlicerManager*> mSlicerManagersCompatible;
//   vvImage::Pointer mCurrentImage;
//   vvSlicerManager * mCurrentSliceManager;
  args_info_clitkBinarizeImage mArgsInfo;

  vtkImageClip* mClipper;
  vtkMarchingSquares* mSquares1;
  vtkPolyDataMapper* mSquaresMapper1;
  vtkActor* mSquaresActor1;

  vvImageContour * mImageContour;

  /* vtkImageClip* mClipper; */
  /* vtkMarchingSquares* mSquares1; */
  /* vtkPolyDataMapper* mSquaresMapper1; */
  /* vtkActor* mSquaresActor1; */

}; // end class vvToolBinarize
//------------------------------------------------------------------------------

#endif

