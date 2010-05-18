/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
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
