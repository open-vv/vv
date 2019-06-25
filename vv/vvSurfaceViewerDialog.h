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
#ifndef _vvSurfaceViewerDialog_H
#define _vvSurfaceViewerDialog_H
#include <vtkVersion.h>
#if (VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 2) || VTK_MAJOR_VERSION >= 9
#include "ui_vvSurfaceViewerDialog.h"
#else
#include "ui_vvSurfaceViewerDialogVTK7.h"
#endif

class vtkPolyDataMapper;
class vtkActor;
class vtkOBJReader;

//#if QT_VERSION >= 0x050000
//#include <QtUiPlugin/QDesignerExportWidget>
//#else
//#include <QtDesigner/QDesignerExportWidget>
//#endif
#include <QTreeWidget>

//====================================================================
class vvSurfaceViewerDialog : public QDialog, private Ui::vvSurfaceViewerDialog {

    Q_OBJECT

public:
    // constructor - destructor
    vvSurfaceViewerDialog(QWidget * parent=0, Qt::WindowFlags f=0);
    ~vvSurfaceViewerDialog();
    void NextTime();
    void PreviousTime();

public slots :
    void LoadSurface();

private :
    std::vector<vtkOBJReader*> mReaders;
    vtkPolyDataMapper* mMapper;
    vtkActor* mActor;
    vtkRenderer * mRenderer;
    int mCurrentTime;

}; // end class vvSurfaceViewerDialog
//====================================================================

#endif /* end #define _vvSurfaceViewerDialog_H */
