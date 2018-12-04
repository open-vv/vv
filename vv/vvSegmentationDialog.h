/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
#ifndef _VVSEGMENTATIONDIALOG_H
#define _VVSEGMENTATIONDIALOG_H
#include <vtkVersion.h>
#if (VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 1) || VTK_MAJOR_VERSION >= 9
#include "ui_vvSegmentationDialog.h"
#else
#include "ui_vvSegmentationDialogVTK7.h"
#endif
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

#if QT_VERSION >= 0x050000
#include <QtUiPlugin/QDesignerExportWidget>
#else
#include <QtDesigner/QDesignerExportWidget>
#endif
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
