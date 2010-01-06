#=========================================================
# CLITK = Command Line ITK
cmake_minimum_required(VERSION 2.4)
cmake_policy(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
PROJECT(clitk)
#=========================================================
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
#=========================================================
# Find ITK (required)
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
  LINK_LIBRARIES(ITKBasicFilters)
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)
#=========================================================
IF(CLITK_BUILD_VV)
    FIND_PACKAGE(VTK)
    IF(VTK_FOUND)
      INCLUDE(${VTK_USE_FILE})
      LINK_LIBRARIES (
        vtkCommon
        vtkRendering
        vtkIO
        vtkFiltering
        vtkGraphics
        vtkWidgets
        vtkImaging
        )
    ELSE(VTK_FOUND)
      MESSAGE(FATAL_ERROR
        "Please set VTK_DIR.")
    ENDIF(VTK_FOUND)
ENDIF(CLITK_BUILD_VV)

#=========================================================
# Find gengetopt
FIND_PATH(CLITK_GENGETOPT gengetopt)
IF (CLITK_GENGETOPT STREQUAL "CLITK_GENGETOPT-NOTFOUND")
  MESSAGE("gengetopt not found, please install it (see http://www.gnu.org/software/gengetopt/gengetopt.html)")
ENDIF (CLITK_GENGETOPT STREQUAL "CLITK_GENGETOPT-NOTFOUND")
#=========================================================

#=========================================================
INCLUDE(cmake/common.cmake)
#=========================================================

#=========================================================
# Building in the source tree is forbidden
IF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
  MESSAGE(FATAL_ERROR "Building in the source tree is not allowed ! Quit; remove the file 'CMakeCache.txt' and the folder 'CMakeFiles' an
d build outside the sources (for example 'mkdir build ; cmake <CLITK_DIR>'.")
ENDIF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
#=========================================================

#=========================================================
OPTION(CLITK_BUILD_VV "Build vv the 4D visualizer (requires VTK and QT)" ON)
OPTION(CLITK_BUILD_TOOLS "Build command-line tools" OFF)
#=========================================================

include_directories(itk filters)

#=========================================================
add_subdirectory(common)
add_subdirectory(filters)

IF (CLITK_BUILD_VV)
  add_subdirectory(vv)
ENDIF(CLITK_BUILD_VV)

IF (CLITK_BUILD_TOOLS)
  add_subdirectory(tools)
ENDIF(CLITK_BUILD_TOOLS)

#=========================================================
