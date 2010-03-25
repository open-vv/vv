#ifndef vvInfoPanel_h
#define vvInfoPanel_h
#include <iostream>
#include <vector>

#include "ui_vvInfoPanel.h"

class vvInfoPanel : public QWidget, private Ui::vvInfoPanel
{
    Q_OBJECT

public:
    vvInfoPanel(QWidget * parent=0):QWidget(parent) {
        setupUi(this);
    }
    ~vvInfoPanel() {}

    void setFileName(QString text);
    void setSizeMM(QString text);
    void setOrigin(QString text);
    void setSpacing(QString text);
    void setNPixel(QString text);
    void setDimension(QString text);
    void setSizePixel(QString text);
    void setCurrentInfo(int visibility, double x, double y, double z, double X, double Y, double Z, double value);
    void setViews(int window, int view, int slice);

public slots:

private:

};

#endif
