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
