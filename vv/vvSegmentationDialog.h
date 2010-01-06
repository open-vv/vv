#ifndef _VVSEGMENTATIONDIALOG_H
#define _VVSEGMENTATIONDIALOG_H

/*=========================================================================

Program:   vv
Module:    $RCSfile: vvSegmentationDialog.h,v $
Language:  C++
Date:      $Date: 2010/01/06 13:31:58 $
Version:   $Revision: 1.1 $
Author :   David Sarrut (david.sarrut@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

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


#include "ui_vvSegmentationDialog.h"
#include "clitkCommon.h"
#include "vvImage.h"
#include "vvSlicerManager.h"
#include "vtkMarchingSquares.h"
#include "vtkImageClip.h"
#include "vtkClipPolyData.h"
#include "vtkImageToPolyDataFilter.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageActor.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyData.h"
#include <vtkPolyDataMapper.h>

#include <QtDesigner/QDesignerExportWidget>
#include <QTreeWidget>

//====================================================================
class vvSegmentationDialog : public QDialog, private Ui::vvSegmentationDialog {

    Q_OBJECT

public:
    // constructor - destructor
    vvSegmentationDialog(QWidget * parent=0, Qt::WindowFlags f=0);
    ~vvSegmentationDialog();
    void SetImage(vvImage::Pointer image);

public slots:
    void clippingvaluechanged(int);
    void UpdateSlice(int slicer,int slice);
    void BinariseSurface();
    void Erode();
    void Dilate();
    void Save();
    void ChangeDimRendering();
    void InsertSeed();
    void KernelValueChanged(int kernel);

protected:
    Ui::vvSegmentationDialog ui;

    vvSlicerManager* mManager;

    vtkImageClip* mClipper;

    vtkMarchingSquares* mSquares1;
    vtkPolyDataMapper* mSquaresMapper1;
    vtkActor* mSquaresActor1;

    vtkMarchingSquares* mSquares2;
    vtkPolyDataMapper* mSquaresMapper2;
    vtkActor* mSquaresActor2;

    std::vector<vtkPolyDataMapper*> m3DMappers;
    //vtkActor* m3DActor;
    vtkMarchingCubes* m3DExtractor;
    std::vector<vtkActor*> m3DActors;

    int mKernelValue;

    std::vector<vtkImageData*> mBinaireImages;

}; // end class vvSegmentationDialog
//====================================================================

#endif /* end #define _VVSEGMENTATIONDIALOG_H */
