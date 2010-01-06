#ifndef vvs_h
#define vvs_h

//This is a dummy executable for tests

#include "ui_vvDummyWindow.h"
#include <QMainWindow>

class vvDummyWindow : public QMainWindow, private Ui::vvDummyWindow
{
    Q_OBJECT
public:
    vvDummyWindow();

public slots:
    void Run();
};

#endif
