

VV, a 4D image viewer, see : [creatis](http://vv.creatis.insa-lyon.fr)

### Status
[![Build Status](https://travis-ci.org/open-vv/vv.svg?branch=master)](https://travis-ci.org/open-vv/vv)

### Download
You can download the binaries here:
 - [Linux 64bits](https://www.creatis.insa-lyon.fr/rio/vv?action=AttachFile&do=get&target=vv-1.4Qt4-linux64)
 - [Windows 32bits](https://www.creatis.insa-lyon.fr/rio/vv?action=AttachFile&do=get&target=vv-1.4Qt4-win32.zip)
 - [Windows 64bits](https://www.creatis.insa-lyon.fr/rio/vv?action=AttachFile&do=get&target=vv-1.4Qt4-win64.zip)

### Installation
To install vv with Linux (e.g.: OpenSuse 42.2), follow the instructions:

## Qt
Install Qt5-devel and widget development library (Designer, Xml, Tools, Gui, Network, OpenGL, ...) with your package manager (e.g.: Yast)

## VTK
mkdir vtk
cd vtk
mkdir src
git clone https://github.com/Kitware/VTK.git src
cd src
git checkout tags/v7.0.0
cd ..
mkdir bin
cd bin
ccmake ../src (use c to configure and g to generate)
Be sure to set (in toogle mode):
  - Module_vtkGUISuppotQt to ON
  - Module_vtkGUISuppotQtOpenGL to ON
  - Module_vtkRenderingQt to ON
  - Module_vtkViewQt to ON
  - VTK_QT_VERSION to 5
  - VTK_RENDERING_BACKEND to OpenGL
  - All Qt5... DIR to /usr/local/Qt-5.../lib/cmake/QT5...
make
cd ../..

## ITK
mkdir itk
cd itk
mkdir src
git clone https://github.com/InsightSoftwareConsortium/ITK.git src
mkdir bin
ccmake ../src
Be sure to set (in toogle mode):
  - Module_ITKReview to ON
  - Module_ITKVtkGlue to ON
  - BUILD_SHARED_LIBS to ON
make
cd ../..

## vv
mkdir vv
cd vv
mkdir src
git clone https://github.com/open-vv/vv.git src
mkdir bin
ccmake ../src
Be sure to set (the correct paths to Qt5 and ITK
make
cd ../..
