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
#include <sstream>
#include <cstdlib>
#include <string>
#include <ctime>

#include <QThread>
#include <QApplication>
#include <QDir>
#include <QMutexLocker>

#include "vtkVOXImageWriter.h"
#include <vtkImageData.h>

#include "clitkCommon.h"
#include "vvSlicerManager.h"
#include "vvDeformableRegistration.h"
#include "vvImage.h"
#include "vvImage.h"
#include "vvImageReader.h"

vvDeformableRegistration::vvDeformableRegistration(vvImage::Pointer image,unsigned int ref,\
    unsigned int iter,  unsigned int nthread,double a, double s,std::string output_f,unsigned int stop) :
  mImage(image),
  refimage(ref),
  nb_iter(iter),
  n_thread(nthread),
  progress(0),
  stop(stop),
  alpha(a),
  sigma(s),
  output_filename(output_f),
  aborted(false)
{
}

void vvDeformableRegistration::abort()
{
  aborted=true;
  std::system("killall deformableregistration");
  std::system("killall clitkVFMerge");
}

unsigned int vvDeformableRegistration::getProgress()
{
  QMutexLocker locker(&progress_mutex);
  unsigned int retval=progress;
  return retval;
}

void vvDeformableRegistration::cleanup(int frame_number) //remove temporary files
{
  std::string temp_dir=mTempPath.toStdString();
  for (int i=0; i<frame_number; i++) {
    std::stringstream filename;
    filename << temp_dir << "/deformation_" << i << ".vf";
    std::system((std::string("rm ") + filename.str()).c_str());
  }
  for (int i=0; i<frame_number; i++) {
    std::stringstream filename;
    filename << temp_dir << "/temp_" << i << ".vox";
    std::system(("rm " + filename.str()).c_str());
  }
  std::stringstream filename;
  filename << temp_dir;
  std::system(("rm -r " + filename.str()).c_str());
}

void vvDeformableRegistration::partial_run(int low_index,int high_index,int refimage,std::string ref_file)
{
  std::string temp_dir=mTempPath.toStdString();
  DD(ref_file);
  DD(low_index);
  DD(high_index);
  for (int i=low_index; i<high_index; i++) {
    if (aborted)
      break;
    if (i==refimage)
      continue;
    std::stringstream filename;
    std::stringstream registration_command;
    filename << temp_dir << "/temp_" << i << ".vox";
    std::stringstream output_vf;
    output_vf << temp_dir << "/deformation_" << i << ".vf";
    registration_command << "deformableregistration -r " << ref_file
                         << " -d " << filename.str() << " -o " << output_vf.str()
                         << " --alpha=" << alpha
                         << " --sigma=" << sigma
                         << " --stop=" << stop
                         << " --iter=" << nb_iter;
    if (i>low_index && i-1 != refimage) { //if possible, use the previous computations to speed up the process
      std::stringstream old_vf;
      old_vf << temp_dir << "/deformation_" << i-1 << ".vf";
      registration_command << " --vf=" << old_vf.str();
    }
    DD(registration_command.str());
    std::system(registration_command.str().c_str());
    progress_mutex.lock();
    progress++;
    progress_mutex.unlock();
  }
}

void vvDeformableRegistration::run()
{
  clock_t start,finish;
  start=clock();
  vtkVOXImageWriter * vox = vtkVOXImageWriter::New();
  std::stringstream command;
  std::string ref_file;
  mTempPath=QDir::tempPath()+QString("/vv-")+QString::number(qApp->applicationPid());
  QDir temp_qt_dir;
  DD(temp_qt_dir.mkpath(mTempPath));
  std::string temp_dir=mTempPath.toStdString();
  DD(temp_dir);
  std::vector<vtkImageData*> images=mImage->GetVTKImages();
  for (unsigned int i=0; i<images.size(); i++) {
    std::stringstream filename;
    filename << temp_dir << "/temp_" << i << ".vox";
    vox->SetInput(images[i]);
    vox->SetFileName(filename.str().c_str());
    if (i==refimage)
      ref_file=filename.str();
    vox->Write();
  }
  vox->Delete();
  progress++;
  int reg_per_thread=static_cast<int>(images.size()-1)/n_thread;
  int remainder=static_cast<int>(images.size()-1) - reg_per_thread*n_thread;
#pragma omp parallel for num_threads(n_thread) schedule(static)
  for (int i=0; i<static_cast<int>(n_thread); i++) {
    ///Static scheduling
    int remainder_shift=((i<remainder)?i:remainder);
    unsigned int start_image=i*reg_per_thread+remainder_shift;
    unsigned int end_image=((i+1)*reg_per_thread+remainder_shift+((i<remainder)?1:0));
    if (end_image<=refimage)
      partial_run(start_image,end_image,refimage,ref_file);
    else if (start_image<=refimage)
      partial_run(start_image,end_image+1,refimage,ref_file);
    else
      partial_run(start_image+1,end_image+1,refimage,ref_file);
  }
  if (aborted) {
    cleanup(images.size());
    return;
  }
  command.str("");
  int computed_vf=(refimage==0)?1:0; //index of an image that isn't the reference image
  command << "clitkZeroVF -i " << temp_dir << "/deformation_" << computed_vf << ".vf -o "  << temp_dir <<
          "/deformation_" << refimage << ".vf";
  DD(command.str()); //create zero VF for the ref image
  std::system(command.str().c_str());
  command.str("");
  command << "clitkVFMerge ";
  for (unsigned int i=0; i<images.size(); i++) command << temp_dir << "/deformation_" << i << ".vf ";
  command << " --xorigin " << images[0]->GetOrigin()[0];
  command << " --yorigin " << images[0]->GetOrigin()[1];
  command << " --zorigin " << images[0]->GetOrigin()[2];
  command << " -o " << output_filename << std::endl;
  DD(command.str());
  std::system(command.str().c_str());
  cleanup(images.size());
  if (aborted) {
    std::system(("rm " + output_filename).c_str());
    return;
  }
  vvImageReader::Pointer reader = vvImageReader::New();
  reader->SetInputFilename(output_filename);
  reader->Update(VECTORFIELD);
  finish=clock();
  DD((finish - start)/static_cast<double>(CLOCKS_PER_SEC));
  mOutput = reader->GetOutput();
}
