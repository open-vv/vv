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
#include <utility>
#include <cassert>
#include <ctime>
#include <string>
#include <ostream>
#include <sstream>
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QDesktopWidget>
#include <QDir>

#if VTK_MAJOR_VERSION > 5
#include <vtkAutoInit.h>
 VTK_MODULE_INIT(vtkInteractionStyle);
 VTK_MODULE_INIT(vtkRenderingOpenGL);
 VTK_MODULE_INIT(vtkRenderingFreeType);
#define vtkRenderingContext2D_AUTOINIT 1(vtkRenderingContextOpenGL)
#endif


#include "clitkIO.h"
#include "vvMainWindow.h"
#include "vvReadState.h"
#include "vvToolsList.h"
#include "vvConfiguration.h"
#if (VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 2) || VTK_MAJOR_VERSION >= 9
#include <QVTKOpenGLWidget.h>
#endif

#include <vtkFileOutputWindow.h>
#include <vtkSmartPointer.h>

#include <itkFileOutputWindow.h>
#include <itkSmartPointer.h>
#include <itksys/SystemTools.hxx>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#pragma comment(lib, "ws2_32.lib")

typedef enum {O_BASE,O_OVERLAY,O_FUSION,O_VF,O_CONTOUR,O_LANDMARKS} OpenModeType;
typedef enum {P_NORMAL,P_SEQUENCE,P_WINDOW,P_LEVEL} ParseModeType;

void load_image_first_error()
{
  std::cerr << "You need to load an image before adding an overlay!" << std::endl;
  exit(1);
}

std::string create_timed_string()
{
  time_t t;
  time(&t);

  struct tm* pt = localtime(&t);

  const int size = 64;
  char st[size];
  strftime(st, size, "%Y%m%d-%H%M%S", pt);

  return st;
}

void open_sequence(vvMainWindow &window,
                   OpenModeType &open_mode,
                   ParseModeType &parse_mode,
                   std::vector<std::string> &sequence_filenames,
                   int n_image_loaded)
{
  const std::string open_mode_names[] = {"base", "overlay", "fusion", "vf", "contour", "fusionSequence"};
  if(open_mode==O_BASE)
    window.LoadImages(sequence_filenames, vvImageReader::MERGEDWITHTIME);
  else if (open_mode==O_OVERLAY && window.CheckAddedImage(n_image_loaded-1, "overlay"))
    window.AddOverlayImage(n_image_loaded-1,sequence_filenames,vvImageReader::MERGEDWITHTIME);
  else if (open_mode==O_LANDMARKS)
    window.AddLandmarks(n_image_loaded-1,sequence_filenames);
  else if (open_mode==O_FUSION && window.CheckAddedImage(n_image_loaded-1, "fusion") && window.CheckAddedImage(n_image_loaded-1, "fusionSequence"))
    window.AddFusionImage(n_image_loaded-1,sequence_filenames,vvImageReader::MERGEDWITHTIME);
  else {
    std::cerr << "Sequences are not managed for opening " << open_mode_names[open_mode] << std::endl;
    exit(1);
  }

  // Reset
  sequence_filenames.clear();
  parse_mode=P_NORMAL;
  open_mode=O_BASE;
}

//------------------------------------------------------------------------------
#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  int argc = __argc;
  char **argv = __argv;
#else
int main( int argc, char** argv )
{
#endif

#if (VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 2) || VTK_MAJOR_VERSION >= 9
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
#endif
  CLITK_INIT;

  QApplication app( argc, argv );
  Q_INIT_RESOURCE(vvIcons);
  
  // 
  // ATTENTION: Rômulo Pinho - 05/08/2011
  // Forcing the locale of the application is necessary
  // because QT initialization changes it to the locale
  // of the language of the system. This can cause 
  // inconsistencies when, e.g., reading float values
  // from DICOM fields with gdcm, since the decimal
  // point may be changed for a comma (as in French).
  // In practice, functions such as scanf and its
  // variations are directly affected.
  // https://bugreports.qt.nokia.com//browse/QTBUG-15247?page=com.atlassian.jira.plugin.system.issuetabpanels%253Achangehistory-tabpanel
  //
#ifndef _WIN32
  std::string old_locale = setlocale(LC_NUMERIC, NULL);
  setlocale(LC_NUMERIC, "POSIX");
#endif

  vvMainWindow window;

  //Try to give the window a sensible default size
  int width=QApplication::desktop()->width()*0.8;
  int height=QApplication::desktop()->height()*0.9;
  if (width> 1.5*height)
    width=1.5*height;
  window.resize(width,height);

  window.show();

  std::vector<std::string> sequence_filenames;
  ParseModeType parse_mode = P_NORMAL;
  OpenModeType open_mode = O_BASE;
  int n_image_loaded=0;
  std::string win(""), lev("");

  int first_of_wl_set = -1;
  bool new_wl_set = false;
	bool link_images = false;
  if (argc >1) {
    for (int i = 1; i < argc; i++) {
      std::string current = argv[i];
      if (!current.compare(0,1,"-")) { // && !current.compare(0,2,"--")) { //We are parsing an option
        if (parse_mode == P_SEQUENCE) {//First finish the current sequence
          open_sequence(window, open_mode, parse_mode, sequence_filenames, n_image_loaded);
        } 
        else if (parse_mode == P_WINDOW) { // handle negative window values
          win=current;
          window.ApplyWindowToSetOfImages(atof(win.c_str()), first_of_wl_set, n_image_loaded-1);
          parse_mode=P_NORMAL;
          new_wl_set = false;
          continue;
        } else if (parse_mode == P_LEVEL) { // handle negative level values
          lev=current;
          window.ApplyLevelToSetOfImages(atof(lev.c_str()), first_of_wl_set, n_image_loaded-1);
          parse_mode=P_NORMAL;
          new_wl_set = false;
          continue;
        }
        if ((current=="--help") || (current=="-h")) {
          std::cout << "vv " << VV_VERSION << ", the 2D, 2D+t, 3D and 3D+t (or 4D) image viewer" << std::endl << std::endl
                    << "Synopsis: vv file(s) [OPTIONS] file(s)" << std::endl << std::endl
                    << "Open file(s) for visualization." << std::endl << std::endl
                    << "OPTIONS may be:" << std::endl
                    << "--help         \t Print command line help and exit." << std::endl
                    << "--window number\t Gray scale window width for set of images appearing before in the cmd line (may appear more than once)." << std::endl
                    << "--level number \t Gray scale window level for set of images appearing before in the cmd line (may appear more than once)." << std::endl
                    << "--linkall      \t Link pan, zoom and spatial position of crosshair in images." << std::endl
                    << "--log          \t Log output messages in vv-log directory." << std::endl
                    << "--state file   \t Read display parameters from file." << std::endl
                    << "--sequence file\t Read all file(s) until next option in a single temporal sequence." << std::endl
                    << std::endl
                    << "These last options must follow a file name since they overlay something on an image:" << std::endl
                    << "--vf file      \t Overlay the vector field in file." << std::endl
                    << "--overlay [--sequence] file(s) \t Overlay the image in file with complementary colors." << std::endl
                    << "--fusion [--sequence] file(s)  \t Overlay the image in file with alpha blending and colormap." << std::endl
                    //<< "--roi file     \t Overlay binary mask images. Option may be repeated on a single base image." << std::endl
                    << "--contour file \t Overlay DICOM RT-STRUCT contours." << std::endl
                    << "--landmarks [--sequence] file(s)  \t Overlay the landmarks in file(s) (.txt or .pts)." << std::endl;
          exit(0);
        } else if (current=="--vf") {
          if (!n_image_loaded) load_image_first_error();
          open_mode = O_VF;
        } else if (current=="--overlay") {
          if (!n_image_loaded) load_image_first_error();
          open_mode = O_OVERLAY;
        } else if (current=="--contour") {
          if (!n_image_loaded) load_image_first_error();
          open_mode = O_CONTOUR;
        } else if (current=="--fusion") {
          if (!n_image_loaded) load_image_first_error();
          open_mode = O_FUSION;
        } else if (current=="--landmarks") {
          if (!n_image_loaded) load_image_first_error();
          open_mode = O_LANDMARKS;
        } else if (current == "--sequence") {
          if(open_mode==O_BASE) n_image_loaded++; //count only one for the whole sequence
          parse_mode=P_SEQUENCE;
          if (!new_wl_set) {
            new_wl_set = true;
            first_of_wl_set = n_image_loaded-1;
          }
        } else if (current == "--window") {
          parse_mode=P_WINDOW;
        } else if (current == "--level") {
          parse_mode=P_LEVEL;
        } else if (current == "--linkall") {
          link_images = true;
        }
        else if (current == "--log") {
          std::string log_dir = QDir::tempPath().toStdString() + std::string("/vv-log");

          if(itksys::SystemTools::FileExists(log_dir.c_str()) &&
              !itksys::SystemTools::FileIsDirectory(log_dir.c_str())) {
            std::cerr << "Error creating log directory, file exists and is not a directory." << std::endl;
            exit(1);
          } else if(!itksys::SystemTools::MakeDirectory(log_dir.c_str())) {
            std::cerr << "Error creating log directory." << std::endl;
            exit(1);
          }

          std::string log_file = log_dir + "/" + create_timed_string() + ".log";

          itk::SmartPointer<itk::FileOutputWindow> itk_log = itk::FileOutputWindow::New();
          itk_log->SetFileName(log_file.c_str());
          itk_log->FlushOn();
          itk_log->AppendOn();
          itk::OutputWindow::SetInstance(itk_log);

          vtkSmartPointer<vtkFileOutputWindow> vtk_log = vtkFileOutputWindow::New();
          vtk_log->SetFileName(log_file.c_str());
          vtk_log->FlushOn();
          vtk_log->AppendOn();
          vtkOutputWindow::SetInstance(vtk_log);
        } else if (current == "--state") {
          //window.ReadSavedStateFile(argv[i+1]);
          vvReadState read_state;
          read_state.Run(&window, argv[i+1]);
          n_image_loaded += read_state.GetNumberOfImages();
          i++;
        }
        
      } else if (parse_mode == P_SEQUENCE) {
        sequence_filenames.push_back(current);
      } else if (parse_mode == P_WINDOW) {
        win=current;
        window.ApplyWindowToSetOfImages(atof(win.c_str()), first_of_wl_set, n_image_loaded-1);
        parse_mode=P_NORMAL;
        new_wl_set = false;
      } else if (parse_mode == P_LEVEL) {
        lev=current;
        window.ApplyLevelToSetOfImages(atof(lev.c_str()), first_of_wl_set, n_image_loaded-1);
        parse_mode=P_NORMAL;
        new_wl_set = false;
      } else {
        std::vector<std::string> image;
        image.push_back(current);
        if(open_mode==O_BASE) {
          window.LoadImages(image, vvImageReader::IMAGE);
          n_image_loaded++;
          if (!new_wl_set) {
            new_wl_set = true;
            first_of_wl_set = n_image_loaded-1;
          }
        }
        else if (open_mode==O_VF && window.CheckAddedImage(n_image_loaded-1, "vector"))
          window.AddField(current.c_str(), n_image_loaded-1);
        else if (open_mode==O_OVERLAY && window.CheckAddedImage(n_image_loaded-1, "overlay"))
          window.AddOverlayImage(n_image_loaded-1,image,vvImageReader::IMAGE);
        else if (open_mode==O_CONTOUR)
          window.AddDCStructContour(n_image_loaded-1,current.c_str());
        else if (open_mode==O_FUSION && window.CheckAddedImage(n_image_loaded-1, "fusion") && window.CheckAddedImage(n_image_loaded-1, "fusionSequence"))
          window.AddFusionImage(n_image_loaded-1,image,vvImageReader::IMAGE);
        else if (open_mode==O_LANDMARKS)
          window.AddLandmarks(n_image_loaded-1,image);
        open_mode = O_BASE;
      }
    }
    if (parse_mode == P_SEQUENCE) { //Finish any current sequence
      open_sequence(window, open_mode, parse_mode, sequence_filenames, n_image_loaded);
    }
  }

//   if(win!="" && lev!="") {
//     window.SetWindowLevel(atof(win.c_str()), atof(lev.c_str()));
//     window.ApplyWindowLevelToAllImages();
//   }

  if (link_images)
    window.LinkAllImages();

  int ret = app.exec();
  
#ifndef _WIN32
  // restoring the locale, just to be clean...
  setlocale(LC_NUMERIC, old_locale.c_str());
#endif

  return ret;
}
