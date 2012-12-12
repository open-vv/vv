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
#ifndef _vvLandmarksPanel_H
#define _vvLandmarksPanel_H
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
    void LoadFromFile(std::string file);
    void Save();
    void RemoveSelectedPoints();
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

