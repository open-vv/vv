#ifndef VV_DEFORMABLE_REGISTRATION
#define VV_DEFORMABLE_REGISTRATION
#include <QMutex>
#include <QString>
class string;
class QThread;
class vvImage;
#include "vvImage.h"

class vvDeformableRegistration : public QThread
{
public:
    vvDeformableRegistration(vvImage::Pointer image,unsigned int ref,\
                             unsigned int iter,  unsigned int nthread,double a, double s,\
                             std::string output_f,unsigned int stop);
    unsigned int getProgress();
    void abort();
    QMutex progress_mutex;
    void run();
    vvImage::Pointer getOutput() {
        return mOutput;
    }
protected:
    vvImage::Pointer mImage;
    vvImage::Pointer  mOutput;
    unsigned int refimage,nb_iter,n_thread,progress,stop;
    double alpha,sigma;
    std::string output_filename;
    bool aborted;
    void cleanup(int);
    void partial_run(int,int,int,std::string);
    ///Temporary path, specific to the current instance of vv
    QString mTempPath;
};


#endif

