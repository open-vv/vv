/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

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
