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

