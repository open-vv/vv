#include <QApplication>

#include "vvImageReader.h"
#include "vvMeshReader.h"
#include "vvs.h"


int main( int argc, char** argv )
{
    initialize_IO();
    QApplication app(argc,argv);
    vvDummyWindow w;
    w.show();
    return app.exec();
}

vvDummyWindow::vvDummyWindow()
{
    setupUi(this);
}

void vvDummyWindow::Run()
{
    vvImageReader imr;
    imr.SetInputFilename("CT_UNTAGGED2MM_0.mhd");
    imr.Update(IMAGE);

    vvMeshReader r;
    r.SetImage(imr.GetOutput());
    r.SetFilename("struct.DCM");
    std::vector<int> selected;
    selected.push_back(1);
    r.SetSelectedItems(selected);
    r.Update();
}
