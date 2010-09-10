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
#include <utility>
#include <cassert>
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QDesktopWidget>

#include "clitkIO.h"
#include "vvMainWindow.h"
#include "vvConstants.h"

void load_image_first_error()
{
  std::cerr << "You need to load an image before adding an overlay!" << std::endl;
  exit(1);
}

//------------------------------------------------------------------------------
int main( int argc, char** argv )
{
  CLITK_INIT;

  QApplication app( argc, argv );
  Q_INIT_RESOURCE(vvIcons);
  //QPixmap pixmap(":/splashscreen.PNG");
  QSplashScreen *splash = new QSplashScreen(QPixmap(QString::fromUtf8(":/new/prefix1/splashscreen.PNG")));
  /*splash->showMessage("VV 1.0 developped by Léon Bérard c`ancer center http://oncora1.lyon.fnclcc.fr and CREATIS-LRMN http://www.creatis.insa-lyon.fr",(Qt::AlignRight | Qt::AlignBottom));*/
  //  splash->show();
  QTimer::singleShot(2000, splash, SLOT(close()));
  while (!splash->isHidden())
    app.processEvents();

  vvMainWindow window;

  //Try to give the window a sensible default size
  int width=QApplication::desktop()->width()*0.8;
  int height=QApplication::desktop()->height()*0.9;
  if (width> 1.5*height)
    width=1.5*height;
  window.resize(width,height);

  window.show();

  std::vector<std::string> sequence_filenames;
  enum {P_NORMAL,P_SEQUENCE};
  int parse_mode=P_NORMAL;
  int n_image_loaded=0;

  if (argc >1) {
    for (int i = 1; i < argc; i++) {
      std::string current = argv[i];
      if (!current.compare(0,2,"--")) { //We are parsing an option
        if (parse_mode == P_SEQUENCE) {//First finish the current sequence
          window.LoadImages(sequence_filenames,MERGEDWITHTIME);
          sequence_filenames.clear();
          parse_mode=P_NORMAL;
        }
        if (current=="--vf") {
          if (!n_image_loaded) load_image_first_error();
          window.AddField(argv[i+1],n_image_loaded-1);
          i++; //skip vf name
        } 
        else if (current=="--overlay") {
            if (!n_image_loaded) load_image_first_error();
            window.AddOverlayImage(n_image_loaded-1,argv[i+1]);
            i++; //skip overlay name
          } 
        else if (current=="--roi") {
            if (!n_image_loaded) load_image_first_error();
            window.AddROI(n_image_loaded-1,argv[i+1]);
            i++; //skip roi name
          } 
        else if (current == "--sequence") {
          n_image_loaded++; //count only one for the sequence
          parse_mode=P_SEQUENCE; }
      }
      else if (parse_mode == P_SEQUENCE)
        sequence_filenames.push_back(current);
      else {
        DD(current);
        std::vector<std::string> image; image.push_back(current);
        window.LoadImages(image,IMAGE);
        n_image_loaded++;
      }
    }
    if (parse_mode == P_SEQUENCE) {//Finish any current sequence
      window.LoadImages(sequence_filenames,MERGEDWITHTIME);
      sequence_filenames.clear();
      parse_mode=P_NORMAL;
    }

  }

  return app.exec();
}
