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

