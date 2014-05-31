cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

# set a default build type if it is undefined, then make sure it goes in the cache
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Build configuration type" FORCE)
set(build_type ${CMAKE_BUILD_TYPE})

#=========================================================
MACRO (DD in)
    MESSAGE(${in}=${${in}})
ENDMACRO(DD)
#=========================================================

project(SuperBuild_VV)

if(MSVC)
  set(CMAKE_C_FLAGS_DEBUG          "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1" CACHE STRING "" FORCE)
  set(CMAKE_C_FLAGS_MINSIZEREL     "/MT /O1 /Ob1 /D NDEBUG" CACHE STRING "" FORCE)
  set(CMAKE_C_FLAGS_RELEASE        "/MT /O2 /Ob2 /D NDEBUG" CACHE STRING "" FORCE)
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG" CACHE STRING "" FORCE)

  set(CMAKE_CXX_FLAGS_DEBUG          "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1" CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "/MT /O1 /Ob1 /D NDEBUG" CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE        "/MT /O2 /Ob2 /D NDEBUG" CACHE STRING "" FORCE)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG" CACHE STRING "" FORCE)
endif(MSVC)

include(ExternalProject)

set(base "${CMAKE_BINARY_DIR}")
set_property(DIRECTORY PROPERTY EP_BASE ${base})
set(source_prefix ${base}/Source)
set(build_prefix ${base}/Build)
set(install_prefix ${base}/Install)

find_package(Git REQUIRED)

#=========================================================
# QT
SET(QtWinSpecFiles "${source_prefix}/qt/mkspecs/win32-msvc2003/qmake.conf;${source_prefix}/qt/mkspecs/win32-msvc2005/qmake.conf;${source_prefix}/qt/mkspecs/win32-msvc2008/qmake.conf;${source_prefix}/qt/mkspecs/win32-msvc2010/qmake.conf")
ExternalProject_Add(
  QT
  SOURCE_DIR ${source_prefix}/qt
  GIT_REPOSITORY git://gitorious.org/qt/qt.git
  GIT_TAG v4.8.3
  PATCH_COMMAND sed -i "s/\\-MD/-MT/g" ${QtWinSpecFiles}
  CONFIGURE_COMMAND ${source_prefix}/qt/configure -confirm-license
                                                  -static 
                                                  -fast
                                                  -opensource
                                                  -release
                                                  -prefix ${build_prefix}/QT
                                                  -nomake examples
                                                  -nomake demos
                                                  -qt-libpng
                                                  -no-libtiff
                                                  -no-libjpeg
                                                  -no-libmng
                                                  -no-glib
  INSTALL_COMMAND ""
)
SET(qmake_executable "${build_prefix}/QT/bin/qmake")
#=========================================================

#=========================================================
# VTK
SET(VTK_CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES}")
IF(MSVC)
  SET(VTK_CMAKE_CXX_STANDARD_LIBRARIES "${VTK_CMAKE_CXX_STANDARD_LIBRARIES} Imm32.lib Winmm.lib Ws2_32.lib")
ENDIF(MSVC)
ExternalProject_Add(
  VTK 
  DEPENDS QT
  SOURCE_DIR ${source_prefix}/vtk
  GIT_REPOSITORY git://vtk.org/VTK.git
  GIT_TAG v5.10.1
  INSTALL_COMMAND ""
  CMAKE_ARGS
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${qmake_executable}
    -DCMAKE_C_FLAGS_DEBUG:STRING=${CMAKE_C_FLAGS_DEBUG}
    -DCMAKE_C_FLAGS_MINSIZEREL:STRING=${CMAKE_C_FLAGS_MINSIZEREL}
    -DCMAKE_C_FLAGS_RELEASE:STRING=${CMAKE_C_FLAGS_RELEASE}
    -DCMAKE_C_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_C_FLAGS_RELWITHDEBINFO}
    -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
    -DCMAKE_CXX_FLAGS_MINSIZEREL:STRING=${CMAKE_CXX_FLAGS_MINSIZEREL}
    -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
    -DCMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DVTK_USE_QT:BOOL=ON
    -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
    -DCMAKE_CXX_STANDARD_LIBRARIES:STRING=${VTK_CMAKE_CXX_STANDARD_LIBRARIES}
)
SET(VTK_DIR ${build_prefix}/VTK)
#=========================================================

#=========================================================
# GDCM
   ExternalProject_Add(
   GDCM
   SOURCE_DIR ${source_prefix}/gdcm
   GIT_REPOSITORY git://git.code.sf.net/p/gdcm/gdcm 
   GIT_TAG v2.2.4
   INSTALL_COMMAND ""
   CMAKE_ARGS
   -DCMAKE_C_FLAGS_DEBUG:STRING=${CMAKE_C_FLAGS_DEBUG}
   -DCMAKE_C_FLAGS_MINSIZEREL:STRING=${CMAKE_C_FLAGS_MINSIZEREL}
   -DCMAKE_C_FLAGS_RELEASE:STRING=${CMAKE_C_FLAGS_RELEASE}
   -DCMAKE_C_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_C_FLAGS_RELWITHDEBINFO}
   -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
   -DCMAKE_CXX_FLAGS_MINSIZEREL:STRING=${CMAKE_CXX_FLAGS_MINSIZEREL}
   -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
   -DCMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
   -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_PREFIX}
   -DCMAKE_BUILD_TYPE:STRING=${build_type}
)
SET(GDCM_DIR ${build_prefix}/GDCM)
#=========================================================

#=========================================================
# ITK
ExternalProject_Add(
  ITK
  SOURCE_DIR ${source_prefix}/itk
  GIT_REPOSITORY git://itk.org/ITK.git
  GIT_TAG v4.5.2
  INSTALL_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_C_FLAGS_DEBUG:STRING=${CMAKE_C_FLAGS_DEBUG}
    -DCMAKE_C_FLAGS_MINSIZEREL:STRING=${CMAKE_C_FLAGS_MINSIZEREL}
    -DCMAKE_C_FLAGS_RELEASE:STRING=${CMAKE_C_FLAGS_RELEASE}
    -DCMAKE_C_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_C_FLAGS_RELWITHDEBINFO}
    -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
    -DCMAKE_CXX_FLAGS_MINSIZEREL:STRING=${CMAKE_CXX_FLAGS_MINSIZEREL}
    -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
    -DCMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
)
SET(ITK_DIR ${build_prefix}/ITK)
#=========================================================

#=========================================================
# VV
SET(VV_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
if(MSVC)
  SET(VV_CMAKE_CXX_FLAGS "${VV_CMAKE_CXX_FLAGS} /bigobj")
endif(MSVC)

SET(VV_CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
if(CMAKE_COMPILER_IS_GNUCXX)
  SET(VV_CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif(CMAKE_COMPILER_IS_GNUCXX)

SET(MAKE_COMMAND "make")
if(MSVC)
  SET(MAKE_COMMAND "nmake")
endif(MSVC)

ExternalProject_Add(
  VV
  DEPENDS QT VTK ITK GDCM
  SOURCE_DIR ${source_prefix}/vv
  GIT_REPOSITORY git://git.creatis.insa-lyon.fr/clitk
  INSTALL_DIR ${install_prefix}
  INSTALL_COMMAND  make install 
  CMAKE_ARGS
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${qmake_executable}
    -DGDCM_DIR:PATH=${GDCM_DIR}
    -DITK_DIR:PATH=${ITK_DIR}
    -DVTK_DIR:PATH=${VTK_DIR}
    -DCMAKE_C_FLAGS_DEBUG:STRING=${CMAKE_C_FLAGS_DEBUG}
    -DCMAKE_C_FLAGS_MINSIZEREL:STRING=${CMAKE_C_FLAGS_MINSIZEREL}
    -DCMAKE_C_FLAGS_RELEASE:STRING=${CMAKE_C_FLAGS_RELEASE}
    -DCMAKE_C_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_C_FLAGS_RELWITHDEBINFO}
    -DCMAKE_CXX_FLAGS_DEBUG:STRING=${CMAKE_CXX_FLAGS_DEBUG}
    -DCMAKE_CXX_FLAGS_MINSIZEREL:STRING=${CMAKE_CXX_FLAGS_MINSIZEREL}
    -DCMAKE_CXX_FLAGS_RELEASE:STRING=${CMAKE_CXX_FLAGS_RELEASE}
    -DCMAKE_CXX_FLAGS_RELWITHDEBINFO:STRING=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
    -DCMAKE_CXX_FLAGS:STRING=${VV_CMAKE_CXX_FLAGS}
    -DCMAKE_EXE_LINKER_FLAGS:STRING=${VV_CMAKE_EXE_LINKER_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
)
#=========================================================

