#ifndef _vvSurfaceViewerDialog_H
#define _vvSurfaceViewerDialog_H

/*=========================================================================

Program:   vv
Language:  C++
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


#include "ui_vvSurfaceViewerDialog.h"

class vtkPolyDataMapper;
class vtkActor;
class vtkOBJReader;

#include <QtDesigner/QDesignerExportWidget>
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
    unsigned int mCurrentTime;

}; // end class vvSurfaceViewerDialog
//====================================================================

#endif /* end #define _vvSurfaceViewerDialog_H */
