#=========================================================
# CLITK = Command Line ITK
cmake_minimum_required(VERSION 2.4)
cmake_policy(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
PROJECT(clitk)
#=========================================================

#=========================================================
INCLUDE(cmake/common.cmake)
#=========================================================

#=========================================================
# Find ITK (required)
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)
#=========================================================

#=========================================================
# Find VTK (required)
FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(VTK_FOUND)
#=========================================================

#=========================================================
# Find gengetopt
FIND_PATH(CLITK_GENGETOPT gengetopt)
IF (CLITK_GENGETOPT STREQUAL "CLITK_GENGETOPT-NOTFOUND")
  MESSAGE("gengetopt not found, please install it (see http://www.gnu.org/software/gengetopt/gengetopt.html)")
ENDIF (CLITK_GENGETOPT STREQUAL "CLITK_GENGETOPT-NOTFOUND")
#=========================================================

#=========================================================
# Building in the source tree is forbidden
IF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
  MESSAGE(FATAL_ERROR "Building in the source tree is not allowed ! Quit; remove the file 'CMakeCache.txt' and the folder 'CMakeFiles' an
d build outside the sources (for example 'mkdir build ; cmake <CLITK_DIR>'.")
ENDIF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
#=========================================================

#=========================================================
# Remove some MS Visual c++ flags
IF(MSVC)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
ENDIF(MSVC)
#=========================================================

#=========================================================
INCLUDE_DIRECTORIES(itk filters segmentation registration tools ${PROJECT_BINARY_DIR})

# Select what is compiled
ADD_SUBDIRECTORY(common)
add_subdirectory(tools)
add_subdirectory(segmentation)
add_subdirectory(registration)

# Compilation options
OPTION(CLITK_EXPERIMENTAL "Enable experimental software and features" OFF)
OPTION(CLITK_BUILD_TOOLS "Build command-line tools" OFF)
OPTION(CLITK_BUILD_SEGMENTATION "Build command-line segmentation tools" OFF)
OPTION(CLITK_BUILD_REGISTRATION "Build command-line registration tools" OFF)

OPTION(CLITK_BUILD_VV "Build vv the 4D visualizer (requires VTK and QT)" ON)
IF (CLITK_BUILD_VV)
  add_subdirectory(vv)
ENDIF(CLITK_BUILD_VV)

#=========================================================

