#ifndef _vvLandmarksPanel_H
#define _vvLandmarksPanel_H

/*=========================================================================

 Program:   vv
 Module:    $RCSfile: vvLandmarksPanel.h,v $
 Language:  C++
 Date:      $Date: 2010/01/06 13:31:57 $
 Version:   $Revision: 1.1 $
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 200COLUMN_IMAGE_NAME
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

#include <iostream>
#include <vector>
#include "vvLandmarks.h"

#include "ui_vvLandmarksPanel.h"

//====================================================================
class vvLandmarksPanel : public QWidget, private Ui::vvLandmarksPanel
{

    Q_OBJECT

public:
    // constructor - destructor
    vvLandmarksPanel(QWidget * parent=0);
    ~vvLandmarksPanel() {}
    void SetCurrentLandmarks(vvLandmarks *lm, int time);
    void SetCurrentPath(std::string path) {
        mCurrentPath = path;
    }
    void SetCurrentImage(std::string filename);

public slots:
    void Load();
    void Save();
    void RemoveLastPoint();
    void AddPoint();
    void CommentsChanged(int row, int column);
signals:
    void UpdateRenderWindows();

private:
    void AddPoint(int);
    vvLandmarks* mCurrentLandmarks;
    std::string mCurrentPath;
}; // end class vvLandmarksPanel
//====================================================================

#endif /* end #define _vvLandmarksPanel_H */

