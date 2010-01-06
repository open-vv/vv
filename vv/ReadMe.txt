How to install and run vv on linux:

1.CMake :
Before doing anything, you need to download the latest stable release of CMake :
	http://www.cmake.org/HTML/Index.html

Uncompress the file and add cmake to your path :
	tar zxvf cmake-2.4.2-Linux-i386.tar.gz
	tar xvf  cmake-2.4.2-Linux-files.tar
	PATH=$PATH:/cmake/bin/directory    

2.ITK:
Download the latest stable release from their page or from their CVS repository :
	http://www.itk.org/HTML/Download.htm

Uncompress it and rename the resulting folder as « Insight » :
	tar zxvf InsightToolkit-2.6.0.tar.gz 
	mkdir Insight
	mv InsightToolkit-2.6.0 Insight

Still from your home, create a binary directory, get into it and run CMake :
	mkdir Insight-binary
	cd Insight-binary
	ccmake ../Insight

NB : You can disable BUILT_EXAMPLES and BUILT_TESTS.

Then compile ITK:
	make

3.Qt 4:
Download the latest stable release from their page :
	http://trolltech.com/downloads/opensource 

Install it following instructions on :
	http://doc.trolltech.com/4.3/install-x11.html

4.VTK:

Download the latest stable release from their CVS repository :
	http://www.vtk.org/get-software.php

Create a binary directory, get into it and run CMake :
	mkdir vtk-binary
	cd vtk-binary
	ccmake ../vtk

NB: you need to change built options to make it compatible with Qt. 
	Set BUILD_SHARED_LIBS to ON,
	Set VTK_USE_GUISUPPORT to ON
	Set VTK_USE_QVTK to ON. Then, specify the Qt version you are using. 

Then compile vtk :
	make

5.vv:

Download the latest release from the git repository :
	git clone http://creatis.insa-lyon.fr/~schaerer/vv.git

Go inside the build directory:
	cd vv/build

run cmake and compile it.
	ccmake ..
	make -j2
