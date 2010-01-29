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
#include <utility>
#include <cassert>
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QDesktopWidget>

#include "clitkCommon.h"
#include "vvMainWindow.h"
#include "vvInit.h"
#include "vvConstants.h"

//------------------------------------------------------------------------------
// (*NEEDED HERE*) Static instances initialization for ToolManager
TOOL_MANAGER_INITIALIZATION;

//------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    initialize_IO();

    QApplication app( argc, argv );
    Q_INIT_RESOURCE(vvIcons);
    //QPixmap pixmap(":/splashscreen.PNG");
    QSplashScreen *splash = new QSplashScreen(QPixmap(QString::fromUtf8(":/new/prefix1/splashscreen.PNG")));
    /*splash->showMessage("VV 1.0 developped by Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr and CREATIS-LRMN http://www.creatis.insa-lyon.fr",(Qt::AlignRight | Qt::AlignBottom));*/
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
    std::vector<std::pair<int ,std::string> > vector_fields;
    if (argc >1)
    {
        for (int i = 1; i < argc; i++)
        {
            std::string temp = argv[i];
            if (temp=="--vf")
            {
                assert(filenames.size()>=1);
                vector_fields.push_back(std::make_pair(filenames.size()-1,argv[i+1]));
                i++; //skip vf name
            }
            else if (temp=="--overlay")
            {
                assert(filenames.size()>=1);
                overlays.push_back(std::make_pair(filenames.size()-1,argv[i+1]));
                i++; //skip overlay name
            }
            else
                filenames.push_back(temp);
        }
        window.LoadImages(filenames,IMAGE);
        for (std::vector<std::pair<int ,std::string> >::iterator i=overlays.begin();
             i!=overlays.end();i++)
          window.AddOverlayImage((*i).first,(*i).second.c_str());
        for (std::vector<std::pair<int ,std::string> >::iterator i=vector_fields.begin();
             i!=vector_fields.end();i++)
          window.AddField((*i).second.c_str(), (*i).first);
        
    }

    return app.exec();
}
