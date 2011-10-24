#=========================================================
# CLITK = Command Line ITK
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
PROJECT(clitk)
#=========================================================

#=========================================================
INCLUDE(cmake/common.cmake)
#=========================================================

#=========================================================
#Support for the CTest dashboard testing system
OPTION(BUILD_TESTING "Build the testing tree" OFF)
IF (BUILD_TESTING)
  OPTION(CLITK_BUILD_TESTING "Test ITK" ON)
  INCLUDE(CTest)
ENDIF(BUILD_TESTING)
#=========================================================

#=========================================================
# Find ITK (required)
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE("${ITK_USE_FILE}")
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)
#=========================================================

#=========================================================
# Find VTK (required)
FIND_PACKAGE(VTK REQUIRED)
IF(VTK_FOUND)
  INCLUDE("${VTK_USE_FILE}")
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(VTK_FOUND)
#=========================================================

#=========================================================
# Find gengetopt, will create a target exe if not found
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
FIND_PACKAGE(Gengetopt)
#=========================================================

#=========================================================
# Find libstatgrab is installed, add clitkMemoryUsage.cxx in the library
FIND_LIBRARY(LIBSTATGRAB NAMES statgrab PATHS)
IF (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
#  MESSAGE("Install libstatgrab (http://www.i-scream.org/libstatgrab/) for memory usage information")
  SET(CLITK_MEMORY_INFO OFF)
ELSE (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")
  SET(CLITK_MEMORY_INFO ON)
ENDIF (${LIBSTATGRAB} MATCHES "LIBSTATGRAB-NOTFOUND")  
#=========================================================

#=========================================================
# If the user choose to build documentation, then search for Doxygen executables.
OPTION(BUILD_DOXYGEN "Build Doxygen Documentation" OFF)
IF(BUILD_DOXYGEN)
  FIND_PACKAGE(Doxygen)
  ADD_SUBDIRECTORY(Doxygen)
ENDIF(BUILD_DOXYGEN)
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
ADD_SUBDIRECTORY(tools)
ADD_SUBDIRECTORY(segmentation)
ADD_SUBDIRECTORY(registration)


# Compilation options
OPTION(CLITK_EXPERIMENTAL "Enable experimental software and features" OFF)
OPTION(CLITK_BUILD_TOOLS "Build command-line tools" OFF)
OPTION(CLITK_BUILD_SEGMENTATION "Build command-line segmentation tools" OFF)
OPTION(CLITK_BUILD_REGISTRATION "Build command-line registration tools" OFF)

OPTION(CLITK_BUILD_VV "Build vv the 4D visualizer (requires VTK and QT)" ON)
IF (CLITK_BUILD_VV)
  ADD_SUBDIRECTORY(vv)
ENDIF(CLITK_BUILD_VV)


#=========================================================
# Build test when vv has been compiled
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(tests)
ENDIF(BUILD_TESTING)

#=========================================================
# Install scripts when running make install
SET(SCRIPTS 
  scripts/calculate_motion_amplitude.sh
  scripts/midp_common.sh
  scripts/registration.sh
  scripts/create_midP.sh
  scripts/create_midP-2.0.sh        
  scripts/create_mhd_4D.sh          
  scripts/create_mhd_4D_pattern.sh  
  scripts/create_midP_masks.sh  
  scripts/create_midP_masks-2.0.sh  
  scripts/pts_to_landmarks.sh
  scripts/create_mhd_3D.sh
  scripts/dicom_info.sh
)

INSTALL (FILES ${SCRIPTS} DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

