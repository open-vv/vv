This page contains additional information on how to compile  [vv](https://github.com/open-vv/vv)  statically. This requires recompiling qt, vtk, itk and vv.

# 1. Preambule

Maybe you need to compile libjpeg (version 8 in our case) statically  [(http://www.ijg.org/)](http://www.ijg.org/)  and install libjpeg8-devel

```bash
./configure --enable-static --prefix="[path]/libjpeg8/build"
make
make install
```

and set in .bashrc file:

```bash
LD_LIBRARY_PATH=[path]/libjpeg8/build/lib64:${LD_LIBRARY_PATH}
LD_RUN_PATH=[path]/libjpeg8/build/lib64:${LD_RUN_PATH}
```

With your package manager you need libpng-devel

# 2. QT

Download and unzip the  [sources](http://www.qt.io/download-open-source/#section-5)  (recommended version Qt4.8.6).

## 2.1. Linux

```bash
../qt-everywhere-opensource-src-4.8.6/configure -static -opensource -confirm-license -release -prefix [path]/Qt4/build -nomake examples -nomake tests -no-gtkstyle -system-libpng -system-libjpeg -system-zlib -no-icu -no-libtiff -no-libmng (-opengl -L/usr/X11R6/lib64)&& make && make install
```

Moreover, you may install all xcb dependecies (eg: with Yast).

-   compiling in a separate directory than the sources (check  [shadow build](https://wiki.qt.io/Qt_shadow_builds)),
    

## 2.2. Windows

With Windows, you have to use the version v4.8.6

Determine your PLATFORM, e.g. win32-msvc2013 for Microsoft Visual c++ 2013.

Open Visual C++ command prompt and execute:

```bash
set PATH=%cd%\bin;%PATH%
..\qt\configure -static -opensource -release -prefix %cd% -nomake examples -nomake tests & nmake
```

# 3. VTK

Download the  [sources](http://www.vtk.org/download)  (VTK > v6 / recommended version VTK6.3.0).

Run CMake and set (be sure that VTK_QT_VERSION is 4 and the links to the different Qt libraries are correct (i.e. the latter Qt5 libraries)).

```bash
BUILD_SHARED_LIBS=OFF
BUILD_TESTING=OFF
CMAKE_BUILD_TYPE=Release
Module_vtkGUISupportQt=ON
Module_vtkGUISupportQtOpenGL=ON
Module_vtkGUISupportQtSQL=ON
Module_vtkRenderingQt=ON
Module_vtkViewsQt=ON
VTK_QT_VERSION=4
VTK_USE_SYSTEM_JPEG=ON
VTK_USE_SYSTEM_PNG=ON
JPEG_INCLUDE_DIR=[...]/libjpeg8/build/include
JPEG_LIBRARY=[...]/libjpeg8/build/lib64/libjpeg.a
```

Compile.

On Windows, Use CMake and Visual Studio:

-   For 32bits: Use Visual Studio 2013 Compiler in Cmake

# 4. ITK

Download the  [sources](http://www.itk.org/ITK/resources/software.html)  (recommended version ITK4.9.1).

Run CMake and set: (with a correct link to the latter VTK).

```bash
BUILD_SHARED_LIBS=OFF
DBUILD_TESTING=OFF
ITK_DYNAMIC_LOADING=OFF
ITK_USE_SYSTEM_JPEG=ON
ITK_USE_SYSTEM_PNG=ON
JPEG_INCLUDE_DIR=[...]/libjpeg8/build/include
JPEG_LIBRARY=[...]/libjpeg8/build/lib64/libjpeg.a
Module_ITKVtkGlue=ON
```

Compile.

On Windows, Use CMake and Visual Studio:

-   For 32bits: Use Visual Studio 2013 Compiler in Cmake
-   With CMake, you can remove ITK_USE_GIT_PROTOCOL and ITK_USE_KWSTYLE

# 5. VV

Download the  [sources](https://github.com/open-vv/vv).

On Linux, run CMake and set: (With the correct path to static ITK)

```bash
JPEG_INCLUDE_DIR=[...]/libjpeg8/build/include
JPEG_LIBRARY=[...]/libjpeg8/build/lib64/libjpeg.a
```
You can do strip ./vv to decrease the size of your executable.

On Windows, with Qt4, you just have to run CMake and compile with Visual Studio:

-   For 32bits: Use Visual Studio 2013 Compiler in Cmake
-   Be sure to parametrize Visual Studio with Release version
-   On Visual Studio, into propertyManager: right click on vv / Properties / Linker / Command Line  
    Add in Additional Options : /FORCE:MULTIPLE

