/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)
 Program:   vv

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
#ifndef vvMidPosition_h
#define vvMidPosition_h

#include <string>
#include <QThread>
#include <QProgressDialog>
#include "vvImage.h"
#include "vvSlicerManager.h"

class vvMidPosition : public QThread
{
public:
    vvMidPosition();

    ///Call this to trigger the computation in a separate thread and wait until it's done
    void Update();
    vvSlicerManager * slicer_manager;
    ///True if there is an error during the computation
    bool error;
    std::string error_message;
    vvImage::Pointer output;
    int reference_image_index;
protected:
    void run();
    ///This is called everytime the main thread wakes up, and updates a progress bar
    void update_progress();
    QProgressDialog p_bar;
    ///Counter for the progress bar. We should use a mutex, but it turns out it isn't needed
    int progress;
};

#endif
