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

  std::vector<std::string> filenames;
  std::vector<std::pair<int ,std::string> > overlays;
  std::vector<std::pair<int ,std::string> > rois;
  std::vector<std::pair<int ,std::string> > vector_fields;
  if (argc >1) {
    for (int i = 1; i < argc; i++) {
      std::string temp = argv[i];
      if (temp=="--vf") {
        assert(filenames.size()>=1);
        vector_fields.push_back(std::make_pair(filenames.size()-1,argv[i+1]));
        i++; //skip vf name
      } 
      else {
	if (temp=="--overlay") {
	  assert(filenames.size()>=1);
	  overlays.push_back(std::make_pair(filenames.size()-1,argv[i+1]));
	  i++; //skip overlay name
	} 
	else {
	  if (temp=="--roi") {
	    assert(filenames.size()>=1);
	    rois.push_back(std::make_pair(filenames.size()-1,argv[i+1]));
	    i++; //skip overlay name
	  } 
	  else {
	    filenames.push_back(temp);
	  }
	}
      }
    }
    window.LoadImages(filenames,IMAGE);
    for (std::vector<std::pair<int ,std::string> >::iterator i=overlays.begin();
         i!=overlays.end(); i++)
      window.AddOverlayImage((*i).first,(*i).second.c_str());
    for (std::vector<std::pair<int ,std::string> >::iterator i=vector_fields.begin();
         i!=vector_fields.end(); i++)
      window.AddField((*i).second.c_str(), (*i).first);
    for (std::vector<std::pair<int ,std::string> >::iterator i=rois.begin();
         i!=rois.end(); i++) {
      DD((*i).second.c_str());
      DD((*i).first);
      // window.AddROI((*i).second.c_str(), (*i).first);
    }

  }

  return app.exec();
}
