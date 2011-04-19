/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/
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
